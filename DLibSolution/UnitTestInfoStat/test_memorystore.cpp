#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include "memorystore_fixture.h"
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////
namespace UnitTestInfoStat
{
	using IDTYPE = unsigned long;
	using INTTYPE = unsigned long;
	using STRINGTYPE = std::string;
	using WEIGHTYPE = double;
	//
	using StoreFixture = MemoryStoreFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using IStoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using DatasetType = typename IStoreType::DatasetType;
	//
	using strings_vector = typename IStoreType::strings_vector;
	using ints_vector = typename IStoreType::ints_vector;
	using ints_string_map = typename IStoreType::ints_string_map;
	//
	using datasets_vector = typename IStoreType::datasets_vector;
	//
	using VariableType = typename IStoreType::VariableType;
	using variables_vector = typename IStoreType::variables_vector;
	//
	using IndivType = typename IStoreType::IndivType;
	using indivs_vector = typename IStoreType::indivs_vector;
	//
	using ValueType = typename IStoreType::ValueType;
	using values_vector = typename IStoreType::values_vector;
	//
	TEST_CLASS(UnitTestMemoryStore)
	{
		static unique_ptr<StoreFixture> st_m_man;
		//
		IStoreType *m_pstore;
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			StoreFixture *p = new StoreFixture();
			Assert::IsNotNull(p);
			st_m_man.reset(p);
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			st_m_man.reset();
		}

