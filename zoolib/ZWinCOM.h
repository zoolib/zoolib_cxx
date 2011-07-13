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

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZWinCOM_Macros.h"

// Necessary when building with Cocotron
#include <ole2.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZRef support

inline void sRetain(IUnknown& iObject)
	{ iObject.AddRef(); }

inline void sRelease(IUnknown& iObject)
	{ iObject.Release(); }

inline void sCheck(IUnknown* iP)
	{ ZAssertStop(1, iP); }

namespace ZWinCOM {

// =================================================================================================
#pragma mark -
#pragma mark * Support for QueryInterface and output-param APIs

template <class T>
static HRESULT sCopy(void** oObjectRef, T* iOb)
	{
	*oObjectRef = iOb;
	if (iOb)
		sRetain(*iOb);
	return NOERROR;
	}

template <class T>
static T** sPtr(ZRef<T>& ioRef)
	{ return &ioRef.OParam(); }

template <class T>
static void** sVoidPtr(ZRef<T>& ioRef)
	{ return (void**)(&ioRef.OParam()); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::OParam

template <class T, bool Sense = true>
class OParam
	{
// Private and unimplemented
	OParam();
	OParam& operator=(const OParam&);
public:
// Public, but unimplemented
	OParam(const OParam&);

	OParam(HRESULT iHRESULT)
	:	fHRESULT(iHRESULT)
		{
		ZAssert(fHasValue);
		if (FAILED(iHRESULT))
			{
			fHasValue = false;
			sDtor_T<T>(fBytes);
			}
		}

	~OParam()
		{
		if (fHasValue)
			sDtor_T<T>(fBytes);
		}

	operator bool() const
		{ return fHasValue == Sense; }

	operator T*() const
		{
		sCtor_T<T>(fBytes);
		fHasValue = true;
		return sFetch_T<T>(fBytes);
		}

	const T& Get() const
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}

	HRESULT GetHRESULT() const
		{ return fHRESULT; }

private:
	#if ZCONFIG(Compiler,GCC)
		mutable char fBytes[sizeof(T)] __attribute__((aligned));
	#else
		mutable char fBytes[sizeof(T)];
	#endif

	mutable bool fHasValue;
	const HRESULT fHRESULT;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::OParam specialized for ZRef<T>

template <class T, bool Sense>
class OParam<ZRef<T>, Sense>
	{
// Private and unimplemented
	OParam();
	OParam& operator=(const OParam&);
public:
// Public, but unimplemented
	OParam(const OParam&);

	OParam(HRESULT iHRESULT)
	:	fHRESULT(iHRESULT)
		{
		if (FAILED(iHRESULT) && fT)
			{
			sRelease(*fT);
			fT = 0;
			}
		}

	~OParam()
		{
		if (fT)
			sRelease(*fT);
		}

	operator bool() const
		{ return (Sense && fT) || (!Sense && !fT); }

	operator T**() const
		{
		fT = 0;
		return &fT;
		}

	ZRef<T> operator->() const
		{
		ZAssert(fT);
		return fT;
		}

	ZRef<T> Get() const
		{
		ZAssert(fT);
		return fT;
		}

	HRESULT GetHRESULT() const
		{ return fHRESULT; }

private:
	mutable T* fT;
	const HRESULT fHRESULT;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::sCreate

template <class Class, class Interface>
ZRef<Interface> sCreate()
	{
	ZRef<Interface> result;

	if (SUCCEEDED(::CoCreateInstance
		(ZMACRO_UUID(Class),
		nullptr,
		CLSCTX_INPROC_SERVER,
		ZMACRO_UUID(Interface),
		sVoidPtr(result)))
		&& result)
		{ return result; }

	return null;
	}

template <class Interface>
ZRef<Interface> sCreate(CLSID iCLSID)
	{
	ZRef<Interface> result;

	if (SUCCEEDED(::CoCreateInstance
		(iCLSID,
		nullptr,
		CLSCTX_INPROC_SERVER,
		ZMACRO_UUID(Interface),
		sVoidPtr(result)))
		&& result)
		{ return result; }

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::sSuccess& pseudo prefix operator

const struct
	{
	bool operator()(HRESULT iResult) const { return SUCCEEDED(iResult); }

	bool operator&(HRESULT iResult) const { return SUCCEEDED(iResult); }
	} sSuccess = {};

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::sFailure& pseudo prefix operator

const struct
	{
	bool operator()(HRESULT iResult) const { return FAILED(iResult); }

	bool operator&(HRESULT iResult) const { return FAILED(iResult); }
	} sFailure = {};

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::Variant

class Variant
:	public VARIANT
	{
public:
	ZMACRO_operator_bool(Variant, operator_bool) const;

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
	Variant(const string8& iVal);
	Variant(const string16& iVal);
	Variant(IUnknown* iVal);
	Variant(IDispatch* iVal);

// ZVal protocol
	template <class S>
	ZQ<S> QGet() const;

	template <class S>
	S DGet(const S& iDefault) const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return theQ.Get();
		return iDefault;
		}

	template <class S>
	S Get() const
		{ return this->DGet(S()); }

	template <class S>
	void Set(const S& iVal);

// Our protocol
	VARIANT& OParam();

// Typename accessors
/// \cond DoxygenIgnore
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
/// \endcond DoxygenIgnore
	};

inline void swap(Variant& a, Variant& b)
	{ a.swap(b); }

// =================================================================================================
#pragma mark -
#pragma mark * ZWinCOM::String

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

	String(const string8& iOther);
	String& operator=(const string8& iOther);

	operator string16() const;
	operator string8() const;

	void Clear();

	operator const BSTR&() const;

	BSTR& OParam();
	const BSTR& IParam() const;

private:
	BSTR fBSTR;
	};

BSTR* sPtr(String& iStr);

bool operator==(const BSTR& l, const string16& r);
bool operator==(const string16& r, const BSTR& l);

bool operator==(const String& l, const string16& r);
bool operator==(const string16& l, const String& r);

} // namespace ZWinCOM
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZWinCOM__
