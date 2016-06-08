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
		InfoColor(unsigned char r, unsigned char g, unsigned char b) :
			red(r), green(g), blue(b) {}
		InfoColor(unsigned char c) :red(c), green(c), blue(c) {}

	};// struct InfoColor
	/////////////////////////////////////////
	enum class MatCellType { noCell, varCell, indCell, summaryCell, histogCell, plainCell };
	////////////////////////////////////
	template <typename STRINGTYPE>
	class BaseDrawItem;
	/////////////////////////////////////////
	template <typename STRINGTYPE>
	class DrawContext {
		using DrawItem = BaseDrawItem<STRINGTYPE>;
	protected:
		InfoColor m_upcolor;
		InfoColor m_downcolor;
		InfoColor m_sumcolor;
		InfoColor m_textcolor;
	public:
		DrawContext() :m_upcolor(255), m_downcolor(0), m_sumcolor(255, 0, 0), m_textcolor(0) {}
		virtual ~DrawContext() {}
	public:
		virtual void draw(DrawItem *pItem, long x0, long y0) {
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
		unsigned long m_width;
		unsigned long m_height;
		unsigned long m_val;
		size_t m_ipos;
		size_t m_jpos;
		MatCellType m_type;
		STRINGTYPE m_text;
	public:
		BaseDrawItem() :m_width(0), m_width(0), m_val(0), m_ipos(-1), m_jpos(-1), m_type(MatCellType::noCell) {}
		BaseDrawItem(MatCellType t, size_t ipos, size_t jpos, unsigned long width, unsigned long height, unsigned long val) :m_width(width), m_height(height),
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
		virtual void draw(DrawContext<STRINGTYPE> *pContext, long xpos = 0, long ypos = 0) {
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
		void get_indexes(size_t &ipos, size_t &jpos) const {
			ipos = this->m_ipos;
			jpos = this->m_jpos;
		}
		void set_indexes(size_t ipos, size_t jpos) {
			this->m_ipos = ipos;
			this->m_jpos = jpos;
		}
		bool has_rect(void) const {
			return ((this->m_width > 0) && (this->m_height > 0));
		}
		const STRINGTYPE &text(void) const {
			return (this->m_text);
		}
		void text(const STRINGTYPE &s) {
			this->m_text = s;
		}
		unsigned long value(void) const {
			return (this->m_val);
		}
		void value(unsigned long n) {
			this->m_val = n;
		}
		unsigned long width(void) const {
			return (this->m_width);
		}
		void width(unsigned long w) {
			this->m_width = w;
		}
		unsigned long height(void) const {
			return (this->m_width);
		}
		void height(unsigned long h) {
			this->m_height = h;
		}
		void get_width_height(unsigned long &w, unsigned long &h) const {
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
		long m_x0;
		long m_y0;
		unsigned long m_deltax;
		unsigned long m_dx;
		unsigned long m_deltay;
		unsigned long m_dy;
		items_vector m_items;
		sizets_vector m_colindexes;
		sizets_vector m_rowindexes;
		//
	public:
		template <typename T>
		DrawItems(size_t nRows, size_t nCols,
			const std::vector<T> &data,
			unsigned long w = 29, unsigned long h = 29,
			unsigned long dx = 3, unsigned long dy = 3) :m_nrows(0), m_ncols(0),
			m_x0(0), m_y0(0), m_deltax(dx), m_dx(w), m_deltay(dy), m_dy(h) {
			this->initialize(nRows, nCols, data, w, h, dx, dy);
		}
		virtual ~DrawItems() {
			items_vector &vv = this->m_items;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				PDrawItemType p = *it;
				delete p;
			}
			vv.clear();
		}
	public:
		template <typename T>
		void initialize(size_t nRows, size_t nCols,
			const std::vector<T> &data,
			unsigned long w, unsigned long h, unsigned long dx, unsigned long dy) {
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
			double delta = (double)h / ((double)vMax - (double)vMin);
			this->set_parameters(w, h, dx, dy);
			this->resize(nRows, nCols);
			for (size_t i = 0; i < nRows; ++i) {
				for (size_t j = 0; j < nCols; ++j) {
					double v = (double)data[i * nCols + j];
					double f = ((v - (double)vMin) * delta + 0.5);
					unsigned long ival = (unsigned long)f;
					PDrawItemType *p = this->create_item(MatCellType::histogCell, i, j, ival);
				}// j
			}// i
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
		virtual void draw(DrawContext *pContext, long xpos = 0, long ypos = 0) {
			const size_t nRows = this->m_nrows;
			const size_t nCols = this->m_ncols;
			long y = ypos + this->m_y0;
			long x0 = xpos + this->m_x0;
			const unsigned long ddx = this->m_deltax + this->m_dx;
			const unsigned long ddy = this->m_deltay + this->m_dy;
			const sizets_vector &rowIndexes = this->m_rowindexes;
			const sizets_vector &colIndexes = this->m_colindexes;
			const items_vector &vv = this->m_items;
			const size_t nTotal = vv.size();
			for (size_t i = 0; i < nRows; ++i) {
				size_t ii = rowIndexes[i];
				long x = x0;
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
		void draw(const sizets_vector &rowIndexes, const sizets_vector &colIndexes,
			DrawContext *pContext, long xpos = 0, long ypos = 0) {
			this->row_indexes(rowIndexes);
			this->col_indexes(colIndexes);
			this->draw(pContext, xpos, ypos);
		}// draw
	protected:
		PDrawItemType create_item(MatCellType t, size_t iRow, size_t iCol, unsigned long ival) {
			PDrawItemType pRet(nullptr);
			const size_t nCols = this->m_ncols;
			if ((iRow >= this->m_nrows) || (iCol >= nCols)) {
				return (pRet);
			}
			items_vector &vv = this->m_items;
			size_t pos = (size_t)((iRow * nCols) + iCol);
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
				pRet->set_indexes(iRow, iCol);
				vv[pos] = pRet;
			}
			return (pRet);
		}// create_item
		void set_parameters(unsigned long w, unsigned long h, unsigned long dx, unsigned long dy) {
			this->m_dx = w;
			this->m_dy = h;
			this->m_deltax = dx;
			this->m_deltay = dy;
		}
		void resize(size_t nRows, size_t nCols) {
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
			double delta = (double)h / ((double)vMax - (double)vMin);
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
