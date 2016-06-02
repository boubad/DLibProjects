#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <distancemap.h>
/////////////////////////////
#include "mytestfixture.h"
#include "mytestvariablefixture.h"
////////////////////////////////////
#include "global_defs.h"
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////
namespace UnitTestInfoStat
{
	///////////////////////////////////
	using MyFixture = MyTestFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using MyVariableFixture = MyTestVariableFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	/////////////////////////////////////
	using IndivType = typename MyFixture::IndivType;
	using DataMap = typename MyFixture::DataMap;
	using IndivTypePtr = typename MyFixture::IndivTypePtr;
	using SourceType = typename MyFixture::SourceType;
	using ints_doubles_map = std::map<IDTYPE, double>;
	using strings_vector = std::vector<STRINGTYPE>;
	using data_vector = std::vector<DATATYPE>;
	using DistanceMapType = DistanceMap<IDTYPE, DISTANCETYPE>;
	using ints_vector = std::vector<IDTYPE>;
	/////////////////////////////////
	//
	TEST_CLASS(UnitTestIndivProvider)
	{
		static unique_ptr<MyFixture> st_m_fixture;
		static unique_ptr<MyVariableFixture> st_m_variablefixture;
		//
		SourceType *m_pMortalSource;
		SourceType *m_pConsoSource;
		SourceType *m_pTestSource;
		SourceType *m_pMortalVariable;
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			st_m_fixture.reset(new MyFixture());
			MyFixture *p = st_m_fixture.get();
			Assert::IsNotNull(p);
			st_m_variablefixture.reset(new MyVariableFixture());
			MyVariableFixture *pv = st_m_variablefixture.get();
			Assert::IsNotNull(pv);
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			st_m_fixture.reset();
		}

