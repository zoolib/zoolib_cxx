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

#include "ZUtil_Strim_TQL.h"
#include "ZUtil_Strim_TQL_Spec.h"
#include "ZUtil_Strim_Tuple.h"

using std::set;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL::Options

ZUtil_Strim_TQL::Options::Options()
:	fEOLString("\n"),
	fIndentString("  ")
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

using ZUtil_Strim_TQL::sWrite_PropName;
using ZUtil_Strim_TQL::sWrite_RelHead;

static void sWriteIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZUtil_Strim_TQL::Options& iOptions)
	{
	while (iCount--)
		iStrimW.Write(iOptions.fIndentString);
	}

static void sWriteLFIndent(const ZStrimW& iStrimW,
	size_t iCount, const ZUtil_Strim_TQL::Options& iOptions)
	{
	iStrimW.Write(iOptions.fEOLString);
	sWriteIndent(iStrimW, iCount, iOptions);
	}

static void sWriteNode(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	ZRef<ZTQL::Node> iNode);

static void sWriteNodeDyadic(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	const string& iFunctionName,
	ZRef<ZTQL::Node> iNodeA, ZRef<ZTQL::Node> iNodeB)
	{
	s << iFunctionName;

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << "(";

	sWriteNode(s, iIndent + 1, iOptions, iNodeA);

	s << ", ";

	sWriteNode(s, iIndent + 1, iOptions, iNodeB);

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << ")";
	}


static void sWriteNode_All(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	const ZTName& iIDPropName, const ZTQL::RelHead& iRelHead)
	{
	if (iIDPropName.Empty())
		{
		s << "All(";
		}
	else
		{
		s << "AID(";
		sWrite_PropName(s, iIDPropName);
		s << ", ";
		}

	sWrite_RelHead(s, iRelHead);

	s << ")";
	}

static void sWriteNode_Explicit(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	const vector<ZTuple>& iTuples)
	{
	s << "Explicit(";
	bool isFirst = true;
	for (vector<ZTuple>::const_iterator i = iTuples.begin(); i != iTuples.end(); ++i)
		{
		if (!isFirst)
			s << ", ";
		isFirst = false;

		s << *i;
		}
	s << ")";	
	}

static void sWriteNode_Project(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	ZRef<ZTQL::Node> iNode, const ZTQL::RelHead& iRelHead)
	{
	s << "Project";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << "(";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	sWrite_RelHead(s, iRelHead);

	s << ",";

	sWriteNode(s, iIndent + 1, iOptions, iNode);

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << ")";
	}

static void sWriteNode_Rename(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	ZRef<ZTQL::Node> iNode, const ZTName& iOld, const ZTName& iNew)
	{
	s << "Rename";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << "(";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	sWrite_PropName(s, iOld);

	s << ", ";

	sWrite_PropName(s, iNew);

	s << ",";

	sWriteNode(s, iIndent + 1, iOptions, iNode);

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << ")";
	}

static void sWriteNode_Restrict(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	ZRef<ZTQL::Node> iNode, const ZTQL::Condition& iCondition)
	{
	s << "Restrict";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << "(";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	ZUtil_Strim_TQL_Spec::sToStrim(s, iCondition);

	s << ",";

	sWriteNode(s, iIndent + 1, iOptions, iNode);

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << ")";
	}


static void sWriteNode_Select(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	ZRef<ZTQL::Node> iNode, const ZRef<ZTQL::LogOp>& iLogOp)
	{
	s << "Select";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << "(";

	sWriteLFIndent(s, iIndent + 1, iOptions);

	ZUtil_Strim_TQL_Spec::sToStrim(s, iLogOp);

	s << ",";

	sWriteNode(s, iIndent + 1, iOptions, iNode);

	sWriteLFIndent(s, iIndent + 1, iOptions);

	s << ")";
	}

