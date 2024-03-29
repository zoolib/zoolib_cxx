// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Name_h__
#define __ZooLib_Name_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/CountedString.h"
#include "zoolib/Util_Relops.h"

#include <utility> // For hash

namespace ZooLib {

// =================================================================================================
#pragma mark - Name

#ifndef ZMACRO_NameUsesString
	#if defined(_LIBCPP_STRING) || defined(_BASIC_STRING_H)
		// when using clang/libc++ or gcc/libstdc++ we have access to their hash functions
		// and can thus use the non-string Name implementation.
		#define ZMACRO_NameUsesString 0
	#endif
#endif

#ifndef ZMACRO_NameUsesString
	#define ZMACRO_NameUsesString 1
#endif

#if ZMACRO_NameUsesString

class Name
	{
public:
	inline Name() {}

	inline Name(const Name& iOther) : fString(iOther.fString) {}

	Name& operator=(const Name& iOther)
		{
		fString = iOther.fString;
		return *this;
		}

	inline ~Name() {}

	Name(const char* iStatic) : fString(iStatic) {}

	Name(const string8& iString) : fString(iString) {}

	Name(const ZP<CountedString>& iCountedString)
	:	fString(sGet(iCountedString))
		{}

	operator string8() const
		{ return fString; }

	const string8& AsString8() const
		{ return fString; }

	inline bool operator<(const Name& iOther) const
		{ return fString < iOther.fString; }

	inline bool operator==(const Name& iOther) const
		{ return fString == iOther.fString; }

	int Compare(const Name& iOther) const
		{ return fString.compare(fString); }

	bool IsEmpty() const
		{ return fString.empty(); }

	void Clear()
		{ fString.clear(); }

	size_t Hash() const
		{ return std::hash<string8>()(fString); }

private:
	string8 fString;
	};

#else // ZMACRO_NameUsesString

class Name
	{
public:
	inline Name()
	:	fIntPtr(0)
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(false)
	#endif
		{}

	inline Name(const Name& iOther)
	:	fIntPtr(iOther.fIntPtr)
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(iOther.fIsCounted)
	#endif
		{
		if (CountedString* theCounted = this->pGetIfCounted())
			spRetain(theCounted);
		}

	Name& operator=(const Name& iOther);

	inline ~Name()
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

	Name(const ZP<CountedString>& iCountedString);
	
	operator string8() const
		{ return this->AsString8(); }

	const string8 AsString8() const;

	inline bool operator<(const Name& iOther) const
		{ return this->Compare(iOther) < 0; }

	inline bool operator==(const Name& iOther) const
		{ return this->Compare(iOther) == 0; }

	int Compare(const Name& iOther) const;

	bool IsEmpty() const;

	void Clear();

	size_t Hash() const;

private:
	CountedString* pGetIfCounted();
	const CountedString* pGetIfCounted() const;

	const char* pAsCharStar() const;

	static void spRetain(const CountedString* iCounted);

	static void spRelease(const CountedString* iCounted);

	uintptr_t fIntPtr;
	#if not ZCONFIG_Is64Bit
		bool fIsCounted;
	#endif
	};

#endif // ZMACRO_NameUsesString

template <> struct RelopsTraits_HasEQ<Name> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Name> : public RelopsTraits_Has {};

template <>
inline int sCompare_T(const Name& iL, const Name& iR)
	{ return iL.Compare(iR); }

inline bool sIsEmpty(const Name& iName)
	{ return iName.IsEmpty(); }

inline bool sNotEmpty(const Name& iName)
	{ return not sIsEmpty(iName); }

} // namespace ZooLib

namespace std {

template <>
struct hash<ZooLib::Name>
	{
	std::size_t operator()(const ZooLib::Name& iName) const noexcept
		{ return iName.Hash(); }
	};

} // namespace std

#endif // __ZooLib_Name_h__
