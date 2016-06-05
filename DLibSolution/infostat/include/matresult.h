#pragma once
#ifndef MATRESULT_H_
#define MATRESULT_H_
/////////////////////////////
#include "info_includes.h"
#include "activeobject.h"
//////////////////////////////////
namespace info {
	///////////////////////////////////////////
	enum class DispositionType {invalid,indiv,variable};
	///////////////////////////////////////////
	template<typename IDTYPE = unsigned long, typename DISTANCETYPE = long, typename STRINGTYPE = std::string>
	class IntraMatElemResult {
	public:
		using ints_vector = std::vector<IDTYPE>;
		using sizets_vector = std::vector<size_t>;
		using IntraMatElemResultType = IntraMatElemResult<IDTYPE,DISTANCETYPE,STRINGTYPE>;
		using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
		//
		DispositionType disposition;
		DISTANCETYPE first;
		sizets_vector second;
		STRINGTYPE sigle;
	public:
		IntraMatElemResult() : disposition(DispositionType::invalid),first(0) {
		}
		IntraMatElemResult(const DISTANCETYPE c, const sizets_vector &v,DispositionType disp = DispositionType::invalid) :
			disposition(disp),first(c), second(v) {
		}
		IntraMatElemResult(const IntraMatElemResultType &other) :
			disposition(other.disposition),first(other.first), second(other.second),sigle(other.sigle) {
		}
		IntraMatElemResultType & operator=(const IntraMatElemResultType &other) {
			if (this != &other) {
				this->disposition = other.disposition;
				this->first = other.first;
				this->second = other.second;
				this->sigle = other.sigle;
			}
			return (*this);
		}
		virtual ~IntraMatElemResult() {
		}
		void clear(void) {
			this->disposition = DispositionType::invalid;
			this->first = 0;
			this->second.clear();
			this->sigle.clear();
		}// clear
	};
	// class IntraMatElemResult
	////////////////////////////////////////
	template<typename IDTYPE = unsigned long, typename DISTANCETYPE = long, typename STRINGTYPE = std::string>
	class MatElemResultBackgounder : boost::noncopyable {
	public:
		using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
	private:
		std::atomic<bool> done;
		SharedQueue<IntraMatElemResultPtr> dispatchQueue;
		std::unique_ptr<std::thread> runnable;
	public:
		MatElemResultBackgounder():done(false) {
			this->runnable.reset(new std::thread([&]() {
				while (!this->done.load()) {
					IntraMatElemResultPtr o = this->dispatchQueue.take();
					if (o.get() == nullptr) {
						done.store(true);
						break;
					}
					this->process_result(o);
				} // while
			}));
		}
		virtual ~MatElemResultBackgounder() {
			IntraMatElemResultPtr o;
			this->done.store(true);
			this->dispatchQueue.put(o);
			this->runnable->join();
		}
	protected:
		virtual void process_result(IntraMatElemResultPtr /*oRes*/) {
			// no nothing in base class
		}
	public:
		void put(IntraMatElemResultPtr oRes) {
			this->dispatchQueue.put(oRes);
		}// put
	}; // class IntraMatElemBackgrounder<IDTYPE,DISTANCETYPE,STRINGTYPE>
	////////////////////////////////////////////
}// namespace info
 ///////////////////////////////////
#endif /* INTRAMAT_H_ */
