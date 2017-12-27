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

#ifndef __ZooLib_Yad_XMLPList_h__
#define __ZooLib_Yad_XMLPList_h__ 1
#include "zconfig.h"

#include "zoolib/ML.h"
#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/Yad_Std.h"

namespace ZooLib {
namespace Yad_XMLPlist {

// =================================================================================================
// MARK: - ParseException

class ParseException : public YadParseException_Std
	{
public:
	ParseException(const std::string& iWhat);
	ParseException(const char* iWhat);
	};

class YadR_XMLPlist
	{
public:
	enum ERead { eRead_EndTag, eRead_EmptyTag, eRead_NoTag };
	};

// =================================================================================================
// MARK: - YadStreamerR_XMLPList

class YadStreamerR_XMLPList
:	public virtual YadStreamerR
	{
public:
	YadStreamerR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From YadR
	virtual void Finish();

// From ZStreamerR via YadStreamerR
	const ZStreamR& GetStreamR();

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
// MARK: - YadStrimmerR_XMLPList

class YadStrimmerR_XMLPList
:	public YadStrimmerR
,	YadR_XMLPlist
	{
public:
	YadStrimmerR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From YadR
	virtual void Finish();

// From ZStrimmerR via YadStrimmerR
	const ZStrimR& GetStrimR();

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	};

// =================================================================================================
// MARK: - YadSeqR_XMLPList

class YadSeqR_XMLPList
:	public YadSeqR_Std
,	YadR_XMLPlist
	{
public:
	YadSeqR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From YadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	};

// =================================================================================================
// MARK: - YadMapR_XMLPList

class YadMapR_XMLPList
:	public YadMapR_Std
,	YadR_XMLPlist
	{
public:
	YadMapR_XMLPList(ZRef<ZML::StrimmerU> iStrimmerU, ERead iRead);

// From YadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<YadR>& oYadR);

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	const ERead fRead;
	};

// =================================================================================================
// MARK: - Yad_XMLPList

ZRef<YadR> sYadR(ZRef<ZML::StrimmerU> iR);

void sToStrim(ZRef<YadR> iYadR, const ZML::StrimW& s);

void sWritePreamble(const ZML::StrimW& s);

} // namespace Yad_XMLPList
} // namespace ZooLib

#endif // __ZooLib_Yad_XMLPList_h__
