/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZASParser.h"

#include "zoolib/ZFile.h"
#include "zoolib/ZStreamRWPos_RAM.h"
#include "zoolib/ZStream_Buffered.h"
#include "zoolib/ZStreamR_HexStrim.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrim_CRLF.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Std.h"
#include "zoolib/ZString.h"
#include "zoolib/ZTextCoder.h"
#include "zoolib/ZTextCoder_Unicode.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Strim.h"

#include <stdio.h>

using std::exception;
using std::pair;
using std::string;
using std::vector;

namespace ZooLib {

namespace { // anonymous

// =================================================================================================
// MARK: - Static helper functions

static bool spAtEnd(ZStrimU& iStrimU)
	{
	UTF32 theCP;
	if (!iStrimU.ReadCP(theCP))
		return true;
	iStrimU.Unread(theCP);
	return false;
	}

// =================================================================================================
// MARK: - DecoderSetter

class DecoderSetter
	{
public:
	DecoderSetter(ZStrimU_Std& iStrim, ZTextDecoder* iDecoder);
	~DecoderSetter();

private:
	ZStrimU_Std& fStrim;
	ZTextDecoder* fPrior;
	};

DecoderSetter::DecoderSetter(ZStrimU_Std& iStrim, ZTextDecoder* iDecoder)
:	fStrim(iStrim)
	{
	fPrior = fStrim.SetDecoderReturnOld(iDecoder);
	}

DecoderSetter::~DecoderSetter()
	{
	delete fStrim.SetDecoderReturnOld(fPrior);
	}

// =================================================================================================
// MARK: - Parser

class NestedParse : public std::exception
	{
public:
	NestedParse(const string& iMessage, const ZFileSpec& iFileSpec, int iLineNumber);
	~NestedParse() throw();

	void Append(const ZFileSpec& iFileSpec, int iLineNumber);

	const string& GetMessage() const { return fMessage; }
	const vector<pair<ZFileSpec, int> >& GetSources() const { return fSources; }

private:
	string fMessage;
	vector<pair<ZFileSpec, int> > fSources;
	};

NestedParse::NestedParse(const string& iMessage, const ZFileSpec& iFileSpec, int iLineNumber)
:	fMessage(iMessage),
	fSources(1, pair<ZFileSpec, int>(iFileSpec, iLineNumber))
	{}

NestedParse::~NestedParse() throw()
	{}

void NestedParse::Append(const ZFileSpec& iFileSpec, int iLineNumber)
	{
	fSources.push_back(pair<ZFileSpec, int>(iFileSpec, iLineNumber));
	}

class Parser
	{
public:
	Parser(ZASParser::ParseHandler& iParseHandler,
			const ZASParser::StreamProvider& iStreamProvider,
			const ZASParser::ErrorHandler& iErrorHandler,
			ZASParser::IncludeHandler* iIncludeHandler);
	~Parser();

	void Parse(const ZFileSpec& iFileSpec, const ZStreamR& iStream);

	void Parse();

	bool TryParseStatement();

	void ParseNameBody(const string& inName);

	void ParseStringTable();

	void ParseBinary();

	void ParseUnion();

	void ParseCharSet();

	bool TryParseQuotedString(string& oString);

	bool TryParseStringBody(string& oString);

protected:
	ZStrimU_Std* fStrimU;
	ZFileSpec fFileSpec;
	int fCumulativeLineCount;
	bool fInBlock;

