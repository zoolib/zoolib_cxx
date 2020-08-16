// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Util_CF_Context_h__
#define __ZooLib_Apple_Util_CF_Context_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZP.h"

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
#pragma mark - Util_CF::Context

template <class T>
class Context
	{
public:
	Context()
	:	fT(T())
		{}

	Context(const Context& iOther)
	:	fT(iOther.fT)
		{
		if (fT.retain)
			fT.retain(fT.info);
		}

	~Context()
		{
		if (fT.release)
			fT.release(fT.info);
		}

	Context& operator=(const Context& iOther)
		{
		if (iOther.fT.retain)
			iOther.fT.retain(iOther.fT.info);

		if (fT.release)
			fT.release(fT.info);

		fT = iOther.fT;

		return *this;
		}

	template <class O>
	Context(const ZP<O>& iRef)
	:	fT(T())
		{
		fT.info = iRef.Get();
		fT.retain = (CFAllocatorRetainCallBack)&ZP<O>::sCFAllocatorRetain;
		fT.release = (CFAllocatorReleaseCallBack)&ZP<O>::sCFAllocatorRelease;
		fT.retain(fT.info);
		}

	operator const T*() const
		{ return &fT; }

	operator T*()
		{ return &fT; }

	const T* IParam() const
		{ return &fT; }

	T* IParam()
		{ return &fT; }

private:
	T fT;
	};

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Util_CF_Context_h__
