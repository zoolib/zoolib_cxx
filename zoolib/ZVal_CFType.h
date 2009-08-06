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

#ifndef __ZVal_CFType__
#define __ZVal_CFType__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZData_CFType.h"
#include "zoolib/ZRef_CFType.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors_Std.h"

NAMESPACE_ZOOLIB_BEGIN

class ZVal_CFType;
class ZList_CFType;
class ZMap_CFType;

typedef ZVal_CFType ZVal_CF;
typedef ZList_CFType ZList_CF;
typedef ZMap_CFType ZMap_CF;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType

class ZVal_CFType
:	public ZRef<CFTypeRef>
,	public ZValR_T<ZVal_CFType>
	{
	typedef ZRef<CFTypeRef> inherited;
public:
	bool sFromAny(const ZAny& iAny, ZVal_CFType& oVal);

	ZVal_Any AsVal_Any() const;
	ZVal_Any AsVal_Any(const ZVal_Any& iDefault) const;

	operator bool() const;

	ZVal_CFType();
	ZVal_CFType(const ZVal_CFType& iOther);
	~ZVal_CFType();
	ZVal_CFType& operator=(const ZVal_CFType& iOther);

	ZVal_CFType(const ZRef<CFTypeRef>& iVal);
	ZVal_CFType(int8 iVal);
	ZVal_CFType(int16 iVal);
	ZVal_CFType(int32 iVal);
	ZVal_CFType(int64 iVal);
	ZVal_CFType(bool iVal);
	ZVal_CFType(float iVal);
	ZVal_CFType(double iVal);
	ZVal_CFType(const string8& iVal);
	ZVal_CFType(const string16& iVal);
	ZVal_CFType(CFStringRef iVal);

	ZVal_CFType(const ZData_CFType& iVal);
	ZVal_CFType(const ZList_CFType& iVal);
	ZVal_CFType(const ZMap_CFType& iVal);

	explicit ZVal_CFType(CFTypeRef iVal);
	explicit ZVal_CFType(CFDataRef iVal);
	explicit ZVal_CFType(CFArrayRef iVal);
	explicit ZVal_CFType(CFDictionaryRef iVal);

	ZVal_CFType& operator=(CFTypeRef iVal);

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);

// Our protocol
	CFTypeRef& OParam();

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Std(ZVal_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, CFString, ZRef<CFStringRef>)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, Data, ZData_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, List, ZList_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, Map, ZMap_CFType)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZList_CFType

class ZList_CFType
:	public ZRef<CFArrayRef>
	{
	typedef ZRef<CFArrayRef> inherited;

public:
	ZList_Any AsList_Any() const;
	ZList_Any AsList_Any(const ZVal_Any& iDefault) const;

	operator bool() const;

	ZList_CFType();
	ZList_CFType(const ZList_CFType& iOther);
	~ZList_CFType();
	ZList_CFType& operator=(const ZList_CFType& iOther);

	ZList_CFType(const ZRef<CFMutableArrayRef>& iOther);
	ZList_CFType(const ZRef<CFArrayRef>& iOther);

	ZList_CFType& operator=(const ZRef<CFMutableArrayRef>& iOther);
	ZList_CFType& operator=(const ZRef<CFArrayRef>& iOther);

// ZList protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_CFType& oVal) const;
	ZVal_CFType DGet(const ZVal_CFType& iDefault, size_t iIndex) const;
	ZVal_CFType Get(size_t iIndex) const;

	void Set(size_t iIndex, const ZVal_CFType& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_CFType& iVal);

	void Append(const ZVal_CFType& iVal);

// Our protocol
	CFArrayRef& OParam();

private:
	CFArrayRef pArray() const;
	CFMutableArrayRef pTouch();
	bool fMutable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_CFType

class ZMap_CFType
:	public ZRef<CFDictionaryRef>
	{
	typedef ZRef<CFDictionaryRef> inherited;

public:
	ZMap_Any AsMap_Any() const;
	ZMap_Any AsMap_Any(const ZVal_Any& iDefault) const;

	operator bool() const;

	ZMap_CFType();
	ZMap_CFType(const ZMap_CFType& iOther);
	~ZMap_CFType();
	ZMap_CFType& operator=(const ZMap_CFType& iOther);

	ZMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther);
	ZMap_CFType(const ZRef<CFDictionaryRef>& iOther);

	ZMap_CFType& operator=(const ZRef<CFMutableDictionaryRef>& iOther);
	ZMap_CFType& operator=(const ZRef<CFDictionaryRef>& iOther);

// ZMap protocol
	void Clear();

	bool QGet(const string8& iName, ZVal_CFType& oVal) const;
	bool QGet(CFStringRef iName, ZVal_CFType& oVal) const;

	ZVal_CFType DGet(const ZVal_CFType& iDefault, const string8& iName) const;
	ZVal_CFType DGet(const ZVal_CFType& iDefault, CFStringRef iName) const;

	ZVal_CFType Get(const string8& iName) const;
	ZVal_CFType Get(CFStringRef iName) const;

	void Set(const string8& iName, const ZVal_CFType& iVal);
	void Set(CFStringRef iName, const ZVal_CFType& iVal);

	void Erase(const string8& iName);
	void Erase(CFStringRef iName);

// Our protocol
	CFDictionaryRef& OParam();

private:
	CFDictionaryRef pDictionary() const;
	CFMutableDictionaryRef pTouch();
	bool fMutable;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZVal_CFType__
