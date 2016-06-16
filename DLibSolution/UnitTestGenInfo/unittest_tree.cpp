#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <info_treeindivs.h>
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
	////////////////////////////////
	using IndivsTreeType = IndivsTree<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	using IndivType = Indiv<IDTYPE, STRINGTYPE>;
	using IndivTypePtr = std::shared_ptr<IndivType>;
	using DataMap = std::map<IDTYPE, InfoValue>;
	using TreeItemType = TreeItem<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	using PTreeItemType = TreeItemType *;
	using ints_sizet_map = std::map<IDTYPE, size_t>;
	using IndivClusterType = IndivCluster<IDTYPE, STRINGTYPE>;
	using treeitems_vector = std::vector<PTreeItemType>;
	using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
	using clusters_vector = std::vector<IndivClusterType>;
	using ints_vector = std::vector<IDTYPE>;
	using sizet_intsvector_map = std::map<size_t, ints_vector>;
	using datamaps_vector = std::vector<DataMap>;
	using ClustersCollectionType = ClustersCollection<IDTYPE, STRINGTYPE>;
	using cancelflag = std::atomic<bool>;
	using pcancelflag = cancelflag *;
	using PBackgrounder = Backgrounder *;
	using ClusterizeResultType = ClusterizeResult<IDTYPE>;
	using ClusterizeResultPtr = std::shared_ptr< ClusterizeResultType>;
	using RESULT = std::shared_ptr< ClusterizeResultType>;
	using function_type = std::function<void(RESULT)>;
	////////////////////////////////
	TEST_CLASS(IndivsTreeTests)
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

		TEST_METHOD(testMortalTreeIndivs)
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
			IndivsTreeType oMat(pCancel, pBack, infologger);
			oMat.process(pProvider, 5);
		}//testMortalKmeans
	};
}// namespace UnitTestGenInfo