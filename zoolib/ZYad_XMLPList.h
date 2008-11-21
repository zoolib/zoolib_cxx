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

#ifndef __ZYad_XMLPList__
#define __ZYad_XMLPList__ 1
#include "zconfig.h"

#include "zoolib/ZML.h"
#include "zoolib/ZStream_ASCIIStrim.h"
#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZYad.h"

class ZStrimW_ML;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLPList

class ZYadParseException_XMLPList : public ZYadParseException
	{
public:
	ZYadParseException_XMLPList(const std::string& iWhat);
	ZYadParseException_XMLPList(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_XMLPList

class ZYadR_XMLPList : public virtual ZYadR
	{
public:
	// Our protocol
	virtual void Finish() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_XMLPList

class ZYadPrimR_XMLPList
:	public ZYadR_TValue,
	public ZYadPrimR,
	public ZYadR_XMLPList
	{
public:
	ZYadPrimR_XMLPList(const ZTValue& iTV);

// From ZYadR_XMLPList
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_XMLPList

class ZYadRawR_XMLPList
:	public ZYadRawR,
	public ZYadR_XMLPList
	{
public:
	ZYadRawR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadR_XMLPList
	virtual void Finish();

// From ZStreamerR via ZYadRawR
	const ZStreamR& GetStreamR();

private:
	ZML::Reader& fR;
	bool fMustReadEndTag;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_XMLPList

class ZYadListR_XMLPList
:	public ZYadListR,
	public ZYadR_XMLPList
	{
public:
	ZYadListR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadR via ZYadListR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR
	virtual size_t GetPosition();

// From ZYadR_XMLPList
	virtual void Finish();

private:
	void pMoveIfNecessary();

	ZML::Reader& fR;
	bool fMustReadEndTag;
	size_t fPosition;
	ZRef<ZYadR_XMLPList> fValue_Current;
	ZRef<ZYadR_XMLPList> fValue_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLPList

class ZYadMapR_XMLPList
:	public ZYadMapR,
	public ZYadR_XMLPList
	{
public:
	ZYadMapR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadR via ZYadMapR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadMapR
	virtual std::string Name();

// From ZYadR_XMLPList
	virtual void Finish();

private:
	void pMoveIfNecessary();

	ZML::Reader& fR;
	bool fMustReadEndTag;
	std::string fName;
	ZRef<ZYadR_XMLPList> fValue_Current;
	ZRef<ZYadR_XMLPList> fValue_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadUtil_XMLPList

namespace ZYadUtil_XMLPList {

ZRef<ZYadR> sMakeYadR(ZML::Reader& iReader);

void sToStrimW_ML(const ZStrimW_ML& s, ZRef<ZYadR> iYadR);

} // namespace ZYadUtil_XMLPList

#endif // __ZYad_XMLPList__
