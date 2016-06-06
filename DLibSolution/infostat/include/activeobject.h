/*
 * activeobject.h
 *
 *  Created on: 31 mai 2016
 *      Author: boubad
 */

#ifndef ACTIVEOBJECT_H_
#define ACTIVEOBJECT_H_
//////////////////////////
#include "info_includes.h"
#include "sharedqueue.h"
/////////////////////////////
namespace info {
////////////////////////
typedef std::function<void()> Operation;
/////////////////////////
class Active {
	std::atomic<bool> done;
	DispatchQueue<Operation> dispatchQueue;
	std::thread runnable;
private:
	Active() :
			done(false) {
	} // Active
	Active(const Active&) = delete;
	Active& operator=(const Active&) = delete;
	void run() {
		while (!this->done.load()) {
			(this->dispatchQueue.take())();
		} // while
	} // run
public:
	~Active() {
		// Schedule a No-Op runnable to flush the dispatch queue
		dispatchQueue.put([this]() {this->done.store(true);});
		runnable.join();
	} // run
	void send(Operation msg_) {
		this->dispatchQueue.put(msg_);
	} // send
	  // Factory: safe construction of object before thread start
	static std::unique_ptr<Active> createActive(void) {
		std::unique_ptr<Active> aPtr(new Active());
		aPtr->runnable = std::thread(&Active::run, aPtr.get());
		return aPtr;
	}	 // create
};
// class ActiveObject
//////////////////////////////////
class Backgrounder {
private:
	std::unique_ptr<Active> _active;
protected:
	void put(Operation op) {
		this->_active->send(op);
	}	 // put
public:
	Backgrounder() :
			_active(Active::createActive()) {
	}
	virtual ~Backgrounder() {
	}
	void send(Operation op) {
		this->_active->send(op);
	}	 // send
};
// class Backgrounder
//////////////////////////////
}// namespace info
///////////////////////
#endif /* ACTIVEOBJECT_H_ */