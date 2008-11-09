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

#ifndef __ZYAD_ZooLibStream__
#define __ZYAD_ZooLibStream__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZYAD.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZooLibStream

class ZYAD_ZooLibStream : public ZYAD
	{
public:
	ZYAD_ZooLibStream(ZType iType, const ZStreamR& iStreamR);
	virtual ~ZYAD_ZooLibStream();

	virtual bool GetTValue(ZTValue& oYalue);

private:
	const ZTValue fTV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_ZooLibStream

class ZYADReaderRep_ZooLibStream: public ZYADReaderRep
	{
public:
	ZYADReaderRep_ZooLibStream(const ZStreamR& iStreamR);

	virtual bool HasValue();
	virtual ZType Type();

	virtual ZRef<ZMapReaderRep> ReadMap();
	virtual ZRef<ZListReaderRep> ReadList();
	virtual ZRef<ZStreamerR> ReadRaw();
	virtual ZRef<ZYAD> ReadYAD();

private:
	void pReadIfNecessary();

	const ZStreamR& fStreamR;
	bool fNeedsRead;
	bool fHasValue;
	ZType fType;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_ZooLibStream

namespace ZYADUtil_ZooLibStream {

void sToStream(const ZStreamW& iStreamW, ZListReader iListReader);

void sToStream(const ZStreamW& iStreamW, ZMapReader iMapReader);

void sToStream(const ZStreamW& iStreamW, const ZStreamR& iStreamR);

void sToStream(const ZStreamW& iStreamW, ZYADReader iYADReader);

} // namespace ZYADUtil_ZooLibStream

#endif // __ZYAD_ZooLibStream__
