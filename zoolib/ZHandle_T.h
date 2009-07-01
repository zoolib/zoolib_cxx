/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZHandle_T__
#define __ZHandle_T__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Carbon)

#include "zoolib/ZTypes.h" // For Adopt_T

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacMemory.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle_T

template <class T = Handle>
class ZHandle_T
	{
public:
	ZHandle_T()
	:	fHandle(0)
		{}

	ZHandle_T(const ZHandle_T& iOther)
	:	fHandle(iOther.fHandle)
		{ ::HandToHand((Handle*)fHandle); }

	~ZHandle_T()
		{
		if (fHandle)
			::DisposeHandle((Handle)fHandle);
		}
	
	ZHandle_T& operator=(const ZHandle_T& iOther)
		{
		if (this != iOther)
			{
			::DisposeHandle((Handle)fHandle);
			fHandle = iOther.fHandle;
			::HandToHand((Handle*)fHandle);
			}
		return *this;
		}

	ZHandle_T(T iOther)
	:	fHandle(iOther)
		{ ::HandToHand((Handle*)fHandle); }

	ZHandle_T(Adopt_t<T> iOther)
	:	fHandle(iOther)
		{}

	ZHandle_T& operator=(T iOther)
		{
		::DisposeHandle((Handle)fHandle);
		fHandle = iOther;
		::HandToHand((Handle*)fHandle);
		return *this;
		}

	ZHandle_T& operator=(Adopt_t<T> iOther)
		{
		::DisposeHandle((Handle)fHandle);
		fHandle = iOther;
		return *this;
		}

	T Get() const
		{
		return fHandle;
		}

	T Orphan()
		{
		Handle result = fHandle;
		fHandle = 0;
		return result;
		}

private:
	T fHandle;
	};


NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Carbon)

#endif // __ZHandle_T__
