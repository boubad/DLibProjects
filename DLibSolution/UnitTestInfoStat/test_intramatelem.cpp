#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <intramat.h>
#include <indivmap.h>
#include <anacompoindivprovider.h>
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
	using sizets_vector = std::vector<size_t>;
	//
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	////////////////////////////////
	using IntraMatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using RescritType = std::atomic<DISTANCETYPE>;
	using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE,STRINGTYPE>;
	using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
	using queue_type = SharedQueue<IntraMatElemResultPtr>;
	////////////////////////
	using IndivMapType = IndivMap<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	/////////////////////////////////
	using CreatorType = AnaCompoIndivSourceCreator<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE, DISTANCETYPE>;
	using DataVectorIndivSourceType = DataVectorIndivSource<IDTYPE, STRINGTYPE>;
	using DataVectorIndivSourcePtr = std::shared_ptr<DataVectorIndivSourceType>;
	using DataVectorIndivSourceTuple = std::tuple<DataVectorIndivSourcePtr, DataVectorIndivSourcePtr>;
	/////////////////////////
	using MatOrdType = IntraMatOrd<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using mutex_type = std::mutex;
	using lock_type = std::lock_guard<mutex_type>;
	/////////////////////////////////////////
	class MatElemLogger : public MatElemResultBackgounder<IDTYPE, DISTANCETYPE, STRINGTYPE> {
	public:
		MatElemLogger() {}
		virtual ~MatElemLogger() {}
	protected:
		virtual void process_result(IntraMatElemResultPtr oRes) {
			STRINGSTREAM os;
			IntraMatElemResultType *p = oRes.get();
			if (p != nullptr) {
				StageType stage = p->stage;
				if (stage == StageType::started) {
					os << "STARTED...\t";
				}
				else if (stage == StageType::finished) {
					os << "FINISHED!!!\t";
				}
				STRINGTYPE sx = p->sigle;
				if (!sx.empty()) {
					os << sx << "\t";
				}
				if (p->disposition == DispositionType::indiv) {
					os << "INDS\t";
				}
				else if (p->disposition == DispositionType::variable) {
					os << "VARS\t";
				}
				DISTANCETYPE crit = p->first;
				os << "Crit: " << crit << std::endl;
			}// p
			STRINGTYPE ss = os.str();
			Logger::WriteMessage(ss.c_str());
		}// process_result
	};// class MatElemLogger
	//////////////////////////////////
	//
	TEST_CLASS(UnitTestIntraMatElem)
	{
		static unique_ptr<MyFixture> st_m_fixture;
		static unique_ptr<MyVariableFixture> st_m_varfixture;
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
			st_m_varfixture.reset(new MyVariableFixture());
			MyVariableFixture *pv = st_m_varfixture.get();
			Assert::IsNotNull(pv);
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			st_m_fixture.reset();
			st_m_varfixture.reset();
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
		TEST_METHOD(TestIntraMatOrd)
		{
			MyVariableFixture *pv = st_m_varfixture.get();
			Assert::IsNotNull(pv);
			//
			MatElemLogger oQueue;
			//
			concurrency::parallel_invoke(
				[&]() {
				MatOrdType oMat;
				STRINGTYPE sId("TEST ");
				oMat.sigle(sId);
				oMat.arrange(m_pTestProvider, pv->test_source(), &oQueue);
			},
				[&]() {
				MatOrdType oMat;
				STRINGTYPE sId("MORTAL ");
				oMat.sigle(sId);
				oMat.arrange(m_pMortalProvider, pv->mortal_source(), &oQueue);
			}, [&]() {
				MatOrdType oMat;
				STRINGTYPE sId("CONSO ");
				oMat.sigle(sId);
				oMat.arrange(m_pConsoProvider, pv->conso_source(), &oQueue);
			}

			);
		}// TestIntraMatOrd
		TEST_METHOD(TestIntraMatElem)
		{
			MatElemLogger oQueue;
			//
			concurrency::parallel_invoke(
				[&]() {
				IntraMatElemType oMat(DispositionType::indiv, &oQueue);
				STRINGTYPE sId("TEST ");
				oMat.sigle(sId);
				oMat.arrange(m_pTestProvider);
			}, [&]() {
				IntraMatElemType oMat(DispositionType::indiv, &oQueue);
				STRINGTYPE sId("MORTAL ");
				oMat.sigle(sId);
				oMat.arrange(m_pMortalProvider);
			}, [&]() {
				IntraMatElemType oMat(DispositionType::indiv, &oQueue);
				STRINGTYPE sId("CONSO ");
				oMat.sigle(sId);
				oMat.arrange(m_pConsoProvider);
			}
			);
		}// TestIntraMatElem
	};
	unique_ptr<MyFixture> UnitTestIntraMatElem::st_m_fixture;
	unique_ptr<MyVariableFixture> UnitTestIntraMatElem::st_m_varfixture;
}