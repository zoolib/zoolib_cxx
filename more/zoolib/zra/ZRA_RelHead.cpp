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
	for (Rename::const_iterator i = iRename.begin(); i != iRename.end(); ++i)
		result[i->second] = i->first;
	return result;
	}

// =================================================================================================
// MARK: - Prefix

bool sHasPrefix(const RelName& iPrefix, const RelName& iRelName)
	{ return iRelName.substr(0, iPrefix.size()) == iPrefix; }

RelName sPrefixInserted(const RelName& iPrefix, const RelName& iRelName)
	{ return iPrefix + iRelName; }

RelName sPrefixErased(const RelName& iPrefix, const RelName& iRelName)
	{
	if (iRelName.substr(0, iPrefix.size()) == iPrefix)
		return iRelName.substr(iPrefix.size(), RelName::npos);
	return iRelName;
	}

RelName sRenamed(const Rename& iRename, const RelName& iRelName)
	{
	Rename::const_iterator iter = iRename.find(iRelName);
	if (iRename.end() == iter)
		return iRelName;
	return iter->second;
	}

bool sHasPrefix(const RelName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return true;

	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		{
		if (!sHasPrefix(iPrefix, *i))
			return false;
		}

	return true;
	}

RelHead sPrefixInserted(const RelName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;

	RelHead result;
	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		result.insert(sPrefixInserted(iPrefix, *i));

	return result;
	}

RelHead sPrefixErased(const RelName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;

	RelHead result;
	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		result.insert(sPrefixErased(iPrefix, *i));

	return result;
	}

RelHead sRenamed(const Rename& iRename, const RelHead& iRelHead)
	{
	if (iRename.empty())
		return iRelHead;

	RelHead result;
	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		result.insert(sRenamed(iRename, *i));

	return result;
	}

} // namespace ZRA
} // namespace ZooLib
