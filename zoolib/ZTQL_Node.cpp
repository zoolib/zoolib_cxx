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
#pragma mark * Node

Node::Node()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Node_Dyadic

Node_Dyadic::Node_Dyadic(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	fNodeA(iNodeA),
	fNodeB(iNodeB)
	{}

ZRef<Node> Node_Dyadic::GetNodeA()
	{ return fNodeA; }

ZRef<Node> Node_Dyadic::GetNodeB()
	{ return fNodeB; }

// =================================================================================================
#pragma mark -
#pragma mark * Node_All

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

bool Node_All::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_All(this); }

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
#pragma mark * Node_Difference

Node_Difference::Node_Difference(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	Node_Dyadic(iNodeA, iNodeB)
	{}

bool Node_Difference::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Difference(this); }

RelHead Node_Difference::GetEffectiveRelHead()
	{
	RelHead theRelHeadA = fNodeA->GetEffectiveRelHead();
	RelHead theRelHeadB = fNodeB->GetEffectiveRelHead();

	if (theRelHeadA != theRelHeadB)
		return RelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Node_Explicit

Node_Explicit::Node_Explicit(const ZTuple* iTuples, size_t iCount)
:	fTuples(iTuples, iTuples + iCount)
	{}

Node_Explicit::Node_Explicit(const std::vector<ZTuple>& iTuples)
:	fTuples(iTuples)
	{}

bool Node_Explicit::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Explicit(this); }

RelHead Node_Explicit::GetEffectiveRelHead()
	{
	// Hmm -- should we be unioning our tuples property names together?
	// Or should an explicit take a RelHead as an initializer?
	return RelHead();
	}

const vector<ZTuple>& Node_Explicit::GetTuples()
	{ return fTuples; }

// =================================================================================================
#pragma mark -
#pragma mark * Node_Intersect

Node_Intersect::Node_Intersect(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	Node_Dyadic(iNodeA, iNodeB)
	{}

bool Node_Intersect::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Intersect(this); }

RelHead Node_Intersect::GetEffectiveRelHead()
	{
	RelHead theRelHeadA = fNodeA->GetEffectiveRelHead();
	RelHead theRelHeadB = fNodeB->GetEffectiveRelHead();

	if (theRelHeadA != theRelHeadB)
		return RelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Node_Join

Node_Join::Node_Join(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	Node_Dyadic(iNodeA, iNodeB)
	{}

bool Node_Join::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Join(this); }

RelHead Node_Join::GetEffectiveRelHead()
	{ return fNodeA->GetEffectiveRelHead() | fNodeB->GetEffectiveRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * Node_Project

Node_Project::Node_Project(ZRef<Node> iNode, const RelHead& iRelHead)
:	fNode(iNode),
	fRelHead(iRelHead)
	{}

bool Node_Project::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Project(this); }

RelHead Node_Project::GetEffectiveRelHead()
	{ return fNode->GetEffectiveRelHead() & fRelHead; }

ZRef<Node> Node_Project::GetNode()
	{ return fNode; }

const RelHead& Node_Project::GetRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Node_Rename

Node_Rename::Node_Rename(ZRef<Node> iNode, const ZTName& iOld, const ZTName& iNew)
:	fNode(iNode),
	fOld(iOld),
	fNew(iNew)
	{}

bool Node_Rename::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Rename(this); }

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
#pragma mark * Node_Restrict

Node_Restrict::Node_Restrict(ZRef<Node> iNode, const Condition& iCondition)
:	fNode(iNode),
	fCondition(iCondition)
	{}

bool Node_Restrict::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Restrict(this); }

RelHead Node_Restrict::GetEffectiveRelHead()
	{ return fNode->GetEffectiveRelHead(); }

ZRef<Node> Node_Restrict::GetNode()
	{ return fNode; }

const Condition& Node_Restrict::GetCondition()
	{ return fCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * Node_Select

Node_Select::Node_Select(ZRef<Node> iNode, ZRef<LogOp> iLogOp)
:	fNode(iNode),
	fLogOp(iLogOp)
	{}

bool Node_Select::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Select(this); }

RelHead Node_Select::GetEffectiveRelHead()
	{ return fNode->GetEffectiveRelHead(); }

ZRef<Node> Node_Select::GetNode()
	{ return fNode; }

ZRef<LogOp> Node_Select::GetLogOp()
	{ return fLogOp; }

// =================================================================================================
#pragma mark -
#pragma mark * Node_Union

Node_Union::Node_Union(ZRef<Node> iNodeA, ZRef<Node> iNodeB)
:	Node_Dyadic(iNodeA, iNodeB)
	{}

bool Node_Union::Accept(NodeVisitor& iVisitor)
	{ return iVisitor.Visit_Union(this); }

RelHead Node_Union::GetEffectiveRelHead()
	{
	RelHead theRelHeadA = fNodeA->GetEffectiveRelHead();
	RelHead theRelHeadB = fNodeB->GetEffectiveRelHead();

	if (theRelHeadA != theRelHeadB)
		return RelHead();

	return theRelHeadA;
	}

// =================================================================================================
#pragma mark -
#pragma mark * NodeVisitor

NodeVisitor::NodeVisitor()
	{}

NodeVisitor::~NodeVisitor()
	{}

bool NodeVisitor::Visit_All(ZRef<Node_All> iNode)
	{ return true; }

bool NodeVisitor::Visit_Difference(ZRef<Node_Difference> iNode)
	{ return true; }

