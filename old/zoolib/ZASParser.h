/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZASParser_h__
#define __ZASParser_h__ 1
#include "zconfig.h"

#include "zoolib/ZRef.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZStrim.h"

#include <vector>

namespace ZooLib {

class ZFileSpec;
class ZStreamerR;

namespace ZASParser {

class ErrorHandler;
class IncludeHandler;
class ParseHandler;
class StreamProvider;

bool sParse(const std::string& iPath, ParseHandler& iParseHandler,
			const StreamProvider& iStreamProvider,
			const ErrorHandler& iErrorHandler, IncludeHandler* iIncludeHandler);

bool sParse(const ZFileSpec& iFileSpec, const ZStreamR& iStream, ParseHandler& iParseHandler,
			const StreamProvider& iStreamProvider,
			const ErrorHandler& iErrorHandler, IncludeHandler* iIncludeHandler);

// =================================================================================================
// MARK: - ParseHandler

class ParseHandler
	{
protected:
	ParseHandler(const ParseHandler&) {}
	ParseHandler& operator=(ParseHandler&) { return *this; }

public:
	ParseHandler() {}
	~ParseHandler() {}

	virtual void StartParse();
	virtual void EndParse();

	virtual void EnterName(const std::string& iName, bool iWasInBlock, bool iNowInBlock);
	virtual void ExitName(bool iWasInBlock, bool iNowInBlock);

	virtual void ParsedString(const std::string& iValue);
	virtual void ParsedStringTable(const std::vector<std::string>& iValues);
	virtual void ParsedBinary(const ZStreamR& iStream);
	virtual void ParsedUnion(const std::vector<std::string>& iNames);

	virtual void EnterInclude(const ZFileSpec& iFileSpec);
	virtual void ExitInclude();

	virtual void EnterCharSet(const std::string& iCharSet, bool iWasInBlock, bool iNowInBlock);
	virtual void ExitCharSet(bool iWasInBlock, bool iNowInBlock);
	};

// =================================================================================================
// MARK: - StreamProvider

class StreamProvider
	{
protected:
	StreamProvider(const StreamProvider&) {}
	StreamProvider& operator=(StreamProvider&) { return *this; }

public:
	StreamProvider() {}
	~StreamProvider() {}

	virtual ZRef<ZStreamerR> ProvideStreamSource(
		const ZFileSpec& iCurrent, const std::string& iPath,
		bool iSearchUserDirectories, ZFileSpec& oFileSpec) const;

	virtual ZRef<ZStreamerR> ProvideStreamBinary(
		const ZFileSpec& iCurrent, const std::string& iPath,
		bool iSearchUserDirectories, ZFileSpec& oFileSpec) const;
	};

// =================================================================================================
// MARK: - ErrorHandler

class ErrorHandler
	{
protected:
	ErrorHandler(const ErrorHandler&) {}
	ErrorHandler& operator=(ErrorHandler&) { return *this; }

public:
	ErrorHandler() {}
	~ErrorHandler() {}

	virtual void ReportError(
		const std::vector<std::pair<ZFileSpec, int> >& iSources,
		const std::string& iMessage) const;

	virtual void ReportError(const std::string& iMessage) const;
	virtual void ReportProgress(int iLinesProcessed) const;
	};

// =================================================================================================
// MARK: - IncludeHandler

class IncludeHandler
	{
protected:
	IncludeHandler() {}
	IncludeHandler(const IncludeHandler&) {}
	~IncludeHandler() {}
	IncludeHandler& operator=(IncludeHandler&) { return *this; }

public:
	virtual void NotifyInclude(const ZFileSpec& iFileSpec) = 0;
	};

// =================================================================================================
// MARK: - ParseHandler_Prettify

class ParseHandler_Prettify : public ZASParser::ParseHandler
	{
public:
	ParseHandler_Prettify(const ZStrimW& iStrimW, bool iDumpBinaries);

	virtual void EnterName(const std::string& iName, bool iWasInBlock, bool iNowInBlock);
	virtual void ExitName(bool iWasInBlock, bool iNowInBlock);

	virtual void ParsedString(const std::string& iValue);
	virtual void ParsedStringTable(const std::vector<std::string>& iValues);
	virtual void ParsedBinary(const ZStreamR& iStream);
	virtual void ParsedUnion(const std::vector<std::string>& iNames);

	virtual void EnterInclude(const ZFileSpec& iFileSpec);
	virtual void ExitInclude();

	virtual void EnterCharSet(const std::string& iCharSet, bool iWasInBlock, bool iNowInBlock);
	virtual void ExitCharSet(bool iWasInBlock, bool iNowInBlock);

protected:
	const ZStrimW& fStrimW;
	bool fDumpBinaries;
	std::vector<std::string> fNames;
	std::vector<std::string> fIncludes;
	std::vector<std::string> fCharSets;
	int fIndent;
	bool fInBlock;
	};

} // namespace ZASParser

} // namespace ZooLib

#endif // __ZASParser_h__
