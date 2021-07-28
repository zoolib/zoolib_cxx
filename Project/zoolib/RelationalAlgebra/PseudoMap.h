// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_PseudoMap_h__
#define __ZooLib_RelationalAlgebra_PseudoMap_h__ 1
#include "zconfig.h"

#include "zoolib/Val_DB.h"
#include "zoolib/Val_ZZ.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - PseudoMap

class PseudoMap
	{
public:
	PseudoMap(const std::map<string8,size_t>* iBindings, const Val_DB* iVals);

	const Val_DB* PGet(const string8& iName) const;
	const ZQ<Val_DB> QGet(const string8& iName) const;
	const Val_DB& Get(const string8& iName) const;

	template <class S>
	const S* PGet(const string8& iName) const
		{
		if (const Val_DB* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{
		if (const Val_DB* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	S Get(const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return sDefault<S>();
		}

	Map_ZZ AsMap() const;

protected:
	const std::map<string8,size_t>* fBindings;
	const Val_DB* fVals;
	};

// -----

inline const Val_DB* sPGet(const PseudoMap& iMap, const string8& iName)
	{ return iMap.PGet(iName); }

inline const ZQ<Val_DB> sQGet(const PseudoMap& iMap, const string8& iName)
	{ return iMap.QGet(iName); }

inline const Val_DB& sGet(const PseudoMap& iMap, const string8& iName)
	{ return iMap.Get(iName); }

// =================================================================================================
#pragma mark - PseudoMap_RelHead

class PseudoMap_RelHead
:	public PseudoMap
	{
public:
	PseudoMap_RelHead(const RelHead& iRH, const Val_DB* iVals);

private:
	std::map<string8,size_t> fBindings_Storage;
	};

// =================================================================================================
#pragma mark - sBuildBindings

void sBuildBindings(const RelHead& iRH, std::map<string8,size_t>& oResult);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_PseudoMap_h__
