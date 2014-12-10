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

#ifndef __ZooLib_Yad_JSON_h__
#define __ZooLib_Yad_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/Chan_XX_Terminated.h"
#include "zoolib/Chan_Bin_AsciiStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/ChanR_Bin_HexStrim.h"
#include "zoolib/CountedVal.h"
#include "zoolib/Yad_Std.h"

namespace ZooLib {
namespace Yad_JSON {

// =================================================================================================
// MARK: - ReadOptions

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
// MARK: - WriteOptions

struct WriteOptions : YadOptions
	{
	WriteOptions();
	WriteOptions(const YadOptions& iOther);
	WriteOptions(const WriteOptions& iOther);

	ZQ<bool> fUseExtendedNotation;
	ZQ<bool> fBinaryAsBase64;
	ZQ<bool> fPreferSingleQuotes;
	ZQ<bool> fNumberSequences;
	};

// =================================================================================================
// MARK: - ParseException

class ParseException : public YadParseException_Std
	{
public:
	ParseException(const std::string& iWhat);
	ParseException(const char* iWhat);
	};

// =================================================================================================
// MARK: - YadStreamerR_Hex

class YadStreamerR_Hex
:	public YadStreamerR
	{
public:
	YadStreamerR_Hex(ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

// From YadR
	virtual void Finish();

// From ChannerR via YadStreamerR
	virtual void GetChan(const ChanR_Bin*& oChanPtr);

private:
	ZRef<ChannerR_UTF> fChannerR;
	ZRef<ChannerU_UTF> fChannerU;
	ChanR_Bin_HexStrim fChanR;
	};

// =================================================================================================
// MARK: - YadStreamerR_Base64

class YadStreamerR_Base64
:	public YadStreamerR
	{
public:
	YadStreamerR_Base64(const Base64::Decode& iDecode,
		ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

// From YadR
	virtual void Finish();

// From ChannerR via YadStreamerR
	virtual void GetChan(const ChanR_Bin*& oChanPtr);

private:
	ZRef<ChannerR_UTF> fChannerR;
	ZRef<ChannerU_UTF> fChannerU;
	ChanR_Bin_ASCIIStrim fChanR_Bin_ASCIIStrim;
	ChanR_XX_Terminated<byte> fChanR_Bin_Boundary;
	ChanR_Bin_Base64Decode fChanR;
	};

// =================================================================================================
// MARK: - YadStrimmerR_JSON

class YadStrimmerR_JSON
:	public ZooLib::YadStrimmerR
	{
public:
	YadStrimmerR_JSON(ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

// From YadR
	virtual void Finish();

// From ChannerR via YadStrimmerR
	virtual void GetChan(const ChanR_UTF*& oChanPtr);

private:
	ZRef<ChannerR_UTF> fChannerR;
	ZRef<ChannerU_UTF> fChannerU;
	ChanR_UTF_Escaped fChanR;
	};

// =================================================================================================
// MARK: - YadSeqR_JSON

class YadSeqR_JSON
:	public YadSeqR_Std
	{
public:
	YadSeqR_JSON(const ZRef<CountedVal<ReadOptions> >& iRO,
		ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

// From YadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR);

private:
	const ZRef<CountedVal<ReadOptions> > fRO;
	ZRef<ChannerR_UTF> fChannerR;
	ZRef<ChannerU_UTF> fChannerU;
	};

// =================================================================================================
// MARK: - YadMapR_JSON

class YadMapR_JSON
:	public YadMapR_Std
	{
public:
	YadMapR_JSON(const ZRef<CountedVal<ReadOptions> >& iRO,
		ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

// From YadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR);

private:
	const ZRef<CountedVal<ReadOptions> > fRO;
	ZRef<ChannerR_UTF> fChannerR;
	ZRef<ChannerU_UTF> fChannerU;
	};

// =================================================================================================
// MARK: - Visitor_Writer

class Visitor_Writer : public Visitor_Yad
	{
public:
	Visitor_Writer(
		size_t iIndent, const WriteOptions& iOptions, const ChanW_UTF& iStrimW);

// From ZVisitor_Yad
	virtual void Visit_YadR(const ZRef<ZooLib::YadR>& iYadR);
	virtual void Visit_YadAtomR(const ZRef<ZooLib::YadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamerR(const ZRef<ZooLib::YadStreamerR>& iYadStreamerR);
	virtual void Visit_YadStrimmerR(const ZRef<ZooLib::YadStrimmerR>& iYadStrimmerR);
	virtual void Visit_YadSeqR(const ZRef<ZooLib::YadSeqR>& iYadSeqR);
	virtual void Visit_YadMapR(const ZRef<ZooLib::YadMapR>& iYadMapR);

private:
	size_t fIndent;
	const WriteOptions fOptions;
	const ChanW_UTF& fChanW;
	bool fMayNeedInitialLF;
	};

// =================================================================================================
// MARK: - sYadR and sToStrim

ZRef<YadR> sYadR(ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

ZRef<YadR> sYadR(const ReadOptions& iReadOptions,
	ZRef<ChannerR_UTF> iChannerR, ZRef<ChannerU_UTF> iChannerU);

void sToChan(ZRef<YadR> iYadR, const ChanW_UTF& w);

void sToChan(size_t iInitialIndent, const WriteOptions& iOptions,
	ZRef<YadR> iYadR, const ChanW_UTF& w);

} // namespace Yad_JSON
} // namespace ZooLib

#endif // __ZooLib_Yad_JSON_h__
