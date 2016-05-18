#pragma once
#ifndef __MEMORYSTATSTORE_H__
#define __MEMORYSTATSTORE_H__
/////////////////////////////////////////
#include "istatstore.h"
/////////////////////////////////////////
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
//////////////////////////////
#include <boost/noncopyable.hpp>
//////////////////////////////////////////
namespace info {
	////////////////////////////////
	template <typename IDTYPE = unsigned long, typename INTTYPE = int, typename STRINGTYPE = std::string, typename WEIGHTYPE = float>
	class MemoryStatStore : public IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>, private boost::noncopyable {
	public:
		using strings_vector = std::vector<STRINGTYPE>;
		using ints_vector = std::vector<IDTYPE>;
		using ints_string_map = std::map<IDTYPE, STRINGTYPE>;
		using DatasetType = StatDataset<IDTYPE, INTTYPE, STRINGTYPE>;
		using datasets_vector = std::vector<DatasetType>;
		using VariableType = StatVariable<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using variables_vector = std::vector<VariableType>;
		using IndivType = StatIndiv<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using indivs_vector = std::vector<IndivType>;
		using ValueType = StatValue<IDTYPE, INTTYPE, STRINGTYPE>;
		using values_vector = std::vector<ValueType>;
		//
		using datasets_map = std::map<IDTYPE, DatasetType>;
		using variables_map = std::map<IDTYPE, VariableType>;
		using indivs_map = std::map<IDTYPE, IndivType>;
		using values_map = std::map<IDTYPE, ValueType>;
	private:
		//
		std::atomic<IDTYPE> m_lastid;
		//
		datasets_vector m_datasets;
		variables_vector m_variables;
		indivs_vector m_indivs;
		values_vector m_values;
		//
		std::mutex m_mutexDataset;
		std::mutex m_mutexIndiv;
		std::mutex m_mutexVariable;
		std::mutex m_mutexValue;
		//
		IDTYPE next_id(void) {
			return (this->m_lastid++);
		}// next_id
	public:
		MemoryStatStore() :m_lastid(1) {}
		virtual ~MemoryStatStore() {}
	public:
		// overrides
		virtual bool is_valid(void) {
			return (true);
		}
		virtual void begin_transaction(void) {
		}
		virtual void commit_transaction(void) {
		}
		virtual void rollback_transaction(void) {
		}
		//
		virtual bool find_all_datasets_count(size_t &nCount) {
			std::unique_lock<std::mutex> lck(this->m_mutexDataset);
			nCount = this->m_datasets.size();
			return (true);
		}
		virtual bool find_all_datasets(datasets_vector &oList, size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			oList.clear();
			if (count < 1) {
				count = DATATRANSFER_CHUNK_SIZE;
			}
			size_t nStart = skip;
			{
				std::unique_lock<std::mutex> lck(this->m_mutexDataset);
				datasets_vector &vv = this->m_datasets;
				size_t nMax = vv.size();
				if (nStart > nMax) {
					nStart = nMax;
				}
				size_t nEnd = nStart + count;
				if (nEnd > nMax) {
					nEnd = nMax;
				}
				size_t nc = nEnd - nStart;
				if (nc > 0) {
					oList.resize(nc);
					for (size_t i = 0; i < nc; ++i) {
						oList[i] = vv[nStart + i];
					}// i
				}// nc
			}// sync
			return (true);
		}//find_all_datasets
		virtual bool find_all_datasets_ids(ints_vector &oList, size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			oList.clear();
			if (count < 1) {
				count = DATATRANSFER_CHUNK_SIZE;
			}
			size_t nStart = skip;
			{
				std::unique_lock<std::mutex> lck(this->m_mutexDataset);
				datasets_vector &vv = this->m_datasets;
				size_t nMax = vv.size();
				if (nStart > nMax) {
					nStart = nMax;
				}
				size_t nEnd = nStart + count;
				if (nEnd > nMax) {
					nEnd = nMax;
				}
				size_t nc = nEnd - nStart;
				if (nc > 0) {
					oList.resize(nc);
					for (size_t i = 0; i < nc; ++i) {
						oList[i] = (vv[nStart + i]).id();
					}// i
				}// nc
			}// sync
			return (true);
		}//find_all_datasets_ids
		virtual bool find_dataset(DatasetType &cur) {
			std::unique_lock<std::mutex> lck(this->m_mutexDataset);
			const datasets_vector &vv = this->m_datasets;
			const IDTYPE nId = cur.id();
			STRINGTYPE sigle = cur.sigle();
			auto it = std::find_if(vv.begin(), vv.end(), [nId, sigle](const DatasetType &p)->bool {
				if ((nId != 0) && (p.id() == nId)) {
					return (true);
				}
				else if ((!sigle.empty()) && (p.sigle() == sigle)) {
					return (true);
				}
				return (false);
			});
			if (it != vv.end()) {
				cur = *it;
				return (true);
			}
			return (false);
		}//find_dataset
		virtual bool maintains_dataset(DatasetType &cur, bool/* bCommit = true*/) {
			if (!cur.is_writeable()) {
				return (false);
			}
			{
				std::unique_lock<std::mutex> lck(this->m_mutexDataset);
				DatasetType xSet(cur);
				this->find_dataset(cur);
				IDTYPE nId = xSet.id();
				if (nId != 0) {
					cur.id(nId);
					cur.version(xSet.version() + 1);
				}
				else {
					cur.id(next_id());
					cur.version(1);
				}
				DatasetType o(cur);
				this->m_datasets.push_back(o);
			}
			return (this->find_dataset(cur));
		}//maintains_dataset
		virtual bool remove_dataset(const DatasetType &cur, bool /*bCommit = true*/) {
			DatasetType xSet(cur);
			if (!this->find_dataset(xSet)) {
				return (false);
			}
			IDTYPE nDatasetId = xSet.id();
			std::unique_lock<std::mutex> lock1(this->m_mutexDataset, std::defer_lock);
			std::unique_lock<std::mutex> lock2(this->m_mutexVariable, std::defer_lock);
			std::unique_lock<std::mutex> lock3(this->m_mutexIndiv, std::defer_lock);
			std::unique_lock<std::mutex> lock4(this->m_mutexValue, std::defer_lock);
			std::lock(lock1, lock2, lock3, lock4);
			datasets_vector &datasets = this->m_datasets;
			variables_vector &variables = this->m_variables;
			indivs_vector &indivs = this->m_indivs;
			values_vector &values = this->m_values;
			ints_vector ids, xids;
			std::for_each(variables.begin(), variables.end(), [&](VariableType &p) {
				IDTYPE nId = p.id();
				if (nId == nDatasetId) {
					ids.push_back(p.id());
					std::for_each(values.begin(), values.end(), [&](ValueType &v) {
						if (v.variable_id() == nId) {
							xids.push_back(v.id());
						}
					});
				}//ok
			});
			std::for_each(ids.begin(), ids.end(), [&](IDTYPE nId) {
				auto it = std::find_if(variables.begin(), variables.end(), [&](VariableType &v)->bool {
					return (v.dataset_id() == nId);
				});
				if (it != variables.end()) {
					variables.erase(it);
				}
			});
			ints_vector nids;
			std::for_each(indivs.begin(), indivs.end(), [&](IndivType &p) {
				if (p.dataset_id() == nDatasetId) {
					nids.push_back(p.id());
				}
			});
			std::for_each(nids.begin(), nids.end(), [&](IDTYPE nId) {
				auto it = std::find_if(indivs.begin(), indivs.end(), [&](IndivType &v)->bool {
					return (v.dataset_id() == nId);
				});
				if (it != indivs.end()) {
					indivs.erase(it);
				}
			});
			std::for_each(xids.begin(), xids.end(), [&](IDTYPE nId) {
				auto it = std::find_if(values.begin(), values.end(), [nId](ValueType &v)->bool {
					return (v.id() == nId);
				});
				if (it != values.end()) {
					values.erase(it);
				}
			});
			auto jt = std::find_if(datasets.begin(), datasets.end(), [nDatasetId](DatasetType &v)->bool {
				return (v.id() == nDatasetId);
			});
			if (jt != datasets.end()) {
				datasets.erase(jt);
			}
			return (true);
		}//remove_dataset
		//
		virtual bool find_dataset_variables_types(const DatasetType &oSet, ints_string_map &oMap) {
			oMap.clear();
			DatasetType xSet(oSet);
			if (!this->find_dataset(xSet)) {
				return (false);
			}
			IDTYPE nDatasetId = xSet.id();
			std::unique_lock<std::mutex> lock2(this->m_mutexVariable);
			variables_vector &vv = this->m_variables;
			std::for_each(vv.begin(), vv.end(), [&](VariableType &v) {
				if (v.dataset_id() == nDatasetId) {
					STRINGTYPE s = v.vartype();
					IDTYPE key = v.id();
					oMap[key] = s;
				}
			});
			return (true);
		}//find_dataset_variables_types
		virtual bool find_dataset_variables_count(const DatasetType &oSet, size_t &nCount) {
			return (true);
		}
		virtual bool find_dataset_variables_ids(const DatasetType &oSet, ints_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_dataset_variables(const DatasetType &oSet, variables_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_variable(VariableType &cur) {
			return (true);
		}
		virtual bool maintains_variables(const variables_vector &oVars, bool bCommit = true,
			bool bRemove = false) {
			return (true);
		}
		virtual bool remove_variable(const VariableType &oVar, bool bCommit = true) {
			return (true);
		}
		//
		virtual bool find_dataset_indivs_count(const DatasetType &oSet, size_t &nCount) {
			return (true);
		}
		virtual bool find_dataset_indivs_ids(const DatasetType &oSet, ints_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_dataset_indivs(const DatasetType &oSet, indivs_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_indiv(IndivType &cur) {
			return (true);
		}
		virtual bool maintains_indivs(const indivs_vector &oInds, bool bCommit = true,
			bool bRemove = false) {
			return (true);
		}
		virtual bool remove_indiv(const IndivType &oInd, bool bCommit = true) {
			return (true);
		}
		//
		virtual bool find_value(ValueType &cur) {
			return (true);
		}
		virtual bool maintains_values(const values_vector &oVals, bool bCommit = true,
			bool bRemove = false) {
			return (true);
		}
		virtual bool find_dataset_values_count(const DatasetType &oSet, size_t &nCount) {
			return (true);
		}
		virtual bool find_dataset_values(const DatasetType &oSet, values_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_variable_values(VariableType &oVar, values_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_variable_distinct_values(VariableType &oVar, strings_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
		virtual bool find_indiv_values(IndivType &oInd, values_vector &oList,
			size_t skip = 0, size_t count = DATATRANSFER_CHUNK_SIZE) {
			return (true);
		}
	};// class MemoryStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>
	/////////////////////////////////////
}// namespace info
/////////////////////////////////////////
#endif // !__MEMORYSTATSTORE_H__
