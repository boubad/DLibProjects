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
using  MatriceWindowType = MatriceDisplayWindow<IDTYPE,DISTANCETYPE,STRINGTYPE,FLOATTYPE,INTTYPE,WEIGHTYPE>;
using strings_vector = std::vector<STRINGTYPE>;
////////////////////////////////////////
class matrice_win : public drawable_window
{
	MatriceWindowType mat_win;
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
	matrice_win(MatCellType aType = MatCellType::plainCell) : mat_win(*this)
	{
		size_t nRows = 0, nCols = 0;
		STRINGTYPE name;
		strings_vector rowNames, colNames;
		std::vector<DATATYPE> data;
		InfoTestData::get_mortal_data(name, nRows, nCols, data, rowNames, colNames);
		this->mat_win.compute(aType, name, nRows, nCols, data, rowNames, colNames);
		this->show();
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
