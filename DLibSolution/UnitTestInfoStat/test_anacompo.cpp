#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <distancemap.h>
#include <anacompoindivprovider.h>
#include <clusterize.h>
/////////////////////////////
#include <mytestfixture.h>
#include <mytestvariablefixture.h>
////////////////////////////////////
#include <global_defs.h>
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
	//
	////////////////////////////////
	using CreatorType = AnaCompoIndivSourceCreator<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE,DISTANCETYPE>;
	using DataVectorIndivSourceType = DataVectorIndivSource<IDTYPE, STRINGTYPE>;
	using DataVectorIndivSourcePtr = std::shared_ptr<DataVectorIndivSourceType>;
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using DataVectorIndivSourceTuple = std::tuple<DataVectorIndivSourcePtr, DataVectorIndivSourcePtr>;
	using IndivMapType = IndivMap<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	using IndivMapTypePtr = std::shared_ptr<IndivMapType>;
	using IndivMapTuple = std::tuple<IndivMapTypePtr, IndivMapTypePtr>;
	///////////////////////////////////
	using ClusterizeKMeansType = ClusterizeKMeans<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	/////////////////////////////////
	//
	TEST_CLASS(UnitTestAnaCompo)
	{
		static unique_ptr<MyFixture> st_m_fixture;
		//
		StoreType *m_pStore;
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
			m_pStore = p->get_store();
			Assert::IsNotNull(m_pStore);
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_pStore = nullptr;
		}// tearDown
	public:
		TEST_METHOD(TestAnaIndivMap)
		{
			//
			STRINGTYPE sigle;
			InfoTestData::get_mortal_name(sigle);
			//
			StoreType *pStore = this->m_pStore;
			IndivMapTuple r = CreatorType::create_indivmaps(pStore, sigle);
			IndivMapTypePtr oInd = std::get<0>(r);
			IndivMapType *pIndMap = oInd.get();
			Assert::IsNotNull(pIndMap);
			IndivMapTypePtr oVar = std::get<1>(r);
			IndivMapType *pVarMap = oVar.get();
			Assert::IsNotNull(pVarMap);
			//
		}//TestClusterizeAnaIndivMap
	};
	unique_ptr<MyFixture> UnitTestAnaCompo::st_m_fixture;
}