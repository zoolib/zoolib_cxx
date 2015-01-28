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

#ifndef __ZooLib_Yad_Std_h__
#define __ZooLib_Yad_Std_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Yad.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark YadParseException_Std

class YadParseException_Std : public YadParseException
	{
public:
	YadParseException_Std(const std::string& iWhat);
	YadParseException_Std(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark YadSeqR_Std

class YadSeqR_Std
:	public YadSeqR
	{
public:
	YadSeqR_Std();
	YadSeqR_Std(bool iFinished);

// From YadR
	virtual void Finish();

// From YadSeqR
	virtual ZRef<YadR> ReadInc();

// Our protocol
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR) = 0;

private:
	bool fStarted;
	ZRef<YadR> fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark YadMapR_Std

class YadMapR_Std
:	public YadMapR
	{
public:
	YadMapR_Std();
	YadMapR_Std(bool iFinished);

// From YadR
	virtual void Finish();

// From YadMapR
	virtual ZRef<YadR> ReadInc(Name& oName);

// Our protocol
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR) = 0;

private:
	bool fStarted;
	ZRef<YadR> fValue;
	};

} // namespace ZooLib

#endif // __ZooLib_Yad_Std_h__
