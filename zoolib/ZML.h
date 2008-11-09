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

#ifndef __ZML__
#define __ZML__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZTuple.h"

#include <string>
#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZML

namespace ZML {

enum EToken
	{
	eToken_TagBegin,
	eToken_TagEnd,
	eToken_TagEmpty,
	eToken_Text,
	eToken_Exhausted
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZML::Reader

// N.B. It is an implementation detail that Reader inherits
// from ZStrimR, hence the private derivation.

class Reader : private ZStrimR, ZooLib::NonCopyable
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Reader, operator_bool_generator_type, operator_bool_type);
public:
	typedef std::string (*EntityCallback)(void* iRefcon, const std::string& iEntity);

	Reader(const ZStrimU& iStrim);
	Reader(const ZStrimU& iStrim, EntityCallback iCallback, void* iRefcon);
	~Reader();

	operator operator_bool_type() const;
	EToken Current() const;
	ZML::Reader& Advance();

	const std::string& Name() const;
	ZTuple Attrs() const;

	const std::string& TagName() const { return this->Name(); }
	ZTuple TagAttributes() const { return this->Attrs(); }

	const ZStrimR& Text();

private:
// From ZStrimR, to support ZML::Text()'s return of a ZStrimR reference.
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

	void pAdvance();

	const ZStrimU& fStrim;

	EntityCallback fCallback;
	void* fRefcon;

	string32 fBuffer;
	size_t fBufferStart;

	EToken fToken;

	std::string fTagName;
	ZTuple fTagAttributes;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZML parsing support

void sSkipText(Reader& r);

bool sSkip(Reader& r, const std::string& iTagName);
bool sSkip(Reader& r, std::vector<std::string>& ioTags);

bool sTryRead_Begin(Reader& r, const std::string& iTagName);

bool sTryRead_End(Reader& r, const std::string& iTagName);

// =================================================================================================
#pragma mark -
#pragma mark * ZML::StrimR

/// A read strim that returns only the text from a ZML::Reader

class StrimR : public ZStrimR
	{
public:
	StrimR(Reader& iReader);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

// Our protocol
	std::string BackName() const;
	ZTuple BackAttr() const;

	void AllNames(std::vector<std::string>& oNames) const;
	void AllAttrs(std::vector<ZTuple>& oAttrs) const;

	const std::vector<std::pair<std::string, ZTuple> >& All() const;

private:
	Reader& fReader;
	std::vector<std::pair<std::string, ZTuple> > fTags;
	};

} // namespace ZML

#endif // __ZML__
