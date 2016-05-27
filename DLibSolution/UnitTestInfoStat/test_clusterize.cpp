#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include "indivsource_fixture.h"
//////////////////////
#include <clusterize.h>
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
	using DISTANCETYPE = long;
	//
	using StoreFixture = TestStoreFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using SourceFixture = TestSourceFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using IndivType = Indiv<IDTYPE, STRINGTYPE>;
	using DataMap = std::map<IDTYPE, InfoValue>;
	using IndivTypePtr = std::shared_ptr<IndivType>;
	using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
	//
	using ClusterizeKMeansType = ClusterizeKMeans<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	//
	TEST_CLASS(UnitTestClusterizeKMeans)
	{
		static unique_ptr<StoreFixture> st_m_store;
		//
		size_t m_nbClusters;
		size_t m_nbIters;
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
			m_nbClusters = 5;
			m_nbIters = 100;
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_fixture.reset();
		}// tearDown
	public:
		TEST_METHOD(TestMortalClusterize)
		{
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
			SourceType *pProvider = px->mortal_source();
			Assert::IsNotNull(pProvider);
			//
			ClusterizeKMeansType oMan;
			size_t nIters = oMan.process(pProvider, m_nbClusters, m_nbIters);
			Assert::IsTrue(nIters > 0);
			STRINGTYPE ss;
			oMan.to_string(ss);
			Logger::WriteMessage(ss.c_str());
		}// TestFillMortalData
		TEST_METHOD(TestConsoClusterize)
		{
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
			SourceType *pProvider = px->conso_source();
			Assert::IsNotNull(pProvider);
			//
			ClusterizeKMeansType oMan;
			size_t nbClusters = 3;
			size_t nIters = oMan.process(pProvider, nbClusters, m_nbIters);
			Assert::IsTrue(nIters > 0);
			STRINGTYPE ss;
			oMan.to_string(ss);
			Logger::WriteMessage(ss.c_str());
		}// TestFillConsolData
		TEST_METHOD(TestTestClusterize)
		{
			SourceFixture *px = m_fixture.get();
			Assert::IsNotNull(px);
			SourceType *pProvider = px->test_source();
			Assert::IsNotNull(pProvider);
			//
			ClusterizeKMeansType oMan;
			size_t nIters = oMan.process(pProvider, m_nbClusters, m_nbIters);
			Assert::IsTrue(nIters > 0);
			STRINGTYPE ss;
			oMan.to_string(ss);
			Logger::WriteMessage(ss.c_str());
		}// TestFillMortalData
	};
	unique_ptr<StoreFixture> UnitTestClusterizeKMeans::st_m_store;
}
