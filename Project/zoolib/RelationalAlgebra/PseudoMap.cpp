/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/RelationalAlgebra/PseudoMap.h"

#include "zoolib/Default.h"
#include "zoolib/Util_STL_map.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark -
#pragma mark PseudoMap

PseudoMap::PseudoMap(const std::map<string8,size_t>* iBindings, const Val_Any* iVals)
:	fBindings(iBindings)
,	fVals(iVals)
	{}

const Val_Any* PseudoMap::PGet(const string8& iName) const
	{
	if (ZQ<size_t> theOffsetQ = Util_STL::sQGet(*fBindings, iName))
		return fVals + *theOffsetQ;
	return nullptr;
	}

const ZQ<Val_Any> PseudoMap::QGet(const string8& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const Val_Any& PseudoMap::DGet(const Val_Any& iDefault, const string8& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

const Val_Any& PseudoMap::Get(const string8& iName) const
	{
	if (const Val_Any* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<Val_Any>();
	}

Map_Any PseudoMap::AsMap() const
	{
	Map_Any result;
	for (std::map<string8,size_t>::const_iterator ii = fBindings->begin(), end = fBindings->end();
		ii != end; ++ii)
		{ result.Set(ii->first, fVals[ii->second]); }

	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark PseudoMap_RelHead

PseudoMap_RelHead::PseudoMap_RelHead(const RelHead& iRH, const Val_Any* iVals)
:	PseudoMap(&fBindings_Storage, iVals)
	{
	size_t index = 0;
	for (RelHead::const_iterator ii = iRH.begin(), end = iRH.end(); ii != end; ++ii, ++index)
		fBindings_Storage.insert(std::pair<string8,size_t>(*ii, index));
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
