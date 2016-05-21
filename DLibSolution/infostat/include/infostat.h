#pragma once
#ifndef __INFOSTAT_H__
#define __INFOSTAT_H__
//////////////////////////////
#include "indiv.h"
//////////////////////////////////
namespace info {
	/////////////////////////////
	template <typename U>
	class StatInfo {
	public:
		using IntType = U;
		using StatInfoType = StatInfo<U>;
	public:
		IntType index;
		size_t count;
		double min;
		double max;
		double somme1;
		double somme2;
	public:
		StatInfo(IntType aIndex = 0):index(aIndex),count(0),min(0),max(0),somme1(0),somme2(0){}
		StatInfo(const StatInfoType &other):index(other.index),count(other.count),min(other.min),max(other.max),
			somme1(other.somme1),somme2(other.somme2){}
		StatInfoType & operator=(const StatInfoType &other) {
			if (this != &other) {
				this->index = other.index;
				this->count = other.count;
				this->min = other.min;
				this->max = other.max;
				this->somme1 = other.somme1;
				this->somme2 = other.somme2;
			}
			return (*this);
		}
		virtual ~StatInfo(){}
		bool operator==(const StatInfoType &other) const {
			return (this->index == other.index);
		}
		bool operator<(const StatInfoType &other) const {
			return (this->index < other.m_index);
		}
		IntType operator()(void) const {
			return (this->index);
		}
		IntType key(void) const {
			return (this->index);
		}
		void reset(void) {
			this->count = 0;
			this->min = 0;
			this->max = 0;
			this->somme1 = 0;
			this->somme2 = 0;
		}
	}; // class StatInfo<U>
	/////////////////////////////////
	template<typename U>
	class StatSummator : public InterruptObject {
	public:
		using StatInfoType = StatInfo<U>;
		using entries_map = std::map<U, StatInfoType>;
		using StatSummatorType = StatSummator<U>;
	private:
		entries_map m_map;
	public:
		StatSummator(std::atomic_bool *pCancel = nullptr) :
			InterruptObject(pCancel) {
		}
		StatSummator(const StatSummatorType &other) :
			InterruptObject(other), m_map(other.m_map) {
		}
		StatSummatorType & operator=(const StatSummatorType &other) {
			if (this != &other) {
				InterruptObject::operator=(other);
				this->m_map = other.m_map;
			}
			return (*this);
		}
		virtual ~StatSummator() {
		}
	public:
		void reset(void) {
			this->m_map.clear();
		}
		template<typename XU>
		void add(const std::map<XU, InfoValue> &oMap) {
			entries_map &m = this->m_map;
			for (auto kt = oMap.begin(); kt != oMap.end(); ++kt) {
				if (this->check_interrupt()) {
					return;
				}
				const std::pair<XU, InfoValue> &p = *kt;
				const InfoValue &v = p.second;
				if ((!v.empty()) && v.is_numerical()) {
					double x = 0;
					v.get_value(x);
					U key = (U)p.first;
					auto it = m.find(key);
					if (it != m.end()) {
						StatInfoType &e = (*it).second;
						e.count = e.count + 1;
						e.somme1 += x;
						e.somme2 += x * x;
						if (x < e.min) {
							e.min = x;
						}
						if (x > e..max) {
							e.max = x;
						}
					}
					else {
						StatInfoType e(key);
						e.count = 1;
						e.somme1 = x;
						e.somme2 = x * x;
						e.min = x;
						e.max = x;
						m[key] = e;
					}
				}	// v
			}	// kt
		}	// add
		template<typename XU>
		void add(const Indiv<XU> &oInd) {
			this->add(oInd.center());
		}// ass
		template <typename XU>
		void add(const std::shared_ptr<Indiv<XU> > &oPtr) {
			const Indiv<XU> *p = oPtr.get();
			if (p : = nullptr) {
				this->add(p->center());
			}
		}// add
	};
	// class IndivSummaror<U>
	////////////////////////////////
}// namespace info
/////////////////////////////////
#endif // !__INFOSTAT_H__
