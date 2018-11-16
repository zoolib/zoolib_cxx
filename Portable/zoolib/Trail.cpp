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

#include "zoolib/Compat_algorithm.h"
#include "zoolib/Trail.h"
#include "zoolib/Unicode.h"

#include <cstring> // For std::strlen

using std::basic_string;
using std::char_traits;
using std::min;
using std::search;
using std::vector;

namespace ZooLib {

static string8 spEmptyString;

// =================================================================================================
#pragma mark - Helper functions

template <class C, class Comp>
void sParseStringAndAppend_T(
	const C* iSeparator,
	const C* iIgnore,
	const C* iBounce,
	const C* iTrail, size_t iTrailSize,
	vector<Comp>& ioComps)
	{
	if (iTrailSize == 0)
		return;

	size_t prevDiv = 0;

	size_t separatorSize = Unicode::sCountCU(iSeparator);
	size_t ignoreSize = Unicode::sCountCU(iIgnore);
	size_t bounceSize = Unicode::sCountCU(iBounce);

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

// =================================================================================================
#pragma mark - Trail

/** \class Trail
*/

void Trail::sParseStringAndAppend(
	const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
	const UTF8* iTrail, size_t iTrailSize,
	vector<string8>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

void Trail::sParseStringAndAppend(
	const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
	const UTF8* iTrail, size_t iTrailSize,
	vector<Comp>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

void Trail::sParseStringAndAppend(
	const UTF16* iSeparator, const UTF16* iIgnore, const UTF16* iBounce,
	const UTF16* iTrail, size_t iTrailSize,
	vector<string16>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

Trail::Trail()
	{}

Trail::Trail(const Trail& iTrail)
:	fComps(iTrail.fComps)
	{}

Trail::~Trail()
	{}

Trail& Trail::operator=(const Trail& iTrail)
	{
	fComps = iTrail.fComps;
	return *this;
	}

Trail::Trail(const string8& iPOSIXTrail)
	{
	if (size_t trailLength = iPOSIXTrail.size())
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail.data(), trailLength, fComps);
	}

Trail::Trail(const char* iPOSIXTrail)
	{
	if (size_t trailLength = std::strlen(iPOSIXTrail))
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail, trailLength, fComps);
	}

Trail::Trail(const char* iPOSIXTrail, size_t iSize)
	{
	if (iSize)
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail, iSize, fComps);
	}

Trail::Trail(const string8& iSeparator, const string8& iIgnore,
	const string8& iBounce, const string8& iTrail)
	{
	if (size_t trailLength = iTrail.size())
		{
		sParseStringAndAppend(iSeparator.c_str(), iIgnore.c_str(),
			iBounce.c_str(), iTrail.data(), trailLength, fComps);
		}
	}

bool Trail::operator==(const Trail& iOther) const
	{ return fComps == iOther.fComps; }

bool Trail::operator<(const Trail& iOther) const
	{ return fComps < iOther.fComps; }

Trail Trail::operator+(const Trail& iTrail) const
	{
	Trail result = *this;
	result += iTrail;
	return result;
	}

Trail& Trail::operator+=(const Trail& iTrail)
	{
	fComps.insert(fComps.end(), iTrail.fComps.begin(), iTrail.fComps.end());
	return *this;
	}

void Trail::AppendTrail(const Trail& iTrail)
	{ fComps.insert(fComps.end(), iTrail.fComps.begin(), iTrail.fComps.end()); }

void Trail::AppendComp(const Comp& iComp)
	{ fComps.push_back(iComp); }

void Trail::AppendBounce()
	{ fComps.push_back(spEmptyString); }

void Trail::AppendBounces(size_t iCount)
	{
	string8 empty;
	while (iCount--)
		fComps.push_back(empty);
	}

void Trail::PrependTrail(const Trail& iTrail)
	{ fComps.insert(fComps.begin(), iTrail.fComps.begin(), iTrail.fComps.end()); }

void Trail::PrependComp(const Comp& iComp)
	{ fComps.insert(fComps.begin(), iComp); }

void Trail::PrependBounce()
	{ fComps.insert(fComps.begin(), spEmptyString); }

void Trail::PrependBounces(size_t iCount)
	{
	string8 empty;
	while (iCount--)
		fComps.insert(fComps.begin(), empty);
	}

string8 Trail::AsString() const
	{ return this->AsString("/", ".."); }

string8 Trail::AsString(const string8& iSeparator, const string8& iBounce) const
	{
	string8 result;
	for (vector<Comp>::const_iterator ii = fComps.begin(); ii != fComps.end(); ++ii)
		{
		if (not result.empty())
			result += iSeparator;

		if (sIsEmpty(*ii))
			result += iBounce;
		else
			result += *ii;
		}
	return result;
	}

Trail Trail::Branch() const
	{
	if (fComps.empty())
		return Trail();
	return Trail(fComps.begin(), fComps.end() - 1);
	}

const Trail::Comp& Trail::Leaf() const
	{
	if (fComps.empty())
		return sDefault<Comp>();
	return fComps.back();
	}

size_t Trail::Count() const
	{ return fComps.size(); }

const Trail::Comp& Trail::At(size_t iIndex) const
	{
	if (iIndex < fComps.size())
		return fComps[iIndex];
	return sDefault<Comp>();
	}

Trail Trail::SubTrail(size_t iBegin, size_t iEnd) const
	{
	iBegin = min(iBegin, fComps.size());
	iEnd = min(iEnd, fComps.size());
	if (iEnd > iBegin)
		return Trail(fComps.begin() + iBegin, fComps.begin() + iEnd);
	return Trail();
	}

Trail Trail::SubTrail(size_t iBegin) const
	{
	iBegin = min(iBegin, fComps.size());
	return Trail(fComps.begin() + iBegin, fComps.end());
	}

/// Return a normalized version of the trail.
Trail Trail::Normalized() const
	{
	vector<Comp> newComps;
	sNormalize_KeepLeadingBounces(fComps, newComps);
	return Trail(newComps.begin(), newComps.end());
	}

/// Normalize the trail and return a reference to it.
Trail& Trail::Normalize()
	{
	vector<Comp> newComps;
	sNormalize_KeepLeadingBounces(fComps, newComps);
	fComps = newComps;
	return *this;
	}

/// Return true if the trail is normalized.
/**
A normalized trail is one which has zero or more leading bounces, followed
by zero or more regular components.
*/
bool Trail::IsNormalized() const
	{
	vector<Comp>::const_iterator ii = fComps.begin();

	// Skip any leading bounces.
	while (ii != fComps.end() && sIsEmpty(*ii))
		++ii;

	// Skip any regular components.
	while (ii != fComps.end() && sNotEmpty(*ii))
		++ii;

	// If we hit the end then there were no bounces following a regular component.
	return ii== fComps.end();
	}

Trail operator+(const string8& iPOSIXTrail, const Trail& iTrail)
	{ return Trail(iPOSIXTrail) + iTrail; }

} // namespace ZooLib
