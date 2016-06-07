#pragma once
#ifndef __BASEDRAWITEM_H__
#define __BASEDRAWITEM_H__
////////////////////////////////
#include "matresult.h"
////////////////////////////////
namespace info {
	////////////////////////////////////////////
	struct InfoColor {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		InfoColor() :red(0), green(0), blue(0) {}
	};// struct InfoColor
	/////////////////////////////////////////
	enum class MatCellType { noCell, varCell, indCell, summaryCell, histogCell, plainCell };
	////////////////////////////////////
	template <typename STRINGTYPE>
	class BaseDrawItem<STRINGTYPE>;
	/////////////////////////////////////////
	template <typename STRINGTYPE>
	class DrawContext {
		using DrawItem = BaseDrawItem<STRINGTYPE>;
	public:
		DrawContext() {}
		virtual ~DrawContext() {}
	public:
		virtual void draw(DrawItem *pItem, int x0, int y0) {
			// do nothing here
		}
	private:
		STRINGTYPE m_dummy;
	}; // class DrawContext<STRINGTYPE>
	////////////////////////////////////////
	template <typename STRINGTYPE>
	class BaseDrawItem {
	public:
		using BaseDrawItemType = BaseDrawItem<STRINGTYPE>;
	private:
		int	m_width;
		int m_height;
		int m_val;
		int m_ipos;
		int m_jpos;
		MatCellType m_type;
		STRINGTYPE m_text;
	public:
		BaseDrawItem() :m_width(0), m_width(0), m_val(0), m_ipos(-1), m_jpos(-1), m_type(MatCellType::noCell) {}
		BaseDrawItem(MatCellType t, int ipos, int jpos, int width, int height, int val) :m_width(width), m_height(height),
			m_val(val), m_ipos(ipos), m_jpos(jpos), m_type(t) {

		}
		BaseDrawItem(const BaseDrawItemType &other) :m_width(other.m_width), m_weight(other.m_weight),
			m_val(other.m_val), m_ipos(other.m_ipos), m_jpos(other.m_jpos), m_type(other.m_type), m_text(other.m_text) {}
		BaseDrawItemType & operator=(const BaseDrawItemType &other) {
			if (this != &other) {
				this->m_width = other.m_width;
				this->m_height = other.m_height;
				this->m_val = other.m_val;
				this->m_ipos = other.m_ipos;
				this->m_jpos = other.m_jpos;
				this->m_type = other.m_type;
				this->m_text = other.m_text;
			}
			return (*this);
		}
		virtual ~BaseDrawItem() {}
	public:
		virtual void draw(DrawContext<STRINGTYPE> *pContext, int xpos = 0, int ypos = 0) {
			if (pContext != nullptr) {
				pContext->draw(this, xpos, ypos);
			}
		}
	public:
		MatCellType type(void) const {
			return (this->m_type);
		}
		void type(MatCellType t) {
			this->m_type = t;
		}
		void get_indexes(int &ipos, int &jpos) const {
			ipos = this->m_ipos;
			jpos = this->m_jpos;
		}
		void set indexes(int ipos, int jpos) {
			this->m_ipos = ipos;
			this->m_jpos = jpos;
		}
		bool has_rect(void) const {
			return ((this->m_width > 0) && (this->m_height > 0);
		}
		const STRINGTYPE &text(void) const {
			return (this->m_text);
		}
		void text(const STRINGTYPE &s) {
			this->m_text = s;
		}
		int value(void) const {
			return (this->m_val);
		}
		void value(int n) {
			this->m_val = n;
		}
		int width(void) const {
			return (this->m_width);
		}
		void width(int w) {
			this->m_width = w;
		}
		int height(void) const {
			return (this->m_width);
		}
		void height(int h) {
			this->m_height = h;
		}
		void get_width_height(int &w, int &h) const {
			w = this->m_width;
			h = this->m_height;
		}
	};// class BaseDrawItem
	//////////////////////////////////////////
	template <typename STRINGTYPE>
	class DrawItems : public BaseDrawItem<STRINGTYPE>, private boost::noncopyable {
	public:
		using BaseDrawItemType = BaseDrawItem<STRINGTYPE>;
		using PDrawItemType = BaseDrawItemType *;
		using items_vector = std::vector<PDrawItemType>;
		using DrawItemsType = DrawItems<STRINGTYPE>;
		using sizets_vector = std::vector<size_t>;
		using ContextType = DrawContext<STRINGTYPE>;
	private:
		size_t m_nrows;
		size_t m_ncols;
		ContextType *m_pcontext;
		int m_x0;
		int m_y0;
		int m_deltax;
		int m_dx;
		int m_deltay;
		int m_dy;
		items_vector m_items;
		sizets_vector m_colindexes;
		sizets_vector m_rowindexes;
		//
	public:
		DrawItems(ContextType *pContext = nullptr,
			int w = 29, int h = 29, int dx = 3, int dy = 3) :m_nrows(0), m_ncols(0), m_pcontext(pContext), m_x0(0), m_y0(0), m_deltax(dx), m_dx(w), m_deltay(dy), m_dy(h) {}
		virtual ~DrawItems() {
			items_vector &vv = this->m_items;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				PDrawItemType p = *it;
				delete p;
			}
			vv.clear();
		}
	public:
		PDrawItemType create_item(MatCellType t, size_t iRow, size_t iCol) {
			PDrawItemType pRet(nullptr);
			if ((iRow >= this->m_nrows) || (iCol >= this->m_ncols)) {
				return (pRet);
			}
			items_vector &vv = this->m_items;
			size_t pos = (size_t)(iRow * this->m_ncols + iCol);
			if (pos >= vv.size()) {
				return (pRet);
			}
			pRet = vv[pos];
			if (pRet != nullptr) {
				pRet->type(t);
			}
			else {
				pRet = new BaseDrawItemType();
				assert(pRet != nullptr);
				pRet->type(t);
				pRet->width(this->m_dx);
				pRet->height(this->m_dy);
				pRet->get_indexes((int)iRow, (int)iCol);
				vv[pos] = pRet;
			}
			return (pRet);
		}// create_item
		void set_parameters(int w, int h, int dx, int dy) {
			this->m_dx = w;
			this->m_dy = h;
			this->m_deltax = dx;
			this->m_deltay = dy;
		}
		virtual void draw(DrawContext *pContext, int xpos = 0, int ypos = 0) {
			const size_t nRows = this->m_nrows;
			const size_t nCols = this->m_ncols;
			int y = ypos + this->m_y0;
			int x0 = xpos + this->m_x0;
			int ddx = this->m_deltax + this->m_dx;
			int ddy = this->m_deltay + this->m_dy;
			const sizets_vector &rowIndexes = this->m_rowindexes;
			const sizets_vector &colIndexes = this->m_colindexes;
			const items_vector &vv = this->m_items;
			const size_t nTotal = vv.size();
			for (size_t i = 0; i < nRows; ++i) {
				size_t ii = rowIndexes[i];
				int x = x0;
				for (size_t j = 0; j < nCols; ++j) {
					size_t jj = colIndexes[j];
					size_t pos = ii * nCols + jj;
					if (pos < nTotal) {
						const PDrawItemType p = vv[pos];
						if (p != nullptr) {
							p->draw(pContext, x, y);
						}// p
					}// pos
					x += ddx;
				}// j
				y += ddy;
			}// i
		}// draw
		void clear(void) {
			items_vector &vv = this->m_items;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				PDrawItemType p = *it;
				delete p;
			}
			vv.clear();
		}// clear
		void resize(size_t nRows, size_t nCols, ContextType *pContext = nullptr) {
			if (pContext != nullptr) {
				this->m_pcontext = pContext;
			}
			items_vector &vv = this->m_items;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				PDrawItemType p = *it;
				delete p;
			}
			vv.clear();
			size_t nn = (size_t)(nRows * nCols);
			PDrawItemType pNull(nullptr);
			vv.resize(nn, pNull);
			this->m_ncols = nCols;
			this->m_nrows = nRows;
			this->m_rowindexes.resize(nRows);
			this->m_colindexes.resize(nCols);
			for (size_t i = 0; i < nRows; ++i) {
				(this->m_rowindexes)[i] = i;
			}
			for (size_t j = 0; j < nCols; ++j) {
				(this->m_colindexes)[i] = i;
			}
		}// resize
		void row_indexes(const sizets_vector &oInds) {
			if (oInds.size() >= this->m_nrows) {
				this->m_rowindexes = oInds;
			}
		}
		void col_indexes(const sizets_vector &oInds) {
			if (oInds.size() >= this->m_ncols) {
				this->m_colindexes = oInds;
			}
		}
	};// class DrawItems<STRINGTYPE>
	///////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE>
	class DrawMatriceBackgrounder : public MatElemResultBackgounder<IDTYPE, DISTANCETYPE, STRINGTYPE> {
	public:
		using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
		using BaseDrawItemType = BaseDrawItem<STRINGTYPE>;
		using PDrawItemType = BaseDrawItemType *;
		using DrawItemsType = DrawItems<STRINGTYPE>;
		using ContextType = DrawContext<STRINGTYPE>;
	private:
		ContextType *m_pcontext;
		DrawItemsType m_items;
	public:
		DrawMatriceBackgrounder(ContextType *pContext = nullptr) :m_pcontext(pContext), m_items(pContext) {
		}
		virtual ~DrawMatriceBackgrounder() {
		}
	public:
		template <typename T>
		void resize(size_t nRows, size_t nCols,
			const std::vector<T> &data,
			int w, int h, int dx, int dy,
			ContextType *pContext = nullptr) {
			assert(nRows > 0);
			assert(nCols > 0);
			assert(data.size() >= (size_t)(nCols * nRows));
			assert(w > 0);
			assert(h > 0);
			assert(dx >= 0);
			assert(dy >= 0);
			T vMin = std::min_element(data.begin(), data.end());
			T vMax = std::max_element(data.begin(), data.end());
			assert(vMin < vMax);
			double delta = (double) h / ((double)vMax - (double)vMin);
			if (pContext != nullptr) {
				this->m_pcontext = oContext;
			}
			this->m_items.set_parameters(w, h, dx, dy);
			this->m_items.resize(nRows, nCols, this->m_pcontext);
			for (size_t i = 0; i < nRows; ++i) {
				for (size_t j = 0; j < nCols; ++j) {
					double v = (double)data[i * nCols + j];
					double f = ((v - (double)vMin) * delta + 0.5);
					int ival = (int)f;
					PDrawItemType *p = this->m_items.create_item(MatCellType::histogCell, i, j);
					assert(p != nullptr);
					p->value(ival);
				}// j
			}// i
		}// resize
		void draw(DrawContext *pContext, int xpos = 0, int ypos = 0) {
			this->m_items.draw(pContext, xpos, ypos);
		}// draw
	protected:
		virtual void process_result(IntraMatElemResultPtr oRes) {
			IntraMatElemResultType *p = oRes.get();
			if (p != nullptr) {
				if (p->disposition == DispositionType::indiv) {
					this->m_items.row_indexes(p->second);
				}
				else if (p->disposition == DispositionType::variable) {
					this->m_items.col_indexes(p->second);
				}
			}// p
		}// process_result

	}; // class IntraMatElemBackgrounder<IDTYPE,DISTANCETYPE,STRINGTYPE>
	   ////////////////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__BASEDRAWITEM_H__
