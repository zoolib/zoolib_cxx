/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUnicode.h"

#include <cstring> // For std::strlen

using std::basic_string;
using std::char_traits;
using std::min;
using std::search;
using std::vector;

namespace ZooLib {

static string8 spEmptyString;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

template <class C>
void sParseStringAndAppend_T
	(const C* iSeparator,
	const C* iIgnore,
	const C* iBounce,
	const C* iTrail, size_t iTrailSize,
	vector<basic_string<C> >& ioComps)
	{
	if (iTrailSize == 0)
		return;

	size_t prevDiv = 0;

	size_t separatorSize = ZUnicode::sCountCU(iSeparator);
	size_t ignoreSize = ZUnicode::sCountCU(iIgnore);
	size_t bounceSize = ZUnicode::sCountCU(iBounce);

	if (separatorSize <= iTrailSize
		&& 0 == char_traits<C>::compare(iTrail, iSeparator, separatorSize))
		{
		prevDiv = separatorSize;
		}

	while (prevDiv < iTrailSize)
		{
		size_t nextDiv = search(iTrail + prevDiv, iTrail + iTrailSize,
			iSeparator, iSeparator + separatorSize) - iTrail;

		if (size_t compLength = nextDiv - prevDiv)
			{
			if (compLength == bounceSize
				&& 0 == char_traits<C>::compare(iTrail + prevDiv, iBounce, bounceSize))
				{
				// It's a bounce.
				ioComps.push_back(basic_string<C>());
				}
			else if (compLength == ignoreSize
				&& 0 == char_traits<C>::compare(iTrail + prevDiv, iIgnore, ignoreSize))
				{
				// It's an ignore component, so we ignore it.
				}
			else
				{
				// It must be a regular component.
				ioComps.push_back(basic_string<C>(iTrail + prevDiv, compLength));
				}
			}

		prevDiv = nextDiv + 1;
		}
	}

static void spNormalize_KeepLeadingBounces(const vector<string8>& iComps, vector<string8>& oComps)
	{
	for (vector<string8>::const_iterator current = iComps.begin();
		current != iComps.end(); ++current)
		{
		// Note that an empty string8 is our notation for a bounce.
		if ((*current).empty() && !oComps.empty() && !oComps.back().empty())
			{
			// The current component is a bounce, the new list is not empty
			// and the component at the end of the the new list is not itself a bounce,
			// so just remove the component at the end of the new list.
			oComps.pop_back();
			}
		else
			{
			oComps.push_back(*current);
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTrail

/** \class ZTrail
*/

void ZTrail::sParseStringAndAppend
	(const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
	const UTF8* iTrail, size_t iTrailSize,
	vector<string8>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

void ZTrail::sParseStringAndAppend
	(const UTF16* iSeparator, const UTF16* iIgnore, const UTF16* iBounce,
	const UTF16* iTrail, size_t iTrailSize,
	vector<string16>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

size_t ZTrail::sNormalize_ReturnLeadingBounces(const vector<string8>& iComps,
	vector<string8>& oComps)
	{
	spNormalize_KeepLeadingBounces(iComps, oComps);
	size_t bounces = 0;
	while (oComps.size() && oComps.front().empty())
		{
		oComps.erase(oComps.begin());
		++bounces;
		}
	return bounces;
	}

ZTrail::ZTrail()
	{}

ZTrail::ZTrail(const ZTrail& iTrail)
:	fComps(iTrail.fComps)
	{}

ZTrail::~ZTrail()
	{}

ZTrail& ZTrail::operator=(const ZTrail& iTrail)
	{
	fComps = iTrail.fComps;
	return *this;
	}

ZTrail::ZTrail(const string8& iPOSIXTrail)
	{
	if (size_t trailLength = iPOSIXTrail.size())
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail.data(), trailLength, fComps);
	}

ZTrail::ZTrail(const char* iPOSIXTrail)
	{
	if (size_t trailLength = std::strlen(iPOSIXTrail))
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail, trailLength, fComps);
	}

ZTrail::ZTrail(const char* iPOSIXTrail, size_t iSize)
	{
	if (iSize)
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail, iSize, fComps);
	}

ZTrail::ZTrail(const string8& iSeparator, const string8& iIgnore,
	const string8& iBounce, const string8& iTrail)
	{
	if (size_t trailLength = iTrail.size())
		{
		sParseStringAndAppend(iSeparator.c_str(), iIgnore.c_str(),
			iBounce.c_str(), iTrail.data(), trailLength, fComps);
		}
	}

bool ZTrail::operator==(const ZTrail& iOther) const
	{ return fComps == iOther.fComps; }

bool ZTrail::operator<(const ZTrail& iOther) const
	{ return fComps < iOther.fComps; }

ZTrail ZTrail::operator+(const ZTrail& iTrail) const
	{
	ZTrail result = *this;
	result += iTrail;
	return result;
	}

ZTrail& ZTrail::operator+=(const ZTrail& iTrail)
	{
	fComps.insert(fComps.end(), iTrail.fComps.begin(), iTrail.fComps.end());
	return *this;
	}

void ZTrail::AppendTrail(const ZTrail& iTrail)
	{ fComps.insert(fComps.end(), iTrail.fComps.begin(), iTrail.fComps.end()); }

void ZTrail::AppendComp(const string8& iComp)
	{ fComps.push_back(iComp); }

void ZTrail::AppendBounce()
	{ fComps.push_back(spEmptyString); }

void ZTrail::AppendBounces(size_t iCount)
	{
	string8 empty;
	while (iCount--)
		fComps.push_back(empty);
	}

void ZTrail::PrependTrail(const ZTrail& iTrail)
	{ fComps.insert(fComps.begin(), iTrail.fComps.begin(), iTrail.fComps.end()); }

void ZTrail::PrependComp(const string8& iComp)
	{ fComps.insert(fComps.begin(), iComp); }

void ZTrail::PrependBounce()
	{ fComps.insert(fComps.begin(), spEmptyString); }

void ZTrail::PrependBounces(size_t iCount)
	{
	string8 empty;
	while (iCount--)
		fComps.insert(fComps.begin(), empty);
	}

string8 ZTrail::AsString() const
	{ return this->AsString("/", ".."); }

string8 ZTrail::AsString(const string8& iSeparator, const string8& iBounce) const
	{
	string8 result;
	for (vector<string8>::const_iterator ii = fComps.begin(); ii != fComps.end(); ++ii)
		{
		if (!result.empty())
			result += iSeparator;

		if (ii->empty())
			result += iBounce;
		else
			result += *ii;
		}
	return result;
	}

ZTrail ZTrail::Branch() const
	{
	if (fComps.empty())
		return ZTrail();
	return ZTrail(fComps.begin(), fComps.end() - 1);
	}

const string8& ZTrail::Leaf() const
	{
	if (fComps.empty())
		return spEmptyString;
	return fComps.back();
	}

size_t ZTrail::Count() const
	{ return fComps.size(); }

const string8& ZTrail::At(size_t iIndex) const
	{
	if (iIndex < fComps.size())
		return fComps[iIndex];
	return spEmptyString;
	}

ZTrail ZTrail::SubTrail(size_t iBegin, size_t iEnd) const
	{
	iBegin = min(iBegin, fComps.size());
	iEnd = min(iEnd, fComps.size());
	if (iEnd > iBegin)
		return ZTrail(fComps.begin() + iBegin, fComps.begin() + iEnd);
	return ZTrail();
	}

ZTrail ZTrail::SubTrail(size_t iBegin) const
	{
	iBegin = min(iBegin, fComps.size());
	return ZTrail(fComps.begin() + iBegin, fComps.end());
	}

/// Return a normalized version of the trail.
ZTrail ZTrail::Normalized() const
	{
	vector<string8> newComps;
	spNormalize_KeepLeadingBounces(fComps, newComps);
	return ZTrail(newComps.begin(), newComps.end());
	}

/// Normalize the trail and return a reference to it.
ZTrail& ZTrail::Normalize()
	{
	vector<string8> newComps;
	spNormalize_KeepLeadingBounces(fComps, newComps);
	fComps = newComps;
	return *this;
	}

/// Return true if the trail is normalized.
/**
A normalized trail is one which has zero or more leading bounces, followed
by zero or more regular components.
*/
bool ZTrail::IsNormalized() const
	{
	vector<string8>::const_iterator ii = fComps.begin();

	// Skip any leading bounces.
	while (ii != fComps.end() && ii->empty())
		++ii;

	// Skip any regular components.
	while (ii != fComps.end() && not ii->empty())
		++ii;

	// If we hit the end then there were no bounces following a regular component.
	return ii== fComps.end();
	}

/**
Return a trail that would navigate from iSource to oDest. The vectors iSource and oDest
are considered to be lists of names of nodes starting at the root or other common node.
This code is used by ZFileLoc implementations, but is also available to application code.
*/
ZTrail ZTrail::sTrailFromTo(const vector<string8>& iSource, const vector<string8>& oDest)
	{
	size_t matchUntil = 0;
	while (matchUntil < iSource.size() && matchUntil < oDest.size()
		&& iSource[matchUntil] == oDest[matchUntil])
		{
		++matchUntil;
		}

	ZTrail theTrail;
	if (matchUntil < iSource.size())
		theTrail.AppendBounces(iSource.size() - matchUntil);

	if (matchUntil < oDest.size())
		theTrail.AppendTrail(oDest.begin() + matchUntil, oDest.end());

	return theTrail;
	}

ZTrail operator+(const string8& iPOSIXTrail, const ZTrail& iTrail)
	{ return ZTrail(iPOSIXTrail) + iTrail; }

} // namespace ZooLib
