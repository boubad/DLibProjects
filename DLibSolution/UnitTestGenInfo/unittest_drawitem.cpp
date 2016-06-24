#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <matrice_drawitems.h>
/////////////////////////////
#include "infotestdata.h"
////////////////////////////////////
#include "global_defs.h"
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////
namespace UnitTestGenInfo
{
	using DrawItemsType = DrawItems<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
	using DrawItemType = BaseDrawItem<STRINGTYPE, FLOATTYPE>;
	using PDrawItemType = DrawItemType *;
	using items_vector = std::vector<PDrawItemType>;
	using DrawItemsType = DrawItems<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
	using sizets_vector = std::vector<size_t>;
	using strings_vector = std::vector<STRINGTYPE>;
	using MatElemResultType = MatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using MatElemResultPtr = std::shared_ptr<MatElemResultType>;
	////////////////////////////////
	TEST_CLASS(DrawItemsTests)
	{
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
		}// tearDown
	public:

		TEST_METHOD(testDrawItem)
		{
			DrawItemsType oDrawItems;
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<DATATYPE> data;
			strings_vector rowNames, colNames;
			InfoTestData::get_mortal_data(name, nRows, nCols, data, rowNames, colNames);
			bool bRet = oDrawItems.initialize(MatCellType::histogCell, nRows, nCols, data, rowNames, colNames);
			Assert::IsTrue(bRet);
			Assert::IsNotNull(oDrawItems.get_indiv_provider());
			Assert::IsNotNull(oDrawItems.get_variable_provider());
		}//testDrawItem
	};
}// namespace UnitTestGenInfo