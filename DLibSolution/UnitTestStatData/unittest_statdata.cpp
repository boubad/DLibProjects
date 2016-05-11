#include "stdafx.h"
#include "CppUnitTest.h"
////////////////////////////////////////////
#include <sqlitestathelper.h>
#include <storeindivprovider.h>
#include <statinfo.h>
#include <numericindivprovider.h>
#include <crititem.h>
#include <indivcluster.h>
///////////////////////////////////
#include "infotestdata.h"
///////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
//////////////////////////////
#include <boost/foreach.hpp>
///////////////////////////////////
namespace UnitTestStatData
{
	/////////////////////////////////////
	static void write_point(const DbValueMap &oMap,std::string &s) {
		std::stringstream os;
		std::for_each(oMap.begin(), oMap.end(), [&](const std::pair<IntType, DbValue> &p) {
			IntType key = p.first;
			const DbValue &v = p.second;
			std::string ss;
			v.string_value(ss);
			os << "(" << key << "," << ss << "), ";
		});
		s = os.str();
	}
	//////////////////////////////////////
	TEST_CLASS(UnitTestSQLiteStatHelper)
	{
		std::unique_ptr<SQLiteStatHelper> m_man;
		DBStatDataset m_oset;
		//
		size_t m_nbcols;
		size_t m_nbrows;
		std::string m_filename;
		std::string m_name;
		std::vector < std::string> m_colnames;
		std::vector < std::string> m_rownames;
		std::vector < int> m_data;
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
			InfoTestData::get_mortal_data(m_name, m_nbrows, m_nbcols, m_data, m_rownames, m_colnames);
			Assert::IsFalse(m_name.empty());
			Assert::IsTrue(m_nbrows > 2);
			Assert::IsTrue(m_nbcols > 2);
			Assert::IsFalse(m_name.empty());
			Assert::IsTrue(m_colnames.size() >= m_nbcols);
			Assert::IsTrue(m_rownames.size() >= m_nbrows);
			Assert::IsTrue(m_data.size() >= (size_t)(m_nbcols * m_nbrows));
			//
			bool bRet = false;
			this->m_man = std::make_unique < SQLiteStatHelper >(this->m_filename);
			IStoreHelper *p = this->m_man.get();
			Assert::IsTrue(p != nullptr);
			Assert::IsTrue(p->is_valid());
			DBStatDataset & oSet = this->m_oset;
			oSet.set_sigle(m_name);
			if (!p->find_dataset(oSet)) {
				bRet = p->maintains_dataset(oSet);
				Assert::IsTrue(bRet);
			}
			Assert::IsTrue(oSet.id() != 0);
			variables_vector oVars;
			for (size_t i = 0; i < m_nbcols; ++i) {
				std::string sigle = m_colnames[i];
				DBStatVariable v(oSet, sigle);
				if (!p->find_variable(v)) {
					oVars.push_back(v);
				}
			} // i
			if (!oVars.empty()) {
				bRet = p->maintains_variables(oVars);
				Assert::IsTrue(bRet);
			}
			indivs_vector oInds;
			for (size_t i = 0; i < m_nbrows; ++i) {
				std::string sigle = m_rownames[i];
				DBStatIndiv v(oSet, sigle);
				if (!p->find_indiv(v)) {
					oInds.push_back(v);
				}
			} // i
			if (!oInds.empty()) {
				bRet = p->maintains_indivs(oInds);
				Assert::IsTrue(bRet);
			}
			oInds.clear();
			bRet = p->find_dataset_indivs(oSet, oInds, 0, m_nbrows);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbrows == oInds.size());
			oVars.clear();
			bRet = p->find_dataset_variables(oSet, oVars, 0, m_nbcols);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbcols == oVars.size());
			//
			std::map<std::string, DBStatVariable *> pVars;
			BOOST_FOREACH(const std::string &s, this->m_colnames) {
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
				} // i
				Assert::IsTrue(p != nullptr);
				pVars[sigle] = p;
			}
			std::map<std::string, DBStatIndiv *> pInds;
			BOOST_FOREACH(const std::string &s, this->m_rownames) {
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
				} // i
				Assert::IsTrue(p != nullptr);
				pInds[sigle] = p;
			}
			values_vector oVals;
			for (size_t i = 0; i < m_nbrows; ++i) {
				std::string sigleind = m_rownames[i];
				DBStatIndiv *pInd = pInds[sigleind];
				Assert::IsTrue(pInd != nullptr);
				for (size_t j = 0; j < m_nbcols; ++j) {
					std::string siglevar = m_colnames[j];
					DBStatVariable *pVar = pVars[siglevar];
					Assert::IsTrue(pVar != nullptr);
					DBStatValue val(*pVar, *pInd);
					if (!p->find_value(val)) {
						float f = (float)m_data[i * m_nbcols + j];
						DbValue vv(f);
						val.set_value(vv);
						oVals.push_back(val);
					}
				} // j
			} // i
			if (!oVals.empty()) {
				bRet = p->maintains_values(oVals);
				Assert::IsTrue(bRet);
			}
		}
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_man.reset();
			m_data.clear();
			m_colnames.clear();
			m_rownames.clear();
		}

	public:
		TEST_METHOD(testDatasets) {
			IStoreHelper *pMan = m_man.get();
			//
			int count = 0;
			bool bRet = pMan->find_all_datasets_count(count);
			Assert::IsTrue(bRet);
			Assert::IsTrue(count > 0);
			datasets_vector oSets;
			bRet = pMan->find_all_datasets(oSets, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oSets.size());
			//
			ints_vector oIds;
			bRet = pMan->find_all_datasets_ids(oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oIds.size());
			//
			std::string sigle;
			this->m_oset.get_sigle(sigle);
			DBStatDataset xSet(sigle);
			bRet = pMan->find_dataset(xSet);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_dataset(xSet);
			Assert::IsTrue(bRet);
			bRet = pMan->find_dataset(xSet);
			Assert::IsTrue(!bRet);
		} // testDatasets
		TEST_METHOD(testVariables) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			int count = 0;
			bool bRet = pMan->find_dataset_variables_count(oSet, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbcols == (size_t)count);
			//
			ints_vector oIds;
			bRet = pMan->find_dataset_variables_ids(oSet, oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oIds.size());
			//
			variables_vector oVars;
			bRet = pMan->find_dataset_variables(oSet, oVars, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oVars.size());
			//
			inttype_string_map oMap;
			bRet = pMan->find_dataset_variables_types(oSet, oMap);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oMap.size());
			//
			DBStatVariable &vv = oVars[0];
			bRet = pMan->find_variable(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_variable(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->find_variable(vv);
			Assert::IsTrue(!bRet);
		} // testVariables
		TEST_METHOD(testIndivs) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			int count = 0;
			bool bRet = pMan->find_dataset_indivs_count(oSet, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbrows == (size_t)count);
			//
			ints_vector oIds;
			bRet = pMan->find_dataset_indivs_ids(oSet, oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oIds.size());
			//
			indivs_vector oInds;
			bRet = pMan->find_dataset_indivs(oSet, oInds, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oInds.size());
			//
			DBStatIndiv &vv = oInds[0];
			bRet = pMan->find_indiv(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_indiv(vv);
			Assert::IsTrue(bRet);
			bRet = pMan->find_indiv(vv);
			Assert::IsTrue(!bRet);
		} // testIndivs
		TEST_METHOD(testValues) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			int count = 0;
			int nTotal = (int)(m_nbcols * m_nbrows);
			bool bRet = pMan->find_dataset_values_count(oSet, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nTotal == count);
			//
			values_vector oVals;
			bRet = pMan->find_dataset_values(oSet, oVals, 0, count);
			Assert::IsTrue(bRet);
			Assert::IsTrue((size_t)count == oVals.size());
			//
			variables_vector oListVars;
			bRet = pMan->find_dataset_variables(oSet, oListVars, 0, 1);
			Assert::IsTrue(bRet);
			Assert::IsTrue(!oListVars.empty());
			DBStatVariable &vv = oListVars[0];
			values_vector vals;
			bRet = pMan->find_variable_values(vv, vals, 0, 1000);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbrows == vals.size());
			strings_vector ss;
			bRet = pMan->find_variable_distinct_values(vv, ss);
			Assert::IsTrue(bRet);
			//
			indivs_vector oListInds;
			bRet = pMan->find_dataset_indivs(oSet, oListInds, 0, 1);
			Assert::IsTrue(bRet);
			Assert::IsTrue(!oListInds.empty());
			DBStatIndiv &vi = oListInds[0];
			vals.clear();
			bRet = pMan->find_indiv_values(vi, vals, 0, 1000);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbcols == vals.size());
			//
			size_t nb = oVals.size();
			DbValue vempty;
			for (size_t i = 0; i < nb; ++i) {
				DBStatValue &v = oVals[i];
				int nx = (int)i % 2;
				if (nx == 0) {
					v.set_value(vempty);
				}
			} // i
			bRet = pMan->maintains_values(oVals);
			Assert::IsTrue(bRet);
			int nz = 0;
			bRet = pMan->find_dataset_values_count(oSet, nz);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nz < count);
		} // testValues
		TEST_METHOD(testIndivProvider) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			StoreIndivProvider oProvider(pMan, oSet);
			Assert::IsTrue(oProvider.is_valid());
			//
			size_t nc = 0;
			bool bRet = oProvider.indivs_count(nc);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbrows == nc);
			//
			variables_map vars;
			bRet = oProvider.get_variables_map(vars);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbcols == vars.size());
			//
			for (size_t i = 0; i < nc; ++i) {
				Indiv oInd;
				bRet = oProvider.find_indiv_at(i, oInd);
				Assert::IsTrue(bRet);
				IntType aIndex = oInd.id();
				Assert::IsTrue(aIndex != 0);
			} // i
		} // testIndivProvider
		TEST_METHOD(testSerialIndivProvider) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
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
					Assert::IsTrue(m_nbrows == nc);
					Assert::IsTrue(oInd1.id() == 0);
					break;
				}
				++nc;
				IntType aIndex1 = oInd1.id();
				Assert::IsTrue(aIndex1 != 0);
				Indiv oInd2;
				bRet = oProvider.next(oInd2);
				if (!bRet) {
					Assert::IsTrue(m_nbrows == nc);
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
				Assert::IsTrue(d1 == d2);
			} while (true);
		} //testSerialIndivProvider
		TEST_METHOD(testStatInfo) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			SerialStoreIndivProvider oProvider(pMan, oSet);
			Assert::IsTrue(oProvider.is_valid());
			//
			statinfos_map oRes;
			size_t n = info_global_compute_stats(&oProvider, oRes);
			Assert::IsTrue(n == this->m_nbcols);
			//
		} //testStatInfo
		TEST_METHOD(testNumIndivProvider) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			StoreIndivProvider oProviderBase(pMan, oSet);
			Assert::IsTrue(oProviderBase.is_valid());
			NumericIndivProvider oProvider(&oProviderBase);
			Assert::IsTrue(oProvider.is_valid());
			//
			size_t nc = 0;
			bool bRet = oProvider.indivs_count(nc);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbrows == nc);
			//
			variables_map vars;
			bRet = oProvider.get_variables_map(vars);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbcols == vars.size());
			ints_vector oIds;
			bRet = oProvider.get_variables_ids(oIds);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbcols == oIds.size());
			for (size_t i = 0; i < m_nbcols; ++i) {
				IntType key = oIds[i];
				StatInfo oInfo;
				bRet = oProvider.get_statinfo(key, oInfo);
				Assert::IsTrue(bRet);
			} // i
			  //
			for (size_t i = 0; i < nc; ++i) {
				Indiv oInd;
				bRet = oProvider.find_indiv_at(i, oInd);
				Assert::IsTrue(bRet);
				IntType aIndex = oInd.id();
				Assert::IsTrue(aIndex != 0);
				doubles_vector vv1, vv2;
				bRet = oProvider.find_indiv(aIndex, vv1);
				Assert::IsTrue(aIndex != 0);
				bRet = oProvider.find_indiv_at(i, aIndex, vv1);
				Assert::IsTrue(aIndex != 0);
			} // i
		} //testNumIndivProvider
		TEST_METHOD(testComputeDistances) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			StoreIndivProvider oProviderBase(pMan, oSet);
			Assert::IsTrue(oProviderBase.is_valid());
			NumericIndivProvider oProvider(&oProviderBase);
			Assert::IsTrue(oProvider.is_valid());
			//
			size_t nc = 0;
			bool bRet = oProvider.indivs_count(nc);
			Assert::IsTrue(bRet);
			Assert::IsTrue(m_nbrows == nc);
			//
			IndivDistanceMap oDistances;
			info_global_compute_distances(&oProvider, oDistances);
			//
			const ints_set &oSetIndivs = oDistances.indexes();
			Assert::IsTrue(oSetIndivs.size() == nc);
			//
			for (size_t i = 0; i < nc; ++i) {
				Indiv oInd1;
				bRet = oProvider.find_indiv_at(i, oInd1);
				Assert::IsTrue(bRet);
				IntType aIndex1 = oInd1.id();
				Assert::IsTrue(aIndex1 != 0);
				for (size_t j = 0; j < i; ++j) {
					Indiv oInd2;
					bRet = oProvider.find_indiv_at(j, oInd2);
					Assert::IsTrue(bRet);
					IntType aIndex2 = oInd2.id();
					Assert::IsTrue(aIndex2 != 0);
					double dRes = 0;
					bRet = oDistances.get(aIndex1, aIndex2, dRes);
					Assert::IsTrue(bRet);
					Assert::IsTrue(dRes > 0);
				} // j
			} // i
		} //testComputeDistances
		TEST_METHOD(testIndivCluster) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			SerialStoreIndivProvider oProvider(pMan, oSet);
			Assert::IsTrue(oProvider.is_valid());
			//
			IntType aIndex = 100;
			IndivCluster oCluster(&oProvider, aIndex);
			oProvider.reset();
			do {
				Indiv oInd;
				if (!oProvider.next(oInd)) {
					break;
				}
				std::string ss;
				write_point(oInd.data(), ss);
				oCluster.add(oInd);
				Logger::WriteMessage(ss.c_str());
			} while (true);
			Logger::WriteMessage("===================");
			const ints_deque &members = oCluster.members();
			size_t nc = members.size();
			Assert::AreEqual(m_nbrows, members.size());
			oCluster.update_center();
			std::string ss;
			write_point(oCluster.center(), ss);
			Logger::WriteMessage(ss.c_str());
		} // testIndivCluster
		TEST_METHOD(testClusterizeKMeans) {
			IStoreHelper *pMan = m_man.get();
			DBStatDataset &oSet = this->m_oset;
			//
			StoreIndivProvider oProvider(pMan, oSet);
			Assert::IsTrue(oProvider.is_valid());
			//
			const size_t NB_RUNS = 50;
			InfoCritItems oInfos;
			//
			size_t nbClusters = 5;
			for (int k = 0; k < NB_RUNS; ++k) {
				indivclusters_vector oClusters;
				size_t nbIters = info_global_clusterize_kmeans(&oProvider, nbClusters, oClusters);
				Assert::IsTrue(nbIters > 0);
				info_global_add_clusterize_result(oClusters, oInfos);
			}// k
			const ints_set &iSet = oInfos.indexes();
			for (auto it =iSet.begin(); it != iSet.end(); ++it) {
				IntType aIndex1 = *it;
				for (auto jt = iSet.begin(); jt != it; ++jt) {
					IntType aIndex2 = *jt;
					double v = 0;
					bool b = oInfos.get(aIndex1, aIndex2, v);
					if (b) {
						double x = v / NB_RUNS;
						std::stringstream os;
						os << aIndex1 << " -->  " << aIndex2 << "\t" << x;
						std::string ss = os.str();
						Logger::WriteMessage(ss.c_str());
					}// b
				}// jt
			}// it
		} // testClusterizeKMeans
	};// clasds UnitTestSQLiteStatHelper
	//////////////////////
}