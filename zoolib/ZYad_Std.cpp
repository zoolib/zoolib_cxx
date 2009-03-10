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
#pragma mark * ZYadPrimR_Std

ZYadPrimR_Std::ZYadPrimR_Std(const ZTValue& iTV)
:	ZYadR_TValue(iTV)
	{}

void ZYadPrimR_Std::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_Std

ZYadRawR_Std::ZYadRawR_Std(const ZMemoryBlock& iMB)
:	ZYadRawRPos_MemoryBlock(iMB)
	{}

void ZYadRawR_Std::Finish()
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

bool ZYadListR_Std::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_Std::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		++fPosition;
		}

	return fValue_Prior;
	}

uint64 ZYadListR_Std::GetPosition()
	{ return fPosition; }

void ZYadListR_Std::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (fFinished)
		return;

	this->Imp_Advance(!fStarted, fValue_Current);
	fStarted = true;

	if (!fValue_Current)
		fFinished = true;
	}

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

bool ZYadMapR_Std::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_Std::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		fName.clear();
		}

	return fValue_Prior;
	}

string ZYadMapR_Std::Name()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		return fName;

	return string();
	}

void ZYadMapR_Std::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (fFinished)
		return;

	this->Imp_Advance(!fStarted, fName, fValue_Current);
	fStarted = true;

	if (!fValue_Current)
		fFinished = true;
	}

NAMESPACE_ZOOLIB_END
