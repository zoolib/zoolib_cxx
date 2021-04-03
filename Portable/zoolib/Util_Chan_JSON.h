// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_JSON_h__
#define __ZooLib_Util_Chan_JSON_h__ 1
#include "zconfig.h"

#include "zoolib/AnyBase.h"
#include "zoolib/ChanR_Bin.h"
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
	PushTextOptions_JSON(bool iPrettyPrint);
	PushTextOptions_JSON(const PushTextOptions& iOther);
	PushTextOptions_JSON(const PushTextOptions_JSON& iOther);

	PushTextOptions_JSON& operator=(const PushTextOptions& iOther);

	ZQ<bool> fUseExtendedNotationQ;
	ZQ<bool> fBinaryAsBase64Q;
	ZQ<bool> fPreferSingleQuotesQ;
	ZQ<bool> fNumberSequencesQ;
	ZQ<bool> fLowercaseHexQ;
	ZQ<bool> fIntegersAsHexQ;
	ZQ<bool> fAnnotateUnhandledQ;
	};

// =================================================================================================
#pragma mark -

void sWrite_LF(const ChanW_UTF& iChanW, const PushTextOptions& iOptions);

void sWrite_Indent(const ChanW_UTF& iChanW, size_t iCount, const PushTextOptions& iOptions);

void sWrite_LFIndent(const ChanW_UTF& iChanW, size_t iCount, const PushTextOptions& iOptions);

void sWrite_String(const ChanW_UTF& iChanW, const string& iString, bool iPreferSingleQuotes);

void sWrite_String(const ChanW_UTF& iChanW, const ChanR_UTF& iChanR);

bool sContainsProblemChars(const string& iString);

void sWrite_PropName(const ChanW_UTF& iChanW, const string& iString, bool iUseSingleQuotes = false);

void sWrite_SimpleValue(const ChanW_UTF& iChanW, const AnyBase& iAny, const PushTextOptions_JSON& iOptions);

void sPull_Bin_Push_JSON(const ChanR_Bin& iChanR,
	size_t iLevel, const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& ww);

ZQ<string8> sQRead_PropName(const ChanRU_UTF& iChanRU);

} // namespace Util_Chan_JSON
} // namespace ZooLib

#endif // __ZooLib_Util_Chan_JSON_h__
