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

#ifndef __ZooLib_HTTP_h__
#define __ZooLib_HTTP_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"

#include "zoolib/ZTrail.h"
#include "zoolib/ZVal_Any.h"

#include <vector>

namespace ZooLib {

typedef ChanU<byte> ChanU_Bin;

namespace HTTP {

using std::string;

typedef ZData_Any Data;
typedef ZVal_Any Val;
typedef ZSeq_Any Seq;
typedef ZMap_Any Map;

// =================================================================================================
// MARK: - Response

class Response
	{
public:
	Response();

	void Clear();

	void SetVersion10();

	void SetResult(int iResult);
	void SetResult(int iResult, const string& iMessage);

	void Set(const string& iName, const string& iValue);
	void Set(const string& iName, int iValue);
	void Set(const string& iName, uint64 iValue);

	void Send(const ChanW_Bin& iChanW) const;

private:
	bool fIsVersion11;
	std::vector<std::pair<string, string> > fHeaders;
	int fResult;
	string fMessage;
	};

// =================================================================================================
// MARK: - Higher level parsing

bool sOrganizeRanges(size_t iSourceSize,
	const Val& iRangeParam, std::vector<std::pair<size_t, size_t> >& oRanges);

bool sQReadRequest(
	const ChanR_Bin& iChanR, string* oMethod, string* oURL, string* oErrorString);

bool sQReadResponse(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	int32* oResultCode, string* oResultMessage);

bool sQReadHeaderNoParsing(const ChanR_Bin& iChanR, Map* oFields);
bool sQReadHeaderLineNoParsing(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQReadHeader(const ChanR_Bin& iChanR, Map* oFields);
bool sQReadHeaderLine(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sParseQuery(const string& iString, Map& oTuple);
bool sParseQuery(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map& oTuple);

bool sDecodeComponent(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& oComponent);
ZTrail sDecodeTrail(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU);
ZTrail sDecodeTrail(const string& iURL);

string sEncodeComponent(const string& iString);
string sEncodeTrail(const ZTrail& iTrail);

string sGetString0(const Val& iVal);

ZRef<ChannerR_Bin> sMakeContentChanner(const Map& iHeader, ZRef<ChannerR_Bin> iChannerR);

// =================================================================================================
// MARK: - Request headers

bool sQRead_accept(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_accept_charset(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_accept_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_accept_language(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_authorization(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_from(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_host(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map& oRange);

//bool sQRead_referer(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

// =================================================================================================
// MARK: - Response headers

//bool sQRead_www_authenticate(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

// =================================================================================================
// MARK: - Request or response headers

bool sQRead_transfer_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_transfer_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& oEncoding);

// =================================================================================================
// MARK: - Entity headers

bool sQRead_content_disposition(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_content_disposition(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map& oTuple);

//bool sQRead_content_encoding(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_content_language(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_content_length(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_content_length(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, int64& oLength);

//bool sQRead_content_location(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
//bool sQRead_content_md5(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);

bool sQRead_content_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_content_range(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	int64& oBegin, int64& oEnd, int64& oMaxLength);

bool sQRead_content_type(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, Map* ioFields);
bool sQRead_content_type(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string& oType, string& oSubType, Map& oParameters);

// =================================================================================================

bool sQReadHTTPVersion(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	int32* oVersionMajor, int32* oVersionMinor);

bool sQReadURI(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string* oURI);

bool sQReadFieldName(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oName, string* oNameExact);

bool sQReadParameter(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oName, string* oValue, string* oNameExact);

bool sQReadParameter_Cookie(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oName, string* oValue, string* oNameExact);

bool sQReadMediaType(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oType, string* oSubtype, Map* oParameters,
	string* oTypeExact, string* oSubtypeExact);

bool sQReadLanguageTag(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string* oLanguageTag);

// =================================================================================================
// MARK: - Lower level parsing

bool sParseURL(const string& iURL,
	string* oScheme, string* oHost, uint16* oPort, string* oPath);

bool sQReadToken(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oTokenLC, string* oTokenExact);

bool sQReadToken_Cookie(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oTokenLC, string* oTokenExact);

bool sQReadQuotedString(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU,
	string* oStringLC, string* oStringExact);

bool sTryReadChar(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, char iChar);

bool sQReadChars(const ChanR_Bin& iChanR, const char* iString);

void sSkipLWS(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU);

bool sQReadDecodedChars(const ChanR_Bin& iChanR, const ChanU_Bin& iChanU, string& ioString);

// =================================================================================================
// MARK: - Lexical classification

bool sIs_CHAR(char iChar);
bool sIs_UPALPHA(char iChar);
bool sIs_LOALPHA(char iChar);
bool sIs_ALPHA(char iChar);
bool sIs_DIGIT(char iChar);
bool sIs_CTL(char iChar);
bool sIs_CR(char iChar);
bool sIs_LF(char iChar);
bool sIs_SP(char iChar);
bool sIs_HT(char iChar);
bool sIs_QUOTE(char iChar);

bool sIs_LWS(char iChar);
bool sIs_TEXT(char iChar);
bool sIs_HEX(char iChar);

bool sIs_tspecials(char iChar);
bool sIs_token(char iChar);

bool sIs_ctext(char iChar);

bool sIs_qdtext(char iChar);

// =================================================================================================
// MARK: - Writing

void sWrite_HeaderLine(const string& iName, const string& iBody, const ChanW_Bin& iChanW);
void sWrite_Header(const Map& iHeader, const ChanW_Bin& iChanW);
void sWrite_MinimalResponse(int iResult, const ChanW_Bin& iChanW);
void sWrite_MinimalResponse_ErrorInBody(int iError, const ChanW_Bin& iChanW);

// =================================================================================================
// MARK: - ChanR_Bin_Chunked

class ChanR_Bin_Chunked : public ChanR_Bin
	{
public:
	ChanR_Bin_Chunked(const ChanR_Bin& iChanR);
	virtual ~ChanR_Bin_Chunked();

// From ZStreamR
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

private:
	const ChanR_Bin& fChanR;
	uint64 fChunkSize;
	bool fHitEnd;
	};

// =================================================================================================
// MARK: - ChanW_Bin_Chunked

class ChanW_Bin_Chunked : public ChanW_Bin
	{
public:
	ChanW_Bin_Chunked(size_t iBufferSize, const ChanW_Bin& iChanW);
	ChanW_Bin_Chunked(const ChanW_Bin& iChanW);
	virtual ~ChanW_Bin_Chunked();

// From ZStreamW
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

private:
	void pFlush();

	const ChanW_Bin& fChanW;
	std::vector<byte> fBuffer;
	size_t fBufferUsed;
	};

} // namespace HTTP
} // namespace ZooLib

#endif // __ZooLib_HTTP_h__
