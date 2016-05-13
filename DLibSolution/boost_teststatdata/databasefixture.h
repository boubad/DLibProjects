#pragma once
/////////////////////////////
#include <sqlitestathelper.h>
////////////////////////////////
#include "infotestdata.h"
//////////////////////////////////
using namespace info;
using namespace std;
////////////////////////////////////
class DatabaseFixture {
protected:
	size_t	 m_nbcols;
	size_t	 m_nbrows;
	unique_ptr<SQLiteStatHelper> m_man;
	DBStatDataset m_oset;
public:
	DatabaseFixture();
	~DatabaseFixture();
};// class  DatabaseFixture
  //
