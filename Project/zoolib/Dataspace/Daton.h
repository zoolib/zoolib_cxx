// Copyright (c) 2016 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Daton_h__
#define __ZooLib_Dataspace_Daton_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/StdInt.h"
#include "zoolib/Util_Relops.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Daton

//! A trivial implementation till I get the signing stuff figured out

class Daton
	{
public:
	Daton();
	Daton(const Daton& iOther);
	~Daton();
	Daton& operator=(const Daton& iOther);

	Daton(Data_ZZ iData);

	bool operator==(const Daton& iOther) const;
	bool operator<(const Daton& iOther) const;

	Data_ZZ GetData() const;

private:
	Data_ZZ fData;
	};

typedef Callable<int64(const Daton* iAsserted, size_t iAssertedCount,
	const Daton* iRetracted, size_t iRetractedCount)> Callable_DatonUpdate;

} // namespace Dataspace

template <> struct RelopsTraits_HasEQ<Dataspace::Daton> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Dataspace::Daton> : public RelopsTraits_Has {};

} // namespace ZooLib

#endif // __ZooLib_Dataspace_Daton_h__
