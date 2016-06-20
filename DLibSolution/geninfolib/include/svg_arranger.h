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
		std::unique_ptr<MatRunnerType> m_runner;
		MatriceWindowType mat_win;
		std::unique_ptr<ModelDataType> m_model;
		matrice_promise_ptr m_promise;
		matrice_future m_future;
	public:
		SVGMatriceArranger() {
			this->m_runner.reset(new MatRunnerType());
			MatRunnerType *pRunner = this->m_runner.get();
			assert(pRunner != nullptr);
			ModelDataType *pModel = new ModelDataType(pRunner);
			assert(pModel != nullptr);
			this->m_model.reset(pModel);
		}
		~SVGMatriceArranger() {
		}
		template <typename T>
		std::future<InfoMatriceResultPairPtr> export_svg(const STRINGTYPE &filename,
			const STRINGTYPE &name,size_t nRows, size_t nCols,const std::vector<T> &data,
			const strings_vector &rowNames, 
			const strings_vector &colNames,MatCellType aType = MatCellType::histogCell){
			return std::async(std::launch::async,
				[this,filename,name,nRows,nCols,data,rowNames,colNames,aType](){
					return (this->arrange(filename,name,nRows,nCols,data,
						rowNames,colNames,aType));
			});
		}// export svg
private:
	template <typename T>
	InfoMatriceResultPairPtr arrange(const STRINGTYPE &filename,
			const STRINGTYPE &name,size_t nRows, size_t nCols,const std::vector<T> &data,
			const strings_vector &rowNames, const strings_vector &colNames,
			MatCellType aType = MatCellType::histogCell); 
		{
			InfoMatriceResultPairPtr oRet;
			if ((nRows > 0) && (nCols > 0) && (data.size() >= (nCols * nRows)) && (rowNames.size() >= nRows) &&
				(colNames.size() >= nCols)) {
				ModelDataType *pModel = this->m_model.get();
				assert(pModel != nullptr);
				std::future<bool> oFuture = pModel->initialize(name, nRows, nCols, 
					data, rowNames, colNames, aType);
				oFuture.wait();
				ViewType *pView = pModel->add_view(DispositionType::variable);
				assert(pView != nullptr);
				this->mat_win.set_view(pView);
				this->m_promise = std::make_shared<matrice_promise>();
				this->m_future = pModel->compute(this->m_promise, [pModel](MatElemResultPtr oRes) {
						pModel->set_result(oRes);
				});
				this->m_future.wait();
				this->mat_win.draw(filename);
				oRet = this->m_future.get();
			}
			return (oRet);
		}	
	};
	////////////////////////////////////////////////////
}// namespace info
////////////////////////////////////
#endif // !__SVG8ARRAGER_H__

