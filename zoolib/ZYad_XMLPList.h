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

#ifndef __ZYad_XMLPList_h__
#define __ZYad_XMLPList_h__ 1
#include "zconfig.h"

#include "zoolib/ZML.h"
#include "zoolib/ZStream_ASCIIStrim.h"
#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZYad_Std.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZYadParseException_XMLPList

class ZYadParseException_XMLPList : public ZYadParseException_Std
	{
public:
	ZYadParseException_XMLPList(const std::string& iWhat);
	ZYadParseException_XMLPList(const char* iWhat);
	};

class ZYadR_XMLPlist
	{
public:
	enum ERead { eRead_EndTag, eRead_EmptyTag, eRead_NoTag };
	};

// =================================================================================================
// MARK: - ZYadStreamR_XMLPList

class ZYadStreamR_XMLPList
:	public ZYadStreamR
,	ZYadR_XMLPlist
	{
public:
	ZYadStreamR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From ZYadR
	virtual void Finish();

// From ZStreamerR via ZYadStreamR
	const ZStreamR& GetStreamR();

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
// MARK: - ZYadStrimR_XMLPList

class ZYadStrimR_XMLPList
:	public ZYadStrimR
,	ZYadR_XMLPlist
	{
public:
	ZYadStrimR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From ZYadR
	virtual void Finish();

// From ZStrimmerR via ZYadStrimR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	};

// =================================================================================================
// MARK: - ZYadSeqR_XMLPList

class ZYadSeqR_XMLPList
:	public ZYadSeqR_Std
,	ZYadR_XMLPlist
	{
public:
	ZYadSeqR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	};

// =================================================================================================
// MARK: - ZYadMapR_XMLPList

class ZYadMapR_XMLPList
:	public ZYadMapR_Std
,	ZYadR_XMLPlist
	{
public:
	ZYadMapR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	};

// =================================================================================================
// MARK: - ZYad_XMLPList

namespace ZYad_XMLPList {

ZRef<ZYadR> sYadR(ZRef<ZML::StrimmerU> iR);

void sToStrim(ZRef<ZYadR> iYadR, const ZML::StrimW& s);

void sWritePreamble(const ZML::StrimW& s);

} // namespace ZYad_XMLPList
} // namespace ZooLib

#endif // __ZYad_XMLPList_h__
