#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
////////////////////////////////
#include <map>
#include <memory>
#include <algorithm>
////////////////////////////////////
namespace info {
	//////////////////////////////////
	template <typename U, typename T, typename W>
	bool info_global_compute_distance(const std::map<U, T> &oMap1,
		const std::map<U, T> &oMap2, W &res) {
		size_t nc = 0;
		res = 0;
		std::for_each(oMap1.begin(), oMap1.end(), [&](const std::pair<T, U> &p) {
			const U key = p.first;
			auto it = oMap2.find(key);
			if (it != oMap2.end()) {
				const T v1 = p.second;
				const T v2 = (*it).second;
				const W tt = (W)v1 - (W)v2;
				res = (W)(res + tt * tt);
				++nc;
			}// it
		});
		if (nc > 1) {
			res = (W)(res / nc);
		}
		return (nc > 0);
	}//info_global_compute_distance
	/////////////////////////////
	template <typename U = unsigned long, typename T = float>
	class Indiv {
	public:
		using IndivType = Indiv<U, T>;
		using DataMap = std::map<U, T>;
	private:
		U m_index;
		DataMap m_center;
	public:
		Indiv(const U aIndex = 0) :m_index(0) {}
		template <typename XU, typename XT>
		Indiv(const XU aIndex, const std::map<XU, XT> &oMap) : m_index((U)aIndex) {
			DataMap &m = this->m_center;
			std::for_each(oMap.begin(), oMap.end(), [&map](const std::pair<XU, XT> &p) {
				m[(U)p.first] = (T)p.second;
			});
		}
		Indiv(IndivType &other) :m_index(other.m_index), m_center(other.m_center) {
		}
		IndivType & operator=(const IndivType &other) {
			if (this != &other) {
				this->m_index = other.m_index;
				this->m_center = other.m_center;
			}
			return (*this);
		}
		virtual ~Indiv() {}
	public:
		U id(void) const {
			return (this->m_index);
		}
		void id(const U a) {
			this->m_index = a;
		}
		const DataMap & center(void) const {
			return (this->m_center);
		}
		template <typename XU, typename XT>
		void center(const std::map<XU, XT> &oMap) {
			DataMap &m = this->m_center;
			m.clear();
			std::for_each(oMap.begin(), oMap.end(), [&map](const std::pair<XU, XT> &p) {
				m[(U)p.first] = (T)p.second;
			});
		}
		template <typename W>
		bool distance(const IndivType &other, W &res) const {
			return (info_global_compute_distance(this->m_center, other.m_center, res));
		}// distance
	};// class Indiv<U,T>
	//////////////////////////////////////
	template <typename U = unsigned long, typename T = float>
	class IIndivSource {
	public:
		using IndivType = Indiv<U, T>;
		using DataMap = std::map<U, T>;
		using IndivTypePtr = std::shared_ptr<IndivType>;
	public:
		virtual size_t count(void) = 0;
		virtual IndivTypePtr get(const size_t pos) = 0;
		virtual void reset(void) = 0;
		virtual IndivTypePtr next(void) = 0;
	public:
		virtual IIndivSource() {}
	};// class IIndivSource<U,T>
	////////////////////////////////
}// namespace info
///////////////////////////////
#endif // !__INDIV_H__
