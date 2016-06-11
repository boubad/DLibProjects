// infodlib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <dlib/gui_widgets.h>
#include <sstream>
#include <string>
/////////////////////////
#include <infodlib/dlib_matrice.h>
//////////////////////////////
#include <infotestdata.h>
//////////////////////////////
#include <global_defs.h>
/////////////////////////////
using namespace std;
using namespace dlib;
using namespace info;
//////////////////////////////
using DrawItemsType = DrawItems<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
using ints_vector = std::vector<IDTYPE>;
using doubles_vector = std::vector<double>;
using strings_vector = std::vector<STRINGTYPE>;
using ints_doubles_map = std::map<IDTYPE, double>;
////////////////////////////////////////
using IntraMatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
using RescritType = std::atomic<DISTANCETYPE>;
using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
using queue_type = SharedQueue<IntraMatElemResultPtr>;
////////////////////////
using MatOrdType = IntraMatOrd<IDTYPE, DISTANCETYPE, STRINGTYPE>;
using backgrounder = MatElemResultBackgounder<IDTYPE, DISTANCETYPE, STRINGTYPE>;
////////////////////////////////
using MatriceDataType = MatriceData<IDTYPE, STRINGTYPE, DATATYPE>;
using MatriceWindowType = MatriceDisplayWindow<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE, DATATYPE>;
using MatElemCritGraphWindowType = MatElemCritGraphWindow<IDTYPE, DISTANCETYPE, STRINGTYPE>;
//////////////////////////////////////////
class matrice_win : public drawable_window
{
	widget_group m_group;
	MatriceWindowType mat_win;
	MatElemCritGraphWindowType mat_graph;
	label lblVar;
	label lblInd;
	label lblStatus;
	MatriceDataType m_data;
protected:
	void process_result(IntraMatElemResultPtr oRes) {
		STRINGTYPE sRes;
		label *pLabel = nullptr;
		IntraMatElemResultType *p = oRes.get();
		if (p != nullptr) {
			p->to_string(sRes);
			if (p->disposition == DispositionType::indiv) {
				pLabel = &(lblInd);
			}
			else if (p->disposition == DispositionType::variable) {
				pLabel = &(lblVar);
			}
		}// p
		if (pLabel != nullptr) {
			pLabel->set_text(sRes);
		}
	}// process_result
	void my_redim(void) {
		unsigned long h = 0, w = 0;
		this->get_size(w, h);
		long x2 = w / 2;
		x2 += 5;
		lblVar.set_pos(5, 5);
		lblInd.set_pos(x2, 5);
		unsigned long hh = (5 + lblVar.height() + 5);
		unsigned long hy = h - hh;
		unsigned long graph_width = w / 4;
		unsigned long mat_width = w - (graph_width + 5);
		mat_win.set_size(mat_width, hy);
		mat_graph.set_size(graph_width, hy);
		m_group.fit_to_contents();
		this->mat_win.window_resized();
		//this->mat_graph.window_resized();
	}// my_redim
	virtual void on_window_resized() {
		this->my_redim();
	}
public:
	matrice_win(MatCellType aType = MatCellType::plainCell) : m_group(*this),
		mat_win(*this),mat_graph(*this),lblVar(*this),lblInd(*this),lblStatus(*this)
	{
		mat_win.set_graph_window(&mat_graph);
		this->set_size(600, 600);
		unsigned long h = 0, w = 0;
		this->get_size(w, h);
		long x2 = w / 2;
		x2 += 5;
		lblVar.set_pos(5, 5);
		lblInd.set_pos(x2, 5);
		unsigned long hh = (5 + lblVar.height() + 5);
		unsigned long hy = h - hh;
		unsigned long graph_width = w / 4;
		unsigned long mat_width = w - (graph_width + 5);
		m_group.set_pos(0, hh);
		m_group.add(mat_graph, 0, 0);
		m_group.add(mat_win, graph_width + 5, 0);
		mat_win.set_size(mat_width, hy);
		mat_graph.set_size(graph_width, hy);
		m_group.fit_to_contents();
		//this->my_redim();
		size_t nRows = 0, nCols = 0;
		STRINGTYPE name;
		std::vector<STRINGTYPE> rowNames, colNames;
		std::vector<DATATYPE> data;
		InfoTestData::get_test_data(name, nRows, nCols, data, rowNames, colNames);
		this->m_data.initialize(name, nRows, nCols, data, rowNames, colNames);
		this->mat_win.set_callback([this](IntraMatElemResultPtr oRes) {
			this->process_result(oRes);
		});
		this->mat_win.arrange(&this->m_data,aType);
		this->mat_win.show();
		DrawContextParams *pParams = this->mat_win.get_context_params();
		pParams->drawType = MatriceDrawType::drawVariables;
		show();
	}
	~matrice_win() {
	}

};
//////////////////////////
int main()
{
	// create our window
	matrice_win my_window(MatCellType::histogCell);
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
