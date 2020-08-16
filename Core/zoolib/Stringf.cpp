// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Stringf.h"

#if ZCONFIG(Compiler, MSVC)
	#define vsnprintf _vsnprintf
#else
	#include <stdio.h> // For vsnprintf
#endif

#ifndef va_copy
	#if defined __va_copy
		#define va_copy __va_copy
	#else
		#define va_copy(dest, src) dest = src
	#endif
#endif

namespace ZooLib {

// =================================================================================================

std::string sStringf(std::string iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const std::string result = sStringv(iString.c_str(), args);
	va_end(args);

	return result;
	}

std::string sStringf(const char* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	const std::string result = sStringv(iString, args);
	va_end(args);

	return result;
	}

std::string sStringv(const char* iString, std::va_list iArgs)
	{
	std::string result(512, ' ');
	while (true)
		{
		va_list localArgs;
		va_copy(localArgs, iArgs);

		int count = vsnprintf(const_cast<char*>(result.data()), result.size(), iString, localArgs);

		va_end(localArgs);

		if (count < 0)
			{
			// Handle -ve result from glibc prior to version 2.1 by growing the string.
			result.resize(result.size() * 2);
			}
		else if (size_t(count) > result.size())
			{
			// Handle C99 standard, where count indicates how much space would have been needed.
			result.resize(count);
			}
		else
			{
			// The string fitted
			result.resize(count);
			return result;
			}
		}
	}

} // namespace ZooLib
