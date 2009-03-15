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

#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void sThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_Std(iMessage);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Std

ZYadParseException_Std::ZYadParseException_Std(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ZYadParseException_Std::ZYadParseException_Std(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_Std

ZYadListR_Std::ZYadListR_Std()
:	fPosition(0),
	fFinished(false),
	fStarted(false)
	{}

ZYadListR_Std::ZYadListR_Std(bool iFinished)
:	fPosition(0),
	fFinished(iFinished),
	fStarted(false)
	{}

void ZYadListR_Std::Finish()
	{
	this->SkipAll();
	}

ZRef<ZYadR> ZYadListR_Std::ReadInc()
	{
	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (!fFinished)
		{
		this->Imp_ReadInc(!fStarted, fValue_Prior);
		fStarted = true;

		if (fValue_Prior)
			++fPosition;
		else
			fFinished = true;
		}

	return fValue_Prior;
	}

uint64 ZYadListR_Std::GetPosition()
	{ return fPosition; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Std

ZYadMapR_Std::ZYadMapR_Std()
:	fFinished(false),
	fStarted(false)
	{}
	
ZYadMapR_Std::ZYadMapR_Std(bool iFinished)
:	fFinished(iFinished),
	fStarted(false)
	{}

void ZYadMapR_Std::Finish()
	{
	this->SkipAll();
	}

ZRef<ZYadR> ZYadMapR_Std::ReadInc(std::string& oName)
	{
	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (!fFinished)
		{
		string curName;

		this->Imp_ReadInc(!fStarted, curName, fValue_Prior);
		fStarted = true;

		if (fValue_Prior)
			oName = curName;
		else
			fFinished = true;
		}

	return fValue_Prior;
	}

NAMESPACE_ZOOLIB_END
