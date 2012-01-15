/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZWinRegistry_Val_h__
#define __ZWinRegistry_Val_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZRef_WinHANDLE.h"
#include "zoolib/ZTagVal.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZVal_Any.h"

#include <string>
#include <vector>

namespace ZooLib {
namespace ZWinRegistry {

using namespace std;

// =================================================================================================
// MARK: - Types

typedef ZTagVal<string16, struct Tag_Env> String_Env;
typedef ZTagVal<string16, struct Tag_Link> String_Link;
typedef vector<string16> StringList;
typedef ZData_Any Data;
class KeyRef;

// =================================================================================================
// MARK: - ZWinRegistry::Val

class Val : public ZAny
	{
	typedef ZAny inherited;

public:
	ZAny AsAny() const;

	Val();
	Val(const Val& iOther);
	~Val();
	Val& operator=(const Val& iOther);

	Val(const string8& iVal);
	Val(const string16& iVal);
	Val(const vector<string16>& iVal);
	Val(const String_Env& iVal);
	Val(const String_Link& iVal);
	Val(const int32& iVal);
	Val(const int64& iVal);
	Val(const Data& iVal);
	Val(const KeyRef& iVal);

	using ZAny::Get;
	Val Get(const string16& iName) const;
	Val Get(const string8& iName) const;

	ZMACRO_ZValAccessors_Decl_Get(Val, String16, string16)
	ZMACRO_ZValAccessors_Decl_Get(Val, StringList, vector<string16>)
	ZMACRO_ZValAccessors_Decl_Get(Val, String_Env, String_Env)
	ZMACRO_ZValAccessors_Decl_Get(Val, String_Link, String_Link)
	ZMACRO_ZValAccessors_Decl_Get(Val, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Get(Val, Int64, int64)
	ZMACRO_ZValAccessors_Decl_Get(Val, Data, Data)
	ZMACRO_ZValAccessors_Decl_Get(Val, KeyRef, KeyRef)
	};

// =================================================================================================
// MARK: - ZWinRegistry::KeyRef

class KeyRef
:	public ZRef<HKEY>
	{
	typedef ZRef<HKEY> inherited;
public:
	static KeyRef sHKCR();
	static KeyRef sHKCU();
	static KeyRef sHKLM();
	static KeyRef sHKU();

	typedef size_t Index_t;
	typedef Val Val_t;

	void swap(KeyRef& rhs);

	KeyRef();
	KeyRef(const KeyRef& iOther);
	~KeyRef();
	KeyRef& operator=(const KeyRef& iOther);

	KeyRef(const ZRef<HKEY>& iOther);
	KeyRef& operator=(const ZRef<HKEY>& iOther);

// ZMap protocol
	ZQ<Val> QGet(const string16& iName) const;
	ZQ<Val> QGet(const string8& iName) const;
	ZQ<Val> QGet(const Index_t& iIndex) const;

	Val DGet(const Val& iDefault, const string16& iName) const;
	Val DGet(const Val& iDefault, const string8& iName) const;
	Val DGet(const Val& iDefault, const Index_t& iIndex) const;

	Val Get(const string16& iName) const;
	Val Get(const string8& iName) const;
	Val Get(const Index_t& iIndex) const;

//	void Set(const string16& iName, const Val& iVal);
//	void Set(const string8& iName, const Val& iVal);
//	void Set(const Index_t& iIndex, const Val& iVal);

//	void Erase(const string16& iName);
//	void Erase(const string8& iName);
//	void Erase(const Index_t& iIndex);

// Extended ZMap protocol
	Index_t Begin() const;
	Index_t End() const;

	string8 NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const string8& iName) const;
	Index_t IndexOf(const KeyRef& iOther, const Index_t& iOtherIndex) const;
	};

inline void swap(KeyRef& a, KeyRef& b)
	{ a.swap(b); }

} // namespace ZWinRegistry
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinRegistry_Val_h__
