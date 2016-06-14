/*
 * activeobject.h
 *
 *  Created on: 31 mai 2016
 *      Author: boubad
 */

#ifndef ACTIVEOBJECT_H_
#define ACTIVEOBJECT_H_
 //////////////////////////
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>
/////////////////////////////
namespace info {
	////////////////////////
	template <typename T>
	class DispatchQueue {
		std::atomic<bool> m_abort;
		T m_dummy;
		std::mutex qlock;
		std::queue<T> ops_queue;
		std::condition_variable empty;
		//
		DispatchQueue(const DispatchQueue<T> &) = delete;
		DispatchQueue<T> & operator=(const DispatchQueue<T> &) = delete;
	public:
		DispatchQueue(T dummy) :m_abort(false), m_dummy(dummy) {}
		virtual ~DispatchQueue() {}
	public:
		void abort(void) {
			this->m_abort.store(true);
		}// abort
		bool is_aborting(void) {
			return (this->m_abort.load());
		}
		void put(T op) {
			std::lock_guard<std::mutex> guard(this->qlock);
			this->ops_queue.push(op);
			this->empty.notify_all();
		} // put
		T take() {
			std::unique_lock<std::mutex> lock(this->qlock);
			this->empty.wait(lock, [this] {return !this->ops_queue.empty(); });
			T oRet(this->m_dummy);
			if (this->m_abort.load()) {
				while (!this->ops_queue.empty()) {
					oRet = this->ops_queue.front();
					this->ops_queue.pop();
				}// while
			}
			else {
				oRet = this->ops_queue.front();
				this->ops_queue.pop();
			}
			return (oRet);
		} // take
	};
	/////////////////////////
	class Active {
	public:
		using Operation = std::function<void()>;
	private:
		std::atomic<bool> done;
		std::unique_ptr<DispatchQueue<Operation> > m_dispatchQueue;
		std::thread runnable;
	private:
		Active() : done(false) {
			this->m_dispatchQueue.reset(new DispatchQueue<Operation>([]() {}));
			this->runnable = std::thread([this]() {
				this->run();
			});
		} // Active
		Active(const Active&) = delete;
		Active& operator=(const Active&) = delete;
	public:
		~Active() {
			this->stop();
		} // run
		void run() {
			while (!this->done.load()) {
				DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
				if (p == nullptr) {
					break;
				}
				if (p->is_aborting()) {
					break;
				}
				Operation op = p->take();
				op();
			} // while
		} // run
		void stop(void) {
			DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
			if (p != nullptr) {
				p->abort();
			}
			if (this->runnable.joinable()) {
				this->done.store(true);
				DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
				if (p != nullptr) {
					p->put([this]() {
						this->done.store(true);
					});
				}
				this->runnable.join();
			}
		}
		void send(Operation msg_) {
			DispatchQueue<Operation> *p = this->m_dispatchQueue.get();
			if (p != nullptr) {
				p->put(msg_);
			}
		} // send
		static Active *createActive(void) {
			Active *pRet = new Active();
			return pRet;
		}// createActive
	};
	// class ActiveObject
	//////////////////////////////////
	class Backgrounder {
	public:
		using Operation = std::function<void()>;
	private:
		std::unique_ptr<Active> _active;
	protected:
		void put(Operation op) {
			this->_active->send(op);
		}	 // put
	public:
		Backgrounder() {
			this->_active.reset(Active::createActive());
		}
		virtual ~Backgrounder() {
			this->stop();
		}
		void send(Operation op) {
			Active *p = this->_active.get();
			if (p == nullptr) {
				this->_active.reset(Active::createActive());
				p = this->_active.get();
			}
			p->send(op);
		}	 // send
		void stop(void) {
			Active *p = this->_active.get();
			if (p != nullptr) {
				p->stop();
			}
		}// stop
	};
	// class Backgrounder
	//////////////////////////////
}// namespace info
///////////////////////
#endif /* ACTIVEOBJECT_H_ */
