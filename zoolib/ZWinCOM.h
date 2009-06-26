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

#ifndef __ZWinCOM__
#define __ZWinCOM__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZRef.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_T.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZWinCOM_Macros.h"
#include "zoolib/ZWinHeader.h"

// Necessary when building with Cocotron
#include <ole2.h>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================

inline void sRetain(IUnknown& iObject)
	{ iObject.AddRef(); }

inline void sRelease(IUnknown& iObject)
	{ iObject.Release(); }

template <class T>
static HRESULT sCOMCopy(void** oObjectRef, T* iOb)
	{
	*oObjectRef = iOb;
	if (iOb)
		sRetain(*iOb);
	return NOERROR;
	}

template <class T>
static T** sCOMPtr(ZRef<T>& iRef)
	{
	iRef.Clear();
	return &iRef.GetPtrRef();
	}

template <class T>
static void** sCOMVoidPtr(ZRef<T>& iRef)
	{ return (void**)(sCOMPtr(iRef)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::Variant

namespace ZWinCOM {

class Variant
:	public VARIANT
,	public ZValR_T<Variant>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Variant,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Int8, int8)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, UInt, uint8)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Int16, int16)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, UInt16, uint16)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, UInt32, uint32)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Int64, int64)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, UInt64, uint64)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Float, float)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Double, double)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, String, string8)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, String8, string8)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, String16, string16)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Unknown, ZRef<IUnknown>)
	ZMACRO_ZValAccessors_Decl_Entry(Variant, Dispatch, ZRef<IDispatch>)

	operator operator_bool_type() const;

	void swap(Variant& iOther);

	Variant();
	Variant(const Variant& iOther);
	~Variant();
	Variant& operator=(const Variant& iOther);

	Variant(const VARIANT& iOther);
	Variant& operator=(const VARIANT& iOther);

	Variant(int8 iVal);
	Variant(uint8 iVal);
	Variant(int16 iVal);
	Variant(uint16 iVal);
	Variant(int32 iVal);
	Variant(uint32 iVal);
	Variant(int64 iVal);
	Variant(uint64 iVal);
	Variant(bool iVal);
	Variant(float iVal);
	Variant(double iVal);
	Variant(const std::string& iVal);
	Variant(const string16& iVal);
	Variant(ZRef<IUnknown> iVal);
	Variant(ZRef<IDispatch> iVal);

	VARIANT* ParamO();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);
	};

} // namespace ZWinCOM

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::String

namespace ZWinCOM {

class String
	{
public:
	String();
	String(const String& iOther);
	~String();
	String& operator=(const String& iOther);

	String(const BSTR& iOther);
	String& operator=(const BSTR& iOther);

	String(const string16& iOther);
	String& operator=(const string16& iOther);

	operator string16() const;

	void Clear();

	BSTR& GetPtrRef();
	const BSTR& GetPtrRef() const;

private:
	BSTR fBSTR;
	};

BSTR* sCOMPtr(String& iStr);

bool operator==(const BSTR& l, const string16& r);
bool operator==(const string16& r, const BSTR& l);

bool operator==(const String& l, const string16& r);
bool operator==(const string16& l, const String& r);

} // namespace ZWinCOM

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinCOM__
