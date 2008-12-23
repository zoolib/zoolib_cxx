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

#include "zoolib/ZTQL_Node.h"

#include "zoolib/ZUtil_STL.h"

using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node

Node::Node()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_All

Node_All::Node_All(const ZTName& iIDPropName)
:	fIDPropName(iIDPropName)
	{}

Node_All::Node_All(const RelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

Node_All::Node_All(const ZTName& iIDPropName, const RelHead& iRelHead)
:	fIDPropName(iIDPropName),
	fRelHead(iRelHead)
	{}

RelHead Node_All::GetEffectiveRelHead()
	{
	RelHead result = fRelHead;
	if (!fIDPropName.Empty())
		result |= fIDPropName;
	return result;
	}

const ZTName& Node_All::GetIDPropName()
	{ return fIDPropName; }

const RelHead& Node_All::GetRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Difference

Node_Difference::Node_Difference(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	fNodeA(iNodeA),
	fNodeB(iNodeB)
	{}

RelHead Node_Difference::GetEffectiveRelHead()
	{
	RelHead theRelHeadA = fNodeA->GetEffectiveRelHead();
	RelHead theRelHeadB = fNodeB->GetEffectiveRelHead();

	if (theRelHeadA != theRelHeadB)
		return RelHead();

	return theRelHeadA;
	}

ZRef<Node> Node_Difference::GetNodeA()
	{ return fNodeA; }

ZRef<Node> Node_Difference::GetNodeB()
	{ return fNodeB; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Explicit

Node_Explicit::Node_Explicit(const ZTuple* iTuples, size_t iCount)
:	fTuples(iTuples, iTuples + iCount)
	{}

Node_Explicit::Node_Explicit(const std::vector<ZTuple>& iTuples)
:	fTuples(iTuples)
	{}

RelHead Node_Explicit::GetEffectiveRelHead()
	{
	return RelHead();
	}

const vector<ZTuple>& Node_Explicit::GetTuples()
	{ return fTuples; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Intersect

Node_Intersect::Node_Intersect(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	fNodeA(iNodeA),
	fNodeB(iNodeB)
	{}

RelHead Node_Intersect::GetEffectiveRelHead()
	{
	RelHead theRelHeadA = fNodeA->GetEffectiveRelHead();
	RelHead theRelHeadB = fNodeB->GetEffectiveRelHead();

	if (theRelHeadA != theRelHeadB)
		return RelHead();

	return theRelHeadA;
	}

ZRef<Node> Node_Intersect::GetNodeA()
	{ return fNodeA; }

ZRef<Node> Node_Intersect::GetNodeB()
	{ return fNodeB; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Join

Node_Join::Node_Join(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	fNodeA(iNodeA),
	fNodeB(iNodeB)
	{}

RelHead Node_Join::GetEffectiveRelHead()
	{
	return fNodeA->GetEffectiveRelHead() | fNodeB->GetEffectiveRelHead();
	}

ZRef<Node> Node_Join::GetNodeA()
	{ return fNodeA; }

ZRef<Node> Node_Join::GetNodeB()
	{ return fNodeB; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Project

Node_Project::Node_Project(ZRef<Node> iNode, const RelHead& iRelHead)
:	fNode(iNode),
	fRelHead(iRelHead)
	{}

RelHead Node_Project::GetEffectiveRelHead()
	{
	return fNode->GetEffectiveRelHead() & fRelHead;
	}

ZRef<Node> Node_Project::GetNode()
	{ return fNode; }

const RelHead& Node_Project::GetRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Rename

Node_Rename::Node_Rename(ZRef<Node> iNode, const ZTName& iOld, const ZTName& iNew)
:	fNode(iNode),
	fOld(iOld),
	fNew(iNew)
	{}

RelHead Node_Rename::GetEffectiveRelHead()
	{
	RelHead theRelHead = fNode->GetEffectiveRelHead();
	if (theRelHead.Contains(fOld))
		{
		theRelHead -= fOld;
		theRelHead |= fNew;
		}
	return theRelHead;
	}

ZRef<Node> Node_Rename::GetNode()
	{ return fNode; }
	
const ZTName& Node_Rename::GetOld()
	{ return fOld; }

const ZTName& Node_Rename::GetNew()
	{ return fNew; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Restrict

Node_Restrict::Node_Restrict(ZRef<Node> iNode, const Condition& iCondition)
:	fNode(iNode),
	fCondition(iCondition)
	{}

RelHead Node_Restrict::GetEffectiveRelHead()
	{
	return fNode->GetEffectiveRelHead();
	}

ZRef<Node> Node_Restrict::GetNode()
	{ return fNode; }

const Condition& Node_Restrict::GetCondition()
	{ return fCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Select

Node_Select::Node_Select(ZRef<Node> iNode, ZRef<LogOp> iLogOp)
:	fNode(iNode),
	fLogOp(iLogOp)
	{}

RelHead Node_Select::GetEffectiveRelHead()
	{
	return fNode->GetEffectiveRelHead();
	}

ZRef<Node> Node_Select::GetNode()
	{ return fNode; }

ZRef<LogOp> Node_Select::GetLogOp()
	{ return fLogOp; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Union

Node_Union::Node_Union(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	fNodeA(iNodeA),
	fNodeB(iNodeB)
	{}

RelHead Node_Union::GetEffectiveRelHead()
	{
	RelHead theRelHeadA = fNodeA->GetEffectiveRelHead();
	RelHead theRelHeadB = fNodeB->GetEffectiveRelHead();

	if (theRelHeadA != theRelHeadB)
		return RelHead();

	return theRelHeadA;
	}

ZRef<Node> Node_Union::GetNodeA()
	{ return fNodeA; }

ZRef<Node> Node_Union::GetNodeB()
	{ return fNodeB; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node::Transformer

ZTQL::Node::Transformer::Transformer()
	{}

ZTQL::Node::Transformer::~Transformer()
	{}

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_All(ZRef<Node_All> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Difference(ZRef<Node_Difference> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Explicit(ZRef<Node_Explicit> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Intersect(ZRef<Node_Intersect> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Join(ZRef<Node_Join> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Project(ZRef<Node_Project> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Rename(ZRef<Node_Rename> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Restrict(ZRef<Node_Restrict> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Select(ZRef<Node_Select> iNode)
	{ return iNode; }

ZRef<ZTQL::Node> ZTQL::Node::Transformer::Transform_Union(ZRef<Node_Union> iNode)
	{ return iNode; }

} // namespace ZTQL

NAMESPACE_ZOOLIB_END
