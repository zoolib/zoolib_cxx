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
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLPList

class ZYadParseException_XMLPList : public ZYadParseException_Std
	{
public:
	ZYadParseException_XMLPList(const std::string& iWhat);
	ZYadParseException_XMLPList(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamR_XMLPList

class ZYadStreamR_XMLPList
:	public ZYadR_Std,
	public ZYadStreamR
	{
public:
	ZYadStreamR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadR_Std
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZML::Reader& fR;
	bool fMustReadEndTag;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimR_XMLPList

class ZYadStrimR_XMLPList
:	public ZYadR_Std,
	public ZYadStrimR
	{
public:
	ZYadStrimR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadR_Std
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZML::Reader& fR;
	bool fMustReadEndTag;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_XMLPList

class ZYadListR_XMLPList : public ZYadListR_Std
	{
public:
	ZYadListR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR_Std>& oYadR);

private:
	ZML::Reader& fR;
	bool fMustReadEndTag;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_XMLPList

class ZYadMapR_XMLPList : public ZYadMapR_Std
	{
public:
	ZYadMapR_XMLPList(ZML::Reader& iReader, bool iMustReadEndTag);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

private:
	ZML::Reader& fR;
	bool fMustReadEndTag;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_XMLPList

namespace ZYad_XMLPList {

ZRef<ZYadR> sMakeYadR(ZML::Reader& iReader);

void sToStrimW_ML(const ZML::StrimW& s, ZRef<ZYadR> iYadR);

} // namespace ZYad_XMLPList

NAMESPACE_ZOOLIB_END

#endif // __ZYad_XMLPList__
