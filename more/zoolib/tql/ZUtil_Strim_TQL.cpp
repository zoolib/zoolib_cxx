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

#include "zoolib/tql/ZUtil_Strim_TQL.h"
#include "zoolib/tql/ZUtil_Strim_TQL_Spec.h"

#include "zoolib/ZUtil_Strim_Tuple.h"
#include "zoolib/ZYad_ZooLibStrim.h"

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

static void sWrite(const string& iString, const ZStrimW& s)
	{ s.Write(iString); }

static void sWrite(const UTF8* iString, const ZStrimW& s)
	{ s.Write(iString); }

static void sWrite_Indent(size_t iCount, const Options& iOptions, const ZStrimW& iStrimW)
	{
	while (iCount--)
		sWrite(iOptions.fIndentString, iStrimW);
	}

static void sWrite_LFIndent(size_t iCount, const Options& iOptions, const ZStrimW& iStrimW)
	{
	sWrite(iOptions.fEOLString, iStrimW);
	sWrite_Indent(iCount, iOptions, iStrimW);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Writer

namespace ZANONYMOUS {

class Writer : public NodeVisitor
	{
public:
	Writer(size_t iIndent, const Options& iOptions, const ZStrimW& iStrimW);

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

	size_t fIndent;
	const Options& fOptions;
	const ZStrimW& fStrimW;
	};

} // anonymous namespace

static void sWrite_EffectiveRelHeadComment(ZRef<Node> iNode, const ZStrimW& s)
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
		ZYad_ZooLibStrim::sWrite_PropName(*i, s);
		}
	}

static void sWrite(size_t iIndent, const Options& iOptions,
	ZRef<Node> iNode,
	const ZStrimW& s)
	{
	sWrite_LFIndent(iIndent, iOptions, s);

//	sWrite_EffectiveRelHeadComment(s, iNode);

//	sWrite_LFIndent(s, iIndent, iOptions);
	Writer(iIndent, iOptions, s).Write(iNode);
	}

Writer::Writer(size_t iIndent, const Options& iOptions, const ZStrimW& iStrimW)
:	fIndent(iIndent),
	fOptions(iOptions),
	fStrimW(iStrimW)
	{}

void Writer::Write(ZRef<Node> iNode)
	{ iNode->Accept(*this); }

bool Writer::pWriteDyadic(const string& iFunctionName, ZRef<Node_Dyadic> iNode)
	{
	ZRef<Node> theNodeA = iNode->GetNodeA();
	ZRef<Node> theNodeB = iNode->GetNodeB();
	sWrite(iFunctionName, fStrimW);
	sWrite_EffectiveRelHeadComment(iNode, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite("(", fStrimW);
	sWrite(fIndent + 1, fOptions, theNodeA, fStrimW);
	sWrite(", ", fStrimW);
	sWrite(fIndent + 1, fOptions, theNodeB, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_All(ZRef<Node_All> iNode)
	{
	const ZTName theIDPropName = iNode->GetIDPropName();
	if (theIDPropName.Empty())
		{
		sWrite("All(", fStrimW);
		}
	else
		{
		sWrite("AID(", fStrimW);
		sWrite_PropName(theIDPropName, fStrimW);
		sWrite(", ", fStrimW);
		}
	sWrite_RelHead(iNode->GetRelHead(), fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Difference(ZRef<Node_Difference> iNode)
	{ return this->pWriteDyadic("Difference", iNode); }

bool Writer::Visit_Explicit(ZRef<Node_Explicit> iNode)
	{
	sWrite("Explicit(", fStrimW);
	bool isFirst = true;
	const vector<Map> theMaps = iNode->GetMaps();
	for (vector<Map>::const_iterator i = theMaps.begin(); i != theMaps.end(); ++i)
		{
		if (!isFirst)
			sWrite(", ", fStrimW);
		isFirst = false;

		fStrimW << *i;
		}
	sWrite(")", fStrimW);
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

	sWrite("Join", fStrimW);
	sWrite_EffectiveRelHeadComment(iNode, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite("( // Join on: ", fStrimW);
	sWrite_RelHead(joinOn, fStrimW);
	sWrite(fIndent + 1, fOptions, theNodeA, fStrimW);
	sWrite(", ", fStrimW);
	sWrite(fIndent + 1, fOptions, theNodeB, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Project(ZRef<Node_Project> iNode)
	{
	sWrite("Project", fStrimW);
	sWrite_EffectiveRelHeadComment(iNode, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite("(", fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite_RelHead(iNode->GetRelHead(), fStrimW);
	sWrite(",", fStrimW);
	sWrite(fIndent + 1, fOptions, iNode->GetNode(), fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Rename(ZRef<Node_Rename> iNode)
	{
	sWrite("Rename", fStrimW);
	sWrite_EffectiveRelHeadComment(iNode, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite("(", fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite_PropName(iNode->GetOld(), fStrimW);
	sWrite(", ", fStrimW);
	sWrite_PropName(iNode->GetNew(), fStrimW);
	sWrite(",", fStrimW);
	sWrite(fIndent + 1, fOptions, iNode->GetNode(), fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Restrict(ZRef<Node_Restrict> iNode)
	{
	sWrite("Restrict", fStrimW);
	sWrite_EffectiveRelHeadComment(iNode, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite("(", fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	ZUtil_Strim_TQL_Spec::sToStrim(iNode->GetCondition(), fStrimW);
	sWrite(",", fStrimW);
	sWrite(fIndent + 1, fOptions, iNode->GetNode(), fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Select(ZRef<Node_Select> iNode)
	{
	sWrite("Select", fStrimW);
	sWrite_EffectiveRelHeadComment(iNode, fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite("(", fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	ZUtil_Strim_TQL_Spec::sToStrim(iNode->GetLogOp(), fStrimW);
	sWrite(",", fStrimW);
	sWrite(fIndent + 1, fOptions, iNode->GetNode(), fStrimW);
	sWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Union(ZRef<Node_Union> iNode)
	{ return this->pWriteDyadic("Union", iNode); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const Query& iQuery, const ZStrimW& s)
	{ sToStrim(0, Options(), iQuery.GetNode(), s); }

void sToStrim(ZRef<Node> iNode, const ZStrimW& s)
	{ sToStrim(0, Options(), iNode, s); }

void sToStrim(
	size_t iInitialIndent, const Options& iOptions,
	const Query& iQuery,
	const ZStrimW& s)
	{
	sWrite(iInitialIndent, iOptions, iQuery.GetNode(), s);
	}

void sToStrim(size_t iInitialIndent, const Options& iOptions,
	ZRef<Node> iNode,
	const ZStrimW& s)
	{
	sWrite(iInitialIndent, iOptions, iNode, s);
	}

void sWrite_PropName(const ZTName& iTName, const ZStrimW& s)
	{
	s.Write("@");
	ZYad_ZooLibStrim::sWrite_PropName(iTName, s);
	}

void sWrite_RelHead(const RelHead& iRelHead, const ZStrimW& s)
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
		sWrite_PropName(*i, s);
		}
	s.Write("]");
	}

} // namespace ZUtil_Strim_TQL

NAMESPACE_ZOOLIB_END
