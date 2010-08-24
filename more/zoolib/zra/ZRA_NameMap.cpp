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

#include "zoolib/ZCompare_Pair.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/zra/ZRA_NameMap.h"

using std::map;
using std::set;

namespace ZooLib {
namespace ZRA {

const ZStrimW& operator<<(const ZStrimW& w, const NameMap& iNM)
	{
	w << "(";
	bool isSubsequent = false;
	const set<NameMap::Elem_t>& theElems = iNM.GetElems();
	for (set<NameMap::Elem_t>::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
		{
		if (isSubsequent)
			w << ", ";
		isSubsequent = true;
		w << i->first << "<--" << i->second;
		}
	w << ")";
	return w;
	}

// =================================================================================================
#pragma mark -
#pragma mark * NameMap

NameMap::NameMap(set<Elem_t>* ioElems)
	{
	ioElems->swap(fElems);
	}

void NameMap::swap(NameMap& iOther)
	{ fElems.swap(iOther.fElems); }

NameMap::NameMap()
	{}

NameMap::NameMap(const NameMap& iOther)
:	fElems(iOther.fElems)
	{}

NameMap::~NameMap()
	{}

NameMap& NameMap::operator=(const NameMap& iOther)
	{
	fElems = iOther.fElems;
	return *this;
	}

NameMap::NameMap(const set<Elem_t>& iElems)
:	fElems(iElems)
	{}

NameMap::NameMap(const RelHead& iRelHead)
	{
	const RelHead::Base_t& theElems = iRelHead.GetElems();
	for (RelHead::Base_t::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
		fElems.insert(std::make_pair(*i, *i));
	}

NameMap NameMap::Inverted() const
	{
	set<Elem_t> result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result.insert(Elem_t(i->second, i->first));
	return NameMap(&result);
	}

bool NameMap::operator==(const NameMap& iOther) const
	{ return fElems == iOther.fElems; }

bool NameMap::operator!=(const NameMap& iOther) const
	{ return fElems != iOther.fElems; }

bool NameMap::operator<(const NameMap& iOther) const
	{ return fElems < iOther.fElems; }

NameMap& NameMap::operator|=(const NameMap& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

NameMap NameMap::operator|(const NameMap& iOther) const
	{
	set<Elem_t> result;
	ZUtil_STL_set::sOr(fElems, iOther.fElems, result);
	return NameMap(&result);
	}

void NameMap::InsertToFrom(const RelName& iNameTo, const RelName& iNameFrom)
	{ fElems.insert(Elem_t(iNameTo, iNameFrom)); }

void NameMap::ApplyToFrom(const RelHead::key_type& iNameTo, const RelHead::key_type& iNameFrom)
	{
	set<Elem_t> result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		{
		if (i->first == iNameFrom)
			{
			result.insert(Elem_t(iNameTo, i->second));
			}
		else
			{
			result.insert(*i);
			}
		}
	fElems.swap(result);
	}

const set<NameMap::Elem_t>& NameMap::GetElems() const
	{ return fElems; }

RelHead NameMap::GetRelHead_To() const
	{
	RelHead result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result |= i->first;
	return result;
	}

RelHead NameMap::GetRelHead_From() const
	{
	RelHead result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result |= i->second;
	return result;
	}

map<RelHead::key_type, RelHead::key_type> NameMap::GetRename() const
	{
	map<RelHead::key_type, RelHead::key_type> result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result[i->second] = i->first;
	return result;
	}

} // namespace ZRA
} // namespace ZooLib

namespace ZooLib {

template <> int sCompare_T(const ZRA::NameMap& iL, const ZRA::NameMap& iR)
	{
	const std::set<ZRA::NameMap::Elem_t>& l = iL.GetElems();
	const std::set<ZRA::NameMap::Elem_t>& r = iR.GetElems();
	return sCompareIterators_T(l.begin(), l.end(), r.begin(), r.end());
	}
} // namespace ZooLib
