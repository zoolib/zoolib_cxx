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

#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/zra/ZRA_RelRename.h"

namespace ZooLib {
namespace ZRA {

using std::map;
using std::set;

// =================================================================================================
#pragma mark -
#pragma mark * RelRename

RelRename::RelRename(set<Elem_t>* ioElems)
	{
	ioElems->swap(fElems);
	}

void RelRename::swap(RelRename& iOther)
	{ fElems.swap(iOther.fElems); }

RelRename::RelRename()
	{}

RelRename::RelRename(const RelRename& iOther)
:	fElems(iOther.fElems)
	{}

RelRename::~RelRename()
	{}

RelRename& RelRename::operator=(const RelRename& iOther)
	{
	fElems = iOther.fElems;
	return *this;
	}

RelRename::RelRename(const set<Elem_t>& iElems)
:	fElems(iElems)
	{}

RelRename::RelRename(const RelHead& iRelHead)
	{
	const set<string8>& theElems = iRelHead.GetElems();
	for (set<string8>::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
		fElems.insert(make_pair(*i, *i));
	}

RelRename RelRename::Inverted() const
	{
	set<Elem_t> result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result.insert(Elem_t((*i).second, (*i).first));
	return RelRename(&result);
	}

bool RelRename::operator==(const RelRename& iOther) const
	{ return fElems == iOther.fElems; }

bool RelRename::operator!=(const RelRename& iOther) const
	{ return fElems != iOther.fElems; }
	
bool RelRename::operator<(const RelRename& iOther) const
	{ return fElems < iOther.fElems; }

RelRename& RelRename::operator|=(const RelRename& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

RelRename RelRename::operator|(const RelRename& iOther) const
	{
	set<Elem_t> result;
	ZUtil_STL_set::sOr(fElems, iOther.fElems, result);
	return RelRename(&result);
	}

void RelRename::ApplyToFrom(const string8& iNameTo, const string8& iNameFrom)
	{
	set<Elem_t> result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		{
		if ((*i).first == iNameFrom)
			{
			result.insert(Elem_t(iNameTo, (*i).second));
			}
		else
			{
			result.insert(*i);
			}
		}
	fElems.swap(result);
	}

const set<RelRename::Elem_t>& RelRename::GetElems() const
	{ return fElems; }

RelHead RelRename::GetRelHead_To() const
	{
	RelHead result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result |= (*i).first;
	return result;
	}

RelHead RelRename::GetRelHead_From() const
	{
	RelHead result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result |= (*i).second;
	return result;
	}

map<string8, string8> RelRename::GetRename() const
	{
	map<string8, string8> result;
	for (set<Elem_t>::const_iterator i = fElems.begin(); i != fElems.end(); ++i)
		result[(*i).second] = (*i).first;
	return result;
	}

} // namespace ZRA
} // namespace ZooLib
