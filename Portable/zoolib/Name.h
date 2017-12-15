/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZooLib_Name_h__
#define __ZooLib_Name_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/CountedVal.h"
#include "zoolib/UnicodeString.h" // For string8
#include "zoolib/Util_Relops.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Name

#define ZMACRO_NameUsesString 1

#if ZMACRO_NameUsesString

class Name
	{
public:
	typedef CountedVal<string8> CountedString;

	inline
	Name()
		{}

	inline
	Name(const Name& iOther)
	:	fString(iOther.fString)
		{}

	Name& operator=(const Name& iOther)
		{
		fString = iOther.fString;
		return *this;
		}

	inline
	~Name()
		{}

	Name(const char* iStatic)
	:	fString(iStatic)
		{}

	Name(const string8& iString)
	:	fString(iString)
		{}

	Name(const ZRef<CountedString>& iRefCountedString)
	:	fString(sGet(iRefCountedString))
		{}

	operator string8() const
		{ return fString; }

//	operator RefCountedString() const;

	inline
	bool operator<(const Name& iOther) const
		{ return fString < iOther.fString; }

	inline
	bool operator==(const Name& iOther) const
		{ return fString == iOther.fString; }

	int Compare(const Name& iOther) const
		{ return fString.compare(fString); }

	bool IsEmpty() const
		{ return fString.empty(); }

	void Clear()
		{ fString.clear(); }

//	size_t Hash() const;

private:
	string8 fString;
	};

#else
class Name
	{
public:
	typedef CountedVal<string8> CountedString;
	typedef ZRef<CountedString> RefCountedString;

	inline
	Name()
	:	fIntPtr(0)
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(false)
	#endif
		{}

	inline
	Name(const Name& iOther)
	:	fIntPtr(iOther.fIntPtr)
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(iOther.fIsCounted)
	#endif
		{
		if (CountedString* theCounted = this->pGetIfCounted())
			spRetain(theCounted);
		}

	Name& operator=(const Name& iOther);

	inline
	~Name()
		{
		if (CountedString* theCounted = this->pGetIfCounted())
			spRelease(theCounted);
		}

	Name(const char* iStatic)
	:	fIntPtr(((intptr_t)iStatic))
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(false)
	#endif
		{}

	Name(const string8& iString);
	Name(const RefCountedString& iRefCountedString);
	
	operator string8() const;
	operator RefCountedString() const;

	inline
	bool operator<(const Name& iOther) const
		{ return this->Compare(iOther) < 0; }

	inline
	bool operator==(const Name& iOther) const
		{ return this->Compare(iOther) == 0; }

	int Compare(const Name& iOther) const;

	bool IsEmpty() const;

	void Clear();

	size_t Hash() const;

private:
	CountedString* pGetIfCounted();
	const CountedString* pGetIfCounted() const;

	const char* pAsCharStar() const;

	ZMACRO_Attribute_NoThrow
	static void spRetain(const CountedString* iCounted);

	ZMACRO_Attribute_NoThrow
	static void spRelease(const CountedString* iCounted);

	intptr_t fIntPtr;
	#if not ZCONFIG_Is64Bit
		bool fIsCounted;
	#endif
	};

#endif

template <> struct RelopsTraits_HasEQ<Name> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Name> : public RelopsTraits_Has {};

template <>
inline
int sCompare_T(const Name& iL, const Name& iR)
	{ return iL.Compare(iR); }

inline
bool sIsEmpty(const Name& iName)
	{ return iName.IsEmpty(); }

inline
bool sNotEmpty(const Name& iName)
	{ return not sIsEmpty(iName); }

} // namespace ZooLib

#endif // __ZooLib_Name_h__
