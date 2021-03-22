// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/PseudoMap.h"

#include "zoolib/Default.h"
#include "zoolib/Util_STL_map.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - PseudoMap

PseudoMap::PseudoMap(const std::map<string8,size_t>* iBindings, const Val_DB* iVals)
:	fBindings(iBindings)
,	fVals(iVals)
	{}

const Val_DB* PseudoMap::PGet(const string8& iName) const
	{
	if (ZQ<size_t> theOffsetQ = Util_STL::sQGet(*fBindings, iName))
		return fVals + *theOffsetQ;
	return nullptr;
	}

const ZQ<Val_DB> PseudoMap::QGet(const string8& iName) const
	{
	if (const Val_DB* theVal = this->PGet(iName))
		return *theVal;
	return null;
	}

const Val_DB& PseudoMap::DGet(const Val_DB& iDefault, const string8& iName) const
	{
	if (const Val_DB* theVal = this->PGet(iName))
		return *theVal;
	return iDefault;
	}

const Val_DB& PseudoMap::Get(const string8& iName) const
	{
	if (const Val_DB* theVal = this->PGet(iName))
		return *theVal;
	return sDefault<Val_DB>();
	}

Map_ZZ PseudoMap::AsMap() const
	{
	Map_ZZ result;
	for (std::map<string8,size_t>::const_iterator ii = fBindings->begin(), end = fBindings->end();
		ii != end; ++ii)
		{ result.Set(ii->first, fVals[ii->second].As<Val_ZZ>()); }

	return result;
	}

// =================================================================================================
#pragma mark - PseudoMap_RelHead

PseudoMap_RelHead::PseudoMap_RelHead(const RelHead& iRH, const Val_DB* iVals)
:	PseudoMap(&fBindings_Storage, iVals)
	{
	sBuildBindings(iRH, fBindings_Storage);
	}

// =================================================================================================
#pragma mark - sBuildBindings

void sBuildBindings(const RelHead& iRH, std::map<string8,size_t>& oResult)
	{
	size_t index = 0;
	for (RelHead::const_iterator ii = iRH.begin(), end = iRH.end(); ii != end; ++ii, ++index)
		oResult.insert(std::pair<string8,size_t>(*ii, index));
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
