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

#ifndef __ZYad_Std__
#define __ZYad_Std__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZYad.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Std

class ZYadParseException_Std : public ZYadParseException
	{
public:
	ZYadParseException_Std(const std::string& iWhat);
	ZYadParseException_Std(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Std

typedef ZYadPrimR_Any ZYadPrimR_Std;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_Std

class ZYadSeqR_Std
:	public ZYadSeqR
	{
public:
	ZYadSeqR_Std();
	ZYadSeqR_Std(bool iFinished);

// From ZYadR
	virtual void Finish();

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc();

// Our protocol
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR) = 0;

private:
	bool fStarted;
	ZRef<ZYadR> fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Std

class ZYadMapR_Std
:	public ZYadMapR
	{
public:
	ZYadMapR_Std();
	ZYadMapR_Std(bool iFinished);

// From ZYadR
	virtual void Finish();

// From ZYadMapR
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// Our protocol
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR) = 0;

private:
	bool fStarted;
	ZRef<ZYadR> fValue;
	};

} // namespace ZooLib

#endif // __ZYad_Std__
