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

#include <string.h> // For strlen

using std::basic_string;
using std::char_traits;
using std::min;
using std::search;
using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

template <class C>
void sParseStringAndAppend_T(
	const C* iSeparator,
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

static void sNormalize_KeepLeadingBounces(const vector<string>& iComps, vector<string>& oComps)
	{
	for (vector<string>::const_iterator current = iComps.begin();
		current != iComps.end(); ++current)
		{
		// Note that an empty string is our notation for a bounce.
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

void ZTrail::sParseStringAndAppend(
	const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
	const UTF8* iTrail, size_t iTrailSize,
	vector<string8>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

void ZTrail::sParseStringAndAppend(
	const UTF16* iSeparator, const UTF16* iIgnore, const UTF16* iBounce,
	const UTF16* iTrail, size_t iTrailSize,
	vector<string16>& ioComps)
	{ return sParseStringAndAppend_T(iSeparator, iIgnore, iBounce, iTrail, iTrailSize, ioComps); }

size_t ZTrail::sNormalize_ReturnLeadingBounces(const vector<string>& iComps,
	vector<string>& oComps)
	{
	sNormalize_KeepLeadingBounces(iComps, oComps);
	size_t bounces = 0;
	while (oComps.size() && oComps.front().empty())
		{
		oComps.erase(oComps.begin());
		++bounces;
		}
	return bounces;
	}

ZTrail::ZTrail()
:	fValid(true)
	{}

ZTrail::ZTrail(bool iValid)
:	fValid(iValid)
	{}

ZTrail::ZTrail(const ZTrail& iTrail)
:	fComps(iTrail.fComps),
	fValid(iTrail.fValid)
	{}

ZTrail::ZTrail(const string& iPOSIXTrail)
:	fValid(true)
	{
	if (size_t trailLength = iPOSIXTrail.size())
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail.data(), trailLength, fComps);
	}

ZTrail::ZTrail(const char* iPOSIXTrail)
:	fValid(true)
	{
	if (size_t trailLength = strlen(iPOSIXTrail))
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail, trailLength, fComps);
	}

ZTrail::ZTrail(const char* iPOSIXTrail, size_t iSize)
:	fValid(true)
	{
	if (iSize)
		sParseStringAndAppend("/", ".", "..", iPOSIXTrail, iSize, fComps);
	}

ZTrail::ZTrail(const std::string& iSeparator, const std::string& iIgnore,
	const std::string& iBounce, const std::string& iTrail)
:	fValid(true)
	{
	if (size_t trailLength = iTrail.size())
		{
		sParseStringAndAppend(iSeparator.c_str(), iIgnore.c_str(),
			iBounce.c_str(), iTrail.data(), trailLength, fComps);
		}
	}

ZTrail::~ZTrail()
	{}

ZTrail::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fValid); }

ZTrail& ZTrail::operator=(const ZTrail& iTrail)
	{
	fComps = iTrail.fComps;
	fValid = iTrail.fValid;
	return *this;
	}

ZTrail ZTrail::operator+(const ZTrail& iTrail) const
	{
	ZTrail result = *this;
	result += iTrail;
	return result;
	}

ZTrail& ZTrail::operator+=(const ZTrail& iTrail)
	{
	// What does it mean to append a valid trail to an invalid one?
	if (iTrail.fValid)
		{
		if (fValid)
			{
			fComps.insert(fComps.end(), iTrail.fComps.begin(), iTrail.fComps.end());
			}
		else
			{
			fValid = true;
			fComps = iTrail.fComps;
			}
		}
	return *this;
	}

void ZTrail::AppendTrail(const ZTrail& iTrail)
	{
	if (iTrail.fValid)
		{
		if (fValid)
			{
			fComps.insert(fComps.end(), iTrail.fComps.begin(), iTrail.fComps.end());
			}
		else
			{
			fValid = true;
			fComps = iTrail.fComps;
			}
		}
	}

