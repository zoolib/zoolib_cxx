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

#ifndef __ZHTTP_h__
#define __ZHTTP_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZVal_Any.h"

#include <vector>

namespace ZooLib {
namespace ZHTTP {

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
	void SetResult(int iResult, const std::string& iMessage);

	void Set(const std::string& iName, const std::string& iValue);
	void Set(const std::string& iName, int iValue);
	void Set(const std::string& iName, uint64 iValue);

	void Send(const ZStreamW& iStreamW) const;

private:
	bool fIsVersion11;
	std::vector<std::pair<std::string, std::string> > fHeaders;
	int fResult;
	std::string fMessage;
	};

// =================================================================================================
// MARK: - Higher level parsing

bool sOrganizeRanges(size_t iSourceSize,
	const Val& iRangeParam, std::vector<std::pair<size_t, size_t> >& oRanges);

bool sReadRequest(
	const ZStreamR& iStreamR, std::string* oMethod, std::string* oURL, std::string* oErrorString);

bool sReadResponse(const ZStreamU& iStream, int32* oResultCode, std::string* oResultMessage);

bool sReadHeaderNoParsing(const ZStreamR& iStream, Map* oFields);
bool sReadHeaderLineNoParsing(const ZStreamU& iStream, Map* ioFields);

bool sReadHeader(const ZStreamR& iStream, Map* oFields);
bool sReadHeaderLine(const ZStreamU& iStream, Map* ioFields);

bool sParseQuery(const std::string& iString, Map& oTuple);
bool sParseQuery(const ZStreamU& iStream, Map& oTuple);

bool sDecodeComponent(const ZStreamU& s, std::string& oComponent);
ZTrail sDecodeTrail(const ZStreamU& s);
ZTrail sDecodeTrail(const std::string& iURL);

std::string sEncodeComponent(const std::string& iString);
std::string sEncodeTrail(const ZTrail& iTrail);

std::string sGetString0(const Val& iVal);

ZRef<ZStreamerR> sMakeContentStreamer(const Map& iHeader, ZRef<ZStreamerR> iStreamerR);

// =================================================================================================
// MARK: - Request headers

bool sRead_accept(const ZStreamU& iStream, Map* ioFields);
//bool sRead_accept_charset(const ZStreamU& iStream, Map* ioFields); // Not done
//bool sRead_accept_encoding(const ZStreamU& iStream, Map* ioFields); // Not done
bool sRead_accept_language(const ZStreamU& iStream, Map* ioFields);
//bool sRead_authorization(const ZStreamU& iStream, Map* ioFields); // Not done
//bool sRead_from(const ZStreamU& iStream, Map* ioFields); // Not done
//bool sRead_host(const ZStreamU& iStream, Map* ioFields); // Not done

bool sRead_range(const ZStreamU& iStream, Map* ioFields);
bool sRead_range(const ZStreamU& iStream, Map& oRange);

//bool sRead_referer(const ZStreamU& iStream, Map* ioFields); // Not done

// =================================================================================================
// MARK: - Response headers

bool sRead_www_authenticate(const ZStreamU& iStream, Map* ioFields); // Not done

// =================================================================================================
// MARK: - Request or response headers

bool sRead_transfer_encoding(const ZStreamU& iStream, Map* ioFields);
bool sRead_transfer_encoding(const ZStreamU& iStream, std::string& oEncoding);

// =================================================================================================
// MARK: - Entity headers

bool sRead_content_disposition(const ZStreamU& iStream, Map* ioFields);
bool sRead_content_disposition(const ZStreamU& iStream, Map& oTuple);

bool sRead_content_encoding(const ZStreamU& iStream, Map* ioFields); // Not done
bool sRead_content_language(const ZStreamU& iStream, Map* ioFields); // Not done

bool sRead_content_length(const ZStreamU& iStream, Map* ioFields);
bool sRead_content_length(const ZStreamU& iStream, int64& oLength);

bool sRead_content_location(const ZStreamU& iStream, Map* ioFields); // Not done
bool sRead_content_md5(const ZStreamU& iStream, Map* ioFields); // Not done

bool sRead_content_range(const ZStreamU& iStream, Map* ioFields);
bool sRead_content_range(const ZStreamU& iStream, int64& oBegin, int64& oEnd, int64& oMaxLength);

bool sRead_content_type(const ZStreamU& iStream, Map* ioFields);
bool sRead_content_type(
	const ZStreamU& iStream, std::string& oType, std::string& oSubType, Map& oParameters);

// =================================================================================================

bool sReadHTTPVersion(const ZStreamU& iStream, int32* oVersionMajor, int32* oVersionMinor);

bool sReadURI(const ZStreamU& iStream, std::string* oURI);

bool sReadFieldName(const ZStreamU& iStream, std::string* oName, std::string* oNameExact);

bool sReadParameter(
	const ZStreamU& iStream, std::string* oName, std::string* oValue, std::string* oNameExact);

bool sReadParameter_Cookie(
	const ZStreamU& iStream, std::string* oName, std::string* oValue, std::string* oNameExact);

bool sReadMediaType(const ZStreamU& iStream,
	std::string* oType, std::string* oSubtype, Map* oParameters,
	std::string* oTypeExact, std::string* oSubtypeExact);

bool sReadLanguageTag(const ZStreamU& iStream, std::string* oLanguageTag);

// =================================================================================================
// MARK: - Lower level parsing

bool sParseURL(const std::string& iURL,
	std::string* oScheme, std::string* oHost, uint16* oPort, std::string* oPath);

bool sReadToken(const ZStreamU& iStream, std::string* oTokenLC, std::string* oTokenExact);

bool sReadToken_Cookie(const ZStreamU& iStream, std::string* oTokenLC, std::string* oTokenExact);

bool sReadQuotedString(const ZStreamU& iStream, std::string* oStringLC, std::string* oStringExact);

bool sReadChar(const ZStreamU& iStream, char iChar);

bool sReadChars(const ZStreamU& iStream, const char* iString);

void sSkipLWS(const ZStreamU& iStream);

bool sReadDecodedChars(const ZStreamU& iStream, std::string& ioString);

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

void sWrite_HeaderLine(const ZStreamW& w, const std::string& iName, const std::string& iBody);
void sWrite_Header(const ZStreamW& w, const Map& iHeader);
void sWrite_MinimalResponse(const ZStreamW& w, int iResult);
void sWrite_MinimalResponse_ErrorInBody(const ZStreamW& w, int iError);

// =================================================================================================
// MARK: - StreamR_Chunked

class StreamR_Chunked : public ZStreamR
	{
public:
	StreamR_Chunked(const ZStreamR& iStreamSource);
	~StreamR_Chunked();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

private:
	const ZStreamR& fStreamSource;
	uint64 fChunkSize;
	bool fHitEnd;
	};

// =================================================================================================
// MARK: - StreamW_Chunked

class StreamW_Chunked : public ZStreamW
	{
public:
	StreamW_Chunked(size_t iBufferSize, const ZStreamW& iStreamSink);
	StreamW_Chunked(const ZStreamW& iStreamSink);
	~StreamW_Chunked();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

private:
	void pFlush();

	const ZStreamW& fStreamSink;
	std::vector<uint8> fBuffer;
	size_t fBufferUsed;
	};

} // namespace ZHTTP
} // namespace ZooLib

#endif // __ZHTTP_h__
