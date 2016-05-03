#pragma once
#ifndef __DLIBSTATHELPER_H__
#define __DLIBSTATHELPER_H__
///////////////////////////////////
#include <vector>
#include <set>
#include <memory>
/////////////////////////////////////
#include <dlib/matrix.h>
#include <dlib/threads.h>
////////////////////////////////////
#include "indiv.h"
//////////////////////////////////////////
namespace info {
	//////////////////////////////////////////
	typedef dlib::matrix<double, 0, 1> indiv_type;
	typedef std::vector<indiv_type> indivtype_vector;
	typedef std::set<IntType> ints_set;
	///////////////////////////////////////////
	class InfoStatData {
	private:
		IntType _index;
		double _vmin;
		double _vmax;
		double _vmean;
		double _vstd;
	private:
		size_t _count;
		size_t _somme1;
		size_t _somme2;
	private:
		dlib::mutex _mutex;
	public:
		InfoStatData(IntType aIndex = 0);
		InfoStatData(const InfoStatData &other);
		InfoStatData & operator=(const InfoStatData &other);
		~InfoStatData();
	public:
		bool operator==(const InfoStatData &other) const;
		bool operator<(const InfoStatData &other) const;
		IntType operator()(void) const {
			return (this->_index);
		}
	public:
		void add(const double dval);
		void compute(void);
	public:
		IntType index(void) const {
			return (this->_index);
		}
		void get(IntType &index, double &vmin, double &vmax, double &vmean, double &vstd) const;
	};// class InfoStatData
	////////////////////////////////////////
	class DLibStatHelper {
	public:
		typedef std::shared_ptr<InfoStatData> InfoStatDataPtr;
		typedef std::map<IntType,InfoStatDataPtr> infostatdata_map;
		typedef std::map<IntType, DbValue> DbValueMap;
	private:
		IIndivProvider		*m_provider;
		infostatdata_map	m_stats;
		//
		void initialize(void);
	public:
		DLibStatHelper(IIndivProvider *pProvider);
		~DLibStatHelper();
	};// class DLibStatHelper
	///////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__DLIBSTATHELPER_H__
