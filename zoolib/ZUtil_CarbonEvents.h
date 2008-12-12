/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZUtil_CarbonEvents__
#define __ZUtil_CarbonEvents__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CarbonEvents

#if ZCONFIG_SPI_Enabled(Carbon)

#include ZMACINCLUDE(Carbon,Carbon.h)

namespace ZUtil_CarbonEvents {

bool sGetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, void* iBuffer);
  
size_t sGetParamLength(EventRef iEventRef, EventParamName iName, EventParamType iType);

template <typename T>
bool sGetParam_T(EventRef iEventRef, EventParamName iName, EventParamType iType, T& oT)
	{
	return sGetParam(iEventRef, iName, iType, sizeof(oT), &oT);
	}

template <typename T>
T sGetParam_T(EventRef iEventRef, EventParamName iName, EventParamType iType)
	{
	T theT;
	if (sGetParam(iEventRef, iName, iType, sizeof(theT), &theT))
		return theT;
	return T();
	}

} // namespace ZUtil_CarbonEvents

#endif // ZCONFIG_SPI_Enabled(Carbon)

#endif // __ZUtil_CarbonEvents__
