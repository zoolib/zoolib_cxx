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

#ifndef __ZYAD_OSXPList__
#define __ZYAD_OSXPList__ 1
#include "zconfig.h"

#include "zoolib/ZML.h"
#include "zoolib/ZYAD.h"

class ZStrimW_ML;

// =================================================================================================
#pragma mark -
#pragma mark * ZYADParseException_OSXPList

class ZYADParseException_OSXPList : public ZYADParseException
	{
public:
	ZYADParseException_OSXPList(const std::string& iWhat);
	ZYADParseException_OSXPList(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_OSXPList declaration

class ZYADReaderRep_OSXPList: public ZYADReaderRep
	{
public:
	ZYADReaderRep_OSXPList(ZML::Reader& iReader);

	virtual bool HasValue();

	virtual ZType Type();

	virtual ZRef<ZMapReaderRep> ReadMap();
	virtual ZRef<ZListReaderRep> ReadList();
	virtual ZRef<ZStreamerR> ReadRaw();
	virtual ZRef<ZYAD> ReadYAD();

	virtual void Skip();

private:
	ZML::Reader& fR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_OSXPList

namespace ZYADUtil_OSXPList {

void sToStrimW_ML(const ZStrimW_ML& s, ZYADReader iYADReader);

bool sFromStrim(const ZStrimU& iStrimU, ZTValue& oTValue);
bool sFromML(ZML::Reader& r, ZTValue& oTValue);

} // namespace ZYADUtil_OSXPList

#endif // __ZYAD_OSXPList__