	ZASParser::ParseHandler& fParseHandler;
	const ZASParser::StreamProvider& fStreamProvider;
	const ZASParser::ErrorHandler& fErrorHandler;
	ZASParser::IncludeHandler* fIncludeHandler;
	};

Parser::Parser(ZASParser::ParseHandler& iParseHandler,
			const ZASParser::StreamProvider& iStreamProvider,
			const ZASParser::ErrorHandler& iErrorHandler,
			ZASParser::IncludeHandler* iIncludeHandler)
:	fStrimU(nullptr),
	fCumulativeLineCount(0),
	fInBlock(false),
	fParseHandler(iParseHandler),
	fStreamProvider(iStreamProvider),
	fErrorHandler(iErrorHandler),
	fIncludeHandler(iIncludeHandler)
	{}

Parser::~Parser()
	{}

void Parser::Parse(const ZFileSpec& iFileSpec, const ZStreamR& iStream)
	{
	using namespace ZUtil_Strim;

	// We start off with an auto-detecting unicode decoder. So we'll try
	// to read a UTF16 BE or LE BOM off the stream. If that's not detected we'll
	// treat the stream as being UTF8, at least until a charset statement
	// is encountered specifying something else.

	ZStrimU_Std theStrimU(new ZTextDecoder_Unicode_AutoDetect, iStream);

	ZStrimU_Std* priorStrimU = fStrimU;
	fStrimU = &theStrimU;

	ZFileSpec priorFileSpec = fFileSpec;
	fFileSpec = iFileSpec;

	try
		{
		try
			{
			this->Parse();
			}
		catch (NestedParse& theEx)
			{
			theEx.Append(iFileSpec, theStrimU.GetLineCount());
			printf("EX1\n");
			throw;
			}
		catch (exception& ex)
			{
			printf("EX2\n");
			throw NestedParse(ex.what(), iFileSpec, theStrimU.GetLineCount());
			}
		}
	catch (...)
		{
		printf("EX3\n");
		fStrimU = priorStrimU;
		fFileSpec = priorFileSpec;
		throw;
		}

	fCumulativeLineCount += theStrimU.GetLineCount();
	fStrimU = priorStrimU;
	fFileSpec = priorFileSpec;
	}

void Parser::Parse()
	{
	using namespace ZUtil_Strim;

	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(*fStrimU);
		if (spAtEnd(*fStrimU))
			return;

		if (!this->TryParseStatement())
			throw ParseException("Expected a top-level statement");
		}
	}

bool Parser::TryParseStatement()
	{
	using namespace ZUtil_Strim;

	fErrorHandler.ReportProgress(fCumulativeLineCount + fStrimU->GetLineCount());

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	string theIdentifier;
	if (!sTryRead_Identifier(*fStrimU, theIdentifier))
		{
		// We didn't see an identifier. It might be a statement without a "name",
		// "directory" or "dir" prefix.
		string name;
		if (!sTryRead_EscapedString(*fStrimU, '"', name))
			return false;
	
		this->ParseNameBody(name);
		}
	else
		{
		theIdentifier = ZUnicode::sToLower(theIdentifier);

		if (theIdentifier == "name" || theIdentifier == "directory" || theIdentifier == "dir")
			{
			sSkip_WSAndCPlusPlusComments(*fStrimU);
	
			string name;
			if (!sTryRead_EscapedString(*fStrimU, '"', name))
				throw ParseException("Expected a quoted string");
	
			this->ParseNameBody(name);
			}
		else if (theIdentifier == "string")
			{
			sSkip_WSAndCPlusPlusComments(*fStrimU);
			string theString;
			if (!this->TryParseStringBody(theString))
				throw ParseException("Expected a valid string body");
			fParseHandler.ParsedString(theString);
			}
		else if (theIdentifier == "stringtable")
			{
			this->ParseStringTable();
			}
		else if (theIdentifier == "binary")
			{
			this->ParseBinary();
			}
		else if (theIdentifier == "union" || theIdentifier == "alias")
			{
			this->ParseUnion();
			}
		else if (theIdentifier == "charset")
			{
			this->ParseCharSet();
			}
		else if (theIdentifier == "include")
			{
			sSkip_WSAndCPlusPlusComments(*fStrimU);
			string path;
			if (!sTryRead_EscapedString(*fStrimU, '"', path))
				throw ParseException("Expected a quoted file name");
	
			ZFileSpec specUsed;
			if (ZRef<ZStreamerR> theStreamer
				= fStreamProvider.ProvideStreamSource(fFileSpec, path, true, specUsed))
				{
				if (fIncludeHandler)
					fIncludeHandler->NotifyInclude(specUsed);
				fParseHandler.EnterInclude(specUsed);
				this->Parse(specUsed, theStreamer->GetStreamR());
				fParseHandler.ExitInclude();
				}
			else
				{
				throw ParseException("Could not open file \"" + path + "\"");
				}
			}
		else
			{
			throw ParseException("Unrecognized keyword \"" + theIdentifier +"\"");
			}
		}
	
	sSkip_WSAndCPlusPlusComments(*fStrimU);

	// Allow an optional statement terminator.
	sTryRead_CP(*fStrimU, ';');
	return true;
	}

void Parser::ParseNameBody(const string& inName)
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	if (sTryRead_CP(*fStrimU, '{'))
		{
		bool wasInBlock = fInBlock;
		fInBlock = true;
		fParseHandler.EnterName(inName, wasInBlock, true);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(*fStrimU);
			if (spAtEnd(*fStrimU))
				break;

			if (!this->TryParseStatement())
				break;
			}
		fParseHandler.ExitName(true, wasInBlock);
		fInBlock = wasInBlock;

