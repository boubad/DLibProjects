#pragma once
#ifndef __INFORUNNER_H__
#define __INFORUNNER_H__
////////////////////////////////
#include <cassert>
#include "activeobject.h"
///////////////////////////////////
namespace info {
	////////////////////////////////////////
	class CancellableObject {
	public:
		using cancelflag = std::atomic<bool>;
		using pcancelflag = cancelflag *;
		using PBackgrounder = Backgrounder *;
		using Operation_result = std::function<void()>;
		
	private:
		pcancelflag m_pcancel;
		PBackgrounder m_pq;

	public:
		CancellableObject(pcancelflag pFlag = nullptr, PBackgrounder pq = nullptr) :
			m_pcancel(pFlag), m_pq(pq) {
		}
		CancellableObject(const CancellableObject &other) :m_pcancel(other.m_pcancel), m_pq(other.m_pq) {

		}
		CancellableObject & operator=(const CancellableObject &other) {
			if (this != &other) {
				this->m_pcancel = other.m_pcancel;
				this->m_pq = other.m_pq;
			}
			return (*this);
		}
		virtual ~CancellableObject() {
		}
	protected:
		pcancelflag get_cancelflag(void) {
			return (this->m_pcancel);
		}
		PBackgrounder get_backgrounder(void) {
			return (this->m_pq);
		}
		void send_result(Operation_result rf) {
			PBackgrounder pq = this->m_pq;
			if (pq != nullptr) {
				pq->send(rf);
			}
		}
		bool is_cancelled(void) {
			return ((this->m_pcancel != nullptr) && this->m_pcancel->load());
		}// is_cancelled
	};// class CancellableObject
	///////////////////////////////////////
	class CancellableActive {
		using mutex_type = std::mutex;
		using lock_type = std::lock_guard<mutex_type>;
	public:
		using cancelflag = std::atomic<bool>;
		using pcancelflag = cancelflag *;
		using PBackgrounder = Backgrounder *;
		using Operation = std::function<void(pcancelflag, PBackgrounder)>;
		using Operation_result = std::function<void()>;
	private:
		std::atomic<bool> done;
		std::atomic<bool> m_cancel;
		std::unique_ptr<Backgrounder> m_backgrounder;
		std::unique_ptr<DispatchQueue<Operation> > m_dispatchQueue;
		std::thread runnable;
	private:
		CancellableActive() : done(false), m_cancel(false) {
			this->m_backgrounder.reset(new Backgrounder());
			this->m_dispatchQueue.reset(new DispatchQueue<Operation>([](pcancelflag f, PBackgrounder g) {}));
			this->runnable = std::thread([this]() {
				this->run();
			});
		} // Active
		CancellableActive(const CancellableActive&) = delete;
		CancellableActive& operator=(const Active&) = delete;
	public:
		void run() {
			DispatchQueue<Operation> *pQueue = this->m_dispatchQueue.get();
			if (pQueue != nullptr)  {
				PBackgrounder pq = this->m_backgrounder.get();
				pcancelflag pCancel = &(this->m_cancel);
				std::atomic<bool> &oDone = this->done;
				while (!oDone.load()) {
					if (pQueue->is_aborting()) {
						break;
					}
					Operation op = pQueue->take();
					if (!pCancel->load()) {
						op(pCancel, pq);
					}
				} // while
			}// pQueue
		} // run
		virtual ~CancellableActive() {
			this->stop();
		} // run
		void stop(void) {
			PBackgrounder pBack = this->m_backgrounder.get();
			if (pBack != nullptr) {
				pBack->stop();
			}
			DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
			if (p != nullptr) {
				p->abort();
			}
			if (this->runnable.joinable()) {
				this->done.store(true);
				DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
				if (p != nullptr) {
					p->put([this](pcancelflag f,PBackgrounder g) {
						this->done.store(true);
					});
				}
				this->runnable.join();
			}
			this->m_dispatchQueue.reset();
			this->m_backgrounder.reset();
		}// stop
		pcancelflag get_cancelflag(void) {
			return (&(this->m_cancel));
		}
		PBackgrounder get_backgrounder(void) {
			return (this->m_backgrounder.get());
		}
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
			DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
			if (p != nullptr) {
				p->put(msg_);
			}
		} // send
		void send_result(Operation_result rf) {
			PBackgrounder pq = this->get_backgrounder();
			if (pq != nullptr) {
				pq->send(rf);
			}
		}
		// Factory: safe construction of object before thread start
		static CancellableActive *createActive(void) {
			CancellableActive *p = new CancellableActive();
			return p;
		}	 // create
	};
	// class CancellableActive
	///////////////////////////////////
	class InfoRunner {
	public:
		using cancelflag = std::atomic<bool>;
		using pcancelflag = cancelflag *;
		using PBackgrounder = Backgrounder *;
		using Operation = std::function<void(pcancelflag, PBackgrounder)>;
	private:
		std::unique_ptr<CancellableActive> _active;
	protected:
		CancellableActive *get_active(void) {
			CancellableActive *p = this->_active.get();
			if (p == nullptr) {
				this->_active.reset(CancellableActive::createActive());
				p = this->_active.get();
			}
			return (p);
		}
	public:
		InfoRunner() {}
		virtual ~InfoRunner() {
			this->stop();
		}
		void stop(void) {
			CancellableActive *p = this->_active.get();
			if (p != nullptr) {
				p->stop();
				this->_active.reset();
			}
		}// stop
		PBackgrounder get_backgrounder(void) {
			CancellableActive *p = this->get_active();
			assert(p != nullptr);
			return (p->get_backgrounder());
		}
		pcancelflag get_cancelflag(void) {
			CancellableActive *p = this->get_active();
			assert(p != nullptr);
			return (p->get_cancelflag());
		}
		void send(Operation op) {
			CancellableActive *p = this->get_active();
			assert(p != nullptr);
			p->send(op);
		}	 // send
	};// class InfoRunner
	////////////////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__INFORUNNER_H__

