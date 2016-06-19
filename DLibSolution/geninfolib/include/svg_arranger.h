#pragma once
#ifndef __SVG_ARRANGER_H__
#define __SVG_ARRANGER_H__
/////////////////////////////////
#include "svg/svg_matrice.h"
#include "matrunner.h"
/////////////////////////////
namespace info {
	/////////////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE, typename FLOATTYPE, typename INTTYPE, typename WEIGHTYPE>
	class SVGMatriceArranger {
	public:
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
		std::unique_ptr<MatRunnerType> m_runner;
		MatriceWindowType mat_win;
		std::unique_ptr<ModelDataType> m_model;
		matrice_promise_ptr m_promise;
		matrice_future m_future;
		std::future<bool> m_init_future;
	public:
		SVGMatriceArranger() :m_ok(false) {
			this->m_runner.reset(new MatRunnerType());
			MatRunnerType *pRunner = this->m_runner.get();
			assert(pRunner != nullptr);
			ModelDataType *pModel = new ModelDataType(pRunner);
			this->m_model.reset(pModel);
		}
		~SVGMatriceArranger() {
		}
		template <typename T>
		bool arrange(const STRINGTYPE &name,size_t nRows, size_t nCols,const std::vector<T> &data,
			const strings_vector &rowNames, const strings_vector &colNames) 
		{
			if ((nRows > 0) && (nCols > 0) && (data.size() >= (nCols * nRows)) && (rowNames.size() >= nRows) &&
				(colNames.size() >= nCols)) {
				ModelDataType *pModel = this->m_model.get();
				assert(pModel != nullptr);
				std::future<bool> oFuture = pModel->initialize(name, nRows, nCols, data, rowNames, colNames, MatCellType::histogCell);
				this->m_ok = oFuture.get();
			}
		}
		
		void set_result(MatElemResultPtr oRes) {
			if (!this->m_ok) {
				return;
			}
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
		std::future<bool> export_svg(const STRINGTYPE &filename) {
			return std::async(std::launch::async, [this, filename]()->bool {
				if (!this->m_ok) {
					return (false);
				}
				this->init_all();
				this->save(filename);
				return (true);
			});
		}// export_svg
	};
	////////////////////////////////////////////////////
}// namespace info
////////////////////////////////////
#endif // !__SVG8ARRAGER_H__

