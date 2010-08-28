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

#ifndef __ZRA_NameMap__
#define __ZRA_NameMap__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/zra/ZRA_RelHead.h"

#include <map>
#include <set>

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * NameMap

class NameMap
	{
public:
	typedef std::pair<RelName, RelName> Elem_t;

	NameMap(std::set<Elem_t>* ioElems);

	void swap(NameMap& iOther);

	NameMap();
	NameMap(const NameMap& iOther);
	~NameMap();
	NameMap& operator=(const NameMap& iOther);

	NameMap(const std::set<Elem_t>& iElems);

	NameMap(const RelHead& iRelHead);

	NameMap Inverted() const;

	bool operator==(const NameMap& iOther) const;
	bool operator!=(const NameMap& iOther) const;
	bool operator<(const NameMap& iOther) const;

	NameMap& operator|=(const NameMap& iOther);
	NameMap operator|(const NameMap& iOther) const;

	void InsertToFrom(const RelName& iNameTo, const RelName& iNameFrom);
	void ApplyToFrom(const RelName& iNameTo, const RelName& iNameFrom);

	const std::set<Elem_t>& GetElems() const;

	RelHead GetRelHead_To() const;
	RelHead GetRelHead_From() const;

	std::map<RelName, RelName> GetRename() const;

private:
	std::set<Elem_t> fElems;
	};

inline void swap(NameMap& a, NameMap& b)
	{ a.swap(b); }

const ZStrimW& operator<<(const ZStrimW& w, const NameMap& iNM);

} // namespace ZRA
} // namespace ZooLib

namespace ZooLib {
template <> int sCompare_T(const ZRA::NameMap& iL, const ZRA::NameMap& iR);
} // namespace ZooLib

#endif // __ZRA_NameMap__
