/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTName_h__
#define __ZTName_h__ 1
#include "zconfig.h"

#include "zoolib/ZMemory.h" // For ZMemCopy
#include "zoolib/ZTypes.h" // For intptr_t

#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - Forward declarations

class ZStreamR;
class ZStreamW;

// =================================================================================================
// MARK: - ZTName

class ZTName
	{
	// The pointer references a PNRep if the low bit is set.
	static bool sIsPNRep(const void* iData);

public:
	/// Call this to pre-register property names that will be shareable across all tuples.
	static int sPreRegister(const char* const* iNames, size_t iCount);

	ZTName();

	~ZTName();

	ZTName(const ZTName& iOther);

	ZTName& operator=(const ZTName& iOther);

	explicit ZTName(const ZStreamR& iStreamR);

	ZTName(const char* iString, size_t iLength);
	ZTName(const std::string& iString);
	ZTName(const char* iString);

	bool operator==(const ZTName& iOther) const;
	bool operator!=(const ZTName& iOther) const;

	bool operator<(const ZTName& iOther) const;
	bool operator<=(const ZTName& iOther) const;
	bool operator>(const ZTName& iOther) const;
	bool operator>=(const ZTName& iOther) const;

	int Compare(const ZTName& iOther) const;

	bool Equals(const ZTName& iOther) const;
	bool Equals(const std::string& iString) const;
	bool Equals(const char* iString, size_t iLength) const;

	void ToStream(const ZStreamW& iStreamW) const;

	bool Empty() const;

	operator std::string() const;
	std::string AsString() const;

private:
	class String;

	union
		{
		const void* fData;
		const String* fString;
		};
	};

// =================================================================================================
// MARK: - ZTName::String

class ZTName::String
	{
private:
	String(); // Not implemented
	String& operator=(const String&); // Not implemented

public:
	void* operator new(size_t iObjectSize, size_t iDataSize)
		{ return new char[iObjectSize + iDataSize - 1]; }

	void operator delete(void* iPtr, size_t iObjectSize)
		{ delete[] static_cast<char*>(iPtr); }

	String(const String& iOther)
	:	fSize(iOther.fSize)
		{ ZMemCopy(fBuffer, iOther.fBuffer, fSize); }

	String(const char* iString, size_t iSize);

	int Compare(const String& iOther) const;
	int Compare(const char* iString, size_t iSize) const;
	int Compare(const std::string& iString) const;

	void ToStream(const ZStreamW& iStreamW) const;

	std::string AsString() const;

	const size_t fSize;
	char fBuffer[1];
	};

// =================================================================================================
// MARK: - ZTName inline methods

inline bool ZTName::sIsPNRep(const void* iData)
	{ return reinterpret_cast<intptr_t>(iData) & 1; }

inline ZTName::ZTName()
:	fData(reinterpret_cast<void*>(1))
	{}

inline ZTName::~ZTName()
	{
	if (!sIsPNRep(fData))
		delete fString;
	}

inline ZTName::ZTName(const ZTName& iOther)
:	fData(iOther.fData)
	{
	if (!sIsPNRep(fData))
		fString = new (fString->fSize) String(*fString);
	}

inline bool ZTName::operator==(const ZTName& iOther) const
	{ return 0 == this->Compare(iOther); }

inline bool ZTName::operator!=(const ZTName& iOther) const
	{ return !(*this == iOther); }

inline bool ZTName::operator<(const ZTName& iOther) const
	{ return 0 > this->Compare(iOther); }

inline bool ZTName::operator<=(const ZTName& iOther) const
	{ return !(iOther < *this); }

inline bool ZTName::operator>(const ZTName& iOther) const
	{ return iOther < *this; }

inline bool ZTName::operator>=(const ZTName& iOther) const
	{ return !(*this < iOther); }

inline bool ZTName::Equals(const ZTName& iOther) const
	{ return 0 == this->Compare(iOther); }

} // namespace ZooLib

#endif // __ZTName_h__
