#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <distancemap.h>
#include <anacompoindivprovider.h>
#include <clusterize.h>
#include <matrice.h>
/////////////////////////////
#include "mytestfixture.h"
#include "mytestvariablefixture.h"
////////////////////////////////////
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
	using CreatorType = AnaCompoIndivSourceCreator<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using DataVectorIndivSourceType = DataVectorIndivSource<IDTYPE, STRINGTYPE>;
	using DataVectorIndivSourcePtr = std::shared_ptr<DataVectorIndivSourceType>;
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using DataVectorIndivSourceTuple = std::tuple<DataVectorIndivSourcePtr, DataVectorIndivSourcePtr>;
	///////////////////////////////////
	using ClusterizeKMeansType = ClusterizeKMeans<IDTYPE, STRINGTYPE, DISTANCETYPE>;
	/////////////////////////////////
	using MatOrdType = MatOrd<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using MatElemResultType = typename MatOrdType::MatElemResultType;
	using MatElemResultPtr = typename MatOrdType::MatElemResultPtr;
	using ConnectionType = typename MatOrdType::ConnectionType;
	/////////////////////////////////////
	//
	TEST_CLASS(UnitTestMatOrd)
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
		TEST_METHOD(TestMatOrd)
		{
			//
			STRINGTYPE sigle;
			InfoTestData::get_mortal_name(sigle);
			//
			StoreType *pStore = this->m_pStore;
			DataVectorIndivSourceTuple r = CreatorType::create(pStore, sigle);
			DataVectorIndivSourcePtr oInd = std::get<0>(r);
			DataVectorIndivSourcePtr oVar = std::get<1>(r);
			DataVectorIndivSourceType *pSourceInd = oInd.get();
			Assert::IsTrue(pSourceInd != nullptr);
			pSourceInd->recode();
			DataVectorIndivSourceType *pSourceVar = oVar.get();
			Assert::IsTrue(pSourceVar != nullptr);
			pSourceVar->recode();
			//
			MatOrdType oMat;
			auto conn =
				oMat.connect(
					[&](MatElemResultPtr oIndCrit, MatElemResultPtr oVarCrit) {
				MatElemResultType *pVarCrit = oVarCrit.get();
				MatElemResultType *pIndCrit = oIndCrit.get();
				if ((pIndCrit != nullptr) && (pVarCrit != nullptr)) {
					DISTANCETYPE varCrit = pVarCrit->first;
					DISTANCETYPE indCrit = pIndCrit->first;
					STRINGTYPE s;
					STRINGTYPE ss;
					write_vector(pVarCrit->second, ss);
					write_vector(pIndCrit->second, s);
					STRINGSTREAM os;
					os << "Ind: " << indCrit << ",\t" << s << ",\tVar: " << varCrit << ", " << ss;
					STRINGTYPE sx = os.str();
					Logger::WriteMessage(sx.c_str());
				}
				else if (pVarCrit != nullptr) {
					DISTANCETYPE varCrit = pVarCrit->first;
					STRINGTYPE s;
					write_vector(pVarCrit->second, s);
					STRINGSTREAM os;
					os << "Var: " << varCrit << ", \t" << s;
					STRINGTYPE sx = os.str();
					Logger::WriteMessage(sx.c_str());
				}
				else if (pIndCrit != nullptr) {
					DISTANCETYPE indCrit = pIndCrit->first;
					STRINGTYPE s;
					write_vector(pIndCrit->second, s);
					STRINGSTREAM os;
					os << "Ind: " << indCrit << ", \t" << s;
					STRINGTYPE sx = os.str();
					Logger::WriteMessage(sx.c_str());
				}
			});
			bool bRet = oMat.process(pSourceInd, pSourceVar);
			Assert::IsTrue(bRet);
			ints_vector varids, indids;
			oMat.get_ids(indids, varids);
			Logger::WriteMessage("========================================");
			Logger::WriteMessage("VARIABLES");
			for (auto &aIndex : varids) {
				IndivTypePtr oInd = pSourceVar->find(aIndex);
				IndivType *p = oInd.get();
				if (p != nullptr) {
					STRINGTYPE s = p->sigle();
					Logger::WriteMessage(s.c_str());
				} // p
			} // varIndex
			Logger::WriteMessage("========================================");
			Logger::WriteMessage("INDIVS");
			for (auto &aIndex : indids) {
				IndivTypePtr oInd = pSourceInd->find(aIndex);
				IndivType *p = oInd.get();
				if (p != nullptr) {
					STRINGTYPE s = p->sigle();
					Logger::WriteMessage(s.c_str());
				} // p
			} // varIndex
		}// TestAnaCompo
	};
	unique_ptr<MyFixture> UnitTestMatOrd::st_m_fixture;
}