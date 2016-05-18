#pragma once
#ifndef __BASEITEM_H__
#define __BASEITEM_H__
/////////////////////////////////////
#include "stringconvert.h"
//////////////////////////////////
#include <boost/any.hpp>
////////////////////////////////
namespace info {
	///////////////////////////////////////
	template <typename IDTYPE, typename INTTYPE, typename STRINGTYPE>
	class BaseInfoStatItem {
		using BaseInfoStatItemType = BaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE>;
	private:
		IDTYPE m_id;
		INTTYPE m_version;
		STRINGTYPE m_status;
	protected:
		BaseInfoStatItem() :m_id(0), m_version(0) {}
		BaseInfoStatItem(const BaseInfoStatItemType &other) :m_id(other.m_id), m_version(other.m_version), m_status(other.m_status) {}
		BaseInfoStatItemType & operator=(const BaseInfoStatItemType &other) {
			if (this != &other) {
				this->m_id = other.m_id;
				this->m_version = other.m_version;
				this->m_status = other.m_status;
			}
			return (*this);
		}
	public:
		bool operator==(const BaseInfoStatItem &other) const {
			return (this->m_id == other.m_id);
		}
		bool operator<(const BaseInfoStatItem &other) const {
			return (this->m_id == other.m_id);
		}
		IDTYPE operator()(void) const {
			return (this->m_id);
		}
		IDTYPE id(void) const {
			return (this->m_id);
		}
		void id(const IDTYPE n) {
			this->m_id = n;
		}
		INTTYPE version(void) const {
			return (this->m_version);
		}
		void version(const INTTYPE n) {
			this->m_version = n;
		}
		const STRINGTYPE &status(void) const {
			return (this->m_status);
		}
		void status(const STRINGTYPE &s) {
			this->m_status = info_trim_upper(s);
		}
		bool gas_id(void) const {
			return (this->m_id != 0);
		}
		bool is_updateable(void) const {
			return (this->has_id() && this->is_writeabale());
		}
	public:
		virtual ~BaseInfoStatItem() {}
		virtual void clear(void) {
			this->m_id = 0;
			this->m_version = 0;
			this->m_status.clear();
		}
		virtual bool is_removeable(void) const {
			return (this->m_id != 0);
		}
		virtual bool is_writeable(void) const {
			return (false);
		}
	}; // class BaseInfoStatItem<IDTYPE,INTTYPE,STRINGTYPE>
	/////////////////////////////////////////
	template <typename IDTYPE, typename INTTYPE, typename STRINGTYPE>
	class NamedBaseInfoStatItem : public BaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE> {
		using BaseInfoStatItemType = BaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE>;
		using NamedBaseInfoStatItemType = NamedBaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE>;
	private:
		STRINGTYPE m_sigle;
		STRINGTYPE m_name;
		STRINGTYPE m_desc;
	protected:
		NamedBaseInfoStatItem() {}
		NamedBaseInfoStatItem(const STRINGTYPE &sSigle) {
			this->sigle(sSigle);
			this->name(sSigle);
			this->description(sSigle);
		}
		NamedBaseInfoStatItem(const NamedBaseInfoStatItemType &other) :BaseInfoStatItemType(other), m_sigle(other.m_sigle),
			m_name(other.m_name), m_desc(other.m_desc) {}
		NamedBaseInfoStatItemType & operator=(const NamedBaseInfoStatItemType &other) {
			if (this != &other) {
				BaseInfoStatItemType::operator=(other);
				this->m_sigle = other.m_sigle;
				this->m_name = other.m_name;
				this->m_desc = other.m_desc;
			}
			return (*this);
		}
	public:
		virtual ~NamedBaseInfoStatItem() {}
		const STRINGTYPE & sigle(void) const {
			return (this->m_sigle);
		}
		void sigle(const STRINGTYPE &s) {
			this->m_sigle = info_trim_upper(s);
		}
		const STRINGTYPE & name(void) const {
			return (this->m_name);
		}
		void name(const STRINGTYPE &s) {
			this->m_name = info_trim(s);
		}
		const STRINGTYPE & description(void) const {
			return (this->m_desc);
		}
		void description(const STRINGTYPE &s) {
			this->m_desc = info_trim(s);
		}
		virtual void clear(void) {
			BaseInfoStatItemType::clear();
			this->m_sigle.clear();
			this->m_name.clear();
			this->m_desc.clear();
		}
		virtual bool is_writeable(void) const {
			return (!this->m_sigle.empty());
		}
	}; // class NamedBaseInfoStatItem<IDTYPE,INTTYPE,STRINGTYPE>;
	///////////////////////////////////////////
	template <typename IDTYPE = unsigned long, typename INTTYPE = int, typename STRINGTYPE = std::string>
	class StatDataset : public NamedBaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE> {
		using NamedBaseInfoStatItemType = NamedBaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE>;
		using StatDatasetType = StatDataset<IDTYPE, INTTYPE, STRINGTYPE>;
	public:
		StatDataset() {}
		StatDataset(const IDTYPE n) {
			this->id(n);
		}
		StatDataset(const STRINGTYPE &s):NamedBaseInfoStatItemType(s){}
		StatDataset(const StatDatasetType &other) :NamedBaseInfoStatItemType(other) {}
		StatDatasetType & operator=(const StatDatasetType &other) {
			if (this != &other) {
				NamedBaseInfoStatItemType::operator=(other);
			}
			return (*this);
		}
		virtual ~StatDataset() {}
	};// class StatDataset<IDTYPE,INTTYPE,STRINGTYPE>;
	////////////////////////////////////////
	template <typename IDTYPE, typename INTTYPE, typename STRINGTYPE, typename WEIGHTYPE>
	class DatasetChild : public NamedBaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE> {
		using NamedBaseInfoStatItemType = NamedBaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE>;
		using DatasetChildType = DatasetChild<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using StatDatasetType = StatDataset<IDTYPE, INTTYPE, STRINGTYPE>;
	private:
		IDTYPE m_datasetid;
		WEIGHTYPE m_weight;
	protected:
		DatasetChild() :m_datasetid(0),m_weight(1) {}
		DatasetChild(const StatDatasetType &oSet, const STRINGTYPE &s) :NamedBaseInfoStatItemType(s),m_datasetid(oSet.id()), m_weight(1) {
		}
		DatasetChild(const DatasetChildType &other) :NamedBaseInfoStatItemType(other) , m_datasetid(other.m_datasetid),m_weight(other.m_weight) {}
		DatasetChildType & operator=(const DatasetChildType &other) {
			if (this != &other) {
				NamedBaseInfoStatItemType::operator=(other);
				this->m_datasetid = other.m_datasetid;
				this->m_weight = other.m_weight;
			}
			return (*this);
		}
	public:
		virtual ~DatasetChild() {}
		IDTYPE dataset_id(void) const {
			return (this->m_datasetid);
		}
		void dataset_id(const IDTYPE n) {
			this->m_datasetid = n;
		}
		WEIGHTYPE weight(void) const {
			return (this->m_weight);
		}
		void weight(const WEIGHTYPE w) {
			this->m_weight = w;
		}
		virtual bool is_writeable(void) const {
			return ((this->m_datasetid != 0) && NamedBaseInfoStatItemType::is_writeable());
		}
		virtual void clear(void) {
			NamedBaseInfoStatItemType::clear();
			this->m_datasetid = 0;
			this->m_weight = 1;
		}
	}; // class DatasetChild
	//////////////////////////////////////////////
	template <typename IDTYPE = unsigned long, typename INTTYPE = int, typename STRINGTYPE = std::string, typename WEIGHTYPE = float>
	class StatVariable : public DatasetChild<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE> {
		using DatasetChildType = DatasetChild<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using StatDatasetType = StatDataset<IDTYPE, INTTYPE, STRINGTYPE>;
		using StatVariableType = StatVariable<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	private:
		bool m_categ;
		STRINGTYPE m_type;
		STRINGTYPE m_genre;
	public:
		StatVariable():m_categ(false){}
		StatVariable(const IDTYPE n) :m_categ(false) {
			this->id(n);
		}
		StatVariable(const StatDatasetType &oSet, const STRINGTYPE &s) :DatasetChildType(oSet,s) {
		}
		StatVariable(const StatVariableType &other) :DatasetChildType(other),
			m_categ(other.m_categ),m_type(other.m_type),m_genre(other.m_genre) {}
		StatVariableType & operator=(const StatVariableType &other) {
			if (this != &other) {
				DatasetChildType::operator=(other);
				this->m_categ = other.m_categ;
				this->m_type = other.m_type;
				this->m_genre = other.m_genre;
			}
			return (*this);
		}
		virtual ~StatVariable(){}
	public:
		bool is_categ(void) const {
			return (this->m_categ);
		}
		void is_categ(const bool b) {
			this->m_categ = b;
		}
		const STRINGTYPE &vartype(void) const {
			return (this->m_type);
		}
		void vartype(const STRINGTYPE &s) {
			this->m_type = info_trim_lower(s);
		}
		const STRINGTYPE &genre(void) const {
			return (this->m_genre);
		}
		void genre(const STRINGTYPE &s) {
			this->m_genre = info_trim_upper(s);
		}
		virtual bool is_writeable(void) const {
			return ((!this->m_type.empty()) && DatasetChildType::is_writeable());
		}
		virtual void clear(void) {
			DatasetChildType::clear();
			this->m_type.clear();
		}

	}; // class StatVariable
	/////////////////////////////////////////////
	template <typename IDTYPE = unsigned long, typename INTTYPE = int, typename STRINGTYPE = std::string, typename WEIGHTYPE = float>
	class StatIndiv : public DatasetChild<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE> {
		using DatasetChildType = DatasetChild<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using StatDatasetType = StatDataset<IDTYPE, INTTYPE, STRINGTYPE>;
		using StatIndivType = StatIndiv<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	public:
		StatIndiv(){}
		StatIndiv(const IDTYPE n) {
			this->id(n);
		}
		StatIndiv(const StatDatasetType &oSet, const STRINGTYPE &s) :DatasetChildType(oSet, s) {
		}
		StatIndiv(const StatIndivType &other) :DatasetChildType(other){}
		StatIndivType & operator=(const StatIndivType &other) {
			if (this != &other) {
				DatasetChildType::operator=(other);
			}
			return (*this);
		}
		virtual ~StatIndiv() {}
	}; // class StatIndiv
	/////////////////////////////////////
	template <typename IDTYPE = unsigned long, typename INTTYPE = int, typename STRINGTYPE = std::string>
	class StatValue :public BaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE> {
		using BaseInfoStatItemType = BaseInfoStatItem<IDTYPE, INTTYPE, STRINGTYPE>;
		using StatValueType = StatValue<IDTYPE, INTTYPE, STRINGTYPE>;
		using any = boost::any;
	private:
		IDTYPE m_varid;
		IDTYPE m_indid;
		any m_val;
	public:
		StatValue(){}
		template <typename WEIGHTYPE>
		StatValue(const StatVariable<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE> &oVar,
			const StatIndiv<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE> &oInd,
			const any &v = any()) :m_varid(oVar.id()), m_indid(oInd.id()), m_val(v) {
		}
		StatValue(const StatValueType &other):BaseInfoStatItemType(other),
			m_varid(other.m_varid),m_indid(other.m_indid),m_val(other.m_val){}
		StatValueType & operator=(const StatValueType &other) {
			if (this != &other) {
				BaseInfoStatItemType::operator=(other);
				this->m_varid = other.m_varid;
				this->m_indid = other.m_indid;
				this->m_val = other.m_val;
			}
			return (*this);
		}
		virtual ~StatValue(){}
	public:
		IDTYPE variable_id(void) const {
			return (this->m_varid);
		}
		void variable_id(const IDTYPE n) {
			this->m_varid = n;
		}
		IDTYPE indiv_id(void) const {
			return (this->m_indid);
		}
		void indiv_id(const IDTYPE n) {
			this->m_indid = n;
		}
		const any  & value(void) const {
			return (this->m_val);
		}
		void value(const any &v) {
			this->m_val = v;
		}
		bool empty(void) const {
			return (this->m_val.empty());
		}
		bool get_value(std::string &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<std::string>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(std::wstring &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<std::wstring>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(bool &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<bool>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(double &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<double>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(float &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<float>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(int &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<int>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(unsigned int &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<unsigned int>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(unsigned long &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<unsigned long>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(long &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<long>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(short &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<short>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
		bool get_value(unsigned short &v) const {
			const boost::any &vx = this->m_val;
			if (vx.empty()) {
				return false;
			}
			try {
				v = boost::any_cast<unsigned short>(vx);
				return (true);
			}
			catch (boost::bad_any_cast &) {
				return  (false);
			}
		}// get_value
	public:
		virtual bool is_writeable(void) const {
			return ((this->m_varid != 0) && (this->m_indid != 0));
		}
		virtual void clear(void) {
			BaseInfoStatItemType::clear();
			this->m_varid = 0;
			this->m_indid = 0;
			this->m_val = any();
		}
	}; // class StatValue<IDTYPE, INTTYPE, STRINGTYPE>;
	////////////////////////////////////
}// namespace info
/////////////////////////////////
#endif // !__BASEITEM_H__
