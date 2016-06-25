#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <base_arranger.h>
/////////////////////////////
#include "infotestdata.h"
////////////////////////////////////
#include "global_defs.h"
#include <fstream>
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////////////
namespace UnitTestGenInfo
{
	using ArrangerType = BASEMatriceArranger<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
	using strings_vector = std::vector<STRINGTYPE>;
	using image_type = InfoImage<double>;
	using image_type_ptr = std::shared_ptr<image_type>;
	///////////////////////////////
	TEST_CLASS(MatImageTests)
	{
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
		}// tearDown
	public:
		TEST_METHOD(testMatImage)
		{
			size_t nRows, nCols;
			STRINGTYPE name;
			strings_vector rowNames, colNames;
			std::vector<DATATYPE> data;
			InfoTestData::get_mortal_data(name, nRows, nCols, data, rowNames, colNames);
			STRINGTYPE filename1(".\\test_mortal_data_start.txt");
			STRINGTYPE filename2(".\\test_mortal_data_end.txt");
			//
			ArrangerType oArrange(name);
			std::future<bool> bFuture = oArrange.initializeAsync(name, nRows, nCols, data, rowNames, colNames);
			bool bRet = bFuture.get();
			Assert::IsTrue(bRet);
			std::future<image_type_ptr> oStartFuture = oArrange.getImageAsync();
			image_type_ptr oIm1 = oStartFuture.get();
			image_type *pStart = oIm1.get();
			Assert::IsNotNull(pStart);
			{
				std::ofstream os(filename1);
				std::future<bool> b = pStart->writeAsync(os,true);
				b.wait();
			}
			std::future<bool> bf2 = oArrange.arrangeAsync();
			bRet = bf2.get();
			Assert::IsTrue(bRet);
			//
			std::future<image_type_ptr> oEndFuture = oArrange.getImageAsync();
			image_type_ptr oIm2 = oEndFuture.get();
			image_type *pEnd = oIm2.get();
			Assert::IsNotNull(pEnd);
			{
				std::ofstream os(filename2);
				std::future<bool> b = pEnd->writeAsync(os,true);
				b.wait();
			}
		}//testArrangeElemsOne
		
	};
}// namespace UnitTestGenInfo