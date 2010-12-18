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

#include "zoolib/zra/ZRA_RelHead.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Rename_t

Rename_t sInverted(const Rename_t& iRename)
	{
	Rename_t result;
	for (Rename_t::const_iterator i = iRename.begin(); i != iRename.end(); ++i)
		result[i->second] = i->first;
	return result;
	}

} // namespace ZooLib

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * RelHead

RelName sPrefixAdd(const RelName& iPrefix, const RelName& iRelName)
	{ return iPrefix + iRelName; }

RelName sPrefixRemove(const RelName& iPrefix, const RelName& iRelName)
	{
	if (iRelName.substr(0, iPrefix.size()) == iPrefix)
		return iRelName.substr(iPrefix.size(), RelName::npos);
	return iRelName;
	}

RelHead sPrefixAdd(const RelName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;

	RelHead result;
	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		result.insert(sPrefixAdd(iPrefix, *i));

	return result;
	}

RelHead sPrefixRemove(const RelName& iPrefix, const RelHead& iRelHead)
	{
	if (iPrefix.empty())
		return iRelHead;

	RelHead result;
	for (RelHead::const_iterator i = iRelHead.begin(); i != iRelHead.end(); ++i)
		result.insert(sPrefixRemove(iPrefix, *i));

	return result;
	}

} // namespace ZRA
} // namespace ZooLib
