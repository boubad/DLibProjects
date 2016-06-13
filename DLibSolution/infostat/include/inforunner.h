#pragma once
#ifndef __INFORUNNER_H__
#define __INFORUNNER_H__
////////////////////////////////
#include "activeobject.h"
///////////////////////////////////
namespace info {
	///////////////////////////////////////
	class CancellableActive {
	public:
		using cancelflag = std::atomic<bool>;
		using pcancelflag = cancelflag *;
		using Operation = std::function<void(pcancelflag)>;
	private:
		std::atomic<bool> done;
		std::atomic<bool> m_cancel;
		DispatchQueue<Operation> dispatchQueue;
		std::thread runnable;
	private:
		CancellableActive() : done(false),m_cancel(false) {
		} // Active
		CancellableActive(const CancellableActive&) = delete;
		CancellableActive& operator=(const Active&) = delete;
		void run() {
			while (!this->done.load()) {
				(this->dispatchQueue.take())(&this->m_cancel);
			} // while
		} // run
	public:
		virtual ~CancellableActive() {
			// Schedule a No-Op runnable to flush the dispatch queue
			this->m_cancel.store(true);
			dispatchQueue.put([this]() {this->done.store(true); });
			runnable.join();
		} // run
		bool is_cancelled(void) {
			return (this->m_cancel.load());
		}
		void reset_cancel(void) {
			this->m_cancel.store(false);
		}
		void cancel(void) {
			this->m_cancel.store(true);
		}
		void send(Operation msg_) {
			this->dispatchQueue.put(msg_);
		} // send
		void clear(void) {
			this->dispatchQueue.clear();
		}
		// Factory: safe construction of object before thread start
		static std::unique_ptr<CancellableActive> createActive(void) {
			std::unique_ptr<CancellableActive> aPtr(new CancellableActive());
			aPtr->runnable = std::thread(&CancellableActive::run, aPtr.get());
			return aPtr;
		}	 // create
	};
	// class CancellableActive
	///////////////////////////////////
	class InfoRunner  {
	public:
		using cancelflag = std::atomic<bool>;
		using pcancelflag = cancelflag *;
		using Operation = std::function<void(pcancelflag)>;
	private:
		std::unique_ptr<CancellableActive> _active;
		InfoRunner(const InfoRunner &) = delete;
		InfoRunner & opertor= (const InfoRunner &) = delete;
	protected:
		void clear() {
			this->_active->clear();
		}// clear
	public:
		InfoRunner() :
			_active(CancellableActive::createActive()) {
		}
		virtual ~InfoRunner() {
			this->_active->cancel();
		}
		void send(Operation op) {
			this->_active->send(op);
		}	 // send
		bool is_cancelled(void) {
			return (this->_active->is_cancelled());
		}
		void reset_cancel(void) {
			this->_active->reset_cancel();
		}
		void cancel(void) {
			this->_active->cancel();
		}
	};// class InfoRunner
	////////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__INFORUNNER_H__

