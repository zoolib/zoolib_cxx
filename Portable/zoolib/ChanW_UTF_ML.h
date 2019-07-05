// Copyright (c) 2019 Andrew Green
// http://www.zoolib.org

#ifndef __ZooLib_ChanW_UTF_ML_h__
#define __ZooLib_ChanW_UTF_ML_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ML.h"

#include <vector>

namespace ZooLib {

/// A write filter strim to help generate well-formed ML-type data (XML, HTML etc).

class ChanW_UTF_ML
:	public ChanW_UTF_Native8
	{
public:
	class Indenter;

	// The copy constructor is deliberately not implemented. See docs for reason.
	ChanW_UTF_ML(const ChanW_UTF_ML&);

	explicit ChanW_UTF_ML(const ChanW_UTF& iStrimSink);

	ChanW_UTF_ML(bool iIndent, const ChanW_UTF& iStrimSink);
	ChanW_UTF_ML(const string8& iEOL, const string8& iIndent, const ChanW_UTF& iStrimSink);
	~ChanW_UTF_ML();

// From ChanW_UTF
	virtual void Flush();

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);

// Our protocol

	/// Close off any pending tag and return the sink stream.
	const ChanW_UTF& Raw() const;

	/// Write an nbsp entity.
	const ChanW_UTF_ML& WriteNBSP() const;
	void NBSP() const { this->WriteNBSP(); }

	/// Write an arbitrary entity.
	const ChanW_UTF_ML& WriteEntity(const string8& iEntity) const;
	const ChanW_UTF_ML& WriteEntity(const UTF8* iEntity) const;

	/// Add a normal begin tag to the top of the stack.
	const ChanW_UTF_ML& Begin(const string8& iTag) const;

	/** \brief Check that the tag on the top of the stack matches iTag,
	and emit the end tag in form </XXX>. */
	const ChanW_UTF_ML& End(const string8& iTag) const;

	/** \brief If the stack is empty trip an error. Otherwise emit
	the tag on the top of the stack in the form </XXX>. */
	const ChanW_UTF_ML& End() const;

	/// Write a tag of the form <XXX a1="xxx" a2="xxx"/>.
	const ChanW_UTF_ML& Empty(const string8& iTag) const;

	/// Write a tag of the form <?XXX a1="xxx" a2="xxx"?>.
	const ChanW_UTF_ML& PI(const string8& iTag) const;

	/** \brief Write a tag of the form <XXX a1="xxx" a2="xxx">, without
	requiring that an end tag be subsequently written. */
	const ChanW_UTF_ML& Tag(const string8& iTag) const;

	/// Add a boolean attribute (one with no value) to the currently pending tag.
	const ChanW_UTF_ML& Attr(const string8& iValue) const;

	/// Add an attribute with the name \a iName and the value \a iValue to the pending tag.
	const ChanW_UTF_ML& Attr(const string8& iName, const string8& iValue) const;

	/// Add an attribute with the name \a iName and the value \a iValue to the pending tag.
	const ChanW_UTF_ML& Attr(const string8& iName, const UTF8* iValue) const;

	/** Add an attribute named \a iName to the currently pending tag. The value
	will be a string containing the base 10 digits of the integer \a iValue. */
	const ChanW_UTF_ML& Attr(const string8& iName, int iValue) const;

	/** Add an attribute with the name \a iName and the printf-formatted
	string iValue etc to the currently pending tag. */
	const ChanW_UTF_ML& Attrf(const string8& iName, const UTF8* iValue, ...) const;

	/** Add attributes to the currently pending tag, taking the names and values from
	properties of iAttrs. String values are added as you would expect, null values
	are added as boolean attributes. This convention is compatible with that
	used by ML::StrimR. */
	const ChanW_UTF_ML& Attrs(const ML::Attrs_t& iAttrs) const;

	/// Set indent enable, and return previous value.
	bool Indent(bool iIndent) const;

	/// Toss any pending tag and its attributes, and clear the stack.
	void Abandon();

	/// Close off any pending tag, emit end tags for everything on the stack and empty the stack.
	const ChanW_UTF_ML& EndAll();

	/// Close off all tags until \a iTag is reached in the stack, or the stack is emptied.
	const ChanW_UTF_ML& EndAll(const string8& iTag);

	/** Close off all tags until \a iTag is reached in the
	stack, but only if \a iTag is in the stack. */
	const ChanW_UTF_ML& EndAllIfPresent(const string8& iTag);

	/** If Begin, Empty, PI or Tag has been called, and no call to Write or End
	has been made, then the tag has not actually been written out as we're
	still allowing attributes to be added. Write any such pending tag, and
	switch to the same state we'd be if we'd called Write. */
	const ChanW_UTF_ML& WritePending() const;

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
#pragma mark - ChanW_UTF_ML::Indenter

class ChanW_UTF_ML::Indenter
	{
public:
	Indenter(ChanW_UTF_ML& ioChanW, bool iIndent);
	~Indenter();

private:
	ChanW_UTF_ML& fChanW;
	bool fPriorIndent;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_UTF_ML_h__
