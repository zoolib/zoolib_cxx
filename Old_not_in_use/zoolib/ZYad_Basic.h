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

#ifndef __ZYad_Basic_h__
#define __ZYad_Basic_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrimmer.h"
#include "zoolib/ZYad_Std.h"

namespace ZooLib {
namespace ZYad_Basic {

// =================================================================================================
#pragma mark -
#pragma mark ParseException

class ParseException : public ZYadParseException_Std
	{
public:
	ParseException(const std::string& iWhat);
	ParseException(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZYad_Basic

struct Options
	{
	Options(UTF32 iNameFromValue, UTF32 iEntryFromEntry);

	UTF32 fSeparator_NameFromValue;
	UTF32 fSeparator_EntryFromEntry;

	// Perhaps should make sure we differentiate terminator and separator?
	};

ZRef<ZYadMapR> sYadR(const Options& iOptions, ZRef<ZStrimmerU> iStrimmerU);

} // namespace ZYad_Basic
} // namespace ZooLib

#endif // __ZYad_Basic_h__
