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
class ZSeq_CFType;
class ZMap_CFType;

typedef ZVal_CFType ZVal_CF;
typedef ZSeq_CFType ZSeq_CF;
typedef ZMap_CFType ZMap_CF;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType

class ZVal_CFType
:	public ZRef<CFTypeRef>
	{
	typedef ZRef<CFTypeRef> inherited;
public:
	ZAny AsAny() const;
	ZAny DAsAny(const ZAny& iDefault) const;

	operator bool() const;

	ZVal_CFType();
	ZVal_CFType(const ZVal_CFType& iOther);
	~ZVal_CFType();
	ZVal_CFType& operator=(const ZVal_CFType& iOther);

	ZVal_CFType(CFTypeRef iVal);
	ZVal_CFType(const ZRef<CFTypeRef>& iVal);
	ZVal_CFType(const Adopt_T<CFTypeRef>& iVal);

	ZVal_CFType(int8 iVal);
	ZVal_CFType(int16 iVal);
	ZVal_CFType(int32 iVal);
	ZVal_CFType(int64 iVal);
	ZVal_CFType(bool iVal);
	ZVal_CFType(float iVal);
	ZVal_CFType(double iVal);
	ZVal_CFType(const char* iVal);
	ZVal_CFType(const string8& iVal);
	ZVal_CFType(const string16& iVal);
	ZVal_CFType(CFStringRef iVal);

	ZVal_CFType(const ZData_CFType& iVal);
	ZVal_CFType(const ZSeq_CFType& iVal);
	ZVal_CFType(const ZMap_CFType& iVal);

	explicit ZVal_CFType(CFDataRef iVal);
	explicit ZVal_CFType(CFArrayRef iVal);
	explicit ZVal_CFType(CFDictionaryRef iVal);

	ZVal_CFType& operator=(CFTypeRef iVal);
	ZVal_CFType& operator=(const ZRef<CFTypeRef>& iVal);
	ZVal_CFType& operator=(const Adopt_T<CFTypeRef>& iVal);

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	S DGet_T(const S& iDefault) const
		{
		S result;
		if (this->QGet_T(result))
			return result;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{ return this->DGet_T(S()); }

	template <class S>
	void Set_T(const S& iVal);

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Std(ZVal_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, CFString, ZRef<CFStringRef>)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, Data, ZData_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, Seq, ZSeq_CFType)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_CFType, Map, ZMap_CFType)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_CFType

class ZSeq_CFType
:	public ZRef<CFArrayRef>
	{
	typedef ZRef<CFArrayRef> inherited;

public:
	ZSeq_Any AsSeq_Any(const ZAny& iDefault) const;

	operator bool() const;

	ZSeq_CFType();
	ZSeq_CFType(const ZSeq_CFType& iOther);
	~ZSeq_CFType();
	ZSeq_CFType& operator=(const ZSeq_CFType& iOther);

	ZSeq_CFType(CFMutableArrayRef iOther);
	ZSeq_CFType(CFArrayRef iOther);

	ZSeq_CFType(const ZRef<CFMutableArrayRef>& iOther);
	ZSeq_CFType(const ZRef<CFArrayRef>& iOther);

	ZSeq_CFType(const Adopt_T<CFMutableArrayRef>& iOther);
	ZSeq_CFType(const Adopt_T<CFArrayRef>& iOther);

	ZSeq_CFType& operator=(CFMutableArrayRef iOther);
	ZSeq_CFType& operator=(CFArrayRef iOther);

	ZSeq_CFType& operator=(const ZRef<CFMutableArrayRef>& iOther);
	ZSeq_CFType& operator=(const ZRef<CFArrayRef>& iOther);

	ZSeq_CFType& operator=(const Adopt_T<CFMutableArrayRef>& iOther);
	ZSeq_CFType& operator=(const Adopt_T<CFArrayRef>& iOther);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZRef<CFTypeRef>& oVal) const;
	ZVal_CFType DGet(const ZVal_CFType& iDefault, size_t iIndex) const;
	ZVal_CFType Get(size_t iIndex) const;

	ZSeq_CFType& Set(size_t iIndex, const ZVal_CFType& iVal);

	ZSeq_CFType& Erase(size_t iIndex);

	ZSeq_CFType& Insert(size_t iIndex, const ZVal_CFType& iVal);

	ZSeq_CFType& Append(const ZVal_CFType& iVal);

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
	ZMap_Any AsMap_Any(const ZAny& iDefault) const;

	operator bool() const;

	ZMap_CFType();
	ZMap_CFType(const ZMap_CFType& iOther);
	~ZMap_CFType();
	ZMap_CFType& operator=(const ZMap_CFType& iOther);

	ZMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther);
	ZMap_CFType(const ZRef<CFDictionaryRef>& iOther);

	ZMap_CFType(CFMutableDictionaryRef iOther);
	ZMap_CFType(CFDictionaryRef iOther);

	ZMap_CFType(const Adopt_T<CFMutableDictionaryRef>& iOther);
	ZMap_CFType(const Adopt_T<CFDictionaryRef>& iOther);

	ZMap_CFType& operator=(CFMutableDictionaryRef iOther);
	ZMap_CFType& operator=(CFDictionaryRef iOther);

	ZMap_CFType& operator=(const ZRef<CFMutableDictionaryRef>& iOther);
	ZMap_CFType& operator=(const ZRef<CFDictionaryRef>& iOther);

	ZMap_CFType& operator=(const Adopt_T<CFMutableDictionaryRef>& iOther);
	ZMap_CFType& operator=(const Adopt_T<CFDictionaryRef>& iOther);

// ZMap protocol
	void Clear();

	bool QGet(const string8& iName, ZRef<CFTypeRef>& oVal) const;
	bool QGet(CFStringRef iName, ZRef<CFTypeRef>& oVal) const;

	ZVal_CFType DGet(const ZVal_CFType& iDefault, const string8& iName) const;
	ZVal_CFType DGet(const ZVal_CFType& iDefault, CFStringRef iName) const;

	ZVal_CFType Get(const string8& iName) const;
	ZVal_CFType Get(CFStringRef iName) const;

	ZMap_CFType& Set(const string8& iName, const ZVal_CFType& iVal);
	ZMap_CFType& Set(CFStringRef iName, const ZVal_CFType& iVal);

	ZMap_CFType& Erase(const string8& iName);
	ZMap_CFType& Erase(CFStringRef iName);

private:
	CFDictionaryRef pDictionary() const;
	CFMutableDictionaryRef pTouch();
	bool fMutable;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZVal_CFType__