		if (!sTryRead_CP(*fStrimU, '}'))
			throw ParseException("Expected '}'");
		}
	else
		{
		bool wasInBlock = fInBlock;
		fInBlock = false;

		fParseHandler.EnterName(inName, wasInBlock, false);

		if (!this->TryParseStatement())
			throw ParseException("Expected a statement after the name");

		fParseHandler.ExitName(false, wasInBlock);

		fInBlock = wasInBlock;
		}
	}

void Parser::ParseStringTable()
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	if (!sTryRead_CP(*fStrimU, '{'))
		throw ParseException("Expected '{'");

	vector<string> strings;
	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(*fStrimU);
		string currentString;
		if (!this->TryParseStringBody(currentString))
			break;
		sSkip_WSAndCPlusPlusComments(*fStrimU);
		if (!sTryRead_CP(*fStrimU, ',') && !sTryRead_CP(*fStrimU, ';'))
			break;
		strings.push_back(currentString);
		}

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	if (!sTryRead_CP(*fStrimU, '}'))
		throw ParseException("Expected '}'");

	fParseHandler.ParsedStringTable(strings);
	}

void Parser::ParseBinary()
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	if (sTryRead_CP(*fStrimU, '{'))
		{
		fParseHandler.ParsedBinary(ZStreamR_HexStrim(true, *fStrimU));

		sSkip_WSAndCPlusPlusComments(*fStrimU);

		if (!sTryRead_CP(*fStrimU, '}'))
			throw ParseException("Expected '}'");
		}
	else
		{
		string fileName;
		if (!sTryRead_EscapedString(*fStrimU, '"', fileName))
			throw ParseException("Expected a quoted file name");

		ZFileSpec fileSpecUsed;
		ZRef<ZStreamerR> theStreamer
			= fStreamProvider.ProvideStreamBinary(fFileSpec, fileName, true, fileSpecUsed);
		if (!theStreamer)
			{
			throw ParseException(
				string("Could not open file \"") + fileName + string("\" to read binary data"));
			}

		if (fIncludeHandler)
			fIncludeHandler->NotifyInclude(fileSpecUsed);

		fParseHandler.ParsedBinary(theStreamer->GetStreamR());
		}
	}

void Parser::ParseUnion()
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	vector<string> names;

	if (sTryRead_CP(*fStrimU, '{'))
		{
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(*fStrimU);
			string currentName;
			if (!sTryRead_EscapedString(*fStrimU, '"', currentName))
				break;
			names.push_back(currentName);
			sSkip_WSAndCPlusPlusComments(*fStrimU);
			if (!sTryRead_CP(*fStrimU, ',') && !sTryRead_CP(*fStrimU, ';'))
				break;
			}
		sSkip_WSAndCPlusPlusComments(*fStrimU);

		if (!sTryRead_CP(*fStrimU, '}'))
			throw ParseException("Expected '}'");
		}
	else
		{
		string theName;
		if (!sTryRead_EscapedString(*fStrimU, '"', theName))
			throw ParseException("Expected a quoted name");

		names.push_back(theName);
		}

	fParseHandler.ParsedUnion(names);
	}

void Parser::ParseCharSet()
	{
	using namespace ZUtil_Strim;

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	string charSetName;
	if (!sTryRead_EscapedString(*fStrimU, '"', charSetName))
		throw ParseException("Expected a quoted string for charset");

	ZTextDecoder* theTextDecoder = ZTextDecoder::sMake(charSetName);
	if (theTextDecoder == nullptr)
		throw ParseException("Unsupported charset \"" + charSetName + "\"");

	DecoderSetter theSetter(*fStrimU, theTextDecoder);

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	if (sTryRead_CP(*fStrimU, '{'))
		{
		bool wasInBlock = fInBlock;
		fInBlock = true;
		fParseHandler.EnterCharSet(charSetName, wasInBlock, true);

		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(*fStrimU);
			if (!this->TryParseStatement())
				break;
			}

		fParseHandler.ExitCharSet(true, wasInBlock);
		fInBlock = wasInBlock;

		if (!sTryRead_CP(*fStrimU, '}'))
			throw ParseException("Expected a '}'");
		}
	else
		{
		sSkip_WSAndCPlusPlusComments(*fStrimU);

		bool wasInBlock = fInBlock;
		fInBlock = false;
		fParseHandler.EnterCharSet(charSetName, wasInBlock, false);

		if (!this->TryParseStatement())
			throw ParseException("#3 Expected a statement");

		fParseHandler.ExitCharSet(false, wasInBlock);
		fInBlock = wasInBlock;
		}
	}

