#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <sqlitestathelper.h>
#include <storeindivprovider.h>
/////////////////////////////////
#include "infotestdata.h"
/////////////////////////////////
#include <memory>
#include <algorithm>
///////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;

namespace UnitTestDatalib
{		
	TEST_CLASS(UnitTestSQLiteStatHelper)
	{
	protected:
		std::unique_ptr<SQLiteStatHelper> m_man;
		DBStatDataset m_oset;
		//
		static size_t m_st_nbcols;
		static size_t m_st_nbrows;
		static std::string m_st_filename;
		static std::string m_st_name;
		static std::vector < std::string> m_st_colnames;
		static std::vector < std::string> m_st_rownames;
		static std::vector < int> m_st_data;
		static DBStatDataset m_st_oset;
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			InfoTestData::get_mortal_data(m_st_name, m_st_nbrows, m_st_nbcols, m_st_data, m_st_rownames, m_st_colnames);
			Assert::IsFalse(m_st_name.empty());
			Assert::IsTrue(m_st_nbrows > 2);
			Assert::IsTrue(m_st_nbcols > 2);
			Assert::IsFalse(m_st_name.empty());
			Assert::IsTrue(m_st_colnames.size() >= m_st_nbcols);
			Assert::IsTrue(m_st_rownames.size() >= m_st_nbrows);
			Assert::IsTrue(m_st_data.size() >= (size_t)(m_st_nbcols * m_st_nbrows));
			//
			DBStatDataset oSet(m_st_name);
			m_st_oset = oSet;
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			m_st_data.clear();
			m_st_colnames.clear();
			m_st_rownames.clear();
		}
		TEST_METHOD_INITIALIZE(Setup)
		{
			bool bRet = false;
			m_man.reset(new SQLiteStatHelper(m_st_filename));
			IStoreHelper *p = m_man.get();
			Assert::IsNotNull(p);
			Assert::IsTrue(p->is_valid());
			m_oset = m_st_oset;
			if (!p->find_dataset(m_oset)) {
				bRet = p->maintains_dataset(m_oset);
				Assert::IsTrue(bRet);
			}
			Assert::IsTrue(this->m_oset.id() != 0);
			variables_vector oVars;
			for (size_t i = 0; i < m_st_nbcols; ++i) {
				std::string sigle = m_st_colnames[i];
				DBStatVariable v(this->m_oset, sigle);
				if (!p->find_variable(v)) {
					oVars.push_back(v);
				}
			}// i
			if (!oVars.empty()) {
				bRet = p->maintains_variables(oVars);
				Assert::IsTrue(bRet);
			}
			indivs_vector oInds;
			for (size_t i = 0; i < m_st_nbrows; ++i) {
				std::string sigle = m_st_rownames[i];
				DBStatIndiv v(this->m_oset, sigle);
				if (!p->find_indiv(v)) {
					oInds.push_back(v);
				}
			}// i
			if (!oInds.empty()) {
				bRet = p->maintains_indivs(oInds);
				Assert::IsTrue(bRet);
			}
			oInds.clear();
			bRet = p->find_dataset_indivs(this->m_oset, oInds, 0, m_st_nbrows);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbrows, oInds.size());
			oVars.clear();
			bRet = p->find_dataset_variables(this->m_oset, oVars, 0, m_st_nbcols);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbcols, oVars.size());
			//
			boost::container::flat_map<std::string, DBStatVariable *> pVars;
			std::for_each(m_st_colnames.begin(), m_st_colnames.end(), [&](const std::string &s) {
				std::string sigle = s;
				std::string rsigle;
				DBStatVariable ovar(this->m_oset, sigle);
				ovar.get_sigle(rsigle);
				DBStatVariable *p = nullptr;
				for (size_t i = 0; i < oVars.size(); ++i) {
					DBStatVariable &vv = oVars[i];
					std::string sx;
					vv.get_sigle(sx);
					if (sx == rsigle) {
						p = &vv;
						break;
					}
				}// i
				Assert::IsNotNull(p);
				pVars[sigle] = p;
			});
			boost::container::flat_map<std::string, DBStatIndiv *> pInds;
			std::for_each(m_st_rownames.begin(), m_st_rownames.end(), [&](const std::string &s) {
				std::string sigle = s;
				std::string rsigle;
				DBStatIndiv ovar(this->m_oset, sigle);
				ovar.get_sigle(rsigle);
				DBStatIndiv *p = nullptr;
				for (size_t i = 0; i < oInds.size(); ++i) {
					DBStatIndiv &vv = oInds[i];
					std::string sx;
					vv.get_sigle(sx);
					if (sx == rsigle) {
						p = &vv;
						break;
					}
				}// i
				Assert::IsNotNull(p);
				pInds[sigle] = p;
			});
			values_vector oVals;
			for (size_t i = 0; i < m_st_nbrows; ++i) {
				std::string sigleind = m_st_rownames[i];
				DBStatIndiv *pInd = pInds[sigleind];
				Assert::IsNotNull(pInd);
				for (size_t j = 0; j < m_st_nbcols; ++j) {
					std::string siglevar = m_st_colnames[j];
					DBStatVariable *pVar = pVars[siglevar];
					Assert::IsNotNull(pVar);
					DBStatValue val(*pVar, *pInd);
					if (!p->find_value(val)) {
						float f = (float)m_st_data[i * m_st_nbcols + j];
						DbValue vv(f);
						val.set_value(vv);
						oVals.push_back(val);
					}
				}// j
			}// i
			if (!oVals.empty()) {
				bRet = p->maintains_values(oVals);
				Assert::IsTrue(bRet);
			}
		}// Setup
		TEST_METHOD_CLEANUP(Teardown)
		{
			m_man.reset();
		}// Teardown
		TEST_METHOD(TestDBDatasets)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			//
			int count = 0;
			bool bRet = pMan->find_all_datasets_count(count);
			Assert::IsTrue(bRet);
			Assert::IsTrue(count > 0);
			datasets_vector oSets;
			bRet = pMan->find_all_datasets(oSets, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oSets.size());
			//
			ints_vector oIds;
			bRet = pMan->find_all_datasets_ids(oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oIds.size());
			//
			std::string sigle;
			this->m_oset.get_sigle(sigle);
			DBStatDataset xSet(sigle);
			bRet = pMan->find_dataset(xSet);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_dataset(xSet);
			Assert::IsTrue(bRet);
			bRet = pMan->find_dataset(xSet);
			Assert::IsFalse(bRet);
		}//TestDBDatasets
		TEST_METHOD(TestDBVariables)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			DBStatDataset &oSet = this->m_oset;
			Assert::IsTrue(oSet.id() != 0);
			//
			int count = 0;
			bool bRet = pMan->find_dataset_variables_count(oSet, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbcols, (size_t)count);
			//
			ints_vector oIds;
			bRet = pMan->find_dataset_variables_ids(oSet, oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oIds.size());
			//
			variables_vector oVars;
			bRet = pMan->find_dataset_variables(oSet, oVars, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oVars.size());
			//
			inttype_string_map oMap;
			bRet = pMan->find_dataset_variables_types(oSet, oMap);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oMap.size());
			//
			DBStatVariable &vv = oVars[0];
			bRet = pMan->find_variable(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_variable(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->find_variable(vv);
			Assert::IsFalse(bRet);
		}//TestDBVariables
		TEST_METHOD(TestDBIndivs)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			DBStatDataset &oSet = this->m_oset;
			Assert::IsTrue(oSet.id() != 0);
			//
			int count = 0;
			bool bRet = pMan->find_dataset_indivs_count(oSet, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbrows, (size_t)count);
			//
			ints_vector oIds;
			bRet = pMan->find_dataset_indivs_ids(oSet, oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oIds.size());
			//
			indivs_vector oInds;
			bRet = pMan->find_dataset_indivs(oSet, oInds, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oInds.size());
			//
			DBStatIndiv &vv = oInds[0];
			bRet = pMan->find_indiv(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_indiv(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->find_indiv(vv);
			Assert::IsFalse(bRet);
		}//TestDBIndivs
		TEST_METHOD(TestDBValues)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			DBStatDataset &oSet = this->m_oset;
			Assert::IsTrue(oSet.id() != 0);
			//
			int count = 0;
			int nTotal = (int)(m_st_nbcols * m_st_nbrows);
			bool bRet = pMan->find_dataset_values_count(oSet, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual(nTotal, count);
			//
			values_vector oVals;
			bRet = pMan->find_dataset_values(oSet, oVals, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oVals.size());
			//
			variables_vector oListVars;
			bRet = pMan->find_dataset_variables(oSet, oListVars, 0, 1);
			Assert::IsTrue(bRet);
			Assert::IsFalse(oListVars.empty());
			DBStatVariable &vv = oListVars[0];
			values_vector vals;
			bRet = pMan->find_variable_values(vv, vals, 0, 1000);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbrows, vals.size());
			strings_vector ss;
			bRet = pMan->find_variable_distinct_values(vv, ss);
			Assert::IsTrue(bRet);
			//
			indivs_vector oListInds;
			bRet = pMan->find_dataset_indivs(oSet, oListInds, 0, 1);
			Assert::IsTrue(bRet);
			Assert::IsFalse(oListInds.empty());
			DBStatIndiv &vi = oListInds[0];
			vals.clear();
			bRet = pMan->find_indiv_values(vi, vals, 0, 1000);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbcols, vals.size());
			//
			size_t nb = oVals.size();
			DbValue vempty;
			for (size_t i = 0; i < nb; ++i) {
				DBStatValue &v = oVals[i];
				int nx = (int)i % 2;
				if (nx == 0) {
					v.set_value(vempty);
				}
			}// i
			bRet = pMan->maintains_values(oVals);
			Assert::IsTrue(bRet);
			int nz = 0;
			bRet = pMan->find_dataset_values_count(oSet, nz);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nz < count);
		}//TestDBValues
		TEST_METHOD(TestStoreIndivProvider)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			DBStatDataset &oSet = this->m_oset;
			Assert::IsTrue(oSet.id() != 0);
			//
			StoreIndivProvider oProvider(pMan, oSet);
			Assert::IsTrue(oProvider.is_valid());
			//
			size_t nc = 0;
			bool bRet = oProvider.indivs_count(nc);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbrows, nc);
			//
			variables_map vars;
			bRet = oProvider.get_variables_map(vars);
			Assert::IsTrue(bRet);
			Assert::AreEqual(m_st_nbcols, vars.size());
			//
			for (size_t i = 0; i < nc; ++i) {
				Indiv oInd;
				bRet = oProvider.find_indiv_at(i, oInd);
				Assert::IsTrue(bRet);
				IntType aIndex = oInd.id();
				Assert::IsTrue(aIndex != 0);
			}// i
		}//TestStoreIndivProvider
		TEST_METHOD(TestSerialStoreIndivProvider)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			DBStatDataset &oSet = this->m_oset;
			Assert::IsTrue(oSet.id() != 0);
			//
			SerialStoreIndivProvider oProvider(pMan, oSet);
			Assert::IsTrue(oProvider.is_valid());
			//
			bool bRet = oProvider.reset();
			Assert::IsTrue(bRet);
			size_t nc = 0;
			do {
				Indiv oInd1;
				bRet = oProvider.next(oInd1);
				if (!bRet) {
					Assert::AreEqual(m_st_nbrows, nc);
					Assert::IsTrue(oInd1.id() == 0);
					break;
				}
				++nc;
				IntType aIndex1 = oInd1.id();
				Assert::IsTrue(aIndex1 != 0);
				Indiv oInd2;
				bRet = oProvider.next(oInd2);
				if (!bRet) {
					Assert::AreEqual(m_st_nbrows, nc);
					Assert::IsTrue(oInd2.id() == 0);
					break;
				}
				++nc;
				IntType aIndex2 = oInd1.id();
				Assert::IsTrue(aIndex2 != 0);
				double d1 = oInd1.distance(oInd2);
				Assert::IsTrue(d1 >= 0);
				double d2 = oInd2.distance(oInd1);
				Assert::IsTrue(d2 >= 0);
				Assert::AreEqual(d1, d2);
			} while (true);
		}//TestSerialStoreIndivProvider
	};
	size_t UnitTestSQLiteStatHelper::m_st_nbcols = 0;
	size_t UnitTestSQLiteStatHelper::m_st_nbrows = 0;
	std::string  UnitTestSQLiteStatHelper::m_st_filename;
	std::string UnitTestSQLiteStatHelper::m_st_name;
	std::vector < std::string> UnitTestSQLiteStatHelper::m_st_colnames;
	std::vector < std::string> UnitTestSQLiteStatHelper::m_st_rownames;
	std::vector < int> UnitTestSQLiteStatHelper::m_st_data;
	DBStatDataset UnitTestSQLiteStatHelper::m_st_oset;
}