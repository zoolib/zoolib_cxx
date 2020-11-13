// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

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

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const char* iString);

const ChanW_Bin& operator<<(const ChanW_Bin& ww, char* iString);

const ChanW_Bin& operator<<(const ChanW_Bin& ww, const std::string& iString);

bool sQWriteCount(const ChanW_Bin& ww, uint64 iValue);

void sEWriteCount(const ChanW_Bin& ww, uint64 iValue);

void sEWriteCountPrefixedString(const ChanW_Bin& ww, const std::string& iString);

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_More_h__