bool Parser::TryParseStringBody(string& oString)
	{
	using namespace ZUtil_Strim;

	oString.resize(0);

	sSkip_WSAndCPlusPlusComments(*fStrimU);

	string curString;
	if (!sTryRead_EscapedString(*fStrimU, '"', curString))
		return false;

	oString = curString;
	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(*fStrimU);
		if (!sTryRead_EscapedString(*fStrimU, '"', curString))
			return true;
		oString += curString;
		}
	}

} // anonymous namespace

// =================================================================================================
// MARK: - ZASParser::sParse

/**
This is the entry point for parsing when you have a source file's path. It just
attempts to open the file and passes it on to the stream-taking sParse, which is
responsible for actually catching and reporting parse exceptions.
*/
bool ZASParser::sParse(const string& iPath, ParseHandler& iParseHandler,
			const StreamProvider& iStreamProvider,
			const ErrorHandler& iErrorHandler, IncludeHandler* iIncludeHandler)
	{
	ZFileSpec fileSpecUsed;
	if (ZRef<ZStreamerR> theStreamer
		= iStreamProvider.ProvideStreamSource(ZFileSpec(), iPath, true, fileSpecUsed))
		{
		return sParse(fileSpecUsed, theStreamer->GetStreamR(),
						iParseHandler, iStreamProvider, iErrorHandler, iIncludeHandler);
		}
	else
		{
		iErrorHandler.ReportError("Could not open source file \"" + iPath + "\"");
		}
	return false;
	}

/**
This is the entry point for parsing when you have source stream and the textual name
of that stream, and is also called
*/
bool ZASParser::sParse(
	const ZFileSpec& iFileSpec, const ZStreamR& iStream, ParseHandler& iParseHandler,
	const StreamProvider& iStreamProvider,
	const ErrorHandler& iErrorHandler, IncludeHandler* iIncludeHandler)
	{
	try
		{
		Parser theParser(iParseHandler, iStreamProvider, iErrorHandler, iIncludeHandler);
		iParseHandler.StartParse();
		theParser.Parse(iFileSpec, iStream);
		iParseHandler.EndParse();
		return true;
		}
	catch (NestedParse& ex)
		{
		printf("EX4\n");
		iErrorHandler.ReportError(ex.GetSources(), ex.GetMessage());
		}		
	catch (exception& ex)
		{
		printf("EX5\n");
		iErrorHandler.ReportError(ex.what());
		}
	catch (...)
		{
		iErrorHandler.ReportError("Unknown exception");
		}
	return false;
	}

// =================================================================================================
// MARK: - ZASParser::ParseHandler

void ZASParser::ParseHandler::StartParse()
	{}

void ZASParser::ParseHandler::EndParse()
	{}

void ZASParser::ParseHandler::EnterName(const string& iName, bool iWasInBlock, bool iNowInBlock)
	{}

void ZASParser::ParseHandler::ExitName(bool iWasInBlock, bool iNowInBlock)
	{}

void ZASParser::ParseHandler::ParsedString(const string& iValue)
	{}

void ZASParser::ParseHandler::ParsedStringTable(const vector<string>& iValues)
	{}

void ZASParser::ParseHandler::ParsedBinary(const ZStreamR& iStream)
	{
	// Just suck up and ignore the data
	iStream.SkipAll(nullptr);
	}

void ZASParser::ParseHandler::ParsedUnion(const vector<string>& iNames)
	{}

void ZASParser::ParseHandler::EnterInclude(const ZFileSpec& iFileSpec)
	{}

void ZASParser::ParseHandler::ExitInclude()
	{}

void ZASParser::ParseHandler::EnterCharSet(
	const string& iCharSet, bool iWasInBlock, bool iNowInBlock)
	{}

void ZASParser::ParseHandler::ExitCharSet(bool iWasInBlock, bool iNowInBlock)
	{}

// =================================================================================================
// MARK: - ZASParser::StreamProvider

