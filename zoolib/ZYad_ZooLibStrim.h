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

#ifndef __ZYad_ZooLibStrim_h__
#define __ZYad_ZooLibStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamR_HexStrim.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZYad_Std.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZYadParseException_ZooLibStrim

class ZYadParseException_ZooLibStrim : public ZYadParseException_Std
	{
public:
	ZYadParseException_ZooLibStrim(const std::string& iWhat);
	ZYadParseException_ZooLibStrim(const char* iWhat);
	};

// =================================================================================================
// MARK: - ZYadStreamR_ZooLibStrim

class ZYadStreamR_ZooLibStrim
:	public ZYadStreamR
	{
public:
	ZYadStreamR_ZooLibStrim(ZRef<ZStrimmerU> iStrimmerU, bool iReadDelimiter);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZRef<ZStrimmerU> fStrimmerU;
	bool fReadDelimiter;
	ZStreamR_HexStrim fStreamR;
	};

// =================================================================================================
// MARK: - ZYadStrimR_ZooLibStrim_Apos

class ZYadStrimR_ZooLibStrim_Apos
:	public ZYadStrimR
	{
public:
	ZYadStrimR_ZooLibStrim_Apos(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZStrimmerU> fStrimmerU;
	ZStrimR_Escaped fStrimR;
	};

// =================================================================================================
// MARK: - ZYadStrimR_ZooLibStrim_Quote

class ZYadStrimR_ZooLibStrim_Quote
:	public ZYadStrimR,
	private ZStrimR
	{
public:
	ZYadStrimR_ZooLibStrim_Quote(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	ZStrimR_Boundary fStrimR_Boundary;
	size_t fQuotesSeen;
	};

// =================================================================================================
// MARK: - ZYadSeqR_ZooLibStrim

class ZYadSeqR_ZooLibStrim : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_ZooLibStrim(ZRef<ZStrimmerU> iStrimmerU, bool iReadDelimiter);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	bool fReadDelimiter;
	};

// =================================================================================================
// MARK: - ZYadMapR_ZooLibStrim

class ZYadMapR_ZooLibStrim : public ZYadMapR_Std
	{
public:
	ZYadMapR_ZooLibStrim(ZRef<ZStrimmerU> iStrimmerU, bool iReadDelimiter);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	bool fReadDelimiter;
	};

// =================================================================================================
// MARK: - ZYad_ZooLibStrim

namespace ZYad_ZooLibStrim {

bool sRead_Identifier
	(const ZStrimU& iStrimU, std::string* oStringLC, std::string* oStringExact);

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU);

void sToStrim(ZRef<ZYadR> iYadR, const ZStrimW& s);

void sToStrim(size_t iInitialIndent, const ZYadOptions& iOptions,
	ZRef<ZYadR> iYadR, const ZStrimW& s);

void sWrite_PropName(const std::string& iPropName, const ZStrimW& s);

} // namespace ZYad_ZooLibStrim
} // namespace ZooLib

#endif // __ZYad_ZooLibStrim_h__
