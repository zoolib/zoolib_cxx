// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_JSON_h__
#define __ZooLib_Util_Chan_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/AnyBase.h"
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

void sWriteSimpleValue(const AnyBase& iAny, const PushTextOptions_JSON& iOptions, const ChanW_UTF& w);

void sPull_Bin_Push_JSON(const ChanR_Bin& iChanR,
	size_t iLevel, const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& w);

ZQ<string8> sQRead_PropName(const ChanRU_UTF& iChanRU);

void sWrite_PropName(const string8& iPropName, const ChanW_UTF& w);

} // namespace Util_Chan_JSON
} // namespace ZooLib

#endif // __ZooLib_Util_Chan_JSON_h__
