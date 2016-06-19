#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <svg/svg_matrice.h>
#include <matrunner.h>
/////////////////////////////
#include "infotestdata.h"
////////////////////////////////////
#include "global_defs.h"
//////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
/////////////////////////////////////////
class svg_win 
{
	using ModelDataType = MatriceModelData<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE, INTTYPE, WEIGHTYPE>;
	using  MatriceWindowType = MatriceSVG<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
	using strings_vector = std::vector<STRINGTYPE>;
	using MatElemType = MatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	using MatElemResultType = typename MatElemType::MatElemResultType;
	using MatElemResultPtr = typename MatElemType::MatElemResultPtr;
	using matelem_function = std::function<void(MatElemResultPtr)>;
	using matelem_future = std::future<MatElemResultPtr>;
	using InfoMatriceResultPair = std::pair<MatElemResultPtr, MatElemResultPtr>;
	using InfoMatriceResultPairPtr = std::shared_ptr<InfoMatriceResultPair>;
	using matrice_promise = std::promise<InfoMatriceResultPairPtr>;
	using matrice_future = std::future<InfoMatriceResultPairPtr>;
	using matrice_promise_ptr = std::shared_ptr<matrice_promise>;
	using ViewType = DrawItemsView<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
	using MatRunnerType = MatRunner<IDTYPE, STRINGTYPE, DISTANCETYPE, INTTYPE, WEIGHTYPE>;
private:
	bool m_ok;
	MatRunnerType *m_prunner;
	MatriceWindowType mat_win;
	std::unique_ptr<ModelDataType> m_model;
	matrice_promise_ptr m_promise;
	matrice_future m_future;
	std::future<bool> m_init_future;
public:
	svg_win(MatRunnerType *pRunner, const STRINGTYPE &name, MatCellType aType = MatCellType::plainCell) :
		m_ok(false), m_prunner(pRunner)
	{
		size_t nRows = 0, nCols = 0;
		strings_vector rowNames, colNames;
		std::vector<DATATYPE> data;
		InfoTestData::get_data(name, nRows, nCols, data, rowNames, colNames);
		if ((nRows > 0) && (nCols > 0) && (data.size() >= (nCols * nRows)) && (rowNames.size() >= nRows) &&
			(colNames.size() >= nCols)) {
			ModelDataType *pModel = new ModelDataType(pRunner);
			this->m_model.reset(pModel);
			this->m_init_future = pModel->initialize(name, nRows, nCols, data, rowNames, colNames, aType);
			this->m_ok = true;
		}
	}
	~svg_win() {
	}
	void set_result(MatElemResultPtr oRes) {
		ModelDataType *pModel = this->m_model.get();
		assert(pModel != nullptr);
		pModel->set_result(oRes);
	}// oRes
	void init_all(void) {
		if (!this->m_ok) {
			return;
		}
		try {
			bool bInit = this->m_init_future.get();
			if (bInit) {
				ModelDataType *pModel = this->m_model.get();
				assert(pModel != nullptr);
				ViewType *pView = pModel->add_view(DispositionType::variable);
				assert(pView != nullptr);
				this->mat_win.set_view(pView);
				this->m_promise = std::make_shared<matrice_promise>();
				this->m_future = pModel->compute(this->m_promise, [this](MatElemResultPtr oRes) {
					this->set_result(oRes);
				});
			}// bInit
		}
		catch (...) {

		}
	}// init_all;
	InfoMatriceResultPairPtr getResult(void) {
		return (this->m_future.get());
	}
	void save(const STRINGTYPE &filename) {
		auto p = this->m_future.get();
		this->mat_win.draw(filename);
	}// save
};
////////////////////////////////
namespace UnitTestGenInfo
{
	using MatRunnerType = MatRunner<IDTYPE, STRINGTYPE, DISTANCETYPE, INTTYPE, WEIGHTYPE>;
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
	//
	using InfoMatriceResultPair = std::pair<MatElemResultPtr, MatElemResultPtr>;
	using InfoMatriceResultPairPtr = std::shared_ptr<InfoMatriceResultPair>;
	using matrice_promise = std::promise<InfoMatriceResultPairPtr>;
	using matrice_future = std::future<InfoMatriceResultPairPtr>;
	using matrice_promise_ptr = std::shared_ptr<matrice_promise>;
	///////////////////////////////
	using ContextType = DrawContext<STRINGTYPE, FLOATTYPE>;
	////////////////////////////////
	TEST_CLASS(MatSvgMatriceTests)
	{
	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
		}// tearDown
	public:
		TEST_METHOD(testSVGArrange)
		{
			MatRunnerType oRunner;
			STRINGTYPE sigle("mortal_data");
			// create our window
			svg_win my_window(&oRunner, sigle, MatCellType::histogCell);
			my_window.init_all();
			STRINGTYPE filename("d:\\temp\\mortal_test.svg");
			my_window.save(filename);
		}//testArrangeElemsOne
	};
}// namespace UnitTestGenInfo