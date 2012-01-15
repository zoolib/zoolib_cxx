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

#ifndef __ZCommandLine_TValue_h__
#define __ZCommandLine_TValue_h__ 1
#include "zconfig.h"

#include "zoolib/ZCommandLine.h"
#include "zoolib/ZTuple.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZCommandLine_TValue

class ZCommandLine::TValue : public ZCommandLine::Opt
	{
public:
	TValue(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	TValue(const std::string& iName, const std::string& iDescription, const ZTValue& iDefault);
	TValue(const std::string& iName, const std::string& iDescription);

	virtual void WriteDefault(const ZStrimW& s);
	virtual bool Parse(const char* iLexeme, const ZStrimW* iStrimErrors);

	const ZTValue& operator()() const;

private:
	ZTValue fDefault;
	ZTValue fValue;
	};

} // namespace ZooLib

#endif // __ZCommandLine_TValue_h__
