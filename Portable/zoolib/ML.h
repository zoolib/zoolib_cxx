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

#ifndef __ZooLib_ML_h__
#define __ZooLib_ML_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Compat_NonCopyable.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/Channer_UTF.h"

#include <vector>

namespace ZooLib {
namespace ML {

// =================================================================================================
#pragma mark - ML

using std::pair;
using std::string;
using std::vector;

enum EToken
	{
	eToken_Exhausted = -1,
	eToken_Fresh = 0,
	eToken_TagBegin,
	eToken_TagEnd,
	eToken_TagEmpty,
	eToken_Text
	};

typedef pair<string, string> Attr_t;
typedef vector<Attr_t> Attrs_t;

typedef Callable<string(string)> Callable_Entity;

// =================================================================================================
#pragma mark - ML::ChanRU

/// Tokenizes the ML from a source strim.

class ChanRU
:	public ZooLib::ChanRU_UTF
	{
public:
	explicit ChanRU(const ZooLib::ChanRU_UTF& iChanRU);

	ChanRU(const ZooLib::ChanRU_UTF& iChanRU,
		bool iRecognizeEntitiesInAttributeValues, ZP<Callable_Entity> iCallable);
	~ChanRU();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);

// Our protocol
//	ZMACRO_operator_bool(ChanU_UTF, operator_bool) const;
	EToken Current() const;
	ChanRU& Advance();

	const string& Name() const;
	Attrs_t Attrs() const;

	ZQ<string> QAttr(const string& iAttrName) const;
	string Attr(const string& iAttrName) const;

private:
	void pAdvance() const;
	void pAdvance();

	const ZooLib::ChanRU_UTF& fChanRU;

	bool fRecognizeEntitiesInAttributeValues;

	ZP<Callable_Entity> fCallable;

	string32 fBuffer;
	size_t fBufferStart;

	EToken fToken;

	string fTagName;
	Attrs_t fTagAttributes;
	};

// =================================================================================================
#pragma mark - ML::ChannerRU

typedef Channer_T<ChanRU> ChannerRU;

ZP<ChannerRU> sChanner(const ZP<ZooLib::ChannerRU<UTF32>>& iChanner);

// =================================================================================================
#pragma mark - ML parsing support

void sSkipText(ChanRU& r);

bool sSkip(ChanRU& r, const string& iTagName);
bool sSkip(ChanRU& r, vector<string>& ioTags);

bool sTryRead_Begin(ChanRU& r, const string& iTagName);

bool sTryRead_Empty(ChanRU& r, const string& iTagName);

bool sTryRead_End(ChanRU& r, const string& iTagName);

} // namespace ML
} // namespace ZooLib

#endif // __ZooLib_ML_h__
