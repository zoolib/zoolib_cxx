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
#include "zoolib/ZRef.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_T.h"
#include "zoolib/ZValAccessors_Std.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFBase.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)

NAMESPACE_ZOOLIB_BEGIN

class ZVal_CFType;
class ZValList_CFType;
class ZValMap_CFType;

typedef ZVal_CFType ZVal_CF;
typedef ZValList_CFType ZValList_CF;
typedef ZValMap_CFType ZValMap_CF;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType

class ZVal_CFType
:	public ZValR_T<ZVal_CFType>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_CFType,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValAccessors_Decl_Std(ZVal_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, List, ZValList_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, Map, ZValMap_CFType)

	operator operator_bool_type() const;

	ZVal_CFType();
	ZVal_CFType(const ZVal_CFType& iOther);
	~ZVal_CFType();
	ZVal_CFType& operator=(const ZVal_CFType& iOther);

	ZVal_CFType(ZRef<CFTypeRef> iCFTypeRef);
	ZVal_CFType(int8 iVal);
	ZVal_CFType(int16 iVal);
	ZVal_CFType(int32 iVal);
	ZVal_CFType(int64 iVal);
	ZVal_CFType(bool iVal);
	ZVal_CFType(float iVal);
	ZVal_CFType(double iVal);
	ZVal_CFType(const string8& iVal);
	ZVal_CFType(const string16& iVal);
	ZVal_CFType(const ZValList_CFType& iVal);
	ZVal_CFType(const ZValMap_CFType& iVal);
	
	operator CFTypeRef() const;

	CFTypeRef* ParamO();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);

private:
	ZRef<CFTypeRef> fCFTypeRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_CFType

class ZValList_CFType
:	public ZValListR_T<ZValList_CFType, ZVal_CFType>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValList_CFType,
		operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	ZValList_CFType();
	ZValList_CFType(const ZValList_CFType& iOther);
	~ZValList_CFType();
	ZValList_CFType& operator=(const ZValList_CFType& iOther);

	ZValList_CFType(const ZRef<CFMutableArrayRef>& iOther);
	ZValList_CFType(const ZRef<CFArrayRef>& iOther);

	ZValList_CFType& operator=(const ZRef<CFMutableArrayRef>& iOther);
	ZValList_CFType& operator=(const ZRef<CFArrayRef>& iOther);

	operator CFArrayRef() const;

	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_CFType& oVal) const;

	void Set(size_t iIndex, const ZVal_CFType& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_CFType& iVal);

	void Append(const ZVal_CFType& iVal);

private:
	void pTouch();

	ZRef<CFMutableArrayRef> fCFMutableArrayRef;
	ZRef<CFArrayRef> fCFArrayRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_CFType

class ZValMap_CFType
:	public ZValMapR_T<ZValMap_CFType, const string8&, ZVal_CFType>
,	public ZValMapR_T<ZValMap_CFType, CFStringRef, ZVal_CFType>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_CFType,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValMapAccessors_Using(ZValMap_CFType, const string8&, ZVal_CFType)
	ZMACRO_ZValMapAccessors_Using(ZValMap_CFType, CFStringRef, ZVal_CFType)

	operator operator_bool_type() const;

	ZValMap_CFType();
	ZValMap_CFType(const ZValMap_CFType& iOther);
	~ZValMap_CFType();
	ZValMap_CFType& operator=(const ZValMap_CFType& iOther);

	ZValMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther);
	ZValMap_CFType(const ZRef<CFDictionaryRef>& iOther);

	ZValMap_CFType& operator=(const ZRef<CFMutableDictionaryRef>& iOther);
	ZValMap_CFType& operator=(const ZRef<CFDictionaryRef>& iOther);

	operator CFDictionaryRef() const;

	void Clear();

	bool QGet(const string8& iName, ZVal_CFType& oVal) const;
	bool QGet(CFStringRef iName, ZVal_CFType& oVal) const;

	void Set(const string8& iName, const ZVal_CFType& iVal);
	void Set(CFStringRef iName, const ZVal_CFType& iVal);

	void Erase(const string8& iName);
	void Erase(CFStringRef iName);

private:
	void pTouch();

	ZRef<CFMutableDictionaryRef> fCFMutableDictionaryRef;
	ZRef<CFDictionaryRef> fCFDictionaryRef;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZVal_CFType__