ZRef<ZStreamerR> ZASParser::StreamProvider::ProvideStreamSource(
	const ZFileSpec& iCurrent, const string& iPath,
	bool iSearchUserDirectories, ZFileSpec& oFileSpec) const
	{ return ZRef<ZStreamerR>(); }

ZRef<ZStreamerR> ZASParser::StreamProvider::ProvideStreamBinary(
	const ZFileSpec& iCurrent, const string& iPath,
	bool iSearchUserDirectories, ZFileSpec& oFileSpec) const
	{ return ZRef<ZStreamerR>(); }

// =================================================================================================
// MARK: - ZASParser::ErrorHandler

void ZASParser::ErrorHandler::ReportError(
	const vector<pair<ZFileSpec, int> >& iSources, const string& iMessage) const
	{}

void ZASParser::ErrorHandler::ReportError(const string& iMessage) const
	{}

void ZASParser::ErrorHandler::ReportProgress(int iLinesProcessed) const
	{}

// =================================================================================================
// MARK: - ZASParser::ParseHandler_Prettify

static void spWriteIndent(const ZStrimW& iStrimW, int iIndent)
	{
	ZAssert(iIndent >= 0);
	for (int x = 0; x < iIndent; ++x)
		iStrimW.WriteCP('\t');
	}

static void spWriteEscapifiedString(const ZStrimW& iStrimW, const string& iString)
	{
	ZStrimW_Escaped(iStrimW).Write(iString);
	}

static void spWriteEscapifiedStringIndented(
	const ZStrimW& iStrimW, const string& iString, int iIndent)
	{
	string eol = "\"\n";
	eol += string(iIndent, '\t');
	eol += "\"";
	ZStrimW_Escaped::Options theOptions;
	theOptions.fEOL = eol;
	ZStrimW_Escaped(theOptions, iStrimW).Write(iString);
	}

ZASParser::ParseHandler_Prettify::ParseHandler_Prettify(const ZStrimW& iStrimW, bool iDumpBinaries)
:	fStrimW(iStrimW),
	fDumpBinaries(iDumpBinaries),
	fIndent(0),
	fInBlock(false)
	{}

void ZASParser::ParseHandler_Prettify::EnterName(
	const string& iName, bool iWasInBlock, bool iNowInBlock)
	{
	if (iWasInBlock)
		spWriteIndent(fStrimW, fIndent);
	fStrimW.Write("name \"");
	spWriteEscapifiedString(fStrimW, iName);
	fStrimW.Write("\"");

	fNames.push_back(iName);
	if (iNowInBlock)
		{
		++fIndent;
		fStrimW.Write("\n");
		spWriteIndent(fStrimW, fIndent);
		fStrimW.Write("{\n");
		}
	else
		{
		fStrimW.Write(" ");
		}
	fInBlock = iNowInBlock;
	}

void ZASParser::ParseHandler_Prettify::ExitName(bool iWasInBlock, bool iNowInBlock)
	{
	if (iWasInBlock)
		{
		spWriteIndent(fStrimW, fIndent);
		fStrimW.Write("} // name \"");
		spWriteEscapifiedString(fStrimW, fNames.back());
		fStrimW.Write("\"\n");
		--fIndent;
		}
	fInBlock = iNowInBlock;
	fNames.pop_back();
	}

void ZASParser::ParseHandler_Prettify::ParsedString(const string& iValue)
	{
	if (fInBlock)
		spWriteIndent(fStrimW, fIndent);
	fStrimW.Write("string \"");
	spWriteEscapifiedStringIndented(fStrimW, iValue, fIndent);
	fStrimW.Write("\"\n");
	}

void ZASParser::ParseHandler_Prettify::ParsedStringTable(const vector<string>& iValues)
	{
	if (fInBlock)
		spWriteIndent(fStrimW, fIndent);

	fStrimW.Write("stringtable\n");

	spWriteIndent(fStrimW, fIndent + 1);
	fStrimW.Write("{\n");

	for (size_t x = 0; x < iValues.size(); ++x)
		{
		spWriteIndent(fStrimW, fIndent + 1);
		fStrimW.Write("\"");
		spWriteEscapifiedStringIndented(fStrimW, iValues[x], fIndent);
		fStrimW.Write("\"");
		if (x != iValues.size() - 1)
			fStrimW.Write(",");
		fStrimW.Write("\n");
		}
	spWriteIndent(fStrimW, fIndent + 1);
	fStrimW.Writef("} // (%d entries)\n", iValues.size());
	}

