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

#ifndef __ZYad_JSON_h__
#define __ZYad_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamR_HexStrim.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZYad_Std.h"

namespace ZooLib {
namespace ZYad_JSON {

// =================================================================================================
#pragma mark -
#pragma mark * ReadOptions

struct ReadOptions
	{
	ZQ<bool> fAllowUnquotedPropertyNames;
	ZQ<bool> fAllowEquals;
	ZQ<bool> fAllowSemiColons;
	ZQ<bool> fAllowTerminators;
	ZQ<bool> fLooseSeparators;
	ZQ<bool> fAllowBinary;
	};

ReadOptions sReadOptions_Extended();

// =================================================================================================
#pragma mark -
#pragma mark * WriteOptions

struct WriteOptions : ZYadOptions
	{
	WriteOptions();
	WriteOptions(const ZYadOptions& iOther);
	WriteOptions(const WriteOptions& iOther);

	ZQ<bool> fUseExtendedNotation;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ParseException

class ParseException : public ZYadParseException_Std
	{
public:
	ParseException(const std::string& iWhat);
	ParseException(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadStreamR

class YadStreamR
:	public ZYadStreamR
	{
public:
	YadStreamR(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZRef<ZStrimmerU> fStrimmerU;
	ZStreamR_HexStrim fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadStrimR

class YadStrimR
:	public ZYadStrimR
	{
public:
	YadStrimR(ZRef<ZStrimmerU> iStrimmerU);

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
#pragma mark * YadSeqR

class YadSeqR : public ZYadSeqR_Std
	{
public:
	YadSeqR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	const ReadOptions fReadOptions;
	};

// =================================================================================================
#pragma mark -
#pragma mark * YadMapR

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	const ReadOptions fReadOptions;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Writer

class Visitor_Writer : public ZVisitor_Yad
	{
public:
	Visitor_Writer
		(size_t iIndent, const WriteOptions& iOptions, const ZStrimW& iStrimW);

// From ZVisitor_Yad
	virtual void Visit_YadR(const ZRef<ZYadR>& iYadR);
	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR);
	virtual void Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR);
	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR);
	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR);

private:
	size_t fIndent;
	const WriteOptions fOptions;
	const ZStrimW& fStrimW;
	bool fMayNeedInitialLF;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sYadR and sToStrim

ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU);
ZRef<ZYadR> sYadR(ZRef<ZStrimmerU> iStrimmerU, const ReadOptions& iReadOptions);

void sToStrim(ZRef<ZYadR> iYadR, const ZStrimW& s);

void sToStrim(size_t iInitialIndent, const WriteOptions& iOptions,
	ZRef<ZYadR> iYadR, const ZStrimW& s);

} // namespace ZYad_JSON
} // namespace ZooLib

#endif // __ZYad_JSON_h__
