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

#ifndef __ZASCompiler_h__
#define __ZASCompiler_h__ 1
#include "zconfig.h"

#include "zoolib/ZASParser.h"
#include "zoolib/ZStream_Count.h"

#include <vector>
#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - ZASCompiler

class ZASCompiler : public ZASParser::ParseHandler
	{
public:
	ZASCompiler(const ZStreamW& inStream);
	virtual ~ZASCompiler();

	virtual void StartParse();
	virtual void EndParse();

	virtual void EnterName(const std::string& inName, bool iWasInBlock, bool iNowInBlock);
	virtual void ExitName(bool iWasInBlock, bool iNowInBlock);

	virtual void ParsedString(const std::string& inValue);
	virtual void ParsedStringTable(const std::vector<std::string>& inValues);
	virtual void ParsedBinary(const ZStreamR& inStream);
	virtual void ParsedUnion(const std::vector<std::string>& iNames);

protected:
	class NameEntry;
	class Node;

	void AddAsset(const std::vector<std::string>& inNameStack,
		const std::string& inType, const ZStreamR& inStream);

	NameEntry* AllocateName(const std::string& inName);

	ZStreamW_Count fStreamW;

	Node* fRoot;
	std::vector<NameEntry*> fNameEntries;
	std::vector<std::string> fNameStack;
	};

} // namespace ZooLib

#endif // __ZASCompiler_h__
