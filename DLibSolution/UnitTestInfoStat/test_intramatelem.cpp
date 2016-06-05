#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <intramat.h>
#include <indivmap.h>
#include <anacompoindivprovider.h>
#include <matrice.h>
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
	using sizets_vector = std::vector<size_t>;
	//
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	////////////////////////////////
	using IntraMatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using RescritType = std::atomic<DISTANCETYPE>;
	using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE>;
	using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
	using SlotType = typename IntraMatElemType::SlotType;
	////////////////////////
	using IndivMapType = IndivMap<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	/////////////////////////////////
	using CreatorType = AnaCompoIndivSourceCreator<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE, DISTANCETYPE>;
	using DataVectorIndivSourceType = DataVectorIndivSource<IDTYPE, STRINGTYPE>;
	using DataVectorIndivSourcePtr = std::shared_ptr<DataVectorIndivSourceType>;
	using DataVectorIndivSourceTuple = std::tuple<DataVectorIndivSourcePtr, DataVectorIndivSourcePtr>;
	/////////////////////////
	using MatOrdType = MatOrd<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using MatOrdResultType = typename MatOrdType::MatElemResultType;
	using MatOrdResultPtr = typename MatOrdType::MatElemResultPtr;
	using MatOrdConnectionType = typename MatOrdType::ConnectionType;
	/////////////////////////////////////
	////////////////////////
	using mutex_type = std::mutex;
	using lock_type = std::lock_guard<mutex_type>;
	//////////////////////////////////
	mutex_type mutex;
	////////////////////////////////////////
	void MatOrdLogger(const STRINGTYPE &sId, MatOrdResultPtr oIndCrit,
		MatOrdResultPtr oVarCrit) {
		STRINGSTREAM os;
		MatOrdResultType *pVarCrit = oVarCrit.get();
		MatOrdResultType *pIndCrit = oIndCrit.get();
		if ((pIndCrit != nullptr) && (pVarCrit != nullptr)) {
			os << sId << "\t";
			DISTANCETYPE varCrit = pVarCrit->first;
			DISTANCETYPE indCrit = pIndCrit->first;
			os << "Ind: " << indCrit << ",\tVar: " << varCrit ;
			STRINGTYPE sz = os.str();
			{
				lock_type oLock(mutex);
				Logger::WriteMessage(sz.c_str());
			}
		}
		else if (pVarCrit != nullptr) {
			os <<  sId << "\t";
			DISTANCETYPE varCrit = pVarCrit->first;
			os << "Var: " << varCrit;
			STRINGTYPE sz = os.str();
			{
				lock_type oLock(mutex);
				Logger::WriteMessage(sz.c_str());
			}
		}
		else if (pIndCrit != nullptr) {
			os << sId << "\t";
			DISTANCETYPE indCrit = pIndCrit->first;
			os << "Ind: " << indCrit;
			STRINGTYPE sz = os.str();
			{
				lock_type oLock(mutex);
				Logger::WriteMessage(sz.c_str());
			}
		}
	}// MatOrdLogger
	/////////////////////////////////////
	void MatElemLogger(const STRINGTYPE &sId, IntraMatElemResultPtr oRes) {
		STRINGSTREAM os;
		os << sId;
		IntraMatElemResultType *p = oRes.get();
		if (p != nullptr) {
			os << "\t";
			DISTANCETYPE crit = p->first;
			const sizets_vector indexes = p->second;
			os << "Crit: " << crit << "\t";
			STRINGTYPE sx;
			write_vector(indexes, sx);
			os << sx;
		}// p
		STRINGTYPE ss = os.str();
		{
			lock_type oLock(mutex);
			Logger::WriteMessage(ss.c_str());
		}
	}// MatElemLogger
	SlotType callbackMortal = [&](IntraMatElemResultPtr oRes) {
		STRINGTYPE sId("MORTAL ");
		MatElemLogger(sId, oRes);
	};
	SlotType callbackConso = [&](IntraMatElemResultPtr oRes) {
		STRINGTYPE sId("CONSO ");
		MatElemLogger(sId, oRes);
	};
	SlotType callbackTest = [&](IntraMatElemResultPtr oRes) {
		STRINGTYPE sId("TEST ");
		MatElemLogger(sId, oRes);
	};
	///////////////////////////////////////
	//
	TEST_CLASS(UnitTestIntraMatElem)
	{
		static unique_ptr<MyFixture> st_m_fixture;
		//
		SourceType *m_pMortalProvider;
		SourceType *m_pConsoProvider;
		SourceType *m_pTestProvider;
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
			m_pMortalProvider = p->mortal_source();
			Assert::IsNotNull(m_pMortalProvider);
			m_pConsoProvider = p->conso_source();
			Assert::IsNotNull(m_pConsoProvider);
			m_pTestProvider = p->test_source();
			Assert::IsNotNull(m_pMortalProvider);
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_pMortalProvider = nullptr;
			m_pConsoProvider = nullptr;
			m_pTestProvider = nullptr;
		}// tearDown
		void perform_test(const STRINGTYPE &sigle) {
			//
			MyFixture *p = st_m_fixture.get();
			StoreType *pStore = p->get_store();
			DataVectorIndivSourceTuple r = CreatorType::create(pStore, sigle);
			DataVectorIndivSourcePtr oInd = std::get<0>(r);
			DataVectorIndivSourcePtr oVar = std::get<1>(r);
			DataVectorIndivSourceType *pSourceInd = oInd.get();
			pSourceInd->recode(100);
			DataVectorIndivSourceType *pSourceVar = oVar.get();
			pSourceVar->recode(100);
			//
			MatOrdType oMat;
			auto conn =
				oMat.connect(
					[&](MatOrdResultPtr oIndCrit, MatOrdResultPtr oVarCrit) {
				MatOrdLogger(sigle, oIndCrit, oVarCrit);
			});
			(void)oMat.process(pSourceInd, pSourceVar);
		} //perform_test}
	public:
		TEST_METHOD(TestAnaCompoArrange)
		{
			//
			STRINGTYPE sigleMortal, sigleConso, sigleTest;
			InfoTestData::get_mortal_name(sigleMortal);
			InfoTestData::get_conso_name(sigleConso);
			InfoTestData::get_test_name(sigleTest);
			//
			std::thread t0([&]() {
				perform_test(sigleTest);
			});
			std::thread t1([&]() {
				perform_test(sigleMortal);
			});
			std::thread t2([&]() {
				perform_test(sigleConso);
			});
			t1.join();
			t2.join();
			t0.join();
		}// TestAnaCompoArrange
		TEST_METHOD(TestIntraMatElemDistanceMap)
		{
			std::thread t1([&]() {
				DistanceMapType oMap(m_pMortalProvider);
				IntraMatElemType oMat;
				oMat.arrange(&oMap, &callbackMortal);
			});
			std::thread t2([&]() {
				DistanceMapType oMap(m_pConsoProvider);
				IntraMatElemType oMat;
				oMat.arrange(&oMap, &callbackConso);
			});
			t1.join();
			t2.join();
		}// TestIntraMatElemDistanceMap
		TEST_METHOD(TestIntraMatElemIndivMap)
		{
			std::thread t1([&]() {
				IndivMapType oMap(m_pMortalProvider);
				IntraMatElemType oMat;
				oMat.arrange(&oMap, &callbackMortal);
			});
			std::thread t2([&]() {
				IndivMapType oMap(m_pConsoProvider);
				IntraMatElemType oMat;
				oMat.arrange(&oMap, &callbackConso);
			});
			t1.join();
			t2.join();
		}// TestIntraMap
		TEST_METHOD(TestIntraMatElemProvider)
		{
			std::thread t1([&]() {
				IntraMatElemType oMat;
				oMat.arrange(m_pMortalProvider, &callbackMortal);
			});
			std::thread t2([&]() {
				IntraMatElemType oMat;
				oMat.arrange(m_pConsoProvider, &callbackConso);
			});
			t1.join();
			t2.join();
		}// TestIntraMap
	};
	unique_ptr<MyFixture> UnitTestIntraMatElem::st_m_fixture;
}