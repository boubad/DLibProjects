#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include "indivsource_fixture.h"
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
	using FixtureType = TestSourceFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using IndivType = Indiv<IDTYPE, STRINGTYPE>;
	using DataMap = std::map<IDTYPE, InfoValue>;
	using IndivTypePtr = std::shared_ptr<IndivType>;
	using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
	//
	TEST_CLASS(UnitTestIndivProvider)
	{
		static unique_ptr<FixtureType> st_m_man;
		//
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			FixtureType *p = new FixtureType();
			Assert::IsNotNull(p);
			st_m_man.reset(p);
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			st_m_man.reset();
		}

		TEST_METHOD_INITIALIZE(setUp)
		{
			
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			
		}// tearDown
	public:
		TEST_METHOD(TestMortalProvider)
		{
			FixtureType *pf = st_m_man.get();
			Assert::IsNotNull(pf);
			SourceType *pProvider = pf->mortal_source();
			Assert::IsNotNull(pProvider);
			size_t nCount = pProvider->count();
			Assert::IsTrue(nCount > 0);
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
				Assert::IsTrue(oCenter.size() > 0);
				Assert::IsTrue(pIndiv->has_numeric_fields());
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
		}// TestFillMortalData
		TEST_METHOD(TestConsoProvider)
		{
			FixtureType *pf = st_m_man.get();
			Assert::IsNotNull(pf);
			SourceType *pProvider = pf->conso_source();
			Assert::IsNotNull(pProvider);
			size_t nCount = pProvider->count();
			Assert::IsTrue(nCount > 0);
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
				Assert::IsTrue(oCenter.size() > 0);
				Assert::IsTrue(pIndiv->has_numeric_fields());
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
		}// TestFillConsolData
		TEST_METHOD(TestTestDataProvider)
		{
			FixtureType *pf = st_m_man.get();
			Assert::IsNotNull(pf);
			SourceType *pProvider = pf->test_source();
			Assert::IsNotNull(pProvider);
			size_t nCount = pProvider->count();
			Assert::IsTrue(nCount > 0);
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
				Assert::IsTrue(oCenter.size() > 0);
				Assert::IsTrue(pIndiv->has_numeric_fields());
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
		}// TestFillMortalData
	};
	unique_ptr<FixtureType> UnitTestIndivProvider::st_m_man;
}