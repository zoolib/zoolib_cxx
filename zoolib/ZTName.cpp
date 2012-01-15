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

#include "zoolib/ZTName.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZStream.h"

#include <vector>

using std::pair;
using std::string;
using std::vector;

namespace ZooLib {

static const int kDebug = 2;

// =================================================================================================
// MARK: - ZTName, helper functions and PNRep

static inline int spCompare(const void* iLeft, size_t iLeftLength,
	const void* iRight, size_t iRightLength)
	{
	if (int compare = std::memcmp(iLeft, iRight,
		iLeftLength < iRightLength ? iLeftLength : iRightLength))
		{
		return compare;
		}
	// Strictly speaking the rules of two's complement mean that we do
	// not need the casts, but let's be clear about what we're doing.
	return int(iLeftLength) - int(iRightLength);
	}

namespace { // anonymous

struct PNRep
	{
	uint8 fLength;
	char fBuffer[1];
	};

typedef pair<const char*, size_t> Key;

struct Compare_PNRep_Key_t
	{
	typedef const PNRep* first_argument_type;
	typedef Key second_argument_type;

	bool operator()(const first_argument_type& iL, const second_argument_type& iR) const
		{ return 0 > spCompare(iL->fBuffer, iL->fLength, iR.first, iR.second); }

	#if ZCONFIG(Compiler,MSVC) && defined(_HAS_ITERATOR_DEBUGGING)
		// When iterator debugging is enabled MSVC
		// asserts ! b < a, when we're doing a < b.
		bool operator()(const second_argument_type& iR, const first_argument_type& iL) const
			{ return 0 < spCompare(iL->fBuffer, iL->fLength, iR.first, iR.second); }

		bool operator()(const first_argument_type& iL, const first_argument_type& iR) const
			{ return 0 > spCompare(iL->fBuffer, iL->fLength, iR->fBuffer, iR->fLength); }
	#endif

