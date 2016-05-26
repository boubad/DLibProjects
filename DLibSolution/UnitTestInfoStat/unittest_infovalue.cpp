#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <infovalue.h>
///////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
//////////////////////////////////
namespace UnitTestInfoStat
{		
	TEST_CLASS(UnitTestInfoValue)
	{
	public:
		
		TEST_METHOD(TestShortInfoValue)
		{
			// TODO: Your test code here
			short val = 11;
			InfoValue v(val);
			//
			Assert::IsFalse(v.is_empty());
			Assert::IsTrue(v.is_valid());
			InfoValueType t = v.get_infovalue_type();
			Assert::IsTrue(t == InfoValueType::shortType);
			Assert::IsTrue(v.is_integral());
			Assert::IsFalse(v.is_floating());
			Assert::IsFalse(v.is_text());
			Assert::IsTrue(v.is_numerical());
			//
			Assert::IsTrue(v.short_value() == (short)val);
			Assert::IsTrue(v.unsigned_short_value() == (unsigned short)val);
			Assert::IsTrue(v.int_value() == (int)val);
			Assert::IsTrue(v.unsigned_int_value() == (unsigned int)val);
			Assert::IsTrue(v.long_value() == (long)val);
			Assert::IsTrue(v.unsigned_long_value() == (unsigned long)val);
			Assert::IsTrue(v.long_long_value() == (long long)val);
			Assert::IsTrue(v.float_value() == (float)val);
			Assert::IsTrue(v.double_value() == (double)val);
			Assert::IsTrue(v.long_double_value() == (long double)val);
			//
			std::string sval;
			bool bRet = v.string_value(sval);
			Assert::IsTrue(bRet);
			Assert::IsTrue(sval == "11");
			std::wstring svalw;
			bRet = v.string_value(svalw);
			Assert::IsTrue(bRet);
			Assert::IsTrue(svalw == L"11");
		}// TestInfoValue

	};
}