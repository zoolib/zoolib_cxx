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

#ifndef __ZooLib_ChanW_Bin_More_h__
#define __ZooLib_ChanW_Bin_More_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_Bin.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -

/**
Write the bytes pointed to by \a iString, up to but not including the terminating zero byte.
Return true if completely written
*/
bool sQWrite(const ChanW_Bin& iChanW, const char* iString);

/**
Write the bytes pointed to by \a iString, up to but not including the terminating zero byte.
May invoke sThrow_ExhaustedW.
*/
void sEWrite(const ChanW_Bin& iChanW, const char* iString);

/**
Write the bytes contained in \a iString, with no terminating zero byte.
Return true if completely written
*/
bool sQWrite(const ChanW_Bin& iChanW, const std::string& iString);

/**
Write the bytes contained in \a iString, with no terminating zero byte.
May invoke sThrow_ExhaustedW.
*/
void sEWrite(const ChanW_Bin& iChanW, const std::string& iString);

/**
Write the bytes pointed to by \a iString, up to but not including the terminating zero byte.
Standard printf-style parameter substitution is applied to the string before writing.
Return true if completely written
*/
bool sQWritef(const ChanW_Bin& iChanW, const char* iString, ...);

/**
Write the bytes pointed to by \a iString, up to but not including the terminating zero byte.
Standard printf-style parameter substitution is applied to the string before writing.
May invoke sThrow_ExhaustedW.
*/
void sEWritef(const ChanW_Bin& iChanW, const char* iString, ...);

// =================================================================================================
#pragma mark -

const ChanW_Bin& operator<<(const ChanW_Bin& w, const char* iString);

const ChanW_Bin& operator<<(const ChanW_Bin& w, char* iString);

const ChanW_Bin& operator<<(const ChanW_Bin& w, const std::string& iString);

bool sQWriteCount(const ChanW_Bin& w, uint64 iValue);

void sEWriteCount(const ChanW_Bin& w, uint64 iValue);

void sEWriteCountPrefixedString(const ChanW_Bin& w, const std::string& iString);

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_More_h__
