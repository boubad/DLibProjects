#pragma once
#ifndef __ANACOMPOINDIVPROVIDER_H__
#define __ANACOMPOINDIVPROVIDER_H__
/////////////////////////////////////
#include "transformed_storeindivsource.h"
#include "anacompo.h"
#include "datavector_indivprovider.h"
//////////////////////////////////////////
namespace info {
	////////////////////////////////////
	template<typename U = unsigned long, typename INTTYPE = unsigned long,
		typename STRINGTYPE = std::string, typename WEIGHTYPE = double>
		class AnaCompoIndivSourceCreator {
		public:
			using DataVectorIndivSourceType = DataVectorIndivSource<U, STRINGTYPE>;
			using DataVectorIndivSourcePtr = std::shared_ptr<DataVectorIndivSourceType>;
			using DataVectorIndivSourceTuple = std::tuple<DataVectorIndivSourcePtr, DataVectorIndivSourcePtr>;
			//
			using ints_vector = std::vector<U>;
			using strings_vector = std::vector<STRINGTYPE>;
			using IndivType = Indiv<U, STRINGTYPE>;
			using DataMap = std::map<U, InfoValue>;
			using IndivTypePtr = std::shared_ptr<IndivType>;
			using indivptrs_vector = std::vector<IndivTypePtr>;
			using StatSummatorType = StatSummator<U, STRINGTYPE>;
			using StatInfoType = StatInfo<U, STRINGTYPE>;
			using TranformedStoreIndivSourceType = TranformedStoreIndivSource<U, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using SourceType = IIndivSource<U, STRINGTYPE>;
		public:
			template <typename XU, typename XSTRING>
			DataVectorIndivSourceTuple create(IIndivSource<XU, XSTRING> *pIndProvider,
				IIndivSource<XU, XSTRING> *pVarProvider, const size_t nbFactsMax = 4) {
				//
				assert(pIndProvider != nullptr);
				assert(pVarProvider != nullptr);
				//
				DataVectorIndivSourcePtr sInd, sVar;
				TranformedStoreIndivSourceType src(pIndProvider, datasetSigle);
				size_t nRows = 0, nCols = 0;
				std::vector<double> oSrcData;
				if (!src.get_data_array(nRows, nCols, oSrcData)) {
					return (std::make_tuple<DataVectorIndivSourcePtr>(sInd, sVar));
				}
				std::vector<double> oVars, oInds;
				size_t nFacts = 0;
				if (!AnaCompo<double>::compute_anacompo(nRows, nCols, oSrcData, nFacts, oVars, oInds)) {
					return (std::make_tuple<DataVectorIndivSourcePtr>(sInd, sVar));
				}
				ints_vector varIds, indIds;
				src.get_indivs_ids(indIds);
				src.get_variables_ids(varIds);
				strings_vector indsNames(nRows), varsNames(nCols);
				src.get_indivs_names(indsNames);
				for (size_t i = 0; i < nCols; ++i) {
					IndivTypePtr o = pVarProvider->find(varIds[i]);
					IndivType *p = o.get();
					if (p != nullptr) {
						varsNames[i] = p->sigle();
					}
				}// i
				//
				size_t nf = nbFactsMax;
				if (nf > nFacts) {
					nf = nFacts;
				}
				ints_vector factIds(nf);
				for (size_t i = 0; i < nf; ++i) {
					facts[i] = (U)(i + 1);
				}
				//
				sInd.reset(new DataVectorIndivSourceType(nRows, nf, oInds, indIds, factIds, indsNames));
				sVar.reset(new DataVectorIndivSourceType(nCols, nf, oVars, varIds, factIds, indsNames));
				return (std::make_tuple<DataVectorIndivSourcePtr>(sInd, sVar));
			}// create
	};
	/////////////////////////////////////
}// namespace info
///////////////////////////////////////
#endif // !__ANACOMPOINDIVPROVIDER_H__
