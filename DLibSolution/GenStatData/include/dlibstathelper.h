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
		double average(void) const {
			return (this->_vmean);
		}
		double standard_deviation(void) const {
			return (this->_vstd);
		}
		double minimum(void) const {
			return (this->_vmin);
		}
		double maximum(void) const {
			return (this->_vmax);
		}
		size_t count(void) const {
			return (this->_count);
		}
		void get(IntType &index, double &vmin, double &vmax, double &vmean, double &vstd) const;
	};// class InfoStatData
	////////////////////////////////////////
	class DLibStatHelper {
	public:
		typedef std::shared_ptr<InfoStatData> InfoStatDataPtr;
		typedef std::map<IntType,InfoStatDataPtr> infostatdata_map;
		typedef std::map<IntType, DbValue> DbValueMap;
		typedef std::map<IntType, DBStatVariable> variables_map;
		typedef std::vector<IntType> ints_vector;
		typedef std::shared_ptr<double> double_ptr;
	private:
		IIndivProvider		*m_provider;
		infostatdata_map	m_stats;
		variables_map	m_vars;
		ints_vector m_ids;
		//
		void initialize(void);
	public:
		DLibStatHelper(IIndivProvider *pProvider);
		~DLibStatHelper();
	public:
		size_t variables_count(void) const {
			return (this->m_ids.size());
		}
		void get_variables_ids(ints_vector &v) const {
			 v = this->m_ids;
		}
		void get_variables_stats(infostatdata_map &oMap) const {
			oMap = this->m_stats;
		}
		void get_variables_map(variables_map &oMap) const {
			oMap = this->m_vars;
		}
		void convert_indiv(const Indiv &oInd, double_ptr &oData) const;
		size_t convert_indivs(std::vector<double_ptr> &ovec);
		void convert_indivs(size_t &nRows, size_t &nCols, double_ptr &oPtr);
	};// class DLibStatHelper
	///////////////////////////////
}// namespace info
/////////////////////////////////////
#endif // !__DLIBSTATHELPER_H__
