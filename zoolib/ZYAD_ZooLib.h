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

#ifndef __ZYAD_ZooLib__
#define __ZYAD_ZooLib__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"
#include "zoolib/ZYAD.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZTValue

class ZYAD_ZTValue : public ZYAD
	{
public:
	ZYAD_ZTValue(const ZTValue& iTV);
	virtual ~ZYAD_ZTValue();

	virtual bool GetTValue(ZTValue& oYalue);

private:
	const ZTValue fTV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZTValue

class ZYADReaderRep_ZTValue: public ZYADReaderRep
	{
public:
	ZYADReaderRep_ZTValue(const ZTValue& iTV);

	virtual bool HasValue();
	virtual ZType Type();

	virtual ZRef<ZMapReaderRep> ReadMap();
	virtual ZRef<ZListReaderRep> ReadList();
	virtual ZRef<ZStreamerR> ReadRaw();
	virtual ZRef<ZYAD> ReadYAD();

private:
	const ZTValue fTV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZTuple

class ZMapReaderRep_ZTuple : public ZMapReaderRep
	{
public:
	ZMapReaderRep_ZTuple(const ZTuple& iTuple);

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

	virtual bool IsSimple(const ZYADOptions& iOptions);
	virtual bool CanRandomAccess();
	virtual ZRef<ZYADReaderRep> ReadWithName(const std::string& iName);

private:
	const ZTuple fTuple;
	ZTuple::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZVector

class ZListReaderRep_ZVector : public ZListReaderRep
	{
public:
	ZListReaderRep_ZVector(const std::vector<ZTValue>& iVector);

	virtual bool HasValue();
	virtual size_t Index() const;
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

	virtual bool IsSimple(const ZYADOptions& iOptions);
	virtual bool CanRandomAccess();
	virtual size_t Count();
	virtual ZRef<ZYADReaderRep> ReadWithIndex(size_t iIndex);

private:
	const std::vector<ZTValue> fVector;
	std::vector<ZTValue>::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLib

namespace ZYADUtil_ZooLib {

bool sFromReader(ZYADReader iYADReader, ZTValue& oTV);
ZTValue sFromReader(ZYADReader iYADReader);

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

void sWrite_PropName(const ZStrimW& iStrimW, const string& iPropName);

} // namespace ZYADUtil_ZooLib

#endif // __ZYAD_ZooLib__
