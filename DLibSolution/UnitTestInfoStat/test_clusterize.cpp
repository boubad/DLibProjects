#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include "mytestfixture.h"
//////////////////////
#include <clusterize.h>
///////////////////////////
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
	//
	using ClusterizeKMeansType = ClusterizeKMeans<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	//
	TEST_CLASS(UnitTestClusterizeKMeans)
	{
		static unique_ptr<MyFixture> st_m_fixture;
		//
		SourceType *m_pMortalSource;
		SourceType *m_pConsoSource;
		SourceType *m_pTestSource;
		size_t m_nbClusters;
		size_t m_nbIters;
		//
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			st_m_fixture.reset(new MyFixture());
			MyFixture *p = st_m_fixture.get();
			Assert::IsNotNull(p);
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
			m_nbClusters = 5;
			m_nbIters = 100;
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_pMortalSource = nullptr;
			m_pConsoSource = nullptr;
			m_pTestSource = nullptr;
		}// tearDown
	public:
		TEST_METHOD(TestMortalClusterize)
		{
			SourceType *pProvider = m_pMortalSource;
			Assert::IsNotNull(pProvider);
			pProvider->reset();
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
			SourceType *pProvider = m_pConsoSource;
			Assert::IsNotNull(pProvider);
			pProvider->reset();
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
			SourceType *pProvider = m_pTestSource;
			Assert::IsNotNull(pProvider);
			pProvider->reset();
			//
			ClusterizeKMeansType oMan;
			size_t nIters = oMan.process(pProvider, m_nbClusters, m_nbIters);
			Assert::IsTrue(nIters > 0);
			STRINGTYPE ss;
			oMan.to_string(ss);
			Logger::WriteMessage(ss.c_str());
		}// TestFillMortalData
	};
	unique_ptr<MyFixture> UnitTestClusterizeKMeans::st_m_fixture;
}