bool NodeVisitor::Visit_Explicit(ZRef<Node_Explicit> iNode)
	{ return true; }

bool NodeVisitor::Visit_Intersect(ZRef<Node_Intersect> iNode)
	{ return true; }

bool NodeVisitor::Visit_Join(ZRef<Node_Join> iNode)
	{ return true; }

bool NodeVisitor::Visit_Project(ZRef<Node_Project> iNode)
	{ return true; }

bool NodeVisitor::Visit_Rename(ZRef<Node_Rename> iNode)
	{ return true; }

bool NodeVisitor::Visit_Restrict(ZRef<Node_Restrict> iNode)
	{ return true; }

bool NodeVisitor::Visit_Select(ZRef<Node_Select> iNode)
	{ return true; }

bool NodeVisitor::Visit_Union(ZRef<Node_Union> iNode)
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * NodeTransformer

NodeTransformer::NodeTransformer()
	{}

NodeTransformer::~NodeTransformer()
	{}

bool NodeTransformer::Visit_All(ZRef<Node_All> iNode)
	{
	fResult = this->Transform_All(iNode);
	return true;
	}

bool NodeTransformer::Visit_Difference(ZRef<Node_Difference> iNode)
	{
	fResult = this->Transform_Difference(iNode);
	return true;
	}

bool NodeTransformer::Visit_Explicit(ZRef<Node_Explicit> iNode)
	{
	fResult = this->Transform_Explicit(iNode);
	return true;
	}

bool NodeTransformer::Visit_Intersect(ZRef<Node_Intersect> iNode)
	{
	fResult = this->Transform_Intersect(iNode);
	return true;
	}

bool NodeTransformer::Visit_Join(ZRef<Node_Join> iNode)
	{
	fResult = this->Transform_Join(iNode);
	return true;
	}

bool NodeTransformer::Visit_Project(ZRef<Node_Project> iNode)
	{
	fResult = this->Transform_Project(iNode);
	return true;
	}

bool NodeTransformer::Visit_Rename(ZRef<Node_Rename> iNode)
	{
	fResult = this->Transform_Rename(iNode);
	return true;
	}

bool NodeTransformer::Visit_Restrict(ZRef<Node_Restrict> iNode)
	{
	fResult = this->Transform_Restrict(iNode);
	return true;
	}

bool NodeTransformer::Visit_Select(ZRef<Node_Select> iNode)
	{
	fResult = this->Transform_Select(iNode);
	return true;
	}

bool NodeTransformer::Visit_Union(ZRef<Node_Union> iNode)
	{
	fResult = this->Transform_Union(iNode);
	return true;
	}

ZRef<Node> NodeTransformer::Transform(ZRef<Node> iNode)
	{
	if (iNode)
		{
		iNode->Accept(*this);
		return fResult;
		}
	return iNode;
	}

ZRef<Node> NodeTransformer::Transform_All(ZRef<Node_All> iNode)
	{ return iNode; }

ZRef<Node> NodeTransformer::Transform_Difference(ZRef<Node_Difference> iNode)
	{
	ZRef<Node> newNodeA = this->Transform(iNode->GetNodeA());
	ZRef<Node> newNodeB = this->Transform(iNode->GetNodeB());

	return new Node_Difference(newNodeA, newNodeB);
	}

ZRef<Node> NodeTransformer::Transform_Explicit(ZRef<Node_Explicit> iNode)
	{ return iNode; }

ZRef<Node> NodeTransformer::Transform_Intersect(ZRef<Node_Intersect> iNode)
	{
	ZRef<Node> newNodeA = this->Transform(iNode->GetNodeA());
	ZRef<Node> newNodeB = this->Transform(iNode->GetNodeB());

	return new Node_Intersect(newNodeA, newNodeB);
	}

ZRef<Node> NodeTransformer::Transform_Join(ZRef<Node_Join> iNode)
	{
	ZRef<Node> newNodeA = this->Transform(iNode->GetNodeA());
	ZRef<Node> newNodeB = this->Transform(iNode->GetNodeB());

	return new Node_Join(newNodeA, newNodeB);
	}

ZRef<Node> NodeTransformer::Transform_Project(ZRef<Node_Project> iNode)
	{
	ZRef<Node> newNode = this->Transform(iNode->GetNode());
	return new Node_Project(newNode, iNode->GetRelHead());
	}

ZRef<Node> NodeTransformer::Transform_Rename(ZRef<Node_Rename> iNode)
	{
	ZRef<Node> newNode = this->Transform(iNode->GetNode());
	return new Node_Rename(newNode, iNode->GetOld(), iNode->GetNew());
	}

ZRef<Node> NodeTransformer::Transform_Restrict(ZRef<Node_Restrict> iNode)
	{
	ZRef<Node> newNode = this->Transform(iNode->GetNode());
	return new Node_Restrict(newNode, iNode->GetCondition());
	}

ZRef<Node> NodeTransformer::Transform_Select(ZRef<Node_Select> iNode)
	{
	ZRef<Node> newNode = this->Transform(iNode->GetNode());
	return new Node_Select(newNode, iNode->GetLogOp());
	}

ZRef<Node> NodeTransformer::Transform_Union(ZRef<Node_Union> iNode)
	{
	ZRef<Node> newNodeA = this->Transform(iNode->GetNodeA());
	ZRef<Node> newNodeB = this->Transform(iNode->GetNodeB());

	return new Node_Union(newNodeA, newNodeB);
	}

} // namespace ZTQL

NAMESPACE_ZOOLIB_END
