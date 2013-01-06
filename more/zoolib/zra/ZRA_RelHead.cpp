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

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompare_String.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/zra/ZRA_RelHead.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare

template <>
int sCompare_T(const ZRA::RelHead& iL, const ZRA::RelHead& iR)
	{ return sCompareIterators_T(iL.begin(), iL.end(), iR.begin(), iR.end()); }

ZMACRO_CompareRegistration_T(ZRA::RelHead)

template <>
int sCompare_T(const ZRA::Rename& iL, const ZRA::Rename& iR)
	{ return sCompareIterators_T(iL.begin(), iL.end(), iR.begin(), iR.end()); }

ZMACRO_CompareRegistration_T(ZRA::Rename)

} // namespace ZooLib

namespace ZooLib {
namespace ZRA {

// =================================================================================================
// MARK: - Rename

Rename sInverted(const Rename& iRename)
	{
	Rename result;
	foreachi (ii, iRename)
		result[ii->second] = ii->first;
	return result;
	}

// =================================================================================================
// MARK: - Prefix

bool sHasPrefix(const ColName& iPrefix, const ColName& iColName)
	{ return iColName.substr(0, iPrefix.size()) == iPrefix; }

ColName sPrefixInserted(const ColName& iPrefix, const ColName& iColName)
	{ return iPrefix + iColName; }

ColName sPrefixErased(const ColName& iPrefix, const ColName& iColName)
	{
	if (iColName.substr(0, iPrefix.size()) == iPrefix)
		return iColName.substr(iPrefix.size(), ColName::npos);
	return iColName;
	}

ColName sRenamed(const Rename& iRename, const ColName& iColName)
	{
	Rename::const_iterator iter = iRename.find(iColName);
	if (iRename.end() == iter)
		return iColName;
	return iter->second;
	}

bool sHasPrefix(const ColName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return true;

	foreachi (ii, iRelHead)
		{
		if (not sHasPrefix(iPrefix, *ii))
			return false;
		}

	return true;
	}

RelHead sPrefixInserted(const ColName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;

	RelHead result;
	foreachi (ii, iRelHead)
		result.insert(sPrefixInserted(iPrefix, *ii));

	return result;
	}

RelHead sPrefixErased(const ColName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;

	RelHead result;
	foreachi (ii, iRelHead)
		result.insert(sPrefixErased(iPrefix, *ii));

	return result;
	}

RelHead sRenamed(const Rename& iRename, const RelHead& iRelHead)
	{
	if (iRename.empty())
		return iRelHead;

	RelHead result;
	foreachi (ii, iRelHead)
		result.insert(sRenamed(iRename, *ii));

	return result;
	}

} // namespace ZRA
} // namespace ZooLib
