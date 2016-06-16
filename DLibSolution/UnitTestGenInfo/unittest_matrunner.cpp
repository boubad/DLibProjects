#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <matrunner.h>
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
	using MatRunnerType = MatRunner<IDTYPE, STRINGTYPE, DISTANCETYPE,INTTYPE,WEIGHTYPE>;
	using cancelflag = std::atomic<bool>;
	using pcancelflag = cancelflag *;
	using PBackgrounder = Backgrounder *;
	using strings_vector = std::vector<STRINGTYPE>;
	using MatElemType = MatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using DistanceMapType = typename MatElemType::DistanceMapType;
	using MatElemResultType = typename MatElemType::MatElemResultType;
	using MatElemResultPtr = typename MatElemType::MatElemResultPtr;
	using matelem_promise = std::promise<MatElemResultPtr>;
	using matelem_promise_ptr = std::shared_ptr<matelem_promise>;
	using matelem_future = std::future<MatElemResultPtr>;
	using matelem_function = std::function<void(MatElemResultPtr)>;
	////////////////////////////////
	TEST_CLASS(MatRunnerTests)
	{
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
		}// tearDown
	public:

		TEST_METHOD(testArrangeElemsOne)
		{
			auto infologger = [&](MatElemResultPtr oRes) {
				MatElemResultType *p = oRes.get();
				if (p != nullptr) {
					STRINGTYPE sr;
					p->to_string(sr);
					Logger::WriteMessage(sr.c_str());
				} // p
			};
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<DATATYPE> gdata;
			strings_vector rowNames, colNames;
			InfoTestData::get_mortal_data(name, nRows, nCols, gdata, rowNames, colNames);
			//
			std::vector<FLOATTYPE> weights;
			bool bComputeWeights = true;
			
			MatRunnerType oRunner;
			matelem_promise_ptr oPromise = std::make_shared<matelem_promise>();
			Assert::IsNotNull(oPromise.get());

			matelem_future oFuture = oRunner.arrange_elem(oPromise, nRows, nCols, gdata, rowNames, weights, bComputeWeights, infologger);
			MatElemResultPtr oRes = oFuture.get();
			Assert::IsNotNull(oRes.get());
		}//testArrangeElemsOne
	};
}// namespace UnitTestGenInfo