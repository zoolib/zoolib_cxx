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
namespace Yad_XMLPList {

// =================================================================================================
// MARK: - ChannerR_Bin_XMLPList

class ChannerR_Bin_XMLPList
:	public ChannerR_Bin
	{
	typedef ChannerR_Bin inherited;
public:
	ChannerR_Bin_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag);

// From ZCounted
	virtual void Finalize();

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount);

private:
	ZRef<ML::ChannerRU_UTF> fStrimmerU;
	const bool fIsEmptyTag;
	ChanR_Bin_ASCIIStrim fStreamR_ASCIIStrim;
	ChanR_Bin_Base64Decode fStreamR_Base64Decode;
	};

// =================================================================================================
// MARK: - ChannerR_UTF_XMLPList

class ChannerR_UTF_XMLPList
:	public ChannerR_UTF
	{
	typedef ChannerR_UTF inherited;
public:
	ChannerR_UTF_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag);

// From ZCounted
	virtual void Finalize();

// From YadR
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	ZRef<ML::ChannerRU_UTF> fStrimmerU;
	const bool fIsEmptyTag;
	};

// =================================================================================================
// MARK: - Chan_RefYad_XMLPList

class ChanR_RefYad_XMLPList
:	public ChanR_RefYad_Std
	{
public:
	ChanR_RefYad_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag);

// From Chan_RefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR);

private:
	ZRef<ML::ChannerRU_UTF> fStrimmerU;
	const bool fIsEmptyTag;
	};

// =================================================================================================
// MARK: - ChanR_NameRefYad_XMLPList

class ChanR_NameRefYad_XMLPList
:	public ChanR_NameRefYad_Std
	{
public:
	ChanR_NameRefYad_XMLPList(ZRef<ML::ChannerRU_UTF> iStrimmerU, bool iIsEmptyTag);

// From ChanR_NameRefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR);

private:
	ZRef<ML::ChannerRU_UTF> fStrimmerU;
	const bool fIsEmptyTag;
	};

// =================================================================================================
// MARK: - Yad_XMLPList

ZRef<YadR> sYadR(ZRef<ML::ChannerRU_UTF> iR);

void sToStrim(ZRef<YadR> iYadR, const ML::StrimW& s);

void sWritePreamble(const ML::StrimW& s);

} // namespace Yad_XMLPList
} // namespace ZooLib

#endif // __ZooLib_Yad_XMLPList_h__