void ZASParser::ParseHandler_Prettify::ParsedBinary(const ZStreamR& iStream)
	{
	if (fDumpBinaries)
		{
		if (fInBlock)
			spWriteIndent(fStrimW, fIndent);

		// Let's see if we've got more than 16 bytes. We only have a read
		// stream, so we can't _ask_ how many bytes would be physically
		// readable (CountReadable only provides a lower limit, and
		// can return zero even when there's data available). So we
		// have to actually suck it and see. We use a ZStreamR_DynamicBuffered,
		// which lets us read the stream, then return the read bytes to
		// be re-read if it turns out we've got enough to warrant doing
		// indentation.

		ZStreamRWPos_RAM theStreamRAM;
		ZStreamR_DynamicBuffered theStream(iStream, theStreamRAM);
		uint64 countSkipped;
		theStream.Skip(17, &countSkipped);
		theStream.Rewind();
		theStream.Commit();

		if (countSkipped <= 16)
			{
			// We've got 16 or fewer bytes, emit them on the same line.
			fStrimW.Write("binary { ");	
			ZStreamW_HexStrim(" ", "", 16, fStrimW).CopyAllFrom(theStream, nullptr, nullptr);	
			fStrimW.Write(" }\n");
			}
		else
			{
			// We've got more than 16 bytes, break them up into nice lines.
			fStrimW.Write("binary\n");
	
			spWriteIndent(fStrimW, fIndent + 1);
			fStrimW.Write("{");
	
			uint64 size;
			string chunkSeparator = "\n" + string(fIndent + 1, '\t');
			ZStreamW_HexStrim(" ", chunkSeparator, 16, fStrimW)
				.CopyAllFrom(theStream, &size, nullptr);	
	
			fStrimW.Write("\n");
			spWriteIndent(fStrimW, fIndent + 1);
			fStrimW.Writef("} // %lld bytes\n", size);
			}
		}
	else
		{
		uint64 size;
		iStream.SkipAll(&size);

		if (fInBlock)
			spWriteIndent(fStrimW, fIndent);

		fStrimW.Writef("binary { /* content not shown */ } // %d bytes\n", size);
		}
	}

void ZASParser::ParseHandler_Prettify::ParsedUnion(const vector<string>& iNames)
	{
	if (fInBlock)
		spWriteIndent(fStrimW, fIndent);

	fStrimW.Write("union\n");

	spWriteIndent(fStrimW, fIndent + 1);
	fStrimW.Write("{\n");

	for (size_t x = 0; x < iNames.size(); ++x)
		{
		spWriteIndent(fStrimW, fIndent + 1);
		fStrimW.Write("\"");
		spWriteEscapifiedString(fStrimW, iNames[x]);
		fStrimW.Write("\"");
		if (x != iNames.size() - 1)
			fStrimW.Write(",");
		fStrimW.Write("\n");
		}
	spWriteIndent(fStrimW, fIndent + 1);
	fStrimW.Writef("} // (%d entries)\n", iNames.size());
	}

void ZASParser::ParseHandler_Prettify::EnterInclude(const ZFileSpec& iFileSpec)
	{
	spWriteIndent(fStrimW, fIndent);
	fStrimW.Write("// -----> enter file \"");
	string fileAsString = iFileSpec.AsString();
	spWriteEscapifiedString(fStrimW, fileAsString);
	fStrimW.Write("\"\n");

	fIncludes.push_back(fileAsString);
	}

void ZASParser::ParseHandler_Prettify::ExitInclude()
	{
	spWriteIndent(fStrimW, fIndent);
	fStrimW.Write("// <----- exit file \"");
	spWriteEscapifiedString(fStrimW, fIncludes.back());
	fStrimW.Write("\"\n");

	fIncludes.pop_back();
	}

void ZASParser::ParseHandler_Prettify::EnterCharSet(
	const string& iCharSet, bool iWasInBlock, bool iNowInBlock)
	{
	// We don't emit any charset designator -- we're dumping everything as unicode.
	// However we do need to handle indentation.
	if (iWasInBlock && !iNowInBlock)
		spWriteIndent(fStrimW, fIndent);
	fInBlock = iNowInBlock;
	}

void ZASParser::ParseHandler_Prettify::ExitCharSet(bool iWasInBlock, bool iNowInBlock)
	{
	fInBlock = iNowInBlock;
	}

} // namespace ZooLib
