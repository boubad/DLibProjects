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
using DrawItemsType = DrawItems<STRINGTYPE, FLOATTYPE>;
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
using ConnectionType = boost::signals2::connection;
////////////////////////////////
using MatriceDataType = MatriceData<IDTYPE, STRINGTYPE, DATATYPE>;
//////////////////////////////////////////
class matrice_win : public drawable_window
{
	std::atomic<bool> m_donevar;
	std::atomic<bool> m_doneind;
	std::atomic<bool> m_cancel;
	std::unique_ptr<MatriceDataType> m_data;
	DrawContextParams oDrawContextParams;
	std::unique_ptr<DrawItemsType> m_items;
	std::unique_ptr<MatOrdType> m_matord;
	backgrounder m_backgrounder;
	ConnectionType m_conn;
	STRINGTYPE m_sigle;
	std::unique_ptr<std::thread> m_runnable;
public:
	matrice_win() :m_donevar(false), m_doneind(false), m_cancel(false)
	{
		STRINGTYPE name;
		size_t nRows = 0, nCols = 0;
		strings_vector rowNames, colNames;
		std::vector<DATATYPE> data;
		InfoTestData::get_test_name(name);
		InfoTestData::get_data(name, nRows, nCols, data, rowNames, colNames);
		this->m_data.reset(new MatriceDataType(name, nRows, nCols, data, rowNames, colNames));

		this->init_data();
		this->m_conn = this->m_backgrounder.connect([this](IntraMatElemResultPtr oRes) {
			DrawItemsType *pItems = this->m_items.get();
			pItems->set_result(oRes);
			this->my_draw();
		});
		this->m_runnable.reset(new std::thread([this]() {
			this->run_matrice();
		}));
		show();
	}

	~matrice_win() {
		this->m_cancel.store(true);
		std::thread *p = this->m_runnable.get();
		if (p != nullptr) {
			p->join();
		}
		close_window();
	}
protected:
	virtual void on_window_resized() {
		unsigned long ww = 0, hh = 0;
		this->get_size(ww, hh);
		if ((ww > 0) && (hh > 0)) {
			MatriceDataType *pMat = this->m_data.get();
			assert(pMat != nullptr);
			//
			size_t nRows = pMat->rows_count();
			size_t nCols = pMat->cols_count();
			unsigned long w = ww / (nCols + 2);
			unsigned long h = hh / (nRows + 2);
			if (w < 8) {
				w = 8;
			}
			if (h < 8) {
				h = 8;
			}
			oDrawContextParams.dx = w;
			oDrawContextParams.dy = h;
			my_draw();
		}
	}
private:
	void my_draw(void) {
		unsigned long ww = 0, hh = 0;
		this->get_size(ww, hh);
		rectangle r(0, 0, ww, hh);
		this->invalidate_rectangle(r);
	}
	virtual void paint(const canvas& c)
	{
		DrawItemsType *p = this->m_items.get();
		if (p != nullptr) {
			unsigned long ww = 0, hh = 0;
			this->get_size(ww, hh);
			rectangle r(0, 0, ww, hh);
			fill_rect(c, r, rgb_pixel(255, 255, 255));
			DLibDrawContext oContext(c, &oDrawContextParams);
			p->draw(&oContext);
		}// p
	}// draw
	void run_matrice(void) {
		MatriceDataType *pData = this->m_data.get();
		assert(pData != nullptr);
		//
		this->m_matord.reset(new MatOrdType(&m_backgrounder, &(this->m_cancel)));
		this->oDrawContextParams.bIndsSum = false;
		this->oDrawContextParams.bVarsSum = false;
		this->m_conn = this->m_backgrounder.connect([this](IntraMatElemResultPtr oRes) {
			IntraMatElemResultType *p = oRes.get();
			if (p != nullptr) {
				if (p->stage == StageType::finished) {
					if (p->disposition == DispositionType::indiv) {
						this->m_doneind.store(true);
					}
					else if (p->disposition == DispositionType::variable) {
						this->m_donevar.store(true);
					}
					if (this->m_donevar.load() && this->m_doneind.load()) {
						DrawContextParams &ctx = this->oDrawContextParams;
						ctx.downcolor = ctx.donecolor;
						ctx.bIndsSum = true;
						ctx.bVarsSum = true;
						this->my_draw();
					}
				}// finished
				DrawItemsType *pItems = this->m_items.get();
				pItems->set_result(oRes);
				this->my_draw();
			}
		});
		MatOrdType *pMat = this->m_matord.get();
		pMat->arrange(pData->indiv_provider(), pData->variable_provider());
	}// run_matrice
	void init_data(void) {
		MatriceDataType *pMat = this->m_data.get();
		assert(pMat != nullptr);
		//
		size_t nRows = pMat->rows_count();
		size_t nCols = pMat->cols_count();
		const std::vector<DATATYPE> & data = pMat->data();
		std::vector<double> varsSum, indsSum;
		varsSum.resize(nCols);
		indsSum.resize(nRows);
		for (size_t i = 0; i < nCols; ++i) {
			double s1 = 0;
			double s2 = 0;
			for (size_t j = 0; j < nRows; ++j) {
				double x = data[j * nCols + i];
				s1 += x;
				s2 += x * x;
			}// j
			s1 /= nRows;
			s2 /= nRows;
			s2 = std::sqrt(s2 - (s1 * s1));
			varsSum[i] = s2 / s1;
		}// i
		for (size_t i = 0; i < nRows; ++i) {
			indsSum[i] = data[i * nCols];
		}
		DrawItemsType *p = new DrawItemsType();
		assert(p != nullptr);
		//MatCellType aType(MatCellType::histogCell);
		MatCellType aType(MatCellType::plainCell);
		const strings_vector &rowNames = pMat->rows_names();
		const strings_vector &colNames = pMat->cols_names();
		bool bRet = p->initialize(aType, nRows, nCols, data, rowNames, colNames, indsSum, varsSum);
		assert(bRet);
		this->m_items.reset(p);
		STRINGTYPE s = pMat->name();
		set_title(s);
		this->set_size(600, 600);
	}// init_data

};
//////////////////////////
//  ----------------------------------------------------------------------------

