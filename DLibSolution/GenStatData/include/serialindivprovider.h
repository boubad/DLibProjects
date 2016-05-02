#pragma once
#ifndef __SERIALINDIVPROVIDER_H__
#define __SERIALINDIVPROVIDER_H__
/////////////////////////////////////
#include "indiv.h"
//////////////////////////////////////
#include <dlib/threads.h>
/////////////////////////////////////////
namespace info {
	/////////////////////////////////
	class SerialIndivProvider : public ISerialIndivProvider {
	private:
		IIndivProvider *m_provider;
		size_t m_current;
	private:
		dlib::mutex _mutex;
	public:
		SerialIndivProvider(IIndivProvider *pProvider) :m_provider(pProvider), m_current(0) {
		}
		virtual bool reset(void) {
			IIndivProvider *p = this->m_provider;
			if (p == nullptr) {
				return (false);
			}
			if (!p->is_valid()) {
				return (false);
			}
			{
				dlib::auto_mutex oLock(this->_mutex);
				this->m_current = 0;
			}
			return (true);
		}
		virtual bool next(Indiv &oInd, const VariableMode mode = VariableMode::modeAll) {
			IIndivProvider *p = this->m_provider;
			if (p == nullptr) {
				return (false);
			}
			if (!p->is_valid()) {
				return (false);
			}
			size_t nMax = 0;
			if (!p->indivs_count(nMax)) {
				return (false);
			}
			bool bRet = false;
			{
				dlib::auto_mutex oLock(this->_mutex);
				size_t n = this->m_current;
				if (n < nMax) {
					bRet = p->find_indiv_at(n, oInd, mode);
				}
				if (bRet) {
					this->m_current = n + 1;
				}
			}
			return (bRet);
		} // next
	};// class SerialIndivProvider
	////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__SERIALINDIVPROVIDER_H__
