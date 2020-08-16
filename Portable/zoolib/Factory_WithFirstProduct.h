// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Factory_WithFirstProduct_h__
#define __ZooLib_Factory_WithFirstProduct_h__ 1
#include "zconfig.h"

#include "zoolib/Factory.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Factory_WithFirstProduct

template <class T>
class Factory_WithFirstProduct
:	public Factory<T>
	{
public:
	Factory_WithFirstProduct(const ZP<T>& iFactory, const T& iFirstProduct)
	:	fFirstProductQ(iFirstProduct)
		{}

// From Callable
	virtual ZQ<T> QCall()
		{
		if (ZQ<T> theQ = sQGetClear(fFirstProductQ))
			return theQ;
		return sQCall(fFactory);
		}

private:
	const ZP<T> fFactory;
	ZQ<T> fFirstProductQ;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_WithFirstProduct_h__
