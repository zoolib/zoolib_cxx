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
:	public T
	{
public:
	Context()
		{
		T::version = 0;
		T::info = nullptr;
		T::retain = nullptr;
		T::release = nullptr;
		T::copyDescription = nullptr;
		}

	Context(const Context& iOther)
		{
		T::version = iOther.version;
		T::info = iOther.info;
		T::retain = iOther.retain;
		T::release = iOther.release;
		T::copyDescription = iOther.copyDescription;
		}

	~Context()
		{
		T::release(T::info);
		}

	Context& operator=(const Context& iOther)
		{
		if (iOther.retain)
			iOther.retain(iOther.info);
		
		if (T::release)
			T::release(T::info);

		T::version = iOther.version;
		T::info = iOther.info;
		T::retain = iOther.retain;
		T::release = iOther.release;
		T::copyDescription = iOther.copyDescription;

		return *this;
		}

	template <class O>
	Context(const ZRef<O>& iRef)
		{
		T::version = 0;
		T::info = iRef.Get();
		T::retain = (CFAllocatorRetainCallBack)&ZRef<O>::sCFRetain;
		T::release = (CFAllocatorReleaseCallBack)&ZRef<O>::sCFRelease;
		T::copyDescription = nullptr;
		T::retain(T::info);
		}

	const T* IParam() const
		{ return this; }

	T* IParam()
		{ return this; }
	};

} // namespace ZUtil_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZUtil_CF_Context__
