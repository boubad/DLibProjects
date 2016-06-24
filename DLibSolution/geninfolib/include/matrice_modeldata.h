#pragma once
#ifndef __MATRICE_MODELDATA_H__
#define __MATRICE_MODELDATA_H__
////////////////////////////////
#include "matrice_drawitems.h"
////////////////////////////////////
namespace info {
	////////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE,
		typename FLOATTYPE, typename INTTYPE, typename WEIGHTYPE>
		class MatriceModelData {
		using ViewType = DrawItemsView<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using strings_vector = std::vector<STRINGTYPE>;
		using MatriceDataType = MatriceData<IDTYPE, STRINGTYPE>;
		using MatriceType = InfoMatrice<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		//
		using MatElemType = MatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using SourceType = typename MatElemType::SourceType;
		using MatElemResultType = typename MatElemType::MatElemResultType;
		using MatElemResultPtr = typename MatElemType::MatElemResultPtr;
		using matelem_function = std::function<void(MatElemResultPtr)>;
		using matelem_future = std::future<MatElemResultPtr>;
		//
		using InfoMatriceResultPair = std::pair<MatElemResultPtr, MatElemResultPtr>;
		using InfoMatriceResultPairPtr = std::shared_ptr<InfoMatriceResultPair>;
		using matrice_promise = std::promise<InfoMatriceResultPairPtr>;
		using matrice_future = std::future<InfoMatriceResultPairPtr>;
		using matrice_promise_ptr = std::shared_ptr<matrice_promise>;
		using DrawItemsViewType = DrawItemsView<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		using ModelDataType = MatriceModelData<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE, INTTYPE, WEIGHTYPE>;
		using DrawItemsType = DrawItems<IDTYPE, DISTANCETYPE, STRINGTYPE, FLOATTYPE>;
		//
		private:
			Backgrounder *m_pback;
			std::atomic<bool> m_busy;
			std::atomic<bool> m_inited;
			matelem_function m_f;
			std::unique_ptr<MatriceDataType> m_matdata;
			std::unique_ptr<MatriceType> m_matrice;
			std::unique_ptr<Backgrounder> m_backgrounder;
			std::unique_ptr<DrawItemsType> m_items;
			STRINGTYPE m_sigle;
		protected:
			virtual DrawItemsType *create_drawitems(void) {
				return new DrawItemsType();
			}	  // create_drawitems
		public:
			MatriceModelData(Backgrounder *pBack = nullptr) : m_pback(pBack),m_busy(false),m_inited(false) {
			}
			virtual ~MatriceModelData() {
			}
			bool is_busy(void) {
				return (this->m_busy.load());
			}
			MatriceDataType *get_matrice_data(void) {
				return (this->m_matdata.get());
			}
			MatriceType *get_matrice(void) {
				return (this->m_matrice.get());
			}
			Backgrounder *get_backgrounder(void) {
				Backgrounder *pRet = this->m_pback;
				if (pRet == nullptr) {
					if ((pRet = this->m_backgrounder.get()) == nullptr) {
						this->m_backgrounder.reset(new Backgrounder());
						pRet = this->m_bavkgrounder.get();
					}
					
				}
				return (pRet);
			}
			DrawItemsType *get_draw_items(void) {
				return (this->m_items.get());
			}
			ViewType *add_view(DispositionType aType) {
				ViewType *pRet = nullptr;
				if (!this->m_inited.load()) {
					return (pRet);
				}
				DrawItemsType *pItems = this->m_items.get();
				if (pItems == nullptr) {
					return (pRet);
				}
				pRet = pItems->add_view(aType);
				return (pRet);
			}	  // add_view
			template<typename DATATYPE>
			std::future<bool> initializeAsync(const STRINGTYPE &sigle, size_t nRows,
				size_t nCols, const std::vector<DATATYPE> &data,
				const strings_vector &rowNames, const strings_vector &colNames,
				MatCellType aType = MatCellType::histogCell) {
				this->m_inited.store(false);
				this->m_busy.store(true);
				return std::async(std::launch::async,
					[this, sigle, nRows, nCols, data, rowNames, colNames, aType]()->bool {
					this->m_sigle = sigle;
					if (this->m_backgrounder.get() == nullptr) {
						this->m_backgrounder.reset(new Backgrounder());
					}
					if (this->m_matdata.get() == nullptr) {
						this->m_matdata.reset(new MatriceDataType());
					}
					if (this->m_items.get() == nullptr) {
						this->m_items.reset(this->create_drawitems());
					}
					MatriceDataType *pData = this->m_matdata.get();
					assert(pData != nullptr);
					std::future<bool> bf = pData->initializeAsync(sigle, nRows, nCols, data, rowNames, colNames);
					bf.wait();
					DrawItemsType *pItems = this->m_items.get();
					assert(pItems != nullptr);
					bool bRet = pItems->initialize(aType, nRows, nCols,
						pData->data(), rowNames, colNames, pData->indivs_summary(),
						pData->variables_summary(), sigle);
					if (bRet) {
						this->m_inited.store(true);
					}	  // bRet
					this->m_busy.store(false);
					return (bRet);
				});
			}	  // initialize
			matrice_future computeAsync(void) {
				this->m_busy.store(true);
				return std::async(std::launch::async,
					[this]()->InfoMatriceResultPairPtr {
					InfoMatriceResultPairPtr oRet;
					if (this->m_inited.load()) {
						MatriceDataType *pData = this->m_matdata.get();
						assert(pData != nullptr);
						Backgrounder *pBack = this->m_backgrounder.get();
						assert(pBack != nullptr);
						oRet = MatriceType::perform_arrange(pData->indiv_provider(), pData->variable_provider(),
							nullptr, pBack, [this](MatElemResultPtr o) {
							DrawItemsType *pp = this->m_items.get();
							if (pp != nullptr) {
								pp->set_result(o);
							}
						},
							this->m_sigle, true);
					}
					oRet.wait();
					this->m_busy.store(false);
					return (oRet);
				});
			}	  // compute
	};
	// class MatriceModelData
}// namespace info
 ///////////////////////////////////
#endif // !__BASEDRAWITEM_H__
