/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_ChanRU_UTF_ML_h__
#define __ZooLib_ChanRU_UTF_ML_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ChanRU_UTF.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/ML.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - ML
namespace ML {

enum EToken
	{
	eToken_Exhausted = -1,
	eToken_Fresh = 0,
	eToken_TagBegin,
	eToken_TagEnd,
	eToken_TagEmpty,
	eToken_Text
	};

} // namespace ML

// =================================================================================================
#pragma mark - ChanRU_UTF_ML

class ChanRU_UTF_ML
:	public ChanRU_UTF
	{
public:
	explicit ChanRU_UTF_ML(const ChanRU_UTF& iChanRU);

	ChanRU_UTF_ML(const ChanRU_UTF& iChanRU,
		bool iRecognizeEntitiesInAttributeValues, ZP<ML::Callable_Entity> iCallable);
	~ChanRU_UTF_ML();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);

// Our protocol
//	ZMACRO_operator_bool(ChanU_UTF, operator_bool) const;
	ML::EToken Current() const;
	ChanRU_UTF_ML& Advance();

	const std::string& Name() const;
	ML::Attrs_t Attrs() const;

	ZQ<std::string> QAttr(const std::string& iAttrName) const;
	std::string Attr(const std::string& iAttrName) const;

private:
	void pAdvance() const;
	void pAdvance();

	const ChanRU_UTF& fChanRU;

	bool fRecognizeEntitiesInAttributeValues;

	ZP<ML::Callable_Entity> fCallable;

	string32 fBuffer;
	size_t fBufferStart;

	ML::EToken fToken;

	std::string fTagName;
	ML::Attrs_t fTagAttributes;
	};

// =================================================================================================
#pragma mark - ML::ChannerRU

typedef Channer_T<ChanRU_UTF_ML> ChannerRU_UTF_ML;

ZP<ChannerRU_UTF_ML> sChannerRU_UTF_ML(const ZP<ChannerRU<UTF32>>& iChanner);

// =================================================================================================
#pragma mark - ML parsing support

void sSkipText(ChanRU_UTF_ML& r);

bool sSkip(ChanRU_UTF_ML& r, const std::string& iTagName);
bool sSkip(ChanRU_UTF_ML& r, std::vector<std::string>& ioTags);

bool sTryRead_Begin(ChanRU_UTF_ML& r, const std::string& iTagName);

bool sTryRead_Empty(ChanRU_UTF_ML& r, const std::string& iTagName);

bool sTryRead_End(ChanRU_UTF_ML& r, const std::string& iTagName);

} // namespace ZooLib

#endif // __ZooLib_ChanRU_UTF_ML_h__