void ZTrail::AppendComp(const string& iComp)
	{
	fValid = true;
	fComps.push_back(iComp);
	}

void ZTrail::AppendBounce()
	{
	fValid = true;
	fComps.push_back(string());
	}

void ZTrail::AppendBounces(size_t iCount)
	{
	fValid = true;
	string empty;
	while (iCount--)
		fComps.push_back(empty);
	}

void ZTrail::PrependTrail(const ZTrail& iTrail)
	{
	if (iTrail.fValid)
		{
		if (fValid)
			{
			fComps.insert(fComps.begin(), iTrail.fComps.begin(), iTrail.fComps.end());
			}
		else
			{
			fValid = true;
			fComps = iTrail.fComps;
			}
		}
	}

void ZTrail::PrependComp(const string& iComp)
	{
	fValid = true;
	fComps.insert(fComps.begin(), iComp);
	}

void ZTrail::PrependBounce()
	{
	fValid = true;
	fComps.insert(fComps.begin(), string());
	}

void ZTrail::PrependBounces(size_t iCount)
	{
	fValid = true;
	string empty;
	while (iCount--)
		fComps.insert(fComps.begin(), empty);
	}

string ZTrail::AsString() const
	{ return this->AsString("/", ".."); }

std::string ZTrail::AsString(const std::string& iSeparator, const std::string& iBounce) const
	{
	string result;
	for (vector<string>::const_iterator i = fComps.begin(); i != fComps.end(); ++i)
		{
		if (!result.empty())
			result += iSeparator;

		if ((*i).empty())
			result += iBounce;
		else
			result += *i;
		}
	return result;
	}

ZTrail ZTrail::Branch() const
	{
	if (fComps.empty())
		return ZTrail();
	return ZTrail(fComps.begin(), fComps.end() - 1);
	}

string ZTrail::Leaf() const
	{
	if (fComps.empty())
		return string();
	return fComps.back();
	}

size_t ZTrail::Count() const
	{ return fComps.size(); }

string ZTrail::At(size_t iIndex) const
	{
	if (iIndex < fComps.size())
		return fComps[iIndex];
	return string();
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
	vector<string> newComps;
	sNormalize_KeepLeadingBounces(fComps, newComps);
	return ZTrail(newComps.begin(), newComps.end());
	}

/// Normalize the trail and return a reference to it.
ZTrail& ZTrail::Normalize()
	{
	vector<string> newComps;
	sNormalize_KeepLeadingBounces(fComps, newComps);
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
	vector<string>::const_iterator i = fComps.begin();

	// Skip any leading bounces.
	while (i != fComps.end() && (*i).empty())
		++i;

	// Skip any regular components.
	while (i != fComps.end() && !(*i).empty())
		++i;

	// If we hit the end then there were no bounces following a regular component.
	return i== fComps.end();
	}

/**
Return a trail that would navigate from iSource to iDest. The vectors iSource and iDest
are considered to be lists of names of nodes starting at the root or other common node.
This code is used by ZFileLoc implementations, but is also available to application code.
*/
ZTrail ZTrail::sTrailFromTo(const vector<string>& iSource, const vector<string>& iDest)
	{
	size_t matchUntil = 0;
	while (matchUntil < iSource.size() && matchUntil < iDest.size()
		&& iSource[matchUntil] == iDest[matchUntil])
		{
		++matchUntil;
		}

	ZTrail theTrail;
	if (matchUntil < iSource.size())
		theTrail.AppendBounces(iSource.size() - matchUntil);

	if (matchUntil < iDest.size())
		theTrail.AppendTrail(iDest.begin() + matchUntil, iDest.end());

	return theTrail;
	}

ZTrail operator+(const string& iPOSIXTrail, const ZTrail& iTrail)
	{ return ZTrail(iPOSIXTrail) + iTrail; }

NAMESPACE_ZOOLIB_END
