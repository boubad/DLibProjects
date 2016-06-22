#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <eps_arranger.h>
/////////////////////////////
#include "infotestdata.h"
////////////////////////////////////
#include "global_defs.h"
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////////////
namespace UnitTestGenInfo
{
	using ArrangerType = EPSMatriceArranger<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
	using strings_vector = std::vector<STRINGTYPE>;
	///////////////////////////////
	TEST_CLASS(MatEpsMatriceTests)
	{
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
		}// tearDown
	public:
		TEST_METHOD(testEPSArrange)
		{
			size_t nRows, nCols;
			STRINGTYPE name;
			strings_vector rowNames, colNames;
			std::vector<DATATYPE> data;
			InfoTestData::get_mortal_data(name, nRows, nCols, data, rowNames, colNames);
			STRINGTYPE filename("D:\\temp\\test_mortal_data.eps");
			//
			ArrangerType oArrange(name);
			std::future<bool> bFuture = oArrange.export_async(filename, nRows, nCols, data, rowNames, colNames);
			bool bRet = bFuture.get();
			Assert::IsTrue(bRet);
		}//testArrangeElemsOne
		TEST_METHOD(testEPSAll)
		{
			size_t nRows, nCols;
			STRINGTYPE name;
			strings_vector rowNames, colNames;
			std::vector<DATATYPE> data;
			InfoTestData::get_mortal_data(name, nRows, nCols, data, rowNames, colNames);
			STRINGTYPE filename1("D:\\temp\\test_mortal_data_initial.eps");
			STRINGTYPE filename2("D:\\temp\\test_mortal_data_ordered.eps");
			//
			ArrangerType oArrange(name);
			std::future<bool> b1 = oArrange.initializeAsync(name, nRows, nCols, data, rowNames, colNames);
			bool bRet = b1.get();
			Assert::IsTrue(bRet);
			std::future<bool> b2 = oArrange.exportAsync(filename1);
			bRet = b2.get();
			Assert::IsTrue(bRet);
			std::future<bool> b3 = oArrange.arrangeAsync();
			bRet = b3.get();
			Assert::IsTrue(bRet);
			std::future<bool> b4 = oArrange.exportAsync(filename2);
			bRet = b4.get();
			Assert::IsTrue(bRet);
		}//testArrangeElemsOne
		TEST_METHOD(testEPSStream)
		{
			size_t nRows, nCols;
			STRINGTYPE name;
			strings_vector rowNames, colNames;
			std::vector<DATATYPE> data;
			InfoTestData::get_mortal_data(name, nRows, nCols, data, rowNames, colNames);
			//
			std::stringstream os;
			ArrangerType oArrange(name);
			std::future<bool> bFuture = oArrange.export_async(os, nRows, nCols, data, rowNames, colNames);
			bool bRet = bFuture.get();
			Assert::IsTrue(bRet);
			std::string ss = os.str();
			Logger::WriteMessage(ss.c_str());
		}//testArrangeElemsOne
	};
}// namespace UnitTestGenInfo