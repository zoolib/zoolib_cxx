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

#ifndef __ZRA_RelRename__
#define __ZRA_RelRename__ 1
#include "zconfig.h"

#include "zoolib/ZUnicodeString.h"
#include "zoolib/zra/ZRA_RelHead.h"

#include <map>
#include <set>

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * RelRename

class RelRename
	{
public:
	typedef std::pair<string8, string8> Elem_t;

	RelRename(std::set<Elem_t>* ioElems);

	void swap(RelRename& iOther);

	RelRename();
	RelRename(const RelRename& iOther);
	~RelRename();
	RelRename& operator=(const RelRename& iOther);

	RelRename(const std::set<Elem_t>& iElems);

	RelRename(const RelHead& iRelHead);

	RelRename Inverted() const;

	bool operator==(const RelRename& iOther) const;
	bool operator!=(const RelRename& iOther) const;
	bool operator<(const RelRename& iOther) const;

	RelRename& operator|=(const RelRename& iOther);
	RelRename operator|(const RelRename& iOther) const;

	void ApplyToFrom(const string8& iNameTo, const string8& iNameFrom);

	const std::set<Elem_t>& GetElems() const;

	RelHead GetRelHead_To() const;
	RelHead GetRelHead_From() const;

	std::map<string8, string8> GetRename() const;

private:
	std::set<Elem_t> fElems;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Rename_t

} // namespace ZRA
} // namespace ZooLib

namespace std {
template <class T>
inline void swap(ZOOLIB_PREFIX::ZRA::RelRename& a, ZOOLIB_PREFIX::ZRA::RelRename& b)
	{ a.swap(b); }
}

#endif // __ZRA_RelRename__
