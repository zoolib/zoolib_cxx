/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZUtil_Strim_TQL.h"
#include "zoolib/ZUtil_Strim_TQL_Spec.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_Strim_TQL {

using std::set;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Options

Options::Options()
:	fEOLString("\n"),
	fIndentString("  ")
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static void sWrite(const ZStrimW& s, const string& iString)
	{ s.Write(iString); }

static void sWrite(const ZStrimW& s, const UTF8* iString)
	{ s.Write(iString); }

static void sWrite_Indent(const ZStrimW& iStrimW,
	size_t iCount, const Options& iOptions)
	{
	while (iCount--)
		sWrite(iStrimW, iOptions.fIndentString);
	}

static void sWrite_LFIndent(const ZStrimW& iStrimW,
	size_t iCount, const Options& iOptions)
	{
	sWrite(iStrimW, iOptions.fEOLString);
	sWrite_Indent(iStrimW, iCount, iOptions);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Writer

namespace ZANONYMOUS {

class Writer : public NodeVisitor
	{
public:
	Writer(const ZStrimW& iStrimW, size_t iIndent, const Options& iOptions);

// From NodeVisitor
	virtual bool Visit_All(ZRef<Node_All> iNode);
	virtual bool Visit_Difference(ZRef<Node_Difference> iNode);
	virtual bool Visit_Explicit(ZRef<Node_Explicit> iNode);
	virtual bool Visit_Intersect(ZRef<Node_Intersect> iNode);
	virtual bool Visit_Join(ZRef<Node_Join> iNode);
	virtual bool Visit_Project(ZRef<Node_Project> iNode);
	virtual bool Visit_Rename(ZRef<Node_Rename> iNode);
	virtual bool Visit_Restrict(ZRef<Node_Restrict> iNode);
	virtual bool Visit_Select(ZRef<Node_Select> iNode);
	virtual bool Visit_Union(ZRef<Node_Union> iNode);

	void Write(ZRef<Node> iNode);

private:
	bool pWriteDyadic(const string& iFunctionName, ZRef<Node_Dyadic> iNode);

	const ZStrimW& fStrimW;
	size_t fIndent;
	const Options& fOptions;
	};

} // anonymous namespace

static void sWrite_EffectiveRelHeadComment(const ZStrimW& s, ZRef<Node> iNode)
	{
	s.Write(" // ");

	set<ZTName> names;
	iNode->GetEffectiveRelHead().GetNames(names);
	bool isFirst = true;
	for (set<ZTName>::iterator i = names.begin(); i != names.end(); ++i)
		{
		if (!isFirst)
			s.Write(", ");
		isFirst = false;
		ZUtil_Strim_Tuple::sWrite_PropName(s, *i);
		}
	}

static void sWrite(const ZStrimW& s,
	size_t iIndent, const Options& iOptions,
	ZRef<Node> iNode)
	{
	sWrite_LFIndent(s, iIndent, iOptions);

//	sWrite_EffectiveRelHeadComment(s, iNode);

//	sWrite_LFIndent(s, iIndent, iOptions);
	Writer(s, iIndent, iOptions).Write(iNode);
	}

Writer::Writer(const ZStrimW& iStrimW, size_t iIndent, const Options& iOptions)
:	fStrimW(iStrimW),
	fIndent(iIndent),
	fOptions(iOptions)
	{}

void Writer::Write(ZRef<Node> iNode)
	{ iNode->Accept(*this); }

bool Writer::pWriteDyadic(const string& iFunctionName, ZRef<Node_Dyadic> iNode)
	{
	ZRef<Node> theNodeA = iNode->GetNodeA();
	ZRef<Node> theNodeB = iNode->GetNodeB();
	sWrite(fStrimW, iFunctionName);
	sWrite_EffectiveRelHeadComment(fStrimW, iNode);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, "(");
	sWrite(fStrimW, fIndent + 1, fOptions, theNodeA);
	sWrite(fStrimW, ", ");
	sWrite(fStrimW, fIndent + 1, fOptions, theNodeB);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_All(ZRef<Node_All> iNode)
	{
	const ZTName theIDPropName = iNode->GetIDPropName();
	if (theIDPropName.Empty())
		{
		sWrite(fStrimW, "All(");
		}
	else
		{
		sWrite(fStrimW, "AID(");
		sWrite_PropName(fStrimW, theIDPropName);
		sWrite(fStrimW, ", ");
		}
	sWrite_RelHead(fStrimW, iNode->GetRelHead());
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Difference(ZRef<Node_Difference> iNode)
	{ return this->pWriteDyadic("Difference", iNode); }

bool Writer::Visit_Explicit(ZRef<Node_Explicit> iNode)
	{
	sWrite(fStrimW, "Explicit(");
	bool isFirst = true;
	const vector<ZTuple> theTuples = iNode->GetTuples();
	for (vector<ZTuple>::const_iterator i = theTuples.begin(); i != theTuples.end(); ++i)
		{
		if (!isFirst)
			sWrite(fStrimW, ", ");
		isFirst = false;

		fStrimW << *i;
		}
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Intersect(ZRef<Node_Intersect> iNode)
	{ return this->pWriteDyadic("Intersect", iNode); }

bool Writer::Visit_Join(ZRef<Node_Join> iNode)
	{
	ZRef<Node> theNodeA = iNode->GetNodeA();
	ZRef<Node> theNodeB = iNode->GetNodeB();
	const RelHead joinOn = theNodeA->GetEffectiveRelHead()
		& theNodeB->GetEffectiveRelHead();

	sWrite(fStrimW, "Join");
	sWrite_EffectiveRelHeadComment(fStrimW, iNode);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, "( // Join on: ");
	sWrite_RelHead(fStrimW, joinOn);
	sWrite(fStrimW, fIndent + 1, fOptions, theNodeA);
	sWrite(fStrimW, ", ");
	sWrite(fStrimW, fIndent + 1, fOptions, theNodeB);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Project(ZRef<Node_Project> iNode)
	{
	sWrite(fStrimW, "Project");
	sWrite_EffectiveRelHeadComment(fStrimW, iNode);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, "(");
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite_RelHead(fStrimW, iNode->GetRelHead());
	sWrite(fStrimW, ",");
	sWrite(fStrimW, fIndent + 1, fOptions, iNode->GetNode());
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Rename(ZRef<Node_Rename> iNode)
	{
	sWrite(fStrimW, "Rename");
	sWrite_EffectiveRelHeadComment(fStrimW, iNode);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, "(");
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite_PropName(fStrimW, iNode->GetOld());
	sWrite(fStrimW, ", ");
	sWrite_PropName(fStrimW, iNode->GetNew());
	sWrite(fStrimW, ",");
	sWrite(fStrimW, fIndent + 1, fOptions, iNode->GetNode());
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Restrict(ZRef<Node_Restrict> iNode)
	{
	sWrite(fStrimW, "Restrict");
	sWrite_EffectiveRelHeadComment(fStrimW, iNode);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, "(");
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	ZUtil_Strim_TQL_Spec::sToStrim(fStrimW, iNode->GetCondition());
	sWrite(fStrimW, ",");
	sWrite(fStrimW, fIndent + 1, fOptions, iNode->GetNode());
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Select(ZRef<Node_Select> iNode)
	{
	sWrite(fStrimW, "Select");
	sWrite_EffectiveRelHeadComment(fStrimW, iNode);
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, "(");
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	ZUtil_Strim_TQL_Spec::sToStrim(fStrimW, iNode->GetLogOp());
	sWrite(fStrimW, ",");
	sWrite(fStrimW, fIndent + 1, fOptions, iNode->GetNode());
	sWrite_LFIndent(fStrimW, fIndent + 1, fOptions);
	sWrite(fStrimW, ")");
	return true;
	}

bool Writer::Visit_Union(ZRef<Node_Union> iNode)
	{ return this->pWriteDyadic("Union", iNode); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZStrimW& iStrimW, const Query& iQuery)
	{ sToStrim(iStrimW, 0, Options(), iQuery.GetNode()); }

void sToStrim(const ZStrimW& iStrimW, ZRef<Node> iNode)
	{ sToStrim(iStrimW, 0, Options(), iNode); }

void sToStrim(const ZStrimW& iStrimW,
	size_t iInitialIndent, const Options& iOptions,
	const Query& iQuery)
	{
	sWrite(iStrimW, iInitialIndent, iOptions, iQuery.GetNode());
	}

void sToStrim(const ZStrimW& iStrimW,
	size_t iInitialIndent, const Options& iOptions,
	ZRef<Node> iNode)
	{
	sWrite(iStrimW, iInitialIndent, iOptions, iNode);
	}

void sWrite_PropName(const ZStrimW& s, const ZTName& iTName)
	{
	s.Write("@");
	ZUtil_Strim_Tuple::sWrite_PropName(s, iTName);
	}

void sWrite_RelHead(const ZStrimW& s, const RelHead& iRelHead)
	{
	set<ZTName> names;
	iRelHead.GetNames(names);
	bool isFirst = true;
	s.Write("[");
	for (set<ZTName>::iterator i = names.begin(); i != names.end(); ++i)
		{
		if (!isFirst)
			s.Write(", ");
		isFirst = false;
		sWrite_PropName(s, *i);
		}
	s.Write("]");
	}

} // namespace ZUtil_Strim_TQL

NAMESPACE_ZOOLIB_END
