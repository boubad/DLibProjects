#pragma once
#ifndef __STOREINDIVSOURCE_H__
#define __STOREINDIVSOURCE_H__
/////////////////////////////////
#include "istatstore.h"
#include "infostat.h"
#include "indiv.h"
///////////////////////////////////
namespace info {
	/////////////////////////////
	template<typename U = unsigned long, typename INTTYPE = unsigned long,
		typename STRINGTYPE = std::string, typename WEIGHTYPE = double>
		class StoreIndivSource : public IIndivSource<U, STRINGTYPE>, private boost::noncopyable {
		using mutex_type = std::mutex;
		public:
		    using IndexType = U;
			using ints_vector = std::vector<U>;
			using IndivType = Indiv<U, STRINGTYPE>;
			using DataMap = std::map<U, InfoValue>;
			using IndivTypePtr = std::shared_ptr<IndivType>;
			using StoreType = IStatStore<U, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using DBIndivType = StatIndiv<U, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using DatasetType = StatDataset<U, INTTYPE, STRINGTYPE>;
			using ValueType = StatValue<U, INTTYPE, STRINGTYPE>;
			using values_vector = std::vector<ValueType>;
			using StoreIndivSourceType = StoreIndivSource<U, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using indivptrs_vector = std::vector<IndivTypePtr>;
			using SourceType = IIndivSource<U, STRINGTYPE>;
			using ints_doubles_map = std::map<U, double>;
			using StatSummatorType = StatSummator<U, STRINGTYPE>;
			using StatInfoType = StatInfo<U, STRINGTYPE>;
		private:
			StoreType *m_pstore;
			std::atomic<size_t> m_ncount;
			std::atomic<size_t> m_current;
			ints_vector m_ids;
			DatasetType m_oset;
			indivptrs_vector m_indivs;
			StatSummatorType m_summator;
			//
			mutex_type _mutex;
		public:
			StoreIndivSource(StoreType *pStore, const STRINGTYPE &datasetName) :
				m_pstore(pStore), m_ncount(0), m_current(0) {
				assert(this->m_pstore != nullptr);
				assert(this->m_pstore->is_valid());
				DatasetType &oSet = this->m_oset;
				oSet.sigle(datasetName);
				pStore->find_dataset(oSet);
				assert(oSet.id() != 0);
				size_t nc = 0;
				pStore->find_dataset_indivs_count(oSet, nc);
				pStore->find_dataset_indivs_ids(oSet, this->m_ids, 0, nc);
				if (nc > 0) {
					this->m_indivs.resize(nc);
				}
				this->m_ncount.store(nc);
			}
			virtual ~StoreIndivSource() {
			}
		public:
			virtual void weights(ints_doubles_map &oWeights) {
				oWeights.clear();
				std::unique_lock<mutex_type> oLock(this->_mutex);
				StatSummatorType &oSum = this->m_summator;
				ints_vector keys;
				oSum.get_keys(keys);
				for (auto &key : keys) {
					StatInfoType info;
					oSum.get(key,info);
					double s2 = info.get_variance();
					if ((s2 > 0) && (info.get_count() > 1)) {
						double px = info.get_weight();
						oWeights[key] = px;
					}
				}// keys
			}// weights
			virtual size_t count(void) {
				return (this->m_ncount.load());
			}
			virtual IndivTypePtr find(const IndexType aIndex) {
				bool bFound = false;
				size_t ipos = 0;
				{
					ints_vector & vv = this->m_ids;
					const size_t n = vv.size();
					for (size_t i = 0; i < n; ++i) {
						if (vv[i] == aIndex) {
							bFound = true;
							ipos = i;
							break;
						}
					}
				}
				if (bFound) {
					return this->get(ipos);
				}
				return (IndivTypePtr());
			}
			virtual IndivTypePtr get(const size_t pos) {
				IndivTypePtr oRet;
				if (pos >= this->m_ncount.load()) {
					return oRet;
				}
				{
					std::unique_lock<mutex_type> oLock(this->_mutex);
					indivptrs_vector & vv = this->m_indivs;
					oRet = vv[pos];
					if (oRet.get() != nullptr) {
						return (oRet);
					}
				}
				StoreType *pStore = this->m_pstore;
				DatasetType &oSet = this->m_oset;
				U aIndex = (this->m_ids)[pos];
				DBIndivType oInd(aIndex);
				oInd.dataset_id(oSet.id());
				size_t nc = 0;
				if (!pStore->find_indiv_values_count(oInd, nc)) {
					return (oRet);
				}
				if (nc < 1) {
					return (oRet);
				}
				values_vector oVals;
				if (!pStore->find_indiv_values(oInd, oVals, 0, nc)) {
					return (oRet);
				}
				DataMap oMap;
				for (auto &v : oVals) {
					if (!v.empty()) {
						const U key = (U)v.variable_id();
						InfoValue val = v.value();
						oMap[key] = val;
					} // v
				}
				oRet = std::make_shared<IndivType>(aIndex, oMap, oInd.sigle());
				{
					std::unique_lock<mutex_type> oLock(this->_mutex);
					indivptrs_vector & vv = this->m_indivs;
					vv[pos] = oRet;
					this->m_summator.add(oRet);
				}
				return (oRet);
			}
			virtual void reset(void) {
				size_t nc = 0;
				{
					std::unique_lock<mutex_type> oLock(this->_mutex);
					StoreType *pStore = this->m_pstore;
					this->m_current.store(0);
					DatasetType &oSet = this->m_oset;
					this->m_indivs.clear();
					this->m_summator.clear();
					pStore->find_dataset_indivs_count(oSet, nc);
					pStore->find_dataset_indivs_ids(oSet, this->m_ids, 0, nc);
					if (nc > 0) {
						this->m_indivs.resize(nc);
					}
					this->m_ncount.store(nc);
				}
				for (size_t i = 0; i < nc; ++i) {
					(void)this->get(i);
				}
				this->m_summator.compute();
			} // reset
			virtual IndivTypePtr next(void) {
				size_t pos = this->m_current.load();
				if (pos >= this->m_ncount.load()) {
					return (IndivTypePtr());
				}
				{
					std::unique_lock<mutex_type> oLock(this->_mutex);
					this->m_current.store((size_t)(pos + 1));
				}
				return this->get(pos);
			} // next
	};
	// class StoreIndivSource<U,T,INTTYPE,STRINGTYPE,WEIGHTYPE>
	/////////////////////////////////
}// namespace info
////////////////////////////////////
#endif // __STOREINDIVSOURCE_H__
