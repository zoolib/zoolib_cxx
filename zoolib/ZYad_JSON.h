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

#ifndef __ZYad_JSON__
#define __ZYad_JSON__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZYad_Std.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_JSON

class ZYadParseException_JSON : public ZYadParseException_Std
	{
public:
	ZYadParseException_JSON(const std::string& iWhat);
	ZYadParseException_JSON(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_JSON

class ZYadStrimR_JSON
:	public ZYadStrimR
	{
public:
	ZYadStrimR_JSON(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZStrimmerU> fStrimmerU;
	ZStrimR_Escaped fStrimR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_JSON

class ZYadSeqR_JSON : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_JSON(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSON

class ZYadMapR_JSON : public ZYadMapR_Std
	{
public:
	ZYadMapR_JSON(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Yad_JSONWriter

class ZVisitor_Yad_JSONWriter : public ZVisitor_Yad
	{
public:
	ZVisitor_Yad_JSONWriter(
		size_t iIndent, const ZYadOptions& iOptions, const ZStrimW& iStrimW);

// From ZVisitor_Yad
	virtual void Visit_YadR(ZRef<ZYadR> iYadR);
	virtual void Visit_YadAtomR(ZRef<ZYadAtomR> iYadAtomR);
	virtual void Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual void Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual void Visit_YadSeqR(ZRef<ZYadSeqR> iYadSeqR);
	virtual void Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);

private:
	class SaveState;
	friend class SaveState;

	size_t fIndent;
	ZYadOptions fOptions;
	const ZStrimW& fStrimW;
	bool fMayNeedInitialLF;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_JSON

namespace ZYad_JSON {

ZRef<ZYadR> sMakeYadR(ZRef<ZStrimmerU> iStrimmerU);

void sToStrim(ZRef<ZYadR> iYadR, const ZStrimW& s);

void sToStrim(size_t iInitialIndent, const ZYadOptions& iOptions,
	ZRef<ZYadR> iYadR, const ZStrimW& s);

} // namespace ZYad_JSON

} // namespace ZooLib

#endif // __ZYad_JSON__
