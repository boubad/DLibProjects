#pragma once
#ifndef __ISTOREHELPER__H__
#define __ISTOREHELPER__H__
////////////////////////////////////
#include "dbstatitems.h"
////////////////////////////////////////////
#include <vector>
#include <map>
////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	class IStoreHelper {
	public:
		virtual void begin_transaction(void) = 0;
		virtual void commit_transaction(void) = 0;
		virtual void rollback_transaction(void) = 0;
		virtual bool is_valid(void) = 0;
		virtual bool find_all_datasets_count(int &nCount) = 0;
		virtual bool find_all_datasets(std::vector<DBStatDataset> &oList, int skip = 0, int count = 100) = 0;
		virtual bool find_all_datasets_ids(std::vector<IntType> &oList, int skip = 0, int count = 100) = 0;
		virtual bool find_dataset(DBStatDataset &cur) = 0;
		virtual bool maintains_dataset(DBStatDataset &cur, bool bCommit = true) = 0;
		virtual bool remove_dataset(const DBStatDataset &cur, bool bCommit = true) = 0;
		//
		virtual bool find_dataset_variables_types(const DBStatDataset &oSet, std::map<IntType, std::string> &oMap) = 0;
		virtual bool find_dataset_variables_count(const DBStatDataset &oSet, int &nCount) = 0;
		virtual bool find_dataset_variables_ids(const DBStatDataset &oSet, std::vector<IntType> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_dataset_variables(const DBStatDataset &oSet,
			std::vector<DBStatVariable> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_variable(DBStatVariable &cur) = 0;
		virtual bool maintains_variables(const std::vector<DBStatVariable> &oVars, bool bCommit = true,
			bool bRemove = false) = 0;
		virtual bool remove_variable(const DBStatVariable &oVar, bool bCommit = true) = 0;
		//
		virtual bool find_dataset_indivs_count(const DBStatDataset &oSet, int &nCount) = 0;
		virtual bool find_dataset_indivs_ids(const DBStatDataset &oSet, std::vector<IntType> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_dataset_indivs(const DBStatDataset &oSet,
			std::vector<DBStatIndiv> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_indiv(DBStatIndiv &cur) = 0;
		virtual bool maintains_indivs(const std::vector<DBStatIndiv> &oInds, bool bCommit = true,
			bool bRemove = false) = 0;
		virtual bool remove_indiv(const DBStatIndiv &oInd, bool bCommit = true) = 0;
		//
		virtual bool find_value(DBStatValue &cur) = 0;
		virtual bool maintains_values(const std::vector<DBStatValue> &oVals, bool bCommit = true,
			bool bRemove = false) = 0;
		virtual bool find_dataset_values_count(const DBStatDataset &oSet, int &nCount) = 0;
		virtual bool find_dataset_values(const DBStatDataset &oSet, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_variable_values(DBStatVariable &oVar, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_variable_distinct_values(DBStatVariable &oVar, std::vector<std::string> &oList,
			int skip = 0, int count = 100) = 0;
		virtual bool find_indiv_values(DBStatIndiv &oInd, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100) = 0;
	public:
		virtual ~IStoreHelper() {}
	};// class IStoreHelper
	//////////////////////////////////////////
}// namespace info
//////////////////////////////////
#endif // !__IDATABASEHELPER__H__
