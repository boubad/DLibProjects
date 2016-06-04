#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <intramat.h>
/////////////////////////////
#include <mytestfixture.h>
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
	////////////////////////////////
	using IntraMatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using RescritType = std::atomic<DISTANCETYPE>;
	using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE>;
	using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
	using SlotType = typename IntraMatElemType::SlotType;
	////////////////////////
	using mutex_type = std::mutex;
	using lock_type = std::lock_guard<mutex_type>;
	//////////////////////////////////
	mutex_type mutex;
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
	public:
		TEST_METHOD(TestIntraMap)
		{
			//
			mutex_type _mutex;
			//
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
			std::thread t1([&]() {
				IntraMatElemType oMat;
				oMat.arrange(m_pMortalProvider, &callbackMortal);
			});
			std::thread t2([&]() {
				IntraMatElemType oMat;
				oMat.arrange(m_pConsoProvider, &callbackConso);
			});
			std::thread t3([&]() {
				IntraMatElemType oMat;
				oMat.arrange(m_pTestProvider, &callbackTest);
			});
			t1.join();
			t2.join();
			t3.join();
		}// TestIntraMap
	};
	unique_ptr<MyFixture> UnitTestIntraMatElem::st_m_fixture;
}