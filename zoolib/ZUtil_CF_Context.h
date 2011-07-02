/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZUtil_CF_Context__
#define __ZUtil_CF_Context__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZRef.h"

namespace ZooLib {
namespace ZUtil_CF {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CF::Context

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
	Context(const ZRef<O>& iRef)
	:	fT(T())
		{
		fT.info = iRef.Get();
		fT.retain = (CFAllocatorRetainCallBack)&ZRef<O>::sCFRetain;
		fT.release = (CFAllocatorReleaseCallBack)&ZRef<O>::sCFRelease;
		fT.retain(fT.info);
		}

	const T* IParam() const
		{ return &fT; }

	T* IParam()
		{ return &fT; }

private:
	T fT;
	};

} // namespace ZUtil_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZUtil_CF_Context__
