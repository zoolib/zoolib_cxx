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

#ifndef __ZooLib_ML_h__
#define __ZooLib_ML_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Compat_NonCopyable.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/Channer_UTF.h"

#include <vector>

namespace ZooLib {
namespace ML {

// =================================================================================================
#pragma mark - ML

using std::pair;
using std::string;
using std::vector;

enum EToken
	{
	eToken_Exhausted = -1,
	eToken_Fresh = 0,
	eToken_TagBegin,
	eToken_TagEnd,
	eToken_TagEmpty,
	eToken_Text
	};

typedef pair<string, string> Attr_t;
typedef vector<Attr_t> Attrs_t;

typedef Callable<string(string)> Callable_Entity;

// =================================================================================================
#pragma mark - ML::ChanRU_UTF

/// Tokenizes the ML from a source strim.

class ChanRU_UTF
:	public ZooLib::ChanRU<UTF32>
	{
public:
	ChanRU_UTF(const ZooLib::ChanRU_UTF& iChanRU);
	ChanRU_UTF(const ZooLib::ChanRU_UTF& iChanRU,
		bool iRecognizeEntitiesInAttributeValues, ZRef<Callable_Entity> iCallable);
	~ChanRU_UTF();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);

// Our protocol
//	ZMACRO_operator_bool(ChanU_UTF, operator_bool) const;
	EToken Current() const;
	ChanRU_UTF& Advance();

	const string& Name() const;
	Attrs_t Attrs() const;

	ZQ<string> QAttr(const string& iAttrName) const;
	string Attr(const string& iAttrName) const;

private:
	void pAdvance() const;
	void pAdvance();

	const ZooLib::ChanRU_UTF& fChanRU;

	bool fRecognizeEntitiesInAttributeValues;

	ZRef<Callable_Entity> fCallable;

	string32 fBuffer;
	size_t fBufferStart;

	EToken fToken;

	string fTagName;
	Attrs_t fTagAttributes;
	};

// =================================================================================================
#pragma mark - ML::ChannerRU_UTF

typedef Channer_T<ChanRU_UTF> ChannerRU_UTF;

ZRef<ChannerRU_UTF> sChanner(const ZRef<ZooLib::ChannerRU<UTF32>>& iChanner);

// =================================================================================================
#pragma mark - ML parsing support

void sSkipText(ChanRU_UTF& r);

bool sSkip(ChanRU_UTF& r, const string& iTagName);
bool sSkip(ChanRU_UTF& r, vector<string>& ioTags);

bool sTryRead_Begin(ChanRU_UTF& r, const string& iTagName);

bool sTryRead_Empty(ChanRU_UTF& r, const string& iTagName);

bool sTryRead_End(ChanRU_UTF& r, const string& iTagName);

// =================================================================================================
#pragma mark - ML::StrimW

/// A write filter strim to help generate well-formed ML-type data (XML, HTML etc).

class StrimW
:	public ChanW_UTF_Native8
	{
public:
	class Indenter;

	// The copy constructor is deliberately not implemented. See docs for reason.
	StrimW(const StrimW&);

	explicit StrimW(const ChanW_UTF& iStrimSink);

	StrimW(bool iIndent, const ChanW_UTF& iStrimSink);
	StrimW(const string8& iEOL, const string8& iIndent, const ChanW_UTF& iStrimSink);
	~StrimW();

// From ChanW_UTF
	virtual void Flush();

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);

// Our protocol

	/// Close off any pending tag and return the sink stream.
	const ChanW_UTF& Raw() const;

	/// Write an nbsp entity.
	const StrimW& WriteNBSP() const;
	void NBSP() const { this->WriteNBSP(); }

	/// Write an arbitrary entity.
	const StrimW& WriteEntity(const string8& iEntity) const;
	const StrimW& WriteEntity(const UTF8* iEntity) const;

	/// Add a normal begin tag to the top of the stack.
	const StrimW& Begin(const string8& iTag) const;

