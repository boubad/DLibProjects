#pragma once
#ifndef MATRESULT_H_
#define MATRESULT_H_
/////////////////////////////
#include "info_includes.h"
//////////////////////////////////
namespace info {
	///////////////////////////////////////////
	template<typename IDTYPE = unsigned long, typename DISTANCETYPE = long>
	class IntraMatElemResult {
	public:
		using ints_vector = std::vector<IDTYPE>;
		using sizets_vector = std::vector<size_t>;
		using IntraMatElemResultType = IntraMatElemResult<DISTANCETYPE>;
		using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
		//
		DISTANCETYPE first;
		sizets_vector second;
		ints_vector third;
	public:
		IntraMatElemResult() :
			first(0) {
		}
		IntraMatElemResult(const DISTANCETYPE c, const sizets_vector &v) :
			first(c), second(v) {
		}
		IntraMatElemResult(const IntraMatElemResultType &other) :
			first(other.first), second(other.second), third(other.third) {
		}
		IntraMatElemResultType & operator=(const IntraMatElemResultType &other) {
			if (this != &other) {
				this->first = other.first;
				this->second = other.second;
				this->third = other.third;
			}
			return (*this);
		}
		virtual ~IntraMatElemResult() {
		}
	};
	// class IntraMatElemResult
	/////////////////////////////////////
}// namespace infi
 ///////////////////////////////////
#endif /* INTRAMAT_H_ */
