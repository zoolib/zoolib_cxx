// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_Chan_UTF_Operators.h"

#include <inttypes.h> // For PRIuLEAST16 and PRIuLEAST32

#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan.h" // For sCopyAll
#include "zoolib/Util_Chan_UTF.h"

// =================================================================================================
#pragma mark - Util_Chan_UTF_Operators

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string32& iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF32* iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF32* iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string16& iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF16* iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF16* iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const string8& iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const UTF8* iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, UTF8* iString)
	{
	sEWrite(w, iString);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ChanR_UTF& r)
	{
	sCopyAll<UTF32>(r, w);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, bool iVal)
	{
	sEWrite(w, iVal ? "true" : "false");
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, char iVal)
	{
	sEWritef(w, "%c", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned char iVal)
	{
	sEWritef(w, "%u", (unsigned int)iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, signed char iVal)
	{
	sEWritef(w, "%d", int(iVal));
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, __wchar_t iVal)
	{
	sEWritef(w, "%lc", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, short iVal)
	{
	sEWritef(w, "%d", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned short iVal)
	{
	sEWritef(w, "%u", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, int iVal)
	{
	sEWritef(w, "%d", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned int iVal)
	{
	sEWritef(w, "%u", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, long iVal)
	{
	sEWritef(w, "%ld", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, unsigned long iVal)
	{
	sEWritef(w, "%lu", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, __int64 iVal)
	{
	sEWritef(w, "%lld", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, __uint64 iVal)
	{
	sEWritef(w, "%llu", iVal);
	return w;
	}

#if ZCONFIG_CPP >= 2011
const ChanW_UTF& operator<<(const ChanW_UTF& w, char16_t iVal)
	{
	// Would like to use PRIuLEAST16, but the compiler complains.
	sEWritef(w, "%u", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, char32_t iVal)
	{
	sEWritef(w, "%u", iVal);
// 	sEWritef(w, "%" PRIuLEAST32, iVal);
	return w;
	}
#endif // ZCONFIG_CPP >= 2011

const ChanW_UTF& operator<<(const ChanW_UTF& w, float iVal)
	{
	Util_Chan::sWriteExact(w, iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, double iVal)
	{
	Util_Chan::sWriteExact(w, iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, long double iVal)
	{
	Util_Chan::sWriteExact(w, iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, const void* iVal)
	{
	sEWritef(w, "%p", iVal);
	return w;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& w, void* iVal)
	{
	sEWritef(w, "%p", iVal);
	return w;
	}

} // namespace Util_Chan_UTF_Operators
} // namespace ZooLib
