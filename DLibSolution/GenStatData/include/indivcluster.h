#pragma once
#ifndef __INDIVCLUSTER_H__
#define __INDIVCLUSTER_H__
//////////////////////////////////////////
#include "indiv.h"
/////////////////////////////////////////////
#include <deque>
/////////////////////////////////////////////////
#include <dlib/threads.h>
//////////////////////////////////////////////
namespace info {
	/////////////////////////////////////////
	class IndivCluster : public Indiv {
	public:
		typedef std::deque<IntType> ints_deque;
		typedef std::map<IntType, DbValue> DataMapType;
	private:
		IIndivProvider *m_provider;
		ints_deque m_members;
	private:
		dlib::read_write_mutex _mutex;
	public:
		IndivCluster(IIndivProvider *pProvider = nullptr);
		IndivCluster(const IntType aIndex, const DataMapType &oMap, IIndivProvider *pProvider = nullptr);
		IndivCluster(const Indiv &oInd, IIndivProvider *pProvider = nullptr);
		IndivCluster(const IndivCluster &other);
		IndivCluster & operator=(const IndivCluster &other);
		virtual ~IndivCluster();
	public:
		IIndivProvider *get_provider(void) const;
		void set_provider(IIndivProvider *pProvider);
		void get_members(ints_deque &oRes) const;
		void clear_members(void);
		void update_center(void);
		bool add(const Indiv &oInd, bool bOrder = false);
		IndivCluster & operator+=(const Indiv &oInd);
		void swap(IndivCluster &other);
	public:
		virtual void get_data(DataMapType &oMap) const;
		virtual void set_data(const DataMapType &oMap);
	};// class IndivCluster
	////////////////////////////////////////////

	///////////////////////////////////////////////
}// namespace info
///////////////////////////////////////////////
inline void swap(info::IndivCluster &v1, info::IndivCluster &v2) {
	v1.swap(v2);
}
//////////////////////////////////////////
#endif // !__INDIVCLUSTER_H__