class color_box : public draggable
{
	/*
	Here I am defining a custom drawable widget that is a colored box that
	you can drag around on the screen.  draggable is a special kind of drawable
	object that, as the name implies, is draggable by the user via the mouse.
	To make my color_box draggable all I need to do is inherit from draggable.
	*/
	unsigned char red, green, blue;

public:
	color_box(
		drawable_window& w,
		rectangle area,
		unsigned char red_,
		unsigned char green_,
		unsigned char blue_
	) :
		draggable(w),
		red(red_),
		green(green_),
		blue(blue_)
	{
		rect = area;
		set_draggable_area(rectangle(10, 10, 400, 400));

		// Whenever you make your own drawable widget (or inherit from any drawable widget 
		// or interface such as draggable) you have to remember to call this function to 
		// enable the events.  The idea here is that you can perform whatever setup you 
		// need to do to get your object into a valid state without needing to worry about 
		// event handlers triggering before you are ready.
		enable_events();
	}

	~color_box(
	)
	{
		// Disable all further events for this drawable object.  We have to do this 
		// because we don't want any events (like draw()) coming to this object while or 
		// after it has been destructed.
		disable_events();

		// Tell the parent window to redraw its area that previously contained this
		// drawable object.
		parent.invalidate_rectangle(rect);
	}

private:

	void draw(
		const canvas& c
	) const
	{
		// The canvas is an object that represents a part of the parent window
		// that needs to be redrawn.  

		// The first thing I usually do is check if the draw call is for part
		// of the window that overlaps with my widget.  We don't have to do this 
		// but it is usually good to do as a speed hack.  Also, the reason
		// I don't have it set to only give you draw calls when it does indeed
		// overlap is because you might want to do some drawing outside of your
		// widget's rectangle.  But usually you don't want to do that :)
		rectangle area = c.intersect(rect);
		if (area.is_empty() == true)
			return;

		// This simple widget is just going to draw a box on the screen.   
		fill_rect(c, rect, rgb_pixel(red, green, blue));
	}
};

//  ----------------------------------------------------------------------------

class win : public drawable_window
{
	/*
	Here I am going to define our window.  In general, you can define as
	many window types as you like and make as many instances of them as you want.
	In this example I am only making one though.
	*/
public:
	win(
	) : // All widgets take their parent window as an argument to their constructor.
		c(*this),
		b(*this),
		cb(*this, rectangle(100, 100, 200, 200), 0, 0, 255), // the color_box will be blue and 101 pixels wide and tall
		mbar(*this)
	{
		// tell our button to put itself at the position (10,60). 
		b.set_pos(10, 60);
		b.set_name("button");

		// let's put the label 5 pixels below the button
		c.set_pos(b.left(), b.bottom() + 5);


		// set which function should get called when the button gets clicked.  In this case we want
		// the on_button_clicked member to be called on *this.
		b.set_click_handler(*this, &win::on_button_clicked);
		// Alternatively, if you have a compiler which supports the lambda functions from the
		// new C++ standard then you can use a lambda function instead of telling the click
		// handler to call one of the member functions.  So for example, you could do this
		// instead (uncomment the code if you have C++0x support):
		/*
		b.set_click_handler([&](){
		++counter;
		ostringstream sout;
		sout << "Counter: " << counter;
		c.set_text(sout.str());
		});
		*/
		// In general, all the functions which register events can take either member 
		// functions or lambda functions.


		// Let's also make a simple menu bar.  
		// First we say how many menus we want in our menu bar.  In this example we only want 1.
		mbar.set_number_of_menus(1);
		// Now we set the name of our menu.  The 'M' means that the M in Menu will be underlined
		// and the user will be able to select it by hitting alt+M
		mbar.set_menu_name(0, "Menu", 'M');

		// Now we add some items to the menu.  Note that items in a menu are listed in the
		// order in which they were added.

		// First let's make a menu item that does the same thing as our button does when it is clicked.
		// Again, the 'C' means the C in Click is underlined in the menu. 
		mbar.menu(0).add_menu_item(menu_item_text("Click Button!", *this, &win::on_button_clicked, 'C'));
		// let's add a separator (i.e. a horizontal separating line) to the menu
		mbar.menu(0).add_menu_item(menu_item_separator());
		// Now let's make a menu item that calls show_about when the user selects it.  
		mbar.menu(0).add_menu_item(menu_item_text("About", *this, &win::show_about, 'A'));


		// set the size of this window
		set_size(430, 380);

		counter = 0;

		set_title("dlib gui example");
		show();
	}

	~win(
	)
	{
		// You should always call close_window() in the destructor of window
		// objects to ensure that no events will be sent to this window while 
		// it is being destructed.  
		close_window();
	}

private:

	void on_button_clicked(
	)
	{
		// when someone clicks our button it will increment the counter and 
		// display it in our label c.
		++counter;
		ostringstream sout;
		sout << "counter: " << counter;
		c.set_text(sout.str());
	}

	void show_about(
	)
	{
		message_box("About", "This is a dlib gui example program");
	}

	unsigned long counter;
	label c;
	button b;
	color_box cb;
	menu_bar mbar;
};

//  ----------------------------------------------------------------------------

int main()
{
	// create our window
	matrice_win my_window;


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
