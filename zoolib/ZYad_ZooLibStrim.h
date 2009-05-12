/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZYad_ZooLibStrim__
#define __ZYad_ZooLibStrim__ 1
#include "zconfig.h"

#include "zoolib/ZStreamR_HexStrim.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_ZooLibStrim

class ZYadParseException_ZooLibStrim : public ZYadParseException_Std
	{
public:
	ZYadParseException_ZooLibStrim(const std::string& iWhat);
	ZYadParseException_ZooLibStrim(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_ZooLibStrim

class ZYadStreamR_ZooLibStrim
:	public ZYadR_Std,
	public ZYadStreamR
	{
public:
	ZYadStreamR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter);

// From ZYadR_Std
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	const ZStrimU& fStrimU;
	bool fReadDelimiter;
	ZStreamR_HexStrim fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_ZooLibStrim_Apos

class ZYadStrimR_ZooLibStrim_Apos
:	public ZYadR_Std,
	public ZYadStrimR
	{
public:
	ZYadStrimR_ZooLibStrim_Apos(const ZStrimU& iStrimU);

// From ZYadR_Std
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	const ZStrimU& fStrimU;
	ZStrimR_Escaped fStrimR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_ZooLibStrim_Quote

class ZYadStrimR_ZooLibStrim_Quote
:	public ZYadR_Std,
	public ZYadStrimR,
	private ZStrimR
	{
public:
	ZYadStrimR_ZooLibStrim_Quote(const ZStrimU& iStrimU);

// From ZYadR_Std
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

private:
	const ZStrimU& fStrimU;
	ZStrimR_Boundary fStrimR_Boundary;
	size_t fQuotesSeen;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStrim

class ZYadListR_ZooLibStrim : public ZYadListR_Std
	{
public:
	ZYadListR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	const ZStrimU& fStrimU;
	bool fReadDelimiter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ZooLibStrim

class ZYadMapR_ZooLibStrim : public ZYadMapR_Std
	{
public:
	ZYadMapR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

private:
	const ZStrimU& fStrimU;
	bool fReadDelimiter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLibStrim

namespace ZYad_ZooLibStrim {

bool sRead_Identifier(
	const ZStrimU& iStrimU, std::string* oStringLC, std::string* oStringExact);

ZRef<ZYadR> sMakeYadR(const ZStrimU& iStrimU);

void sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR);

void sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions);

void sWrite_PropName(const ZStrimW& iStrimW, const std::string& iPropName);

} // namespace ZYad_ZooLibStrim

NAMESPACE_ZOOLIB_END

#endif // __ZYad_ZooLibStrim__
