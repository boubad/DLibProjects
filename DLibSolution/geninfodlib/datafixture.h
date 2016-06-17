/*
 * datafixture.h
 *
 *  Created on: 15 juin 2016
 *      Author: boubad
 */

#ifndef DATAFIXTURE_H_
#define DATAFIXTURE_H_
 /////////////////////////////////
#include "basefixture.h"
////////////////////////////
namespace info {
	///////////////////////////////////
	template<typename U, typename STRINGTYPE, typename DATATYPE>
	class InfoDataFixture : public DataFixture<U, STRINGTYPE, DATATYPE> {
	public:
		using cancelflag = std::atomic<bool>;
		using pcancelflag = cancelflag *;
		using PBackgrounder = Backgrounder *;
	private:
		std::unique_ptr<InfoRunner> m_runner;
	public:
		InfoDataFixture() {
		}
		virtual ~InfoDataFixture() {
		}
	public:
		InfoRunner *get_runner(void) {
			InfoRunner *p = this->m_runner.get();
			if (p == nullptr) {
				this->m_runner.reset(new InfoRunner());
				p = this->m_runner.get();
				assert(p != nullptr);
			}
			return (p);
		}
		PBackgrounder get_backgrounder(void) {
			InfoRunner *p = this->get_runner();
			return (p->get_backgrounder());
		}
		pcancelflag get_cancelflag(void) {
			InfoRunner *p = this->get_runner();
			return (p->get_cancelflag());
		}
		void reset_cancel(void) {
			InfoRunner *p = this->get_runner();
			p->reset_cancel();
		}
		void cancel(void) {
			InfoRunner *p = this->get_runner();
			p->cancel();
		}
	};
	// class InfoDataFixture<U,STRINGTYPE,DATATYPE>
	///////////////////////////////////////
}// namespace info
///////////////////////////////
#endif /* DATAFIXTURE_H_ */
