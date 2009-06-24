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

class Variant : public VARIANT
	{
public:
	void swap(Variant& iOther);

	Variant();
	Variant(const VARIANT& iOther);
	~Variant();
	Variant& operator=(const VARIANT& iOther);

	Variant(bool iParam);
	Variant(int8 iParam);
	Variant(uint8 iParam);
	Variant(int16 iParam);
	Variant(uint16 iParam);
	Variant(int32 iParam);
	Variant(uint32 iParam);
	Variant(int64 iParam);
	Variant(uint64 iParam);
	Variant(float iParam);
	Variant(double iParam);
	Variant(const std::string& iParam);
	Variant(const string16& iParam);
	Variant(ZRef<IUnknown> iParam);
	Variant(ZRef<IDispatch> iParam);

	bool GetBool() const;
	bool GetBool(bool& oValue) const;
	bool DGetBool(bool iDefault) const;

	int8 GetInt8() const;
	bool GetInt8(int8& oValue) const;
	int8 DGetInt8(int8 iDefault) const;

	uint8 GetUInt8() const;
	bool GetUInt8(uint8& oValue) const;
	uint8 DGetUInt8(uint8 iDefault) const;

	int16 GetInt16() const;
	bool GetInt16(int16& oValue) const;
	int16 DGetInt16(int16 iDefault) const;

	uint16 GetUInt16() const;
	bool GetUInt16(uint16& oValue) const;
	uint16 DGetUInt16(uint16 iDefault) const;

	int32 GetInt32() const;
	bool GetInt32(int32& oValue) const;
	int32 DGetInt32(int32 iDefault) const;

	uint32 GetUInt32() const;
	bool GetUInt32(uint32& oValue) const;
	uint32 DGetUInt32(uint32 iDefault) const;

	int64 GetInt64() const;
	bool GetInt64(int64& oValue) const;
	int64 DGetInt64(int64 iDefault) const;

	uint64 GetUInt64() const;
	bool GetUInt64(uint64& oValue) const;
	uint64 DGetUInt64(uint64 iDefault) const;

	float GetFloat() const;
	bool GetFloat(float& oValue) const;
	float DGetFloat(float iDefault) const;

	double GetDouble() const;
	bool GetDouble(double& oValue) const;
	double DGetDouble(double iDefault) const;

	string16 GetString16() const;
	bool GetString16(string16& oValue) const;
	string16 DGetString16(const string16& iDefault) const;
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
