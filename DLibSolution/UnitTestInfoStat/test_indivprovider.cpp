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
	using StoreFixture = TestStoreFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using SourceFixture = TestSourceFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using IndivType = Indiv<IDTYPE, STRINGTYPE>;
	using DataMap = std::map<IDTYPE, InfoValue>;
	using IndivTypePtr = std::shared_ptr<IndivType>;
	using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
	//
	TEST_CLASS(UnitTestIndivProvider)
	{
		static unique_ptr<StoreFixture> st_m_store;
		//
		unique_ptr<SourceFixture> m_fixture;
		//
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			StoreFixture *p = new StoreFixture();
			Assert::IsNotNull(p);
			st_m_store.reset(p);
			StoreType *ps = p->get_memory_store();
			Assert::IsNotNull(ps);
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			st_m_store.reset();
		}

		TEST_METHOD_INITIALIZE(setUp)
		{
			StoreFixture *pf = st_m_store.get();
			Assert::IsNotNull(pf);
			StoreType *ps = pf->get_memory_store();
			Assert::IsNotNull(ps);
			m_fixture.reset(new SourceFixture(ps));
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_fixture.reset();
		}// tearDown
	public:
		TEST_METHOD(TestMortalProvider)
		{
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
			SourceType *pProvider = px->mortal_source();
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
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
			SourceType *pProvider = px->conso_source();
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
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
			SourceType *pProvider = px->test_source();
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
	unique_ptr<StoreFixture> UnitTestIndivProvider::st_m_store;
}