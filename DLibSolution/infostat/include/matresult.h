#pragma once
#ifndef MATRESULT_H_
#define MATRESULT_H_
/////////////////////////////
#include "info_includes.h"
#include "activeobject.h"
//////////////////////////////////////
#include <boost/signals2/signal.hpp>
//////////////////////////////////
namespace info {
	///////////////////////////////////////////
	enum class DispositionType { invalid, indiv, variable };
	enum class StageType { started, finished, aborted, current };
	///////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE>
	class IntraMatElemResult {
	public:
		using ints_vector = std::vector<IDTYPE>;
		using sizets_vector = std::vector<size_t>;
		using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
		//
		StageType stage;
		DispositionType disposition;
		DISTANCETYPE first;
		sizets_vector second;
		STRINGTYPE sigle;
	public:
		IntraMatElemResult() : stage(StageType::current), disposition(DispositionType::invalid), first(0) {
		}
		IntraMatElemResult(const DISTANCETYPE c, const sizets_vector &v,
			DispositionType disp = DispositionType::invalid, StageType st = StageType::current) :
			stage(st), disposition(disp), first(c), second(v) {
		}
		IntraMatElemResult(const IntraMatElemResultType &other) :
			stage(other.stage), disposition(other.disposition), first(other.first), second(other.second), sigle(other.sigle) {
		}
		IntraMatElemResultType & operator=(const IntraMatElemResultType &other) {
			if (this != &other) {
				this->stage = other.stage;
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
			this->stage = StageType::current;
			this->disposition = DispositionType::invalid;
			this->first = 0;
			this->second.clear();
			this->sigle.clear();
		}// clear
	};
	// class IntraMatElemResult
	////////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE>
	class MatElemResultClient : boost::noncopyable {
	public:
		using MatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatElemResultPtr = std::shared_ptr<MatElemResultType>;
	public:
		MatElemResultClient() {
		}
		virtual ~MatElemResultClient() {
		}
	protected:
		virtual void process_result(MatElemResultPtr /*oRes*/) {
			// no nothing in base class
		}//process_result
	public:
		virtual void put(MatElemResultPtr oRes) {
			this->process_result(oRes);
		}// put
	public:
		void operator()(MatElemResultPtr oRes) {
			this->process_result(oRes);
		}
	}; // class IntraMatElemBackgrounder<IDTYPE,DISTANCETYPE,STRINGTYPE>
	////////////////////////////////////////
	template<typename IDTYPE, typename DISTANCETYPE, typename STRINGTYPE>
	class MatElemResultBackgounder : MatElemResultClient<IDTYPE,DISTANCETYPE,STRINGTYPE> {
	public:
		using MatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		using MatElemResultPtr = std::shared_ptr<MatElemResultType>;
		using SignalType = typename boost::signals2::signal<void(MatElemResultPtr)>;
		using SlotType = typename SignalType::slot_type;
		using ConnectionType = boost::signals2::connection;
	private:
		bool m_hasconnect;
		std::atomic<bool> done;
		SharedQueue<MatElemResultPtr> dispatchQueue;
		std::unique_ptr<std::thread> runnable;
		SignalType m_signal;
	public:
		MatElemResultBackgounder() :m_hasconnect(false), done(false) {
			this->runnable.reset(new std::thread([this]() {
				while (!this->done.load()) {
					MatElemResultPtr o = this->dispatchQueue.take();
					if (o.get() == nullptr) {
						this->done.store(true);
						break;
					}
					this->process_result(o);
					if (this->m_hasconnect) {
						this->m_signal(o);
					}
				} // while
			}));
		}
		virtual ~MatElemResultBackgounder() {
			IntraMatElemResultPtr o;
			this->done.store(true);
			this->dispatchQueue.put(o);
			this->runnable->join();
		}
	public:
		ConnectionType connect(const SlotType &subscriber) {
			this->m_hasconnect = true;
			return m_signal.connect(subscriber);
		}//connect
		virtual void put(MatElemResultPtr oRes) {
			this->dispatchQueue.put(oRes);
		}// put
	}; // class IntraMatElemBackgrounder<IDTYPE,DISTANCETYPE,STRINGTYPE>
	////////////////////////////////////////////
}// namespace info
 ///////////////////////////////////
#endif /* INTRAMAT_H_ */
