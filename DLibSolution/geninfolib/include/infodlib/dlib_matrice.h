#pragma once
#ifndef __DLIB_MATRICE__H__
#define __DLIB_MATRICE__H__
/////////////////////////////////
#include <dlib/threads.h>
#include <dlib/gui_widgets.h>
///////////////////////////////////
#include "../info_drawcontext.h"
#include "../matrunner.h"
//////////////////////////////////////
namespace info {
	////////////////////////////////////////////
	template <typename STRINGTYPE, typename FLOATTYPE>
	class DLibDrawContext : public DrawContext<STRINGTYPE, FLOATTYPE> {
		using DrawItem = BaseDrawItem<STRINGTYPE, FLOATTYPE>;
		using PDrawItem = DrawItem *;
		using BaseType = DrawContext<STRINGTYPE, FLOATTYPE>;
	private:
		const dlib::canvas &m_canvas;
	public:
		DLibDrawContext(const dlib::canvas &m, const DrawContextParams *params) :BaseType(params), m_canvas(m) {
		}
		virtual ~DLibDrawContext() {}
	public:
		virtual void draw(PDrawItem pItem, coord_type x0 = 0, coord_type y0 = 0) const {
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
	  ////////////////////////////////////////////////
	template <typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE, typename FLOATTYPE>
	class MatriceShowWindow : public dlib::button {
	public:
		using DrawItemsViewType = DrawItemsView<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using ContextType = DLibDrawContext<STRINGTYPE, FLOATTYPE>;
		using MatriceShowWindowType = MatriceShowWindow<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using MatElemResultType = MatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatElemResultPtr = std::shared_ptr<MatElemResultType>;
	private:
		dlib::label m_label;
		DrawItemsViewType *m_pview;
		DrawContextParams m_params;
	private:
		void notify(MatElemResultPtr oRes) {
			STRINGTYPE ss;
			MatElemResultType *p = oRes.get();
			if (p != nullptr) {
				p->to_string(ss, false);
			}// p
			this->m_label.set_text(ss);
			this->show();
		}// notify
	public:
		MatriceShowWindow(dlib::drawable_window& w, DrawItemsViewType *pView = nullptr) : dlib::button(w), m_label(w), m_pview(pView) {
			this->m_label.set_pos(5, 5);
			if (this->m_pview != nullptr) {
				this->m_pview->set_function([this](MatElemResultPtr o) {
					this->notify(o);
				});
			}
			this->show();
		}
		virtual ~MatriceShowWindow() {}
		void set_view(DrawItemsViewType *pView) {
			this->m_pview = pView;
			if (this->m_pview != nullptr) {
				this->m_pview->set_function([this](MatElemResultPtr o) {
					this->notify(o);
				});
			}
		}
	protected:
		virtual void draw(const dlib::canvas& c) const
		{
			if (this->m_pview != nullptr) {
				unsigned long ww = this->width();
				unsigned long hh = this->height();
				long h0 = (long)(5 + this->m_label.height() + 3);
				{
					MatriceShowWindowType *p = const_cast<MatriceShowWindowType *>(this);
					p->m_params.width = ww;
					p->m_params.height = hh - h0;
				}
				dlib::rectangle r(0, h0, ww, hh);
				dlib::fill_rect(c, r, dlib::rgb_pixel(255, 255, 255));
				ContextType oContext(c, &m_params);
				this->m_pview->draw(&oContext, 0, h0);
			}// pview
		}// draw
	};
	//////////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE, typename FLOATTYPE, typename INTTYPE, typename WEIGHTYPE>
	class MatriceDisplayWindow : public dlib::button {
		using MatRunnerType = MatRunner<IDTYPE, STRINGTYPE, DISTANCETYPE, INTTYPE, WEIGHTYPE>;
		using MatriceShowWindowType = MatriceShowWindow<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using DrawItemsType = DrawItems<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using ViewType = DrawItemsView<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using strings_vector = std::vector<STRINGTYPE>;
		//
		using MatElemType = MatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatElemResultType = typename MatElemType::MatElemResultType;
		using MatElemResultPtr = typename MatElemType::MatElemResultPtr;
		using matelem_future = std::future<MatElemResultPtr>;
		//
		using InfoMatriceResultPair = std::pair<MatElemResultPtr, MatElemResultPtr>;
		using InfoMatriceResultPairPtr = std::shared_ptr<InfoMatriceResultPair>;
		using matrice_promise = std::promise<InfoMatriceResultPairPtr>;
		using matrice_future = std::future<InfoMatriceResultPairPtr>;
		using matrice_promise_ptr = std::shared_ptr<matrice_promise>;
		using DrawItemsViewType = DrawItemsView<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		//
		using MatriceDisplayType = MatriceDisplayWindow<IDTYPE,DISTANCETYPE,STRINGTYPE,FLOATTYPE,INTTYPE,WEIGHTYPE>;
		using ContextType = DLibDrawContext<STRINGTYPE, FLOATTYPE>;
		using mutex_type = std::mutex;
		using lock_type = std::lock_guard<mutex_type>;
		//
	private:
		dlib::label m_label;
		ViewType *m_pview;
		DrawContextParams m_params;
		std::unique_ptr<MatRunnerType> m_runner;
		std::unique_ptr<DrawItemsType> m_items;
		matrice_promise_ptr m_promise;
		matrice_future m_future;
		mutex_type _mutex;
	private:
		void notify(MatElemResultPtr oRes) {
			DrawItemsType *pp = this->m_items.get();
			if (pp != nullptr) {
				pp->set_result(oRes);
			}
			STRINGTYPE ss;
			MatElemResultType *p = oRes.get();
			if (p != nullptr) {
				p->to_string(ss, false);
			}// p
			this->m_label.set_text(ss);
			this->show();
		}// notify
	public:
		MatriceDisplayWindow(dlib::drawable_window& w) : dlib::button(w), m_label(w), m_pview(nullptr) {
			this->show();
		}//MatriceDisplayWindow
		~MatriceDisplayWindow() {
			MatRunnerType *pRunner = this->m_runner.get();
			if (pRunner != nullptr) {
				pRunner->cancel();
			}
		}
		void set_view(DrawItemsViewType *pView) {
			this->m_pview = pView;
			if (this->m_pview != nullptr) {
				this->m_pview->set_function([this](MatElemResultPtr o) {
					this->notify(o);
				});
			}
		}
		template <typename DATATYPE>
		bool compute(MatCellType aType, const STRINGTYPE &sigle, size_t nRows, size_t nCols, const std::vector<DATATYPE> &data,
			const strings_vector &rowNames, const strings_vector &colNames) {
			if (this->m_items.get() == nullptr) {
				this->m_items.reset(new DrawItemsType());
			}
			DrawItemsType *pItems = this->m_items.get();
			assert(pItems != nullptr);
			bool bRet = pItems->initialize(aType, nRows, nCols, data, rowNames, colNames);
			if (bRet) {
				ViewType *pView = pItems->add_view(MatriceDrawType::drawVariables);
				assert(pView != nullptr);
				this->m_pview = pView;
				if (this->m_runner.get() == nullptr) {
					this->m_runner.reset(new MatRunnerType());
				}
				this->m_promise = std::make_shared<matrice_promise>();
				MatRunnerType *pRunner = this->m_runner.get();
				assert(pRunner != nullptr);
				this->m_future = pRunner->arrange_matrice(this->m_promise,
					pItems->get_indiv_provider(), pItems->get_variable_provider(), sigle,
					[this](MatElemResultPtr oRes) {
					this->notify(oRes);
				});
			}// bRet
			return (bRet);
		}// compute
	protected:
		virtual void draw(const dlib::canvas& c) const
		{
			MatriceDisplayType *p = const_cast<MatriceDisplayType *>(this);
			if (this->m_pview != nullptr) {
				unsigned long ww = this->width();
				unsigned long hh = this->height();
				long h0 = (long)(5 + this->m_label.height() + 3);
				{
					p->m_params.width = ww;
					p->m_params.height = hh - h0;
				}
				dlib::rectangle r(0, h0, ww, hh);
				dlib::fill_rect(c, r, dlib::rgb_pixel(255, 255, 255));
				ContextType oContext(c, &m_params);
				this->m_pview->draw(&oContext, 0, h0);
			}// pview
		}// draw
	};// class MatriceDisplayWindow
	  ////////////////////////////////////////
}// namespace info
 ////////////////////////////////////
#endif // !__DLIB_MATRICE__H__

