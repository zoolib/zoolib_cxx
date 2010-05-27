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

#include "zoolib/ZYad_MapAsSeq.h"

namespace ZooLib {

using std::string;

namespace { // anonymous

class YadMapR : public ZYadMapR
	{
public:
	YadMapR(const string& iNamePropName, const string& iPropName, ZRef<ZYadMapR> iYadMapR);

// From ZYadR via ZYadMapR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadMapR
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

private:
	bool fReturnedPropName;
	const string& fNamePropName;
	const string& fPropName;
	ZRef<ZYadMapR> fYadMapR;
	};

YadMapR::YadMapR(const string& iNamePropName, const string& iPropName, ZRef<ZYadMapR> iYadMapR)
:	fReturnedPropName(false)
,	fNamePropName(iNamePropName)
,	fPropName(iPropName)
,	fYadMapR(iYadMapR)
	{}

bool YadMapR::IsSimple(const ZYadOptions& iOptions)
	{ return fYadMapR->IsSimple(iOptions); }

ZRef<ZYadR> YadMapR::ReadInc(std::string& oName)
	{
	if (!fReturnedPropName)
		{
		fReturnedPropName = true;
		oName = fNamePropName;
		return sMakeYadR(fPropName);
		}
	return fYadMapR->ReadInc(oName);
	}

// We want to be able to take the name under which the value was stored, and
// optionally shove it into a returned map. Or do it in nested fashion, propname="something", value = stored value

class YadSeqR : public ZYadSeqR
	{
public:
	YadSeqR(const string& iNamePropName, ZRef<ZYadMapR> iYadMapR);

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc();

private:
	const string& fNamePropName;
	ZRef<ZYadMapR> fYadMapR;
	};

YadSeqR::YadSeqR(const string& iNamePropName, ZRef<ZYadMapR> iYadMapR)
:	fNamePropName(iNamePropName)
,	fYadMapR(iYadMapR)
	{}

ZRef<ZYadR> YadSeqR::ReadInc()
	{
	string thePropName;
	if (ZRef<ZYadR> theYadR = fYadMapR->ReadInc(thePropName))
		{
		return theYadR;
		}
	return ZRef<ZYadR>();
	}

} // namespace ZANONYMOUS

// =================================================================================================
#pragma mark -
#pragma mark * sMapAsSeq

ZRef<ZYadSeqR> sMapAsSeq(const string& iNamePropName, ZRef<ZYadMapR> iYadMapR)
	{
	return new YadSeqR(iNamePropName, iYadMapR);
	}

} // namespace ZooLib