static void sWriteNode(const ZStrimW& s,
	size_t iIndent, const ZUtil_Strim_TQL::Options& iOptions,
	ZRef<ZTQL::Node> iNode)
	{
	sWriteLFIndent(s, iIndent, iOptions);

	s << "// ";
	sWrite_RelHead(s, iNode->GetEffectiveRelHead());

	sWriteLFIndent(s, iIndent, iOptions);

	if (ZRef<ZTQL::Node_All> theNode =
		ZRefDynamicCast<ZTQL::Node_All>(iNode))
		{
		sWriteNode_All(s, iIndent, iOptions,
			theNode->GetIDPropName(), theNode->GetRelHead());
		}
	else if (ZRef<ZTQL::Node_Difference> theNode =
		ZRefDynamicCast<ZTQL::Node_Difference>(iNode))
		{
		sWriteNodeDyadic(s, iIndent, iOptions, "Difference",
			theNode->GetNodeA(), theNode->GetNodeB());
		}
	else if (ZRef<ZTQL::Node_Explicit> theNode =
		ZRefDynamicCast<ZTQL::Node_Explicit>(iNode))
		{
		sWriteNode_Explicit(s, iIndent, iOptions, theNode->GetTuples());
		}
	else if (ZRef<ZTQL::Node_Intersect> theNode =
		ZRefDynamicCast<ZTQL::Node_Intersect>(iNode))
		{
		sWriteNodeDyadic(s, iIndent, iOptions, "Intersect",
			theNode->GetNodeA(), theNode->GetNodeB());
		}
	else if (ZRef<ZTQL::Node_Join> theNode =
		ZRefDynamicCast<ZTQL::Node_Join>(iNode))
		{
		ZRef<ZTQL::Node> theNodeA = theNode->GetNodeA();
		ZRef<ZTQL::Node> theNodeB = theNode->GetNodeB();
		const ZTQL::RelHead joinOn = theNodeA->GetEffectiveRelHead()
			& theNodeB->GetEffectiveRelHead();

		s << "// Join on:";
		sWrite_RelHead(s, joinOn);

		sWriteLFIndent(s, iIndent, iOptions);

		sWriteNodeDyadic(s, iIndent, iOptions, "Join",
			theNode->GetNodeA(), theNode->GetNodeB());
		}
	else if (ZRef<ZTQL::Node_Project> theNode =
		ZRefDynamicCast<ZTQL::Node_Project>(iNode))
		{
		sWriteNode_Project(s, iIndent, iOptions,
			theNode->GetNode(), theNode->GetRelHead());
		}
	else if (ZRef<ZTQL::Node_Rename> theNode =
		ZRefDynamicCast<ZTQL::Node_Rename>(iNode))
		{
		sWriteNode_Rename(s, iIndent, iOptions,
			theNode->GetNode(), theNode->GetOld(), theNode->GetNew());
		}
	else if (ZRef<ZTQL::Node_Restrict> theNode =
		ZRefDynamicCast<ZTQL::Node_Restrict>(iNode))
		{
		sWriteNode_Restrict(s, iIndent, iOptions,
			theNode->GetNode(), theNode->GetCondition());
		}
	else if (ZRef<ZTQL::Node_Select> theNode =
		ZRefDynamicCast<ZTQL::Node_Select>(iNode))
		{
		sWriteNode_Select(s, iIndent, iOptions,
			theNode->GetNode(), theNode->GetLogOp());
		}
	else if (ZRef<ZTQL::Node_Union> theNode =
		ZRefDynamicCast<ZTQL::Node_Union>(iNode))
		{
		sWriteNodeDyadic(s, iIndent, iOptions, "Union",
			theNode->GetNodeA(), theNode->GetNodeB());
		}	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void ZUtil_Strim_TQL::sToStrim(const ZStrimW& iStrimW, const ZTQL::Query& iQuery)
	{
	sWriteNode(iStrimW, 0, Options(), iQuery.GetNode());
	}

void ZUtil_Strim_TQL::sToStrim(const ZStrimW& iStrimW, ZRef<ZTQL::Node> iNode)
	{
	sWriteNode(iStrimW, 0, Options(), iNode);
	}

void ZUtil_Strim_TQL::sToStrim(const ZStrimW& iStrimW,
	size_t iInitialIndent, const Options& iOptions,
	const ZTQL::Query& iQuery)
	{
	sWriteNode(iStrimW, iInitialIndent, iOptions, iQuery.GetNode());
	}

void ZUtil_Strim_TQL::sToStrim(const ZStrimW& iStrimW,
	size_t iInitialIndent, const Options& iOptions,
	ZRef<ZTQL::Node> iNode)
	{
	sWriteNode(iStrimW, iInitialIndent, iOptions, iNode);
	}


void ZUtil_Strim_TQL::sWrite_PropName(const ZStrimW& s, const ZTName& iTName)
	{
	s << "@";
	ZUtil_Strim_Tuple::sWrite_PropName(s, iTName);
	}

void ZUtil_Strim_TQL::sWrite_RelHead(const ZStrimW& s, const ZTQL::RelHead& iRelHead)
	{
	set<ZTName> names;
	iRelHead.GetNames(names);
	bool isFirst = true;
	s << "[";
	for (set<ZTName>::iterator i = names.begin(); i != names.end(); ++i)
		{
		if (!isFirst)
			s << ", ";
		isFirst = false;
		sWrite_PropName(s, *i);
		}
	s << "]";
	}
