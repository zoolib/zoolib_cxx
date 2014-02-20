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

#ifndef __ZName_h__
#define __ZName_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZCountedVal.h"
#include "zoolib/ZUnicodeString.h" // For string8
#include "zoolib/ZUtil_Relops.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZName

class ZName
	{
public:
	typedef ZCountedVal<string8> CountedString;
	typedef ZRef<CountedString> RefCountedString;

	inline
	ZName()
	:	fIntPtr(0)
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(false)
	#endif
		{}

	inline
	ZName(const ZName& iOther)
	:	fIntPtr(iOther.fIntPtr)
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(iOther.fIsCounted)
	#endif
		{
		if (CountedString* theCounted = this->pGetIfCounted())
			spRetain(theCounted);
		}

	ZName& operator=(const ZName& iOther);

	inline
	~ZName()
		{
		if (CountedString* theCounted = this->pGetIfCounted())
			spRelease(theCounted);
		}

	ZName(const char* iStatic)
	:	fIntPtr(((intptr_t)iStatic))
	#if not ZCONFIG_Is64Bit
	,	fIsCounted(false)
	#endif
		{}

	ZName(const string8& iString);
	ZName(const RefCountedString& iRefCountedString);
	
	operator string8() const;
	operator RefCountedString() const;

	inline
	bool operator<(const ZName& iOther) const
		{ return this->Compare(iOther) < 0; }

	inline
	bool operator==(const ZName& iOther) const
		{ return this->Compare(iOther) == 0; }

	int Compare(const ZName& iOther) const;

	bool IsEmpty() const;

	void Clear();

	std::size_t Hash() const;

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

template <> struct RelopsTraits_HasEQ<ZName> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<ZName> : public RelopsTraits_Has {};

template <>
inline
int sCompare_T(const ZName& iL, const ZName& iR)
	{ return iL.Compare(iR); }

inline
bool sIsEmpty(const ZName& iName)
	{ return iName.IsEmpty(); }

inline
bool sNotEmpty(const ZName& iName)
	{ return not sIsEmpty(iName); }

} // namespace ZooLib

#if ZMACRO_Has_TR1

	ZMACRO_namespace_tr1_begin
	template <typename T> struct hash;

	template <>
	struct hash<ZooLib::ZName>
		{ public: std::size_t operator()(const ZooLib::ZName& iName) const { return iName.Hash(); } };

	ZMACRO_namespace_tr1_end

#endif

#endif // __ZName_h__
