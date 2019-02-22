/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#ifndef __ZooLib_ParseException_h__
#define __ZooLib_ParseException_h__ 1
#include "zconfig.h"

#include <stdexcept>
#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - ParseException

class ParseException : public std::runtime_error
	{
public:
	ParseException(const std::string& iWhat)
	:	runtime_error(iWhat)
		{}

	ParseException(const char* iWhat)
	:	runtime_error(iWhat)
		{}
	};

ZMACRO_NoReturn_Pre
inline bool sThrow_ParseException(const std::string& iMessage);
ZMACRO_NoReturn_Post

inline bool sThrow_ParseException(const std::string& iMessage)
	{ throw ParseException(iMessage); }

} // namespace ZooLib

#endif // __ZooLib_ParseException_h__
