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

#include "zoolib/Yad_Std.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark YadParseException_Std

YadParseException_Std::YadParseException_Std(const string& iWhat)
:	YadParseException(iWhat)
	{}

YadParseException_Std::YadParseException_Std(const char* iWhat)
:	YadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark YadSeqR_Std

YadSeqR_Std::YadSeqR_Std()
:	fStarted(false)
	{}

YadSeqR_Std::YadSeqR_Std(bool iFinished)
:	fStarted(iFinished)
	{}

void YadSeqR_Std::Finish()
	{ this->SkipAll(); }

ZRef<YadR> YadSeqR_Std::ReadInc()
	{
	if (not sGetSet(fStarted, true))
		{
		this->Imp_ReadInc(true, fValue);
		}
	else if (fValue)
		{
		fValue->Finish();
		fValue.Clear();
		this->Imp_ReadInc(false, fValue);
		}

	return fValue;
	}

// =================================================================================================
#pragma mark -
#pragma mark YadMapR_Std

YadMapR_Std::YadMapR_Std()
:	fStarted(false)
	{}

YadMapR_Std::YadMapR_Std(bool iFinished)
:	fStarted(iFinished)
	{}

void YadMapR_Std::Finish()
	{ this->SkipAll(); }

ZRef<YadR> YadMapR_Std::ReadInc(Name& oName)
	{
	oName.Clear();
	if (not sGetSet(fStarted, true))
		{
		this->Imp_ReadInc(true, oName, fValue);
		}
	else if (fValue)
		{
		fValue->Finish();
		fValue.Clear();
		this->Imp_ReadInc(false, oName, fValue);
		}

	return fValue;
	}

} // namespace ZooLib
