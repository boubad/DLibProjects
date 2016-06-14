#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <info_matord.h>
////////////////////////////////////
#include <global_defs.h>
#include <infotestdata.h>
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////
namespace UnitTestInfoStat
{
	///////////////////////////////////
	using ints_doubles_map = std::map<IDTYPE, double>;
	using ints_vector = std::vector<IDTYPE>;
	using strings_vector = std::vector<STRINGTYPE>;
	using MatElemType = MatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using Operation_result = std::function<void()>;
	using MatElemResultType = typename MatElemType::IntraMatElemResultType;
	using MatElemResultPtr = typename MatElemType::IntraMatElemResultPtr;
	using MatElemFunctionType = std::function<void(MatElemResultPtr)>;
	using Runner = InfoMatriceRunner<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using InfoMatResultType = std::pair<MatElemResultPtr, MatElemResultPtr>;
	/////////////////////////////////////////
	////////////////////
	MatElemFunctionType infologger = [&](MatElemResultPtr oRes) {
		STRINGTYPE ss;
		STRINGSTREAM os;
		MatElemResultType *p = oRes.get();
		if (p != nullptr) {
			p->to_string(ss);
		}// p
		Logger::WriteMessage(ss.c_str());
	};
	//////////////////////////////////
	//
	TEST_CLASS(UnitTestInfoMatrice)
	{
		static size_t m_nrows;
		static size_t m_ncols;
		static STRINGTYPE m_sigle;
		static std::vector<DATATYPE> m_gdata;
		static strings_vector m_colnames;
		static strings_vector m_rownames;
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			InfoTestData::get_mortal_name(m_sigle);
			InfoTestData::get_data(m_sigle, m_nrows, m_ncols, m_gdata, m_rownames, m_colnames);
		}
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
		}

		TEST_METHOD_INITIALIZE(setUp)
		{

		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{

		}// tearDown

	public:
		TEST_METHOD(TestInfoMatArrange)
		{
			std::shared_ptr<std::promise<MatElemResultPtr>> oPromise(new std::promise<MatElemResultPtr>());
			Assert::IsNotNull(oPromise.get());
			Runner oRunner;
			std::vector<double> weights;
			std::future<MatElemResultPtr> oFuture = oRunner.arrange_elem(m_nrows, m_ncols, m_gdata, m_rownames, weights, true, oPromise, infologger);
			MatElemResultPtr oRes = oFuture.get();
			MatElemResultType *p = oRes.get();
			Assert::IsNotNull(p);
		}// TestInfoMatArrange
		TEST_METHOD(TestInfoMatrice)
		{
			std::shared_ptr<std::promise<bool>> oPromise(new std::promise<bool>());
			Assert::IsNotNull(oPromise.get());
			Runner oRunner;
			std::vector<double> weights;
			std::future<bool> oFuture = oRunner.arrange_matrice(m_nrows, m_ncols, m_gdata,m_rownames, m_colnames,true,oPromise, infologger);
			bool p = oFuture.get();
			Assert::IsTrue(p);
		}// TestInfoMatArrange
	};
	size_t UnitTestInfoMatrice::m_nrows;
	size_t UnitTestInfoMatrice::m_ncols;
	STRINGTYPE UnitTestInfoMatrice::m_sigle;
	std::vector<DATATYPE> UnitTestInfoMatrice::m_gdata;
	strings_vector UnitTestInfoMatrice::m_colnames;
	strings_vector UnitTestInfoMatrice::m_rownames;
}