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

#ifndef __ZYAD_ZooLibStrim__
#define __ZYAD_ZooLibStrim__ 1
#include "zconfig.h"

#include "zoolib/ZStream_HexStrim.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZYAD.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZYADParseException_ZooLibStrim

class ZYADParseException_ZooLibStrim : public ZYADParseException
	{
public:
	ZYADParseException_ZooLibStrim(const std::string& iWhat);
	ZYADParseException_ZooLibStrim(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZooLibStrim

class ZYADReaderRep_ZooLibStrim: public ZYADReaderRep
	{
public:
	ZYADReaderRep_ZooLibStrim(const ZStrimU& iStrimU);

	virtual bool HasValue();
	virtual ZType Type();

	virtual ZRef<ZMapReaderRep> ReadMap();
	virtual ZRef<ZListReaderRep> ReadList();
	virtual ZRef<ZStreamerR> ReadRaw();
	virtual ZRef<ZYAD> ReadYAD();

	void Finish();

private:
	void pReadIfNecessary();

	const ZStrimU& fStrimU;
	bool fNeedsReadClose;
	ZType fType;
	bool fHasValue;
	ZTValue fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZooLibStrim

class ZMapReaderRep_ZooLibStrim : public ZMapReaderRep
	{
public:
	ZMapReaderRep_ZooLibStrim(const ZStrimU& iStrimU);

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();

private:
	void pMoveIfNecessary();

	const ZStrimU& fStrimU;
	string fName;
	ZRef<ZYADReaderRep_ZooLibStrim> fValue_Current;
	ZRef<ZYADReaderRep_ZooLibStrim> fValue_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZooLibStrim

class ZListReaderRep_ZooLibStrim : public ZListReaderRep
	{
public:
	ZListReaderRep_ZooLibStrim(const ZStrimU& iStrimU);

	virtual bool HasValue();
	virtual size_t Index();
	virtual ZRef<ZYADReaderRep> Read();

private:
	void pMoveIfNecessary();

	const ZStrimU& fStrimU;
	ZRef<ZYADReaderRep_ZooLibStrim> fValue_Current;
	ZRef<ZYADReaderRep_ZooLibStrim> fValue_Prior;
	size_t fIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_ZooLibStrim

class ZStreamerR_ZooLibStrim
:	public ZStreamerR
	{
public:
	ZStreamerR_ZooLibStrim(const ZStrimU& iStrimU);
	virtual ~ZStreamerR_ZooLibStrim();
	
// From ZStreamerR
	const ZStreamR& GetStreamR();

private:
	const ZStrimU& fStrimU;
	ZStreamR_HexStrim fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLibStrim

namespace ZYADUtil_ZooLibStrim {

bool sRead_Identifier(
	const ZStrimU& iStrimU, std::string* oStringLC, std::string* oStringExact);

void sToStrim(const ZStrimW& s, ZListReader iListReader);

void sToStrim(const ZStrimW& s, ZListReader iListReader,
	size_t iInitialIndent, const ZYADOptions& iOptions);

void sToStrim(const ZStrimW& s, ZMapReader iMapReader);

void sToStrim(const ZStrimW& s, ZMapReader iMapReader,
	size_t iInitialIndent, const ZYADOptions& iOptions);

void sToStrim(const ZStrimW& s, const ZStreamR& iStreamR);

void sToStrim(const ZStrimW& s, const ZStreamR& iStreamR,
	size_t iInitialIndent, const ZYADOptions& iOptions);

void sToStrim(const ZStrimW& s, ZYADReader iYADReader);

void sToStrim(const ZStrimW& s, ZYADReader iYADReader,
	size_t iInitialIndent, const ZYADOptions& iOptions);

void sWrite_PropName(const ZStrimW& iStrimW, const std::string& iPropName);

} // namespace ZYADUtil_ZooLibStrim

#endif // __ZYAD_ZooLibStrim__
