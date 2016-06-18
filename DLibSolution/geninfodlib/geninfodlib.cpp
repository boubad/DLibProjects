#include "stdafx.h"

#include <dlib/gui_widgets.h>
/////////////////////////
#include <infodlib/dlib_matrice.h>
//////////////////////////////
#include "infotestdata.h"
//////////////////////////////
#include "global_defs.h"
/////////////////////////////
using namespace std;
using namespace dlib;
using namespace info;
///////////////////////////////
using MatRunnerType = MatRunner<IDTYPE, STRINGTYPE, DISTANCETYPE, INTTYPE, WEIGHTYPE>;
////////////////////////////////////////
class matrice_win : public drawable_window
{
	using ModelDataType = MatriceModelData<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE, INTTYPE, WEIGHTYPE>;
	using  MatriceWindowType = MatriceDisplayWindow<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
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
private:
	MatRunnerType *m_prunner;
	MatriceWindowType mat_win;
	std::unique_ptr<ModelDataType> m_model;
	matrice_promise_ptr m_promise;
	matrice_future m_future;
	std::future<bool> m_init_future;
protected:
	
	void my_redim(void) {
		unsigned long h = 0, w = 0;
		this->get_size(w, h);
		h -= 5;
		w -= 5;
		this->mat_win.set_pos(2, 2);
		this->mat_win.set_size(w, h);
	}// my_redim
	virtual void on_window_resized() {
		this->my_redim();
	}
public:
	matrice_win(MatRunnerType *pRunner,const STRINGTYPE &name, MatCellType aType = MatCellType::plainCell) : 
		m_prunner(pRunner),mat_win(*this)
	{
		size_t nRows = 0, nCols = 0;
		strings_vector rowNames, colNames;
		std::vector<DATATYPE> data;
		InfoTestData::get_data(name, nRows, nCols, data, rowNames, colNames);
		ModelDataType *pModel = new ModelDataType(*this,pRunner);
		this->m_model.reset(pModel);
		this->m_init_future = pModel->initialize(name, nRows, nCols, data, rowNames, colNames,aType);
		this->show();
	}
	~matrice_win() {
	}
	void set_result(MatElemResultPtr oRes) {
		STRINGSTREAM os;
		os << "Matrice ";
		MatElemResultType *p = oRes.get();
		if (p != nullptr) {
			STRINGTYPE ss;
			p->to_string(ss, false);
			os << ss;
		}
		STRINGTYPE st = os.str();
		this->set_title(st);
		ModelDataType *pModel = this->m_model.get();
		assert(pModel != nullptr);
		pModel->set_result(oRes);
	}// oRes
	void init_all(void) {
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
};
//////////////////////////
int main()
{
	//
	MatRunnerType oRunner;
	STRINGTYPE sigle("mortal_data");
	// create our window
	matrice_win my_window(&oRunner,sigle,MatCellType::histogCell);
	my_window.init_all();
	// wait until the user closes this window before we let the program 
	// terminate.
	my_window.wait_until_closed();

	return 0;
}
//  ----------------------------------------------------------------------------

// Normally, if you built this application on MS Windows in Visual Studio you
// would see a black console window pop up when you ran it.  The following
// #pragma directives tell Visual Studio to not include a console window along
// with your application.  However, if you prefer to have the console pop up as
// well then simply remove these #pragma statements.
#ifdef _MSC_VER
#   pragma comment( linker, "/entry:mainCRTStartup" )
#   pragma comment( linker, "/SUBSYSTEM:WINDOWS" )
#endif

//  ----------------------------------------------------------------------------
