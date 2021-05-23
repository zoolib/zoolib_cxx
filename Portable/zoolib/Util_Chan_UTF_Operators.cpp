// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_Chan_UTF_Operators.h"

#include <inttypes.h>
#include <string.h> // For strlen

#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan.h" // For sCopyAll
#include "zoolib/Util_Chan_UTF.h"

// =================================================================================================
#pragma mark - Util_Chan_UTF_Operators

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const string32& iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const UTF32* iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, UTF32* iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const string16& iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const UTF16* iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, UTF16* iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const string8& iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const UTF8* iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, UTF8* iString)
	{
	sEWrite(ww, iString);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ChanR_UTF& rr)
	{
	sECopyAll<UTF32>(rr, ww);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, bool iVal)
	{
	sEWrite(ww, iVal ? "true" : "false");
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, char iVal)
	{
	sEWritef(ww, "%c", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned char iVal)
	{
	sEWritef(ww, "%u", (unsigned int)iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, signed char iVal)
	{
	sEWritef(ww, "%d", int(iVal));
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, __wchar_t iVal)
	{
	sEWritef(ww, "%lc", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, short iVal)
	{
	sEWritef(ww, "%d", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned short iVal)
	{
	sEWritef(ww, "%u", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, int iVal)
	{
	sEWritef(ww, "%d", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned int iVal)
	{
	sEWritef(ww, "%u", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, long iVal)
	{
	sEWritef(ww, "%ld", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned long iVal)
	{
	sEWritef(ww, "%lu", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, long long iVal)
	{
	sEWritef(ww, "%lld", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, unsigned long long iVal)
	{
	sEWritef(ww, "%llu", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, char16_t iVal)
	{
	// clang on mac warns without the cast.
	sEWritef(ww, "%" PRIuLEAST16, std::uint_least16_t(iVal));
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, char32_t iVal)
	{
 	sEWritef(ww, "%" PRIuLEAST32, iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, float iVal)
	{
	Util_Chan::sWrite_Exact(ww, iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, double iVal)
	{
	Util_Chan::sWrite_Exact(ww, iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, long double iVal)
	{
	Util_Chan::sWrite_Exact(ww, iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const void* iVal)
	{
	sEWritef(ww, "%p", iVal);
	return ww;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& ww, void* iVal)
	{
	sEWritef(ww, "%p", iVal);
	return ww;
	}

} // namespace Util_Chan_UTF_Operators
} // namespace ZooLib
