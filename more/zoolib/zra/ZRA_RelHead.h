/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZRA_RelHead__
#define __ZRA_RelHead__ 1
#include "zconfig.h"

#include "zoolib/ZUnicodeString.h"

#include <map>
#include <set>

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * RelHead

class RelHead
	{
	explicit RelHead(std::set<string8>* ioElems);

public:
	void swap(RelHead& iOther);

	RelHead();
	RelHead(const RelHead& iOther);
	~RelHead();
	RelHead& operator=(const RelHead& iOther);

	RelHead(const string8& iElem);

	RelHead(const std::set<string8>& iElems);

	template <class Iterator>
	RelHead(Iterator iBegin, Iterator iEnd);

	template <class Iterator>
	RelHead(Iterator iBegin, size_t iCount);

	bool operator==(const RelHead& iOther) const;
	bool operator!=(const RelHead& iOther) const;
	bool operator<(const RelHead& iOther) const;

	RelHead& operator&=(const RelHead& iOther);
	RelHead operator&(const RelHead& iOther) const;

	RelHead& operator|=(const RelHead& iOther);
	RelHead operator|(const RelHead& iOther) const;

	RelHead& operator-=(const RelHead& iOther);
	RelHead operator-(const RelHead& iOther) const;

	RelHead& operator^=(const RelHead& iOther);
	RelHead operator^(const RelHead& iOther) const;

	bool Contains(const RelHead& iOther) const;
	bool Contains(const string8& iElem) const;

	RelHead& Insert(const string8& iElem);
	RelHead& Erase(const string8& iElem);

	const std::set<string8>& GetElems() const;

private:
	std::set<string8> fElems;
	};

template <class Iterator>
RelHead::RelHead(Iterator iBegin, Iterator iEnd)
:	fElems(iBegin, iEnd)
	{}

template <class Iterator>
RelHead::RelHead(Iterator iBegin, size_t iCount)
:	fElems(iBegin, iBegin + iCount)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * RelHead operators

inline RelHead operator&(const string8& iElem, const RelHead& iRelHead)
	{ return iRelHead & iElem; }

inline RelHead operator|(const string8& iElem, const RelHead& iRelHead)
	{ return iRelHead | iElem; }

inline RelHead operator&(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead & string8(iElem); }

inline RelHead operator&(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead & string8(iElem); }

inline RelHead operator|(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead | string8(iElem); }

inline RelHead operator|(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead | string8(iElem); }

inline RelHead operator-(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead - string8(iElem); }

inline RelHead operator^(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead ^ string8(iElem); }

inline RelHead operator^(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead ^ string8(iElem); }

// =================================================================================================
#pragma mark -
#pragma mark * Rename_t

typedef std::map<string8, string8> Rename_t;

Rename_t sInvert(const Rename_t& iRename);

} // namespace ZRA
} // namespace ZooLib

namespace std {
template <class T>
inline void swap(ZooLib::ZRA::RelHead& a, ZooLib::ZRA::RelHead& b)
	{ a.swap(b); }
}

#endif // __ZRA_RelHead__
