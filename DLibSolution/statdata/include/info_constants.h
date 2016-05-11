#pragma once
#ifndef __INFO_CONSTANTS_H__
#define  __INFO_CONSTANTS_H__
//////////////////////////////
#define DATATRANSFER_CHUNK_SIZE (100)
////////////////////////////////////////////
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <list>
#include <set>
#include <vector>
#include <map>
#include <deque>
////////////////////////////////////////////
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/container/list.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
///////////////////////////////////
#if defined(USE_DLIB)
#include <dlib/threads.h>
#else
#include <mutex>
#endif // USE_DLIB
////////////////////////////////////
namespace info {
	typedef unsigned long IntType;
	//////////////////////////////////////////////////////
	enum class VariableMode { modeInvalid, modeNumeric, modeNominal, modeAll };
	////////////////////////////////////////////////////
	typedef std::list<IntType> ints_list;
	typedef std::vector<IntType> ints_vector;
	typedef std::deque<IntType> ints_deque;
	typedef std::set<IntType> ints_set;
	typedef std::vector<double> doubles_vector;
	typedef std::map<IntType, std::string> inttype_string_map;
	typedef std::vector<std::string> strings_vector;
	///////////////////////////////////////////////////////////////
#if defined(USE_DLIB)
	typedef dlib::mutex info_mutex;
	typedef dlib::auto_mutex  info_lock;
	typedef dlib::read_write_mutex info_read_write_mutex;
	typedef dlib::auto_unlock_readonly info_read_lock;
	typedef dlib::auto_unlock info_write_lock;
#else
	typedef std::mutex info_mutex;
	typedef std::lock_guard<std::mutex>  info_lock;
	typedef std::mutex info_read_write_mutex;
	typedef std::lock_guard<std::mutex> info_read_lock;
	typedef std::lock_guard<std::mutex> info_write_lock;
#endif // USE_DLIB
}// namspace info
//////////////////////////////
#endif // !__INFO_CONSTANTS_H__
