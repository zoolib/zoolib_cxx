/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#ifndef __ZooLib_Util_Chan_JSON_h__
#define __ZooLib_Util_Chan_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/PullTextOptions.h"
#include "zoolib/PushTextOptions.h"

namespace ZooLib {
namespace Util_Chan_JSON {

using std::string;

// =================================================================================================
#pragma mark - ReadOptions

struct PullTextOptions_JSON : PullTextOptions
	{
	ZQ<bool> fAllowUnquotedPropertyNames;
	ZQ<bool> fAllowEquals;
	ZQ<bool> fAllowSemiColons;
	ZQ<bool> fAllowTerminators;
	ZQ<bool> fLooseSeparators;
	ZQ<bool> fAllowBinary;
	};

PullTextOptions_JSON sPullTextOptions_Extended();

// =================================================================================================
#pragma mark -

string sPrettyName(const std::type_info& iTI);

bool sTryRead_Identifier(const ChanRU_UTF& iChanRU,
	string* oStringLC, string* oStringExact);

bool sTryRead_PropertyName(const ChanRU_UTF& iChanRU,
	string& oName, bool iAllowUnquoted);

bool sTryRead_JSONString(const ChanRU_UTF& iChanRU,
	string& oString);

// =================================================================================================
#pragma mark - PushTextOptions_JSON

struct PushTextOptions_JSON : PushTextOptions
	{
	PushTextOptions_JSON();
	PushTextOptions_JSON(const PushTextOptions& iOther);
	PushTextOptions_JSON(const PushTextOptions_JSON& iOther);

	PushTextOptions_JSON& operator=(const PushTextOptions& iOther);

	ZQ<bool> fUseExtendedNotationQ;
	ZQ<bool> fBinaryAsBase64Q;
	ZQ<bool> fPreferSingleQuotesQ;
	ZQ<bool> fNumberSequencesQ;
	ZQ<bool> fLowercaseHexQ;
	ZQ<bool> fIntegersAsHexQ;
	};

// =================================================================================================
#pragma mark -

void sWriteLF(const PushTextOptions& iOptions, const ChanW_UTF& iChanW);

void sWriteIndent(size_t iCount, const PushTextOptions& iOptions, const ChanW_UTF& iChanW);

void sWriteLFIndent(size_t iCount, const PushTextOptions& iOptions, const ChanW_UTF& iChanW);

void sWriteString(const string& iString, bool iPreferSingleQuotes, const ChanW_UTF& iChanW);

void sWriteString(const ChanR_UTF& iChanR, const ChanW_UTF& iChanW);

bool sContainsProblemChars(const string& iString);

void sWritePropName(const string& iString, bool iUseSingleQuotes, const ChanW_UTF& w);

void sWriteSimpleValue(const Any& iAny, const PushTextOptions_JSON& iOptions, const ChanW_UTF& w);

void sPull_Bin_Push_JSON(const ChanR_Bin& iChanR,
	size_t iLevel, const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& w);

ZQ<string8> sQRead_PropName(const ChanRU_UTF& iChanRU);

void sWrite_PropName(const string8& iPropName, const ChanW_UTF& w);

} // namespace Util_Chan_JSON
} // namespace ZooLib

#endif // __ZooLib_Util_Chan_JSON_h__