	typedef bool result_type;
	};

} // anonymous namespace

static vector<const PNRep*>* spNames;

static inline bool spDifferent(const PNRep* iPNRep, const char* iName, size_t iLength)
	{ return spCompare(iPNRep->fBuffer, iPNRep->fLength, iName, iLength); }

static const PNRep* spLookupAndTag(const char* iName, size_t iLength)
	{
	if (not iLength)
		return reinterpret_cast<const PNRep*>(1);

	if (!spNames)
		return nullptr;

	vector<const PNRep*>::iterator theIter =
		lower_bound(spNames->begin(), spNames->end(), Key(iName, iLength), Compare_PNRep_Key_t());

	if (theIter == spNames->end() || spDifferent(*theIter, iName, iLength))
		return nullptr;

	return reinterpret_cast<const PNRep*>(reinterpret_cast<intptr_t>(*theIter) | 1);
	}

static const PNRep* spLookupAndTag(const string& iName)
	{
	if (iName.empty())
		return reinterpret_cast<const PNRep*>(1);

	if (!spNames)
		return nullptr;

	vector<const PNRep*>::iterator theIter = lower_bound
		(spNames->begin(), spNames->end(), Key(iName.data(), iName.length()),
		Compare_PNRep_Key_t());

	if (theIter == spNames->end() || spDifferent(*theIter, iName.data(), iName.length()))
		return nullptr;

	return reinterpret_cast<const PNRep*>(reinterpret_cast<intptr_t>(*theIter) | 1);
	}

static inline const PNRep* spGetPNRep(const void* iData)
	{ return reinterpret_cast<const PNRep*>(reinterpret_cast<intptr_t>(iData) & ~1); }

// =================================================================================================
// MARK: - ZTName::String

// ZTName::String is only used for non-zero length names that do not
// appear in the pre-registered names table. We assert that fSize is non-zero
// because there are a few places in ZTName where we rely on the fact.

inline ZTName::String::String(const char* iString, size_t iSize)
:	fSize(iSize)
	{
	ZAssertStop(kDebug, fSize);
	ZMemCopy(fBuffer, iString, fSize);
	}

inline int ZTName::String::Compare(const String& iOther) const
	{
	ZAssertStop(kDebug, fSize);
	return spCompare(fBuffer, fSize, iOther.fBuffer, iOther.fSize);
	}

inline int ZTName::String::Compare(const char* iString, size_t iSize) const
	{
	ZAssertStop(kDebug, fSize);
	return spCompare(fBuffer, fSize, iString, iSize);
	}

inline int ZTName::String::Compare(const std::string& iString) const
	{
	ZAssertStop(kDebug, fSize);
	if (size_t otherSize = iString.size())
		return spCompare(fBuffer, fSize, iString.data(), otherSize);
	// We can't be empty, so we must be greater than iString.
	return 1;
	}

inline string ZTName::String::AsString() const
	{ return string(fBuffer, fSize); }

// =================================================================================================
// MARK: - ZTName

int ZTName::sPreRegister(const char* const* iNames, size_t iCount)
	{
	if (!spNames)
		spNames = new vector<const PNRep*>;

	// This function should be called under very constrained
	// circumstances, generally from a static initialization.
	while (iCount--)
		{
		const char* theName = *iNames++;
		if (size_t theLength = std::strlen(theName))
			{
			// It's important that we only allow strings < 255 (ie 254 or fewer).
			// ZTName::ToStream writes the entirety of the
			// PNRep in a single call, and a string of length 255 would look
			// like a marker byte followed by 4 bytes of length, and things will break.
			if (theLength < 255)
				{
				vector<const PNRep*>::iterator theIter = lower_bound
					(spNames->begin(), spNames->end(), Key(theName, theLength),
					Compare_PNRep_Key_t());

				if (theIter == spNames->end() || spDifferent(*theIter, theName, theLength))
					{
					PNRep* newRep = reinterpret_cast<PNRep*>(new char[theLength + 1]);
					newRep->fLength = theLength;
					ZMemCopy(newRep->fBuffer, theName, theLength);
					spNames->insert(theIter, newRep);
					}
				}
			}
		}
	return 0;
	}

ZTName& ZTName::operator=(const ZTName& iOther)
	{
	ZAssertStop(kDebug, fData);
	ZAssertStop(kDebug, iOther.fData);

	if (!sIsPNRep(fData))
		delete fString;

	if (sIsPNRep(iOther.fData))
		fData = iOther.fData;
	else
		fString = new (iOther.fString->fSize) String(*iOther.fString);

	return *this;
	}

ZTName::ZTName(const ZStreamR& iStreamR)
	{
	uint32 theSize = iStreamR.ReadCount();
	if (theSize <= sStackBufferSize)
		{
		char buffer[sStackBufferSize];
		iStreamR.Read(buffer, theSize);
		fData = spLookupAndTag(buffer, theSize);
		if (!fData)
			fString = new (theSize) String(buffer, theSize);
		}
	else
		{
		// theSize must be > 0 (it's greater than sStackBufferSize).
		vector<char> buffer(theSize);
		iStreamR.Read(&buffer[0], theSize);
		fData = spLookupAndTag(&buffer[0], theSize);
		if (!fData)
			fString = new (theSize) String(&buffer[0], theSize);
		}
	}

ZTName::ZTName(const char* iString, size_t iLength)
	{
	fData = spLookupAndTag(iString, iLength);
	if (!fData)
		fString = new (iLength) String(iString, iLength);
	}

ZTName::ZTName(const std::string& iString)
	{
	fData = spLookupAndTag(iString);
	if (!fData)
		fString = new (iString.size()) String(iString.data(), iString.size());
	}

ZTName::ZTName(const char* iString)
	{
	size_t theLength = std::strlen(iString);
	fData = spLookupAndTag(iString, theLength);
	if (!fData)
		fString = new (theLength) String(iString, theLength);
	}

int ZTName::Compare(const ZTName& iOther) const
	{
	ZAssertStop(kDebug, fData);
	ZAssertStop(kDebug, iOther.fData);

	if (fData == iOther.fData)
		return 0;

	if (sIsPNRep(fData))
		{
		// This is a PNRep.
		if (sIsPNRep(iOther.fData))
			{
			// Other is a PNRep.
			if (const PNRep* thisPNRep = spGetPNRep(fData))
				{
				// This is allocated, thus not empty.
				if (const PNRep* otherPNRep = spGetPNRep(iOther.fData))
					{
					// Other is allocated, thus not empty.
					return spCompare(thisPNRep->fBuffer, thisPNRep->fLength,
						otherPNRep->fBuffer, otherPNRep->fLength);
					}
				else
					{
					// Other is unallocated, thus empty.

					// This is greater than other.
					return 1;
					}
				}
			else
				{
				// This is unallocated, thus empty.
				if (spGetPNRep(iOther.fData))
					{
					// Other is allocated, and thus not empty.

					// This is less than other.
					return -1;
					}
				else
					{
					// Other is unallocated, thus empty.

					// This equals other.
					return 0;
					}
				}
			}
		else
			{
			// Other is not a PNRep.
			if (const PNRep* thisPNRep = spGetPNRep(fData))
				{
				// This is allocated. We could call other's String::Compare and negate
				// the result, but that little minus sign is easy to miss:
				// return - iOther.fString->Compare(thisPNRep->fBuffer, thisPNRep->fLength);

				// So we do the work explicitly:
				return spCompare(thisPNRep->fBuffer, thisPNRep->fLength,
					iOther.fString->fBuffer, iOther.fString->fSize);
				}
			else
				{
				// This is unallocated, thus empty. iOther is a String, which
				// cannot be empty, so this must be less than other.
				return -1;
				}

			}
		}
	else if (sIsPNRep(iOther.fData))
		{
		if (const PNRep* otherPNRep = spGetPNRep(iOther.fData))
			{
			// Other is allocated.
			return fString->Compare(otherPNRep->fBuffer, otherPNRep->fLength);
			}
		else
			{
			// Other is unallocated, thus empty. This is a String, which
			// cannot be empty, so this must be greater than other.
			return 1;
			}
		}
	else
		{
		return fString->Compare(*iOther.fString);
		}
	}

bool ZTName::Equals(const char* iString, size_t iLength) const
	{
	ZAssertStop(kDebug, fData);

	if (sIsPNRep(fData))
		{
		if (const PNRep* thePNRep = spGetPNRep(fData))
			{
			// We're allocated.
			return 0 == spCompare(thePNRep->fBuffer, thePNRep->fLength, iString, iLength);
			}
		else
			{
			// We're unallocated, and thus empty. We match if iString is empty.
			return 0 == iLength;
			}
		}
	else
		{
		return 0 == fString->Compare(iString, iLength);
		}
	}

bool ZTName::Equals(const std::string& iString) const
	{
	ZAssertStop(kDebug, fData);

	if (sIsPNRep(fData))
		{
		if (const PNRep* thePNRep = spGetPNRep(fData))
			{
			// We're not unallocated.
			if (size_t otherLength = iString.size())
				{
				// Nor is iString -- match characters.
				return 0 == spCompare(thePNRep->fBuffer, thePNRep->fLength,
					iString.data(), otherLength);
				}
			else
				{
				return 0 == thePNRep->fLength;
				}
			}
		else
			{
			// We're unallocated, and thus empty. We match if iString is empty.
			return iString.empty();
			}
		}
	else
		{
		return 0 == fString->Compare(iString);
		}
	}

bool ZTName::Empty() const
	{
	ZAssertStop(kDebug, fData);

	if (sIsPNRep(fData))
		return !spGetPNRep(fData);

	ZAssertStop(kDebug, fString->fSize);

	return false;
	}

ZTName::operator std::string() const
	{ return this->AsString(); }

string ZTName::AsString() const
	{
	ZAssertStop(kDebug, fData);

	if (sIsPNRep(fData))
		{
		if (const PNRep* thePNRep = spGetPNRep(fData))
			return string(thePNRep->fBuffer, thePNRep->fLength);
		return string();
		}
	else
		{
		return fString->AsString();
		}
	}

void ZTName::ToStream(const ZStreamW& iStreamW) const
	{
	ZAssertStop(kDebug, fData);

	if (sIsPNRep(fData))
		{
		if (const PNRep* thePNRep = spGetPNRep(fData))
			{
			// Because our length < 255 this preserves the
			// format used by ZStreamW::WriteCount.
			iStreamW.Write(thePNRep, thePNRep->fLength + 1);
			}
		else
			{
			iStreamW.WriteUInt8(0);
			}
		}
	else
		{
		fString->ToStream(iStreamW);
		}
	}

// =================================================================================================
// MARK: - ZTName::String

void ZTName::String::ToStream(const ZStreamW& iStreamW) const
	{
	ZAssertStop(kDebug, fSize);
	iStreamW.WriteCount(fSize);
	iStreamW.Write(fBuffer, fSize);
	}

} // namespace ZooLib
