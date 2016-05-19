#pragma once
#ifndef __STOREINDIVSOURCE_H__
#define __STOREINDIVSOURCE_H__
/////////////////////////////////
#include "istatstore.h"
#include "indiv.h"
///////////////////////////////////
#include <cassert>
#include <mutex>
/////////////////////////////////
#include <boost/noncopyable.hpp>
//////////////////////////////////
namespace info {
	/////////////////////////////
	template <typename U = unsigned long,
		typename T = float,
		typename INTTYPE = int,
		typename STRINGTYPE = std::string,
		typename WEIGHTYPE = float>
		class StoreIndivSource : public IIndivSource<U, T>,
		private boost::noncopyable {
		public:
			using ints_vector = std::vector<U>;
			using IndivType = Indiv<U, T>;
			using DataMap = std::map<U, T>;
			using IndivTypePtr = std::shared_ptr<IndivType>;
			using StoreType = IStatStore<U, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using DBIndivType = StatIndiv<U, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using DatasetType = StatDataset<U, INTTYPE, STRINGTYPE>;
			using ValueType = StatValue<U, INTTYPE, STRINGTYPE>;
			using values_vector = std::vector<ValueType>;
			using StoreIndivSourceType = StoreIndivSource<U, T, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		private:
			StoreType *m_pstore;
			size_t m_current;
			ints_vector m_ids;
			DatasetType m_oset;
			//
			std::mutex _mutex;
		public:
			StoreIndivSource(StoreType *pStore, const STRINGTYPE &datasetName) :m_pstore(pStore), m_current(0) {
				assert(this->m_pstore != nullptr);
				assert(this->m_pstore->is_valid());
				this->m_oset.sigle(datasetName);
				pStore->find_dataset(this->m_oset);
				assert(this->m_oset.id() != 0);
				size_t nc = 0;
				pStore->find_dataset_indivs_count(this->m_oset, nc);
				pStore->find_dataset_indivs_ids(this->m_oset, this->m_ids, 0, nc);
			}
			virtual ~StoreIndivSource() {}
		public:
			virtual size_t count(void) {
				std::unique_lock<std::mutex> oLock(this->_mutex);
				return (this->m_ids.size());
			}
			virtual IndivTypePtr get(const size_t pos) {
				std::unique_lock<std::mutex> oLock(this->_mutex);
				assert(pos < this->m_ids.size());
				StoreType *pStore = this->m_pstore;
				assert(pStore != nullptr);
				DatasetType &oSet = this->m_oset;
				U aIndex = (this->m_ids)[pos];
				DBIndivType oInd(aIndex);
				oInd.dataset_id(oSet.id());
				IndivTypePtr oRet;
				size_t nc = 0;
				if (!pStore->find_indiv_values_count(oInd, nc)) {
					return (oRet);
				}
				values_vector oVals;
				if (!pStore->find_indiv_values(oInd, oVals, 0, nc)) {
					return (oRet);
				}
				DataMap oMap;
				std::for_each(oVals.begin(), oVals.end(), [&](const ValueType &v) {
					if (!v.empty()) {
						T val;
						if (v.get_value(val)) {
							oMap[v.variable_id()] = val;
						}
					}// v
				});
				if (oMap.empty()) {
					return (oRet);
				}
				oRet = std::make_shared<IndivType>(aIndex, oMap);
				return (oRet);
			}
			virtual void reset(void) {
				std::unique_lock<std::mutex> oLock(this->_mutex);
				this->m_current = 0;
				size_t nc = 0;
				pStore->find_dataset_indivs_count(this->m_oset, nc);
				pStore->find_dataset_indivs_ids(this->m_oset, this->m_ids, 0, nc);
			}// reset
			virtual IndivTypePtr next(void) {
				IndivTypePtr oRet;
				size_t n = 0;
				{
					std::unique_lock<std::mutex> oLock(this->_mutex);
					n = this->m_current;
					if (n >= this->m_ids.size()) {
						return (oRet);
					}
					this->m_current = n + 1;
				}
				return (this->get(n));
			}// next
	};// class StoreIndivSource<U,T,INTTYPE,STRINGTYPE,WEIGHTYPE>
	/////////////////////////////////
}// namespace info
////////////////////////////////////
#endif // __STOREINDIVSOURCE_H__
