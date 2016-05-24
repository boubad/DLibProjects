#pragma once
#ifndef __DISTANCEMAP_H__
#define __DISTANCEMAP_H__
///////////////////////////////////
#include "crititem.h"
/////////////////////////////
namespace info {
	//////////////////////////////////////
	template<typename U = unsigned long, typename W = long>
	class DistanceMap {
	public:
		using ints_set = std::set<U>;
		using entry_type = std::map<U, W>;
		using map_type = std::map<U, entry_type>;
		using DistanceType = DistanceMap<U, W>;
		using DistanceMapType = DistanceMap<U, W>;
	private:
		map_type m_map;
		ints_set m_set;
		//
		std::mutex _mutex;
	public:
		DistanceMap() {
		}
		DistanceMap(const DistanceMapType &other) :m_map(other.m_map), m_set(other.m_set) {}
		DistanceMapType & operator=(const DistanceMapType &other) {
			if (this != &other) {
				this->m_map = other.m_map;
				this->m_set = other.m_map;
			}
			return (*this);
		}
		virtual ~DistanceMap() {
		}
	public:
		template <typename XU>
		bool has_entry(const XU i1, const XU i2) {
			U index1 = (U)i1, index2 = (U)i2;
			if (index1 > index2) {
				const U t = index1;
				index1 = index2;
				index2 = t;
			}
			map_type &oMap = this->m_map;
			auto it = oMap.find(index1);
			if (it == oMap.end()) {
				return (false);
			}
			entry_type &e = (*it).second;
			return (e.find() != e.end());
		}
		template <typename XU, typename XW>
		bool get(const XU i1, const XU i2, XW &res) {
			U index1 = (U)i1, index2 = (U)i2;
			if (index1 > index2) {
				const U t = index1;
				index1 = index2;
				index2 = t;
			}
			map_type &oMap = this->m_map;
			auto it = oMap.find(index1);
			if (it == oMap.end()) {
				return (false);
			}
			entry_type &e = (*it).second;
			auto jt = e.find(index2);
			if (jt != e.end()) {
				res = (XW)(*jt).second;
				return (true);
			}
			return (false);
		}
		void clear(void) {
			std::lock_guard<std::mutex> oLock(this->_mutex);
			this->m_map.clear();
			this->m_set.clear();
		} // clear
		const ints_set & indexes(void) const {
			return (this->m_set);
		}
		void indexes(ints_set &oSet) {
			oSet = this->m_set;
		}
		template <typename XU, typename XW>
		void add(const XU i1, const XU i2, const XW xval) {
			if (i1 == i2) {
				return;
			}
			U index1 = (U)i1, index2 = (U)i2;
			if (index1 > index2) {
				const U t = index1;
				index1 = index2;
				index2 = t;
			}
			W val = (W)xval;
			std::lock_guard<std::mutex> oLock(this->_mutex);
			map_type &oMap = this->m_map;
			auto it = oMap.find(index1);
			if (it == oMap.end()) {
				entry_type e;
				e[index2] = val;
				oMap[index1] = e;
			}
			else {
				entry_type &e = (*it).second;
				e[index2] = val;
			}
			ints_set &oSet = this->m_set;
			oSet.insert(index1);
			oSet.insert(index2);
		} // add
		template <typename XU, typename XW>
		void add(const CritItem<XU, XW> &oCrit) {
			this->add((U)oCrit.first(), (U)oCrit.second(), (W)oCrit.criteria());
		} // add
		void increase_count(const U i1, const U i2) {
			if (i1 == i2) {
				return;
			}
			U index1 = i1, index2 = i2;
			if (index1 > index2) {
				const U t = index1;
				index1 = index2;
				index2 = t;
			}
			std::lock_guard<std::mutex> oLock(this->_mutex);
			map_type &oMap = this->m_map;
			auto it = oMap.find(index1);
			if (it == oMap.end()) {
				entry_type e;
				e[index2] = (W)1;
				oMap[index1] = e;
			}
			else {
				entry_type &e = (*it).second;
				auto jt = e.find(index2);
				if (jt == e.end()) {
					e[index2] = (W)1;
				}
				else {
					W old = (*jt).second;
					++old;
					e[index2] = old;
				}
			}
			ints_set &oSet = this->m_set;
			oSet.insert(index1);
			oSet.insert(index2);
		} // add
	};
	// class DistanceMap<U,W>
	/////////////////////////////////////
}// namespace info
 //////////////////////////////
////////////////////////////////////
#endif // !__DISTANCEMAP_H__
