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
// MARK: - Name

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

#if ZMACRO_Has_tr1

	ZMACRO_namespace_tr1_begin
	template <typename T> struct hash;

	template <>
	struct hash<ZooLib::Name>
		{ public: size_t operator()(const ZooLib::Name& iName) const { return iName.Hash(); } };

	ZMACRO_namespace_tr1_end

#endif

#endif // __ZooLib_Name_h__
