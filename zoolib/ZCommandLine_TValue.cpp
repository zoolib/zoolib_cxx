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

#include "zoolib/ZCommandLine_TValue.h"
#include "zoolib/ZStream_Memory.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

#include <stdio.h>

using std::exception;
using std::string;

namespace ZooLib {

// TODO. Paramaterize the value type, so we can use CFType or
// ZVal_Any, and also parameterize the parser.

// =================================================================================================
// MARK: - ZCommandLine::TValue

ZCommandLine::TValue::TValue(const string& iName, const string& iDescription, EFlags iFlags)
:	Opt(iName, iDescription, iFlags, false)
	{}

ZCommandLine::TValue::TValue
	(const string& iName, const string& iDescription, const ZTValue& iDefault)
:	Opt(iName, iDescription, eOptional, true),
	fDefault(iDefault)
	{}

ZCommandLine::TValue::TValue(const string& iName, const string& iDescription)
:	Opt(iName, iDescription, eOptional, false)
	{}

void ZCommandLine::TValue::WriteDefault(const ZStrimW& s)
	{
	s << fDefault;
	}

bool ZCommandLine::TValue::Parse(const char* iLexeme, const ZStrimW* iStrimErrors)
	{
	try
		{
		if (not ZUtil_Strim_Tuple::sFromStrim
			(ZStrimU_Unreader
			(ZStrimR_StreamUTF8
			(ZStreamRPos_Memory(iLexeme, strlen(iLexeme)))),
			fValue))
			{
			if (iStrimErrors)
				*iStrimErrors << "Could not parse parameter to option " << fName << "\n";
			return false;
			}
		}
	catch (exception& ex)
		{
		if (iStrimErrors)
			{
			*iStrimErrors << "Could not parse parameter to option " << fName
				<< ", caught exception \"" << ex.what() << "\"\n";
			}
		return false;
		}
	fHasValue = true;
	return true;
	}

const ZTValue& ZCommandLine::TValue::operator()() const
	{
	if (fHasValue)
		return fValue;
	return fDefault;
	}

} // namespace ZooLib
