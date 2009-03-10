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

#include "zoolib/ZStream_HexStrim.h"
#include "zoolib/ZStrim.h"
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
#pragma mark * ZYadRawR_ZooLibStrim

class ZYadRawR_ZooLibStrim
:	public ZYadRawR,
	public ZYadR_Std
	{
public:
	ZYadRawR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter);

// From ZYadR_Std
	virtual void Finish();

// From ZStreamerR via ZYadRawR
	const ZStreamR& GetStreamR();

private:
	const ZStrimU& fStrimU;
	bool fReadDelimiter;
	ZStreamR_HexStrim fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_ZooLibStrim

class ZYadListR_ZooLibStrim : public ZYadListR_Std
	{
public:
	ZYadListR_ZooLibStrim(const ZStrimU& iStrimU, bool iReadDelimiter);

// From ZYadListR_Std
	virtual void Imp_Advance(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

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
	virtual void Imp_Advance(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

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

void sToStrim_List(const ZStrimW& s, ZRef<ZYadListR> iYadListR);

void sToStrim_List(const ZStrimW& s, ZRef<ZYadListR> iYadListR,
	size_t iInitialIndent, const ZYadOptions& iOptions);

void sToStrim_Map(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR);

void sToStrim_Map(const ZStrimW& s, ZRef<ZYadMapR> iYadMapR,
	size_t iInitialIndent, const ZYadOptions& iOptions);

void sToStrim_Raw(const ZStrimW& s, ZRef<ZYadRawR> iYadRawR);

void sToStrim_Raw(const ZStrimW& s, ZRef<ZYadRawR> iYadRawR,
	size_t iInitialIndent, const ZYadOptions& iOptions);

void sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR);

void sToStrim(const ZStrimW& s, ZRef<ZYadR> iYadR,
	size_t iInitialIndent, const ZYadOptions& iOptions);

void sWrite_PropName(const ZStrimW& iStrimW, const std::string& iPropName);

} // namespace ZYad_ZooLibStrim

NAMESPACE_ZOOLIB_END

#endif // __ZYad_ZooLibStrim__
