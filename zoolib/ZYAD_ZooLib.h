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

#include "zoolib/ZTuple.h"
#include "zoolib/ZYAD.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZooLib

class ZYAD_ZooLib : public ZYAD
	{
public:
	ZYAD_ZooLib(const ZTValue& iTV);
	virtual ~ZYAD_ZooLib();

	virtual bool GetTValue(ZTValue& oYalue);

private:
	ZTValue fTV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZooLib

class ZYADReaderRep_ZooLib: public ZYADReaderRep
	{
public:
	ZYADReaderRep_ZooLib(const ZTValue& iTV);

	virtual bool HasValue();
	virtual ZType Type();

	virtual ZRef<ZMapReaderRep> ReadMap();
	virtual ZRef<ZListReaderRep> ReadList();
	virtual ZRef<ZStreamerR> ReadRaw();
	virtual ZRef<ZYAD> ReadYAD();

private:
	const ZTValue& fTV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_ZooLib

class ZMapReaderRep_ZooLib : public ZMapReaderRep
	{
public:
	ZMapReaderRep_ZooLib(const ZTuple& iTuple);

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

private:
	const ZTuple& fTuple;
	ZTuple::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_ZooLib

class ZListReaderRep_ZooLib : public ZListReaderRep
	{
public:
	ZListReaderRep_ZooLib(const std::vector<ZTValue>& iVector);

	virtual bool HasValue();
	virtual size_t Index() const;
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

private:
	const std::vector<ZTValue>& fVector;
	std::vector<ZTValue>::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLib

namespace ZYADUtil_ZooLib {

bool sFromReader(ZYADReader iYADReader, ZTValue& oTV);
ZTValue sFromReader(ZYADReader iYADReader);

} // namespace ZYADUtil_ZooLib

#endif // __ZYAD_ZooLib__
