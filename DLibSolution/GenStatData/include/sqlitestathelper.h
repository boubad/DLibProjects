#pragma once
#ifndef __SQLITESTATHELPER_H__
#define __SQLITESTATHELPER_H__
////////////////////////////////
#include <dlib/sqlite.h>
/////////////////////////////////////
#include "istorehelper.h"
//////////////////////////////////
namespace info {
	///////////////////////////////
	class SQLiteStatHelper : public IStoreHelper {
	private:
		// no implementation
		SQLiteStatHelper(const SQLiteStatHelper &other);
		SQLiteStatHelper & operator=(const SQLiteStatHelper &other);
	private:
		bool m_intransaction;
		dlib::database m_base;
		//
		void check_schema(void);
		void read_dataset(dlib::statement &q, DBStatDataset &cur);
		void read_variable(dlib::statement &q, DBStatVariable &cur);
		void read_indiv(dlib::statement &q, DBStatIndiv &cur);
		void read_value(dlib::statement &q, DBStatValue &cur);
	public:
		static const std::string DEFAULT_DATABASE_NAME;
	public:
		SQLiteStatHelper(const std::string &sDatabaseName = DEFAULT_DATABASE_NAME);
		SQLiteStatHelper(const std::wstring &sDatabaseName);
		virtual ~SQLiteStatHelper();
		//
		virtual void begin_transaction(void);
		virtual void commit_transaction(void);
		virtual void rollback_transaction(void);
		virtual bool is_valid(void);
		virtual bool find_all_datasets_count(int &nCount);
		virtual bool find_all_datasets(std::vector<DBStatDataset> &oList, int skip = 0, int count = 100);
		virtual bool find_all_datasets_ids(std::vector<IntType> &oList, int skip = 0, int count = 100);
		virtual bool find_dataset(DBStatDataset &cur);
		virtual bool maintains_dataset(DBStatDataset &cur,bool bCommit = true);
		virtual bool remove_dataset(const DBStatDataset &cur, bool bCommit = true);
		//
		virtual bool find_dataset_variables_types(const DBStatDataset &oSet, std::map<IntType, std::string> &oMap);
		virtual bool find_dataset_variables_count(const DBStatDataset &oSet, int &nCount);
		virtual bool find_dataset_variables_ids(const DBStatDataset &oSet, std::vector<IntType> &oList,
			int skip = 0, int count = 100);
		virtual bool find_dataset_variables(const DBStatDataset &oSet,
			std::vector<DBStatVariable> &oList,
			int skip = 0, int count = 100);
		virtual bool find_variable(DBStatVariable &cur);
		virtual bool maintains_variables(const std::vector<DBStatVariable> &oVars, bool bCommit = true,
			bool bRemove = false);
		virtual bool remove_variable(const DBStatVariable &oVar, bool bCommit = true);
		//
		virtual bool find_dataset_indivs_count(const DBStatDataset &oSet, int &nCount);
		virtual bool find_dataset_indivs_ids(const DBStatDataset &oSet, std::vector<IntType> &oList,
			int skip = 0, int count = 100);
		virtual bool find_dataset_indivs(const DBStatDataset &oSet,
			std::vector<DBStatIndiv> &oList,
			int skip = 0, int count = 100);
		virtual bool find_indiv(DBStatIndiv &cur);
		virtual bool maintains_indivs(const std::vector<DBStatIndiv> &oInds, bool bCommit = true,
			bool bRemove = false);
		virtual bool remove_indiv(const DBStatIndiv &oInd, bool bCommit = true);
		//
		virtual bool find_value(DBStatValue &cur);
		virtual bool maintains_values(const std::vector<DBStatValue> &oVals, bool bCommit = true,
			bool bRemove = false);
		virtual bool find_dataset_values_count(const DBStatDataset &oSet, int &nCount);
		virtual bool find_dataset_values(const DBStatDataset &oSet, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100);
		virtual bool find_variable_values(DBStatVariable &oVar, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100);
		virtual bool find_variable_distinct_values(DBStatVariable &oVar, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100);
		virtual bool find_indiv_values(DBStatIndiv &oInd, std::vector<DBStatValue> &oList,
			int skip = 0, int count = 100);
	};// class SQLiteStatHelper
	/////////////////////////////////
}// namespace info
////////////////////////////////
#endif // !__DBSTATHELPER_H