	/** \brief Check that the tag on the top of the stack matches iTag,
	and emit the end tag in form </XXX>. */
	const StrimW& End(const string8& iTag) const;

	/** \brief If the stack is empty trip an error. Otherwise emit
	the tag on the top of the stack in the form </XXX>. */
	const StrimW& End() const;

	/// Write a tag of the form <XXX a1="xxx" a2="xxx"/>.
	const StrimW& Empty(const string8& iTag) const;

	/// Write a tag of the form <?XXX a1="xxx" a2="xxx"?>.
	const StrimW& PI(const string8& iTag) const;

	/** \brief Write a tag of the form <XXX a1="xxx" a2="xxx">, without
	requiring that an end tag be subsequently written. */
	const StrimW& Tag(const string8& iTag) const;

	/// Add a boolean attribute (one with no value) to the currently pending tag.
	const StrimW& Attr(const string8& iValue) const;

	/// Add an attribute with the name \a iName and the value \a iValue to the pending tag.
	const StrimW& Attr(const string8& iName, const string8& iValue) const;

	/// Add an attribute with the name \a iName and the value \a iValue to the pending tag.
	const StrimW& Attr(const string8& iName, const UTF8* iValue) const;

	/** Add an attribute named \a iName to the currently pending tag. The value
	will be a string containing the base 10 digits of the integer \a iValue. */
	const StrimW& Attr(const string8& iName, int iValue) const;

	/** Add an attribute with the name \a iName and the printf-formatted
	string iValue etc to the currently pending tag. */
	const StrimW& Attrf(const string8& iName, const UTF8* iValue, ...) const;

	/** Add attributes to the currently pending tag, taking the names and values from
	properties of iAttrs. String values are added as you would expect, null values
	are added as boolean attributes. This convention is compatible with that
	used by ML::StrimR. */
	const StrimW& Attrs(const Attrs_t& iAttrs) const;

	/// Set indent enable, and return previous value.
	bool Indent(bool iIndent) const;

	/// Toss any pending tag and its attributes, and clear the stack.
	void Abandon();

	/// Close off any pending tag, emit end tags for everything on the stack and empty the stack.
	const StrimW& EndAll();

	/// Close off all tags until \a iTag is reached in the stack, or the stack is emptied.
	const StrimW& EndAll(const string8& iTag);

	/** Close off all tags until \a iTag is reached in the
	stack, but only if \a iTag is in the stack. */
	const StrimW& EndAllIfPresent(const string8& iTag);

	/** If Begin, Empty, PI or Tag has been called, and no call to Write or End
	has been made, then the tag has not actually been written out as we're
	still allowing attributes to be added. Write any such pending tag, and
	switch to the same state we'd be if we'd called Write. */
	const StrimW& WritePending() const;

protected:
	enum ETagType
		{ eTagTypeNone, eTagTypeNormal, eTagTypeEmpty, eTagTypePI, eTagTypeNoEnd };

	void pBegin(const string8& iTag, ETagType iTagType);
	void pPreText();
	void pPreTag();
	void pWritePending();
	void pAttr(const string8& iName, string8* iValue);
	void pEnd();

	const ChanW_UTF& fStrimSink;

	ETagType fTagType;
	bool fWrittenSinceLastTag;
	bool fLastWasBegin;
	bool fLastWasEOL;
	bool fIndentEnabled;
	string8 fString_EOL;
	string8 fString_Indent;
	std::vector<string8> fTags;
	std::vector<string8> fAttributeNames;
	std::vector<string8*> fAttributeValues;
	};

// =================================================================================================
#pragma mark - ML::StrimW::Indenter

class StrimW::Indenter
	{
public:
	Indenter(StrimW& iStrimW, bool iIndent);
	~Indenter();

private:
	StrimW& fStrimW;
	bool fPriorIndent;
	};

} // namespace ML
} // namespace ZooLib

#endif // __ZooLib_ML_h__
