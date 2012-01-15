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

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - ZYadParseException_Std

ZYadParseException_Std::ZYadParseException_Std(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ZYadParseException_Std::ZYadParseException_Std(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
// MARK: - ZYadSeqR_Std

ZYadSeqR_Std::ZYadSeqR_Std()
:	fStarted(false)
	{}

ZYadSeqR_Std::ZYadSeqR_Std(bool iFinished)
:	fStarted(iFinished)
	{}

void ZYadSeqR_Std::Finish()
	{ this->SkipAll(); }

ZRef<ZYadR> ZYadSeqR_Std::ReadInc()
	{
	if (!fStarted)
		{
		fStarted = true;
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
// MARK: - ZYadMapR_Std

ZYadMapR_Std::ZYadMapR_Std()
:	fStarted(false)
	{}

ZYadMapR_Std::ZYadMapR_Std(bool iFinished)
:	fStarted(iFinished)
	{}

void ZYadMapR_Std::Finish()
	{ this->SkipAll(); }

ZRef<ZYadR> ZYadMapR_Std::ReadInc(std::string& oName)
	{
	oName.clear();
	if (!fStarted)
		{
		fStarted = true;
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
