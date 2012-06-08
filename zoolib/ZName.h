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

namespace ZooLib {

// =================================================================================================
// MARK: -

class ZName
	{
public:
	typedef ZCountedVal<string8> CountedString;
	typedef ZRef<CountedString> ZRefCountedString;

	ZName();
	ZName(const ZName& iOther);
	ZName& operator=(const ZName& iOther);
	~ZName();

	ZName(const char* iStatic);
	ZName(const string8& iString);
	ZName(const ZRefCountedString& iCountedString);
	
	operator string8() const;

	bool operator<(const ZName& iOther) const;
	bool operator==(const ZName& iOther) const;

	int Compare(const ZName& iOther) const;

	bool IsNull() const;
	void Clear();

	std::size_t Hash() const;

private:
	intptr_t fIntPtr;
	bool fIsCounted;
	};

template <>
inline int sCompare_T(const ZName& iL, const ZName& iR)
	{ return iL.Compare(iR); }

} // namespace ZooLib

namespace std { namespace tr1 { 

template<typename T> struct hash;
template<>
struct hash<ZooLib::ZName>
	{ std::size_t operator()(const ZooLib::ZName& iName) const { return iName.Hash(); } };
}}	

#endif // __ZName_h__