		TEST_METHOD_INITIALIZE(setUp)
		{
			MyFixture *p = st_m_fixture.get();
			Assert::IsNotNull(p);
			m_pMortalSource = p->mortal_source();
			m_pConsoSource = p->conso_source();
			m_pTestSource = p->test_source();
			//
			MyVariableFixture *pv = st_m_variablefixture.get();
			Assert::IsNotNull(pv);
			m_pMortalVariable = pv->mortal_source();
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_pMortalSource = nullptr;
			m_pConsoSource = nullptr;
			m_pTestSource = nullptr;
			m_pMortalVariable = nullptr;
		}// tearDown
	public:
		TEST_METHOD(TestIndivProvider)
		{
			//
			size_t nRows = 0, nCols = 0;
			data_vector data;
			strings_vector varNames, indsNames;
			STRINGTYPE name;
			//
			InfoTestData::get_mortal_data(name, nRows, nCols, data, indsNames, varNames);
			//
			SourceType *pProvider = m_pMortalSource;
			Assert::IsNotNull(pProvider);
			pProvider->reset();
			size_t nCount = pProvider->count();
			Assert::AreEqual(nRows, nCount);
			Assert::IsTrue(nCount > 0);
			ints_doubles_map weights;
			pProvider->weights(weights);
			Assert::AreEqual(nCols, weights.size());
			//STRINGSTREAM os;
			for (size_t i = 0; i < nCount; ++i) {
				IndivTypePtr oInd = pProvider->get(i);
				IndivType *pIndiv = oInd.get();
				Assert::IsNotNull(pIndiv);
				IDTYPE aIndex = pIndiv->id();
				Assert::IsTrue(aIndex != 0);
				IndivTypePtr xInd = pProvider->find(aIndex);
				IndivType *pIndiv2 = xInd.get();
				Assert::IsNotNull(pIndiv2);
				const DataMap &oCenter = pIndiv->center();
				Assert::AreEqual(nCols, oCenter.size());
				Assert::IsTrue(pIndiv->has_numeric_fields());
			//	os << *pIndiv << std::endl;
			}//i
			pProvider->reset();
			size_t nc = 0;
			do {
				IndivTypePtr oInd = pProvider->next();
				IndivType *pIndiv = oInd.get();
				if (pIndiv == nullptr) {
					break;
				}
				++nc;
			} while (true);
			Assert::IsTrue(nc == nCount);
		//	STRINGTYPE ss = os.str();
		//	Logger::WriteMessage(ss.c_str());
		}// TestFillMortalData
		TEST_METHOD(TestVariableProvider)
		{
			//
			size_t nRows = 0, nCols = 0;
			data_vector data;
			strings_vector varNames, indsNames;
			STRINGTYPE name;
			//
			InfoTestData::get_mortal_data(name, nRows, nCols, data, indsNames, varNames);
			//
			SourceType *pProvider = m_pMortalVariable;
			Assert::IsNotNull(pProvider);
			pProvider->reset();
			size_t nCount = pProvider->count();
			Assert::AreEqual(nCols, nCount);
			Assert::IsTrue(nCount > 0);
			//STRINGSTREAM os;
			for (size_t i = 0; i < nCount; ++i) {
				IndivTypePtr oInd = pProvider->get(i);
				IndivType *pIndiv = oInd.get();
				Assert::IsNotNull(pIndiv);
				IDTYPE aIndex = pIndiv->id();
				Assert::IsTrue(aIndex != 0);
				IndivTypePtr xInd = pProvider->find(aIndex);
				IndivType *pIndiv2 = xInd.get();
				Assert::IsNotNull(pIndiv2);
				const DataMap &oCenter = pIndiv->center();
				Assert::AreEqual(nRows, oCenter.size());
				Assert::IsTrue(pIndiv->has_numeric_fields());
				//os << *pIndiv << std::endl;
			}//i
			pProvider->reset();
			size_t nc = 0;
			do {
				IndivTypePtr oInd = pProvider->next();
				IndivType *pIndiv = oInd.get();
				if (pIndiv == nullptr) {
					break;
				}
				++nc;
			} while (true);
			Assert::IsTrue(nc == nCount);
			//STRINGTYPE ss = os.str();
			//Logger::WriteMessage(ss.c_str());
		}// TestFillMortalData
		TEST_METHOD(TestIndivDistanceMap)
		{
			//
			size_t nRows = 0, nCols = 0;
			data_vector data;
			strings_vector varNames, indsNames;
			STRINGTYPE name;
			//
			InfoTestData::get_mortal_data(name, nRows, nCols, data, indsNames, varNames);
			//
			SourceType *pProvider = m_pMortalSource;
			Assert::IsNotNull(pProvider);
			//
			DistanceMapType oDist(pProvider);
			ints_vector ids;
			oDist.indexes(ids);
			Assert::AreEqual(nRows, ids.size());
			for (auto &aIndex1 : ids) {
				for (auto & aIndex2 : ids) {
					if (aIndex1 != aIndex2) {
						DISTANCETYPE d1 = 0, d2 = 0;
						bool bRet = oDist.get(aIndex1, aIndex2, d1);
						Assert::IsTrue(bRet);
						Assert::IsTrue(d1 > 0);
						bRet = oDist.get(aIndex2, aIndex1, d2);
						Assert::IsTrue(bRet);
						Assert::AreEqual(d1, d2);
					}// check
				}// aIndex2
			}// aIndex1
		//	STRINGSTREAM os;
		//	STRINGTYPE ss;
		//	write_distancemap(oDist, ss);
		//	Logger::WriteMessage(ss.c_str());
		}//TestIndivDistanceMap
		TEST_METHOD(TestVariableDistanceMap)
		{
			//
			size_t nRows = 0, nCols = 0;
			data_vector data;
			strings_vector varNames, indsNames;
			STRINGTYPE name;
			//
			InfoTestData::get_mortal_data(name, nRows, nCols, data, indsNames, varNames);
			//
			SourceType *pProvider = m_pMortalVariable;
			Assert::IsNotNull(pProvider);
			//
			DistanceMapType oDist(pProvider);
			ints_vector ids;
			oDist.indexes(ids);
			Assert::AreEqual(nCols, ids.size());
			for (auto &aIndex1 : ids) {
				for (auto & aIndex2 : ids) {
					if (aIndex1 != aIndex2) {
						DISTANCETYPE d1 = 0, d2 = 0;
						bool bRet = oDist.get(aIndex1, aIndex2, d1);
						Assert::IsTrue(bRet);
						Assert::IsTrue(d1 > 0);
						bRet = oDist.get(aIndex2, aIndex1, d2);
						Assert::IsTrue(bRet);
						Assert::AreEqual(d1, d2);
					}// check
				}// aIndex2
			}// aIndex1
			STRINGSTREAM os;
			STRINGTYPE ss;
			write_distancemap(oDist, ss);
			Logger::WriteMessage(ss.c_str());
		}//TestIndivDistanceMap
	};
	unique_ptr<MyFixture> UnitTestIndivProvider::st_m_fixture;
	unique_ptr<MyVariableFixture>  UnitTestIndivProvider::st_m_variablefixture;
}