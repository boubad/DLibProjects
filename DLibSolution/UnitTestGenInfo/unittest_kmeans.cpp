#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <clusterize.h>
/////////////////////////////
#include "datafixture.h"
////////////////////////////////////
#include "global_defs.h"
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////
namespace UnitTestGenInfo
{		
	using MyFixture = InfoDataFixture<IDTYPE, STRINGTYPE, DATATYPE>;
	using SourceType = typename MyFixture::SourceType;
	using MatriceDataType = typename MyFixture::MatriceDataType;
	using strings_vector = typename MyFixture::strings_vector;
	using pcancelflag = typename MyFixture::pcancelflag;
	using PBackgrounder = typename MyFixture::PBackgrounder;
	////////////////////////////////
	using ints_sizet_map = std::map<IDTYPE, size_t>;
	using IndivClusterType = IndivCluster<IDTYPE, STRINGTYPE>;
	using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
	using clusters_vector = std::vector<IndivClusterType>;
	using ints_vector = std::vector<IDTYPE>;
	using sizet_intsvector_map = std::map<size_t, ints_vector>;
	using ClustersCollectionType = ClustersCollection<IDTYPE, STRINGTYPE>;
	using cancelflag = std::atomic<bool>;
	using pcancelflag = cancelflag *;
	using PBackgrounder = Backgrounder *;
	using ClusterizeResultType = ClusterizeResult<IDTYPE>;
	using ClusterizeResultPtr = std::shared_ptr< ClusterizeResultType>;
	using ClusterizeKMeansType = ClusterizeKMeans<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	////////////////////////////////
	TEST_CLASS(KMeansTests)
	{
		std::unique_ptr<MyFixture> m_fixture;
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
			m_fixture.reset(new MyFixture());
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_fixture.reset();
		}// tearDown
	public:
		
		TEST_METHOD(testMortalKmeans)
		{
			auto infologger = [&](ClusterizeResultPtr oRes) {
				ClusterizeResultType *p = oRes.get();
				if (p != nullptr) {
					STRINGTYPE sr;
					p->to_string(sr);
					Logger::WriteMessage(sr.c_str());
				} // p
			};
			SourceType *pProvider = m_fixture->mortal_indiv_provider();
			Assert::IsTrue(pProvider != nullptr);
			//
			pcancelflag pCancel = m_fixture->get_cancelflag();
			Assert::IsTrue(pCancel != nullptr);
			PBackgrounder pBack = m_fixture->get_backgrounder();
			Assert::IsTrue(pBack != nullptr);
			//
			ClusterizeKMeansType oMat(pCancel, pBack, infologger);
			oMat.process(pProvider, 5, 10);
		}//testMortalKmeans
	};
}// namespace UnitTestGenInfo