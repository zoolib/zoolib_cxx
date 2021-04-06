// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_string_h__
#define __ZooLib_Util_string_h__ 1
#include "zconfig.h"

#include "zoolib/StdInt.h" // For int64
#include "zoolib/ZQ.h"

#include <string>
#include <vector>

namespace ZooLib {
namespace Util_string {

using std::string;

// =================================================================================================
#pragma mark - Util_string

ZQ<int64> sQInt64(const string& iString);
int64 sDInt64(int64 iDefault, const string& iString);
int64 sInt64(const string& iString);

ZQ<uint64> sQUInt64(const string& iString);
uint64 sDUInt64(uint64 iDefault, const string& iString);
uint64 sUInt64(const string& iString);

ZQ<double> sQDouble(const string& iString);
double sDDouble(double iDefault, const string& iString);
double sDouble(const string& iString);

// -----

bool sContains(const string& iTarget, const string& iPattern);
bool sContainsi(const string& iTarget, const string& iPattern);

int sComparei(const string& iLeft, const string& iRight);
bool sEquali(const string& iLeft, const string& iRight);

// -----

ZQ<string> sQWithoutPrefix(const string& iTarget, const string& iPrefix);
ZQ<string> sQWithoutSuffix(const string& iTarget, const string& iSuffix);

bool sStartsWith(const string& iTarget, const string& iPossiblePrefix);
bool sEndsWith(const string& iTarget, const string& iPossibleSuffix);

// -----

extern const char* const sStandardWSCharacters;

string sTrimmedL(const string& iString, const char* iWS = sStandardWSCharacters);
string sTrimmedR(const string& iString, const char* iWS = sStandardWSCharacters);
string sTrimmed(const string& iString, const char* iWS = sStandardWSCharacters);

// -----

string sFromPString(const unsigned char* iPString);
void sToPString(const string& iString, unsigned char* oPString, size_t iMaxLength);
void sToPString(const char* iString, unsigned char* oPString, size_t iMaxLength);

// -----

std::vector<string> sSplit(char iChar, const string& iString);

// -----

inline PaC<const char> sPaC(const std::string& iString)
	{
	return PaC<const char>(iString.size() ? iString.data() : nullptr, iString.size());
	}

inline PaC<char> sPaC(std::string& ioString)
	{
	return PaC<char>(ioString.size() ? const_cast<char*>(ioString.data()) : nullptr, ioString.size());
	}

} // namespace Util_string
} // namespace ZooLib

#endif // __ZooLib_Util_string_h__