		TEST_METHOD_INITIALIZE(setUp)
		{
			StoreFixture *p = st_m_man.get();
			Assert::IsNotNull(p);
			this->m_pstore = p->get_store();
			Assert::IsNotNull(this->m_pstore);
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			this->m_pstore = nullptr;
		}// tearDown
	public:
		TEST_METHOD(TestMorta)
		{
			IStoreType *pStore = this->m_pstore;
			Assert::IsNotNull(pStore);
			STRINGTYPE sigle;
			InfoTestData::get_mortal_name(sigle);
			Assert::IsFalse(sigle.empty());
			DatasetType oSet(sigle);
			bool bRet = pStore->find_dataset(oSet);
			Assert::IsTrue(bRet);
			Assert::IsTrue(oSet.is_valid());
			size_t nbCols = 0;
			bRet = pStore->find_dataset_variables_count(oSet, nbCols);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nbCols > 0);
			size_t nbRows = 0;
			bRet = pStore->find_dataset_indivs_count(oSet, nbRows);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nbRows > 0);
			//
			size_t nChunk = 8;
			size_t skip = 0;
			do {
				variables_vector vars;
				bRet = pStore->find_dataset_variables(oSet, vars, skip, nChunk);
				Assert::IsTrue(bRet);
				size_t nc = vars.size();
				if (nc < 1) {
					break;
				}
				Assert::IsTrue(nc <= nChunk);
				for (auto &v : vars) {
					Assert::IsTrue(v.is_valid());
					size_t nv = 0;
					bRet = pStore->find_variable_values_count(v, nv);
					Assert::IsTrue(nv == nbRows);
					values_vector vals;
					bRet = pStore->find_variable_values(v, vals, 0, nv);
					Assert::IsTrue(vals.size() == nv);
					for (auto &vx : vals) {
						Assert::IsTrue(vx.is_valid());
					}// vx
				}// vars
				skip += nc;
			} while (skip < nbCols);
			Assert::IsTrue(skip == nbCols);
			//
			nChunk = 50;
			skip = 0;
			do {
				indivs_vector vars;
				bRet = pStore->find_dataset_indivs(oSet, vars, skip, nChunk);
				Assert::IsTrue(bRet);
				size_t nc = vars.size();
				if (nc < 1) {
					break;
				}
				Assert::IsTrue(nc <= nChunk);
				for (auto &v : vars) {
					Assert::IsTrue(v.is_valid());
					size_t nv = 0;
					bRet = pStore->find_indiv_values_count(v, nv);
					Assert::IsTrue(nv == nbCols);
					values_vector vals;
					bRet = pStore->find_indiv_values(v, vals, 0, nv);
					Assert::IsTrue(vals.size() == nv);
					for (auto &vx : vals) {
						Assert::IsTrue(vx.is_valid());
					}// vx
				}// vars
				skip += nc;
			} while (skip < nbRows);
			Assert::IsTrue(skip == nbRows);
		}// TestFillMortalData
		TEST_METHOD(TestConso)
		{
			IStoreType *pStore = this->m_pstore;
			Assert::IsNotNull(pStore);
			STRINGTYPE sigle;
			InfoTestData::get_conso_name(sigle);
			Assert::IsFalse(sigle.empty());
			DatasetType oSet(sigle);
			bool bRet = pStore->find_dataset(oSet);
			Assert::IsTrue(bRet);
			Assert::IsTrue(oSet.is_valid());
			size_t nbCols = 0;
			bRet = pStore->find_dataset_variables_count(oSet, nbCols);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nbCols > 0);
			size_t nbRows = 0;
			bRet = pStore->find_dataset_indivs_count(oSet, nbRows);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nbRows > 0);
			//
			size_t nChunk = 8;
			size_t skip = 0;
			do {
				variables_vector vars;
				bRet = pStore->find_dataset_variables(oSet, vars, skip, nChunk);
				Assert::IsTrue(bRet);
				size_t nc = vars.size();
				if (nc < 1) {
					break;
				}
				Assert::IsTrue(nc <= nChunk);
				for (auto &v : vars) {
					Assert::IsTrue(v.is_valid());
					size_t nv = 0;
					bRet = pStore->find_variable_values_count(v, nv);
					Assert::IsTrue(nv == nbRows);
					values_vector vals;
					bRet = pStore->find_variable_values(v, vals, 0, nv);
					Assert::IsTrue(vals.size() == nv);
					for (auto &vx : vals) {
						Assert::IsTrue(vx.is_valid());
					}// vx
				}// vars
				skip += nc;
			} while (skip < nbCols);
			Assert::IsTrue(skip == nbCols);
			//
			nChunk = 50;
			skip = 0;
			do {
				indivs_vector vars;
				bRet = pStore->find_dataset_indivs(oSet, vars, skip, nChunk);
				Assert::IsTrue(bRet);
				size_t nc = vars.size();
				if (nc < 1) {
					break;
				}
				Assert::IsTrue(nc <= nChunk);
				for (auto &v : vars) {
					Assert::IsTrue(v.is_valid());
					size_t nv = 0;
					bRet = pStore->find_indiv_values_count(v, nv);
					Assert::IsTrue(nv == nbCols);
					values_vector vals;
					bRet = pStore->find_indiv_values(v, vals, 0, nv);
					Assert::IsTrue(vals.size() == nv);
					for (auto &vx : vals) {
						Assert::IsTrue(vx.is_valid());
					}// vx
				}// vars
				skip += nc;
			} while (skip < nbRows);
			Assert::IsTrue(skip == nbRows);
		}// TestFillConsolData
		TEST_METHOD(TestTestData)
		{
			IStoreType *pStore = this->m_pstore;
			Assert::IsNotNull(pStore);
			STRINGTYPE sigle;
			InfoTestData::get_test_name(sigle);
			Assert::IsFalse(sigle.empty());
			DatasetType oSet(sigle);
			bool bRet = pStore->find_dataset(oSet);
			Assert::IsTrue(bRet);
			Assert::IsTrue(oSet.is_valid());
			size_t nbCols = 0;
			bRet = pStore->find_dataset_variables_count(oSet, nbCols);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nbCols > 0);
			size_t nbRows = 0;
			bRet = pStore->find_dataset_indivs_count(oSet, nbRows);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nbRows > 0);
			//
			size_t nChunk = 8;
			size_t skip = 0;
			do {
				variables_vector vars;
				bRet = pStore->find_dataset_variables(oSet, vars, skip, nChunk);
				Assert::IsTrue(bRet);
				size_t nc = vars.size();
				if (nc < 1) {
					break;
				}
				Assert::IsTrue(nc <= nChunk);
				for (auto &v : vars) {
					Assert::IsTrue(v.is_valid());
					size_t nv = 0;
					bRet = pStore->find_variable_values_count(v, nv);
					Assert::IsTrue(nv == nbRows);
					values_vector vals;
					bRet = pStore->find_variable_values(v, vals, 0, nv);
					Assert::IsTrue(vals.size() == nv);
					for (auto &vx : vals) {
						Assert::IsTrue(vx.is_valid());
					}// vx
				}// vars
				skip += nc;
			} while (skip < nbCols);
			Assert::IsTrue(skip == nbCols);
			//
			nChunk = 50;
			skip = 0;
			do {
				indivs_vector vars;
				bRet = pStore->find_dataset_indivs(oSet, vars, skip, nChunk);
				Assert::IsTrue(bRet);
				size_t nc = vars.size();
				if (nc < 1) {
					break;
				}
				Assert::IsTrue(nc <= nChunk);
				for (auto &v : vars) {
					Assert::IsTrue(v.is_valid());
					size_t nv = 0;
					bRet = pStore->find_indiv_values_count(v, nv);
					Assert::IsTrue(nv == nbCols);
					values_vector vals;
					bRet = pStore->find_indiv_values(v, vals, 0, nv);
					Assert::IsTrue(vals.size() == nv);
					for (auto &vx : vals) {
						Assert::IsTrue(vx.is_valid());
					}// vx
				}// vars
				skip += nc;
			} while (skip < nbRows);
			Assert::IsTrue(skip == nbRows);
		}// TestFillMortalData
	};
	unique_ptr<StoreFixture> UnitTestMemoryStore::st_m_man;
}