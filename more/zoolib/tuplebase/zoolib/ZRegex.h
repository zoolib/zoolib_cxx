/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZRegex__
#define __ZRegex__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Regex
#	define ZCONFIG_API_Avail__Regex ZCONFIG_SPI_Enabled(boost)
#endif

#ifndef ZCONFIG_API_Desired__Regex
#	define ZCONFIG_API_Desired__Regex 1
#endif

#if ZCONFIG_API_Enabled(Regex)

#include "zoolib/ZRef.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZRegex

class ZRegexRep;

class ZRegex
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZRegex, operator_bool_generator_type, operator_bool_type);
public:
	ZRegex();
	ZRegex(const ZRegex& iOther);
	ZRegex(const std::string& iRegex, int iStrength);
	ZRegex& operator=(const ZRegex& iOther);
	~ZRegex();

	operator operator_bool_type() const;

	bool Matches(const std::string& iTarget) const;

private:
	ZRef<ZRegexRep> fRep;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Regex)

#endif // __ZRegex__
