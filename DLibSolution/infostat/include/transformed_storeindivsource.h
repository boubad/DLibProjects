#pragma once
#ifndef __TRANSFORMED_STOREINDIVSOURCE_H__
#define __TRANSFORMED_STOREINDIVSOURCE_H__
///////////////////////////////////////////////
#include "storeindivsource.h"
///////////////////////////////////
namespace info {
	/////////////////////////////
	template<typename U = unsigned long, typename INTTYPE = unsigned long,
		typename STRINGTYPE = std::string, typename WEIGHTYPE = double>
		class TranformedStoreIndivSource : public StoreIndivSource<U, INTTYPE,
		STRINGTYPE, WEIGHTYPE> {
		public:
			using ints_vector = std::vector<U>;
			using strings_vector = std::vector<STRINGTYPE>;
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
			using StatSummatorType = StatSummator<U, STRINGTYPE>;
			using StatInfoType = StatInfo<U, STRINGTYPE>;
		protected:
			TransformationType m_transf;
			StatSummatorType m_summator;
			ints_vector m_varids;
			ints_vector m_indids;
			strings_vector m_names;
			//
		public:
			TranformedStoreIndivSource(StoreType *pStore, const STRINGTYPE &datasetName,
				const TransformationType mode = TransformationType::noTransf) :
				StoreIndivSourceType(pStore, datasetName), m_transf(mode) {
				StatSummatorType &oSum = this->m_summator;
				oSum.clear();
				const size_t nc = this->count();
				for (size_t i = 0; i < nc; ++i) {
					IndivTypePtr oInd = StoreIndivSourceType::get(i);
					oSum.add(oInd);
				} // i
				oSum.get_keys(this->m_varids);
				for (size_t i = 0; i < nc; ++i) {
					(void)this->get(i);
				}
			}
			virtual ~TranformedStoreIndivSource() {
			}
		public:
			const StatSummatorType &get_summator(void) const {
				return (this->m_summator);
			}
			TransformationType transformation(void) {
				return (this->m_transf);
			}
			void transformation(TransformationType t) {
				this->m_transf = t;
			}
		public:
			virtual IndivTypePtr get(const size_t pos) {
				IndivTypePtr oInd = StoreIndivSourceType::get(pos);
				IndivType *pInd = oInd.get();
				if (pInd != nullptr) {
					StatSummatorType &oSum = this->m_summator;
					DataMap &center = pInd->center();
					for (auto &key : this->m_varids) {
						if (center.find(key) == center.end()) {
							StatInfoType info;
							if (oSum.get(key, info)) {
								double vx = info.get_mean();
								InfoValue v(vx);
								center[key] = v;
							}//
						}
					}// key
					IndivTypePtr rr = oSum.transform(oInd, this->m_transf);
					IndivType *pRet = rr.get();
					if (pRet != nullptr) {
						ints_vector &m = this->m_indids;
						const U nId = pRet->id();
						auto it = std::find(m.begin(), m.end(), nId);
						if (it == m.end()) {
							m.push_back(nId);
						}
					}
					return (rr);
				}
				else {
					return (oInd);
				}
			}
			virtual void reset(void) {
				StoreIndivSourceType::reset();
				StatSummatorType &oSum = this->m_summator;
				oSum.clear();
				const size_t nc = this->count();
				for (size_t i = 0; i < nc; ++i) {
					IndivTypePtr oInd = StoreIndivSourceType::get(i);
					oSum.add(oInd);
				} // i
			} // reset
			size_t get_variables_count(void) const {
				return (this->m_varids.size());
			}
			void get_variables_ids(ints_vector &v) const {
				v = this->m_varids;
			}
			void get_indivs_names(strings_vector &v) const {
				v = this->m_names;
			}
			bool get_variable_id(const size_t pos, U &aIndex) const {
				bool bRet = false;
				const ints_vector &vars = this->m_varids;
				if (pos < vars.size()) {
					aIndex = vars[pos];
					bRet = true;
				}
				else {
					aIndex = 0;
				}
				return	(bRet);
			}
			void get_indivs_ids(ints_vector &v) const {
				v = this->m_indids;
			}
			bool get_indiv_id(const size_t pos, U &aIndex) const {
				bool bRet = false;
				const ints_vector &inds = this->m_indids;
				if (pos < inds.size()) {
					aIndex = inds[pos];
					bRet = true;
				}
				else {
					aIndex = 0;
				}
				return	(bRet);
			}
			template <typename T>
			bool get_data_array(size_t &nRows, size_t &nCols, std::vector<T> &data) {
				ints_vector &inds = this->m_indids;
				ints_vector &vars = this->m_varids;
				nRows = inds.size();
				nCols = vars.size();
				size_t nn = (size_t)(nRows * nCols);
				if (nn < 1) {
					data.clear();
					return (false);
				}
				data.resize(nn);
				this->m_names.resize(nRows);
				for (size_t i = 0; i < nRows; ++i) {
					const U aIndex = inds[i];
					IndivTypePtr oInd = this->find(aIndex);
					IndivType *pIndiv = oInd.get();
					if (pIndiv != nullptr) {
						const DataMap &oMap = pIndiv->center();
						size_t base_pos = (size_t)(i * nCols);
						(this->m_names)[i] = pIndiv->sigle();
						for (size_t j = 0; j < nCols; ++j) {
							const U nVarId = vars[j];
							auto it = oMap.find(nVarId);
							if (it != oMap.end()) {
								const InfoValue &vv = (*it).second;
								T val = 0;
								vv.get_value(val);
								data[base_pos + j] = val;
							}// ok
						}// j
					}// pIndiv
				}// i
				return (true);
			}// get_data_array
	};
	// class StoreIndivSource<U,INTTYPE,STRINGTYPE,WEIGHTYPE>
	/////////////////////////////////
}// namespace info
/////////////////////////////////////////////////
#endif // !__TRANSFORMED_STOREINDIVSOURCE_H__
