#pragma once
#ifndef __DLIB_MATRICE__H__
#define __DLIB_MATRICE__H__
/////////////////////////////////
#include <dlib/threads.h>
#include <dlib/gui_widgets.h>
///////////////////////////////////
#include "../intramat.h"
#include "../base_drawitem.h"
#include "../datavector_indivprovider.h"
//////////////////////////////////////
namespace info {
	////////////////////////////////////////////
	template <typename STRINGTYPE, typename FLOATTYPE>
	class DLibDrawContext : public info::DrawContext<STRINGTYPE, FLOATTYPE> {
		using DrawItem = BaseDrawItem<STRINGTYPE, FLOATTYPE>;
		const dlib::canvas &m_canvas;
	public:
		DLibDrawContext(const dlib::canvas &m, const DrawContextParams *params) :info::DrawContext<STRINGTYPE, FLOATTYPE>(params), m_canvas(m) {
		}
		virtual ~DLibDrawContext() {}
	public:
		virtual void draw(DrawItem *pItem, coord_type x0 = 0, coord_type y0 = 0) const {
			if (pItem == nullptr) {
				return;
			}
			const DrawContextParams *pParams = this->draw_params();
			assert(pParams != nullptr);
			dist_type w = pParams->dx;
			dist_type  h = pParams->dy;
			if ((w <= 0) || (h <= 0)) {
				return;
			}
			dlib::rectangle r(x0, y0, x0 + w, y0 + h);
			dlib::rectangle area = m_canvas.intersect(r);
			if (area.is_empty() == true) {
				return;
			}
			MatCellType aType = pItem->type();
			switch (aType)
			{
			case MatCellType::summaryIndCell:
			{
				double v = (double)pItem->value();
				if ((v >= 0.0) && (v <= 1.0)) {
					dist_type hx = (h < w) ? h : w;
					double rf = hx * std::sqrt(v);
					int r = (int)rf;
					dlib::point center(x0 + (w / 2), y0 + (h / 2));
					dlib::rgb_pixel color(pParams->sumindcolor.red, pParams->sumindcolor.green, pParams->sumindcolor.blue);
					dlib::draw_solid_circle(m_canvas, center, r, color);
				}
			}
			break;
			case MatCellType::summaryVarCell:
			{
				double v = (double)pItem->value();
				if ((v >= 0.0) && (v <= 1.0)) {
					dist_type hx = (h < w) ? h : w;
					double rf = hx * std::sqrt(v);
					int r = (int)rf;
					dlib::point center(x0 + (w / 2), y0 + (h / 2));
					dlib::rgb_pixel color(pParams->sumvarcolor.red, pParams->sumvarcolor.green, pParams->sumvarcolor.blue);
					dlib::draw_solid_circle(m_canvas, center, r, color);
				}
			}
			break;
			case MatCellType::plainCell:
			{
				double v = (double)pItem->value();
				if ((v >= 0.0) && (v <= 1.0)) {
					double f = 256.0 * v;
					int n = (int)f;
					if (n > 255) {
						n = 255;
					}
					dlib::rectangle rr(x0, y0, x0 + w, y0 + h);
					dlib::rgb_pixel color((unsigned char)n, (unsigned char)n, (unsigned char)n);
					dlib::fill_rect(m_canvas, rr, color);
				}
			}
			break;
			case MatCellType::histogCell:
			{
				double v = (double)pItem->value();
				if ((v >= 0.0) && (v <= 1.0)) {
					double f = h * v;
					int hx = (int)f;
					dlib::rectangle rr(x0, y0 + h - hx, x0 + w, y0 + h);
					unsigned char c = pParams->downcolor.red;
					dlib::rgb_pixel color(c, c, c);
					dlib::fill_rect(m_canvas, rr, color);
				}
			}
			break;
			default:
				break;
			}
		}// draw
	};// class DLibDrawContext
	/////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE>
	class MatOrdAgent : public dlib::threaded_object {
	public:
		using MatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using DistanceMapType = typename MatElemType::DistanceMapType;
		using MatElemResultType = typename MatElemType::IntraMatElemResultType;
		using MatElemResultPtr = typename MatElemType::IntraMatElemResultPtr;
		using SourceType = typename MatElemType::SourceType;
		using MatOrdType = IntraMatOrd<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using queue_type = MatElemResultClient<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	private:
		queue_type *m_pdest;
		std::atomic<bool> m_cancel;
		SourceType *m_pIndSource;
		SourceType *m_pVarSource;
		std::unique_ptr<MatOrdType> m_matord;
	private:
		virtual void thread() {
			if ((this->m_pIndSource != nullptr) && (this->m_pVarSource != nullptr)) {
				MatOrdType *pMat = m_matord.get();
				if (pMat != nullptr) {
					this->m_cancel.store(false);
					pMat->arrange(this->m_pIndSource, this->m_pVarSource, this->m_pdest);
				}// pMat
			}// soucres
		}// thread
	public:
		MatOrdAgent(queue_type *pDest = nullptr) :m_pdest(pDest), m_cancel(false), m_pIndSource(nullptr), m_pVarSource(nullptr) {
			m_matord.reset(new MatOrdType(m_pdest, &m_cancel));
			MatOrdType *pMat = m_matord.get();
			assert(pMat != nullptr);
		}
		MatOrdAgent(SourceType *pInd, SourceType *pVar, queue_type *pDest = nullptr) : m_pdest(pDest),
			m_cancel(false), m_pIndSource(pInd), m_pVarSource(pVar) {
			m_matord.reset(new MatOrdType(m_pdest, &m_cancel));
			MatOrdType *pMat = m_matord.get();
			assert(pMat != nullptr);
			this->start();
		}
		virtual ~MatOrdAgent() {}
	public:
		bool arrange(SourceType *pInd, SourceType *pVar, queue_type *pDest = nullptr) {
			assert(pInd != nullptr);
			assert(pVar != nullptr);
			if (!this->is_running()) {
				this->m_pIndSource = pInd;
				this->m_pVarSource = pVar;
				if (pDest != nullptr) {
					this->m_pdest = pDest;
				}
				this->start();
				return (true);
			}
			return (false);
		}// arrange
		void cancel(void) {
			if (this->is_running()) {
				this->m_cancel.store(true);
				this->stop();
			}
		}// cancel
	};// class MatOrdAgent<IDTYPE,DISTANCETYPE,STRINGTYPE>
	////////////////////////////////////////////////
	template <typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE>
	class MatElemCritGraphWindow : public dlib::button {
	public:
		using MatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatElemResultType = typename MatElemType::IntraMatElemResultType;
		using MatElemResultPtr = typename MatElemType::IntraMatElemResultPtr;
		using criterias_vector = std::vector<DISTANCETYPE>;
	private:
		DrawContextParams *m_params;
		criterias_vector m_inddata;
		criterias_vector m_vardata;
	public:
		MatElemCritGraphWindow(dlib::drawable_window& w, DrawContextParams *pParams = nullptr) : dlib::button(w), m_params(pParams) {
			this->window_resized();
		}
		virtual ~MatElemCritGraphWindow() {}
		void set_draw_params(DrawContextParams *p) {
			this->m_params = p;
		}
		void set_result(MatElemResultPtr oRes) {
			MatElemResultType *p = oRes.get();
			if (p != nullptr) {
				DISTANCETYPE c = p->first;
				if (p->disposition == DispositionType::indiv) {
					criterias_vector &v = this->m_inddata;
					if (!v.empty()) {
						if (v.back() != c) {
							v.push_back(c);
							this->my_draw();
						}
					}
					else {
						v.push_back(c);
						this->my_draw();
					}
				}
				else if (p->disposition == DispositionType::variable) {
					criterias_vector &v = this->m_vardata;
					if (!v.empty()) {
						if (v.back() != c) {
							v.push_back(c);
							this->my_draw();
						}
					}
					else {
						v.push_back(c);
						this->my_draw();
					}
				}
			}// p
		}// process_result
		void window_resized() {
			my_draw();
		}
	protected:
		void my_draw(void) {
			this->show();
		}
		virtual void draw(const dlib::canvas& c) const
		{
			if (this->m_params == nullptr) {
				return;
			}
			unsigned long ww = 0, hh = 0;
			ww = this->width();
			hh = this->height();
			dlib::rectangle r(0, 0, ww, hh);
			dlib::fill_rect(c, r, dlib::rgb_pixel(255, 255, 255));
			const criterias_vector &vv = this->m_inddata;
			size_t n = vv.size();
			if (n > 1) {
				auto p = std::minmax_element(vv.begin(), vv.end());
				DISTANCETYPE dMin = *(p.first);
				DISTANCETYPE dMax = *(p.second);
				if (dMin < dMax) {
					double fMin = (double)dMin;
					double deltay = (double)hh / (dMax - fMin);
					double deltax = (double)ww / (double)(n + 2);
					for (size_t i = 1; i < n; ++i) {
						unsigned long x1 = i * deltax;
						unsigned long y1 = hh - (unsigned long)((vv[i - 1] - fMin) * deltay);
						unsigned long x2 = x1 + deltax;
						unsigned long y2 = hh - (unsigned long)((vv[i] - fMin) * deltay);
						dlib::rgb_pixel color(m_params->sumindcolor.red, m_params->sumindcolor.green, m_params->sumindcolor.blue);
						dlib::point p1(x1, y1);
						dlib::point p2(x2, y2);
						dlib::draw_line(c, p1, p2, color);
					}// i
				}// draw
			}
			const criterias_vector &vvx = this->m_vardata;
			n = vvx.size();
			if (n > 1) {
				auto p = std::minmax_element(vvx.begin(), vvx.end());
				DISTANCETYPE dMin = *(p.first);
				DISTANCETYPE dMax = *(p.second);
				if (dMin < dMax) {
					double fMin = (double)dMin;
					double deltay = (double)hh / (dMax - fMin);
					double deltax = (double)ww / (double)(n + 2);
					for (size_t i = 1; i < n; ++i) {
						unsigned long x1 = i * deltax;
						unsigned long y1 = hh - (unsigned long)((vvx[i - 1] - fMin) * deltay);
						unsigned long x2 = x1 + deltax;
						unsigned long y2 = hh - (unsigned long)((vvx[i] - fMin) * deltay);
						dlib::rgb_pixel color(m_params->sumvarcolor.red, m_params->sumvarcolor.green, m_params->sumvarcolor.blue);
						dlib::point p1(x1, y1);
						dlib::point p2(x2, y2);
						dlib::draw_line(c, p1, p2, color);
					}// i
				}
			}
		}// draw
	};
	//////////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE, typename FLOATTYPE, typename DATATYPE>
	class MatriceDisplayWindow : public dlib::button, public  MatElemResultBackgounder<IDTYPE, DISTANCETYPE, STRINGTYPE > {
		using MatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatElemResultType = typename MatElemType::IntraMatElemResultType;
		using MatElemResultPtr = typename MatElemType::IntraMatElemResultPtr;
		using MatOrdAgentType = MatOrdAgent<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatriceDataType = MatriceData<IDTYPE, STRINGTYPE, DATATYPE>;
		using ContextType = DLibDrawContext<STRINGTYPE, FLOATTYPE>;
		using DrawItemsType = DrawItems<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using MatElemFunctionType = std::function<void(MatElemResultPtr)>;
		using queue_type = MatElemResultClient<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using strings_vector = std::vector<STRINGTYPE>;
		using MatElemCritGraphWindowType = MatElemCritGraphWindow<IDTYPE, DISTANCETYPE, STRINGTYPE>;
	private:
		MatElemCritGraphWindowType *m_pgraph;
		MatElemFunctionType m_dest;
		std::atomic<bool> m_donevar;
		std::atomic<bool> m_doneind;
		MatriceDataType *m_pMatData;
		DrawContextParams oDrawContextParams;
		std::unique_ptr<MatOrdAgentType> m_matord;
		std::unique_ptr<DrawItemsType> m_items;
	public:
		MatriceDisplayWindow(dlib::drawable_window& w, MatElemCritGraphWindowType *pGraph = nullptr) : dlib::button(w),m_pgraph(pGraph),
			m_dest([](MatElemResultPtr oRes) {}),
			m_donevar(false), m_doneind(false), m_pMatData(nullptr) {
			this->window_resized();
			this->show();
		}//MatriceDisplayWindow
		~MatriceDisplayWindow() {}
		void set_graph_window(MatElemCritGraphWindowType *pGraph) {
			this->m_pgraph = pGraph;
			if (this->m_pgraph != nullptr) {
				this->m_pgraph->set_draw_params(&oDrawContextParams);
			}
		}
		const DrawContextParams *get_context_params(void) const {
			return (&(this->oDrawContextParams));
		}
		DrawContextParams *get_context_params(void) {
			return (&(this->oDrawContextParams));
		}
		void set_callback(MatElemFunctionType f) {
			this->m_dest = f;
		}
		void arrange(MatriceDataType *pData, MatCellType aType = MatCellType::plainCell) {
			this->m_pMatData = pData;
			this->init_data(aType);
			this->run_matrice();
			this->show();
		}
		void window_resized() {
			unsigned long ww = 0, hh = 0;
			ww = this->width();
			hh = this->height();
			oDrawContextParams.width = ww;
			oDrawContextParams.height = hh;
			size_t nCols = 0;
			size_t nRows = 0;
			if (this->m_pMatData != nullptr) {
				nCols = m_pMatData->cols_count();
				nRows = m_pMatData->rows_count();
			}
			oDrawContextParams.dx = ww / (nCols + 3);
			oDrawContextParams.dy = hh / (nRows + 3);
			my_draw();
		}
	protected:
		virtual void process_result(MatElemResultPtr oRes) {
			MatElemResultType *p = oRes.get();
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
					}
				}// finished
				DrawItemsType *pItems = this->m_items.get();
				assert(pItems != nullptr);
				pItems->set_result(oRes);
				this->my_draw();
				if (this->m_pgraph != nullptr) {
					this->m_pgraph->set_result(oRes);
				}
				(this->m_dest)(oRes);
			}
		}//process_result
		virtual void draw(const dlib::canvas& c) const
		{
			DrawItemsType *p = this->m_items.get();
			if (p != nullptr) {
				unsigned long ww = 0, hh = 0;
				ww = this->width();
				hh = this->height();
				dlib::rectangle r(0, 0, ww, hh);
				dlib::fill_rect(c, r, dlib::rgb_pixel(255, 255, 255));
				ContextType oContext(c, &oDrawContextParams);
				p->draw(&oContext);
			}// p
		}// draw
	private:
		void my_draw(void) {
			this->show();
		}
		void init_data(MatCellType aType = MatCellType::plainCell) {
			MatriceDataType *pMat = this->m_pMatData;
			if (pMat == nullptr) {
				return;
			}
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
			const strings_vector &rowNames = pMat->rows_names();
			const strings_vector &colNames = pMat->cols_names();
			bool bRet = p->initialize(aType, nRows, nCols, data, rowNames, colNames, indsSum, varsSum);
			assert(bRet);
			this->m_items.reset(p);
		}// init_data
		void run_matrice(void) {
			MatriceDataType *pData = this->m_pMatData;
			if (pData == nullptr) {
				return;
			}
			this->m_matord.reset(new MatOrdAgentType(this));
			MatOrdAgentType *pMat = this->m_matord.get();
			DrawContextParams &ctx = this->oDrawContextParams;
			ctx.downcolor = InfoColor(0, 0, 0);
			ctx.bIndsSum = false;
			ctx.bVarsSum = false;
			if (this->m_pgraph != nullptr) {
				this->m_pgraph->set_draw_params(&ctx);
			}
			pMat->arrange(pData->indiv_provider(), pData->variable_provider());
		}// run_matrice
	};// class MatriceDisplayWindow
	////////////////////////////////////////
}// namespace info
////////////////////////////////////
#endif // !__DLIB_MATRICE__H__

