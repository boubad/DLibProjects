#pragma once
#ifndef __INFO_DRAWCONTEXT_H__
#define __INFO_DRAWCONTEXT_H__
////////////////////////////////
#include "info_includes.h"
/////////////////////////////////
namespace info {
////////////////////////////////////////////
struct InfoColor {
public:
	using byte = unsigned char;
	//
	byte red;
	byte green;
	byte blue;
	InfoColor() :
			red(0), green(0), blue(0) {
	}
	InfoColor(byte r, byte g, byte b) :
			red(r), green(g), blue(b) {
	}
	InfoColor(byte c) :
			red(c), green(c), blue(c) {
	}
};
// struct InfoColor
///////////////////////////////////////
enum class MatriceDrawType {
	drawIndivs, drawVariables
};
/////////////////////////////////////
struct DrawContextParams {
public:
	using coord_type = double;
	using dist_type = double;
	//
	size_t nRows;
	size_t nCols;
	dist_type dmin;
	double fangle;
	int kfactor;
	dist_type variableFontSize;
	dist_type indivFontSize;
	dist_type width;
	dist_type height;
	MatriceDrawType drawType;
	bool bIndsNames;
	bool bIndsSum;
	bool bVarsNames;
	bool bVarsSum;
	coord_type x0;
	coord_type y0;
	dist_type deltax;
	dist_type dx;
	dist_type deltay;
	dist_type dy;
	InfoColor upcolor;
	InfoColor downcolor;
	InfoColor sumindcolor;
	InfoColor sumvarcolor;
	InfoColor textcolor;
	InfoColor donecolor;
	InfoColor strokecolor;
	//
	DrawContextParams() :
			nRows(0),nCols(0),dmin(16),fangle(285), kfactor(8), variableFontSize(15), indivFontSize(12), width(
					595), height(841), drawType(MatriceDrawType::drawIndivs), bIndsNames(
					true), bIndsSum(true), bVarsNames(true), bVarsSum(true), x0(
					0), y0(0), deltax(0), dx(32), deltay(0), dy(32), upcolor(
					255), downcolor(0), sumindcolor(0, 255, 0), sumvarcolor(255,
					0, 0), textcolor(0), donecolor(127), strokecolor(0) {
	}
	void update(size_t nRows, size_t nCols,bool bUpdate = true) {
		this->nRows = nRows;
		this->nCols = nCols;
		double fcol = (double) this->kfactor
				/ ((this->kfactor + 1) * (nRows + 3));
		this->dx = (dist_type) (fcol * this->width);
		this->deltax = (dist_type) (this->dx / this->kfactor);
		if ((this->dx < this->dmin) && bUpdate) {
			this->dx = this->dmin;
			this->deltax = (dist_type) (this->dx / this->kfactor);
			this->width = (dist_type) ((nRows + 3) * (this->dx + this->deltax));
		}
		double frow = (double) this->kfactor
				/ ((this->kfactor + 1) * (nCols + 3));
		this->dy = (dist_type) (frow * this->height);
		this->deltay = (dist_type) (this->dy / this->kfactor);
		if ((this->dy < this->dmin) && bUpdate) {
			this->dy = this->dmin;
			this->deltay = (dist_type) (this->dy / this->kfactor);
			this->height =
					(dist_type) ((nCols + 3) * (this->dy + this->deltay));
		}
	}	  // update
};
// struct DrawContextParams
/////////////////////////////////////////
enum class MatCellType {
	noCell,
	varCell,
	indCell,
	summaryVarCell,
	summaryIndCell,
	histogCell,
	plainCell
};
////////////////////////////////////
template<typename STRINGTYPE, typename FLOATTYPE>
class BaseDrawItem;
/////////////////////////////////////////
template<typename STRINGTYPE, typename FLOATTYPE>
class DrawContext {
public:
	using coord_type = DrawContextParams::coord_type;
	using dist_type = DrawContextParams::dist_type;
	using DrawItem = BaseDrawItem<STRINGTYPE, FLOATTYPE>;
	using ContextType = DrawContext<STRINGTYPE, FLOATTYPE>;
protected:
	DrawContextParams *m_params;
	FLOATTYPE m_fdummy;
	STRINGTYPE m_dummy;
	std::unique_ptr<DrawContextParams> m_oparam;
public:
	virtual void save(std::ostream &os, const std::string &stitle = std::string()) {
		// do nothing here
	}
	void save(std::wostream &wos, const std::wstring &sTitle = std::wstring()) {
		std::stringstream os;
		this->save(os);
		std::string s2 = os.str();
		std::wstring sz = info_2ws(s2);
		wos << sz;
	}// save
	virtual void save(const std::string &filename) {
		std::ofstream os(filename.c_str());
		this->save(os, filename);
	} // save
	virtual void save(const std::wstring &filename) {
		std::wofstream os(info_2s(filename.c_str()));
		this->save(os, filename);
	} // sav
	virtual void set_indivs_font(void) {
	}//set_indivs_font
	virtual void set_variables_font(void) {
	}//set_indivs_font
	virtual void set_variables_font_color(void) {
	}//set_variables_font_color
	virtual void set_indivs_summary_color(void) {
	}// set_indivs_summary_color
	virtual void set_variables_summary_color(void) {
	}// set_variables_summary_color
	virtual void set_histog_color(void) {
	}// set_histog_color
	virtual void set_plain_color(double f) {
	}// set_plain_color
	virtual void set_separator(void) {
	}
	virtual void set_endline(void) {
	}
public:
	DrawContext(DrawContextParams *pParams = nullptr) :
			m_params(pParams), m_fdummy(0) {
	}
	virtual ~DrawContext() {
	}
	const DrawContextParams *draw_params(void) const {
		const DrawContextParams *pRet = this->m_params;
		if (pRet == nullptr) {
			pRet = this->m_oparam.get();
			if (pRet == nullptr) {
				ContextType *p = const_cast<ContextType *>(this);
				p->m_oparam.reset(new DrawContextParams());
				pRet = const_cast<const DrawContextParams *>(p->m_oparam.get());
			}
		}
		return (pRet);
	}	  // draw_params
	DrawContextParams *draw_params(void) {
		DrawContextParams *pRet = this->m_params;
		if (pRet == nullptr) {
			pRet = this->m_oparam.get();
			if (pRet == nullptr) {
				this->m_oparam.reset(new DrawContextParams());
				pRet = this->m_oparam.get();
			}
		}
		return (pRet);
	}	  // draw_params
	virtual void draw(DrawItem *pItem, coord_type x0 = 0, coord_type y0 = 0) {
		// do nothing here
	}	  // draw
public:
	void get_origin(coord_type &x, coord_type &y) {
		const DrawContextParams *p = this->draw_params();
		x = p->x0;
		y = p->y0;
	}
	void get_cell_draw_params(dist_type &w, dist_type &dx, dist_type &h,
			dist_type &dy) {
		const DrawContextParams *p = this->draw_params();
		w = p->dx;
		dx = p->deltax;
		h = p->dy;
		dy = p->deltay;
	}
};
// class DrawContext<STRINGTYPE,FLOATTYPE>
////////////////////////////////////////
template<typename STRINGTYPE, typename FLOATTYPE>
class BaseDrawItem {
public:
	using coord_type = DrawContextParams::coord_type;
	using dist_type = DrawContextParams::dist_type;
	using ContextType = DrawContext<STRINGTYPE, FLOATTYPE>;
	using BaseDrawItemType = BaseDrawItem<STRINGTYPE, FLOATTYPE>;
private:
	FLOATTYPE m_val;
	MatCellType m_type;
	STRINGTYPE m_text;
public:
	BaseDrawItem() :
			m_val(0), m_type(MatCellType::noCell) {
	}
	BaseDrawItem(MatCellType t, FLOATTYPE val) :
			m_val(val), m_type(t) {
	}
	BaseDrawItem(MatCellType t, const STRINGTYPE &s) :
			m_val(0), m_type(t), m_text(s) {
	}
	BaseDrawItem(const BaseDrawItemType &other) :
			m_val(other.m_val), m_type(other.m_type), m_text(other.m_text) {
	}
	BaseDrawItemType & operator=(const BaseDrawItemType &other) {
		if (this != &other) {
			this->m_val = other.m_val;
			this->m_type = other.m_type;
			this->m_text = other.m_text;
		}
		return (*this);
	}
	virtual ~BaseDrawItem() {
	}
public:
	MatCellType type(void) const {
		return (this->m_type);
	}
	void type(MatCellType t) {
		this->m_type = t;
	}
	const STRINGTYPE &text(void) const {
		return (this->m_text);
	}
	void text(const STRINGTYPE &s) {
		this->m_text = s;
	}
	FLOATTYPE value(void) const {
		return (this->m_val);
	}
	void value(FLOATTYPE n) {
		this->m_val = n;
	}
public:
	virtual bool draw(ContextType *pContext, coord_type x0 = 0, coord_type y0 = 0) {
		return (true);
	}	   // draw
};
// class BaseDrawItem
///////////////////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__BASEDRAWITEM_H__
