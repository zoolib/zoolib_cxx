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

#ifndef __ZTQL_Node__
#define __ZTQL_Node__ 1
#include "zconfig.h"

#include "zoolib/tql/ZTQL_LogOp.h"
#include "zoolib/tql/ZTQL_RelHead.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

class NodeVisitor;

// =================================================================================================
#pragma mark -
#pragma mark * Node

class Node : public ZRefCounted
	{
protected:
	Node();

public:
	virtual bool Accept(NodeVisitor& iVisitor) = 0;
	virtual RelHead GetEffectiveRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Dyadic

class Node_Dyadic : public Node
	{
protected:
	Node_Dyadic(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

public:
// Our protocol
	ZRef<Node> GetNodeA();
	ZRef<Node> GetNodeB();

protected:
	ZRef<Node> fNodeA;
	ZRef<Node> fNodeB;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_All

class Node_All : public Node
	{
public:
	Node_All(const ZTName& iIDPropName);
	Node_All(const RelHead& iRelHead);
	Node_All(const ZTName& iIDPropName, const RelHead& iRelHead);

// From Node
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	const ZTName& GetIDPropName();
	const RelHead& GetRelHead();

private:
	ZTName fIDPropName;
	RelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Difference

class Node_Difference : public Node_Dyadic
	{
public:
	Node_Difference(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node via Node_Dyadic
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Explicit

class Node_Explicit : public Node
	{
public:
	Node_Explicit(const Map* iMaps, size_t iCount);
	Node_Explicit(const std::vector<Map>& iMaps);

// From Node
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	const std::vector<Map>& GetMaps();

private:
	std::vector<Map> fMaps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Intersect

class Node_Intersect : public Node_Dyadic
	{
public:
	Node_Intersect(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node via Node_Dyadic
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Join

class Node_Join : public Node_Dyadic
	{
public:
	Node_Join(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node via Node_Dyadic
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Project

class Node_Project : public Node
	{
public:
	Node_Project(ZRef<Node> iNode, const RelHead& iRelHead);

// From Node
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	const RelHead& GetRelHead();

private:
	ZRef<Node> fNode;
	RelHead fRelHead;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Rename

class Node_Rename : public Node
	{
public:
	Node_Rename(ZRef<Node> iNode, const ZTName& iOld, const ZTName& iNew);

// From Node
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	const ZTName& GetOld();
	const ZTName& GetNew();

private:
	ZRef<Node> fNode;
	ZTName fOld;
	ZTName fNew;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Restrict

class Node_Restrict : public Node
	{
public:
	Node_Restrict(ZRef<Node> iNode, const Condition& iCondition);

// From Node
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	const Condition& GetCondition();

private:
	ZRef<Node> fNode;
	Condition fCondition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Select

class Node_Select : public Node
	{
public:
	Node_Select(ZRef<Node> iNode, ZRef<LogOp> iLogOp);

// From Node
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	ZRef<LogOp> GetLogOp();

private:
	ZRef<Node> fNode;
	ZRef<LogOp> fLogOp;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Node_Union

class Node_Union : public Node_Dyadic
	{
public:
	Node_Union(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node via Node_Dyadic
	virtual bool Accept(NodeVisitor& iVisitor);
	virtual RelHead GetEffectiveRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * NodeVisitor

class NodeVisitor
	{
public:
	NodeVisitor();
	~NodeVisitor();

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
	};

// =================================================================================================
#pragma mark -
#pragma mark * NodeTransformer

class NodeTransformer : public NodeVisitor
	{
public:
	NodeTransformer();
	~NodeTransformer();

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

// Our protocol
	ZRef<Node> Transform(ZRef<Node> iNode);

	virtual ZRef<Node> Transform_All(ZRef<Node_All> iNode);
	virtual ZRef<Node> Transform_Difference(ZRef<Node_Difference> iNode);
	virtual ZRef<Node> Transform_Explicit(ZRef<Node_Explicit> iNode);
	virtual ZRef<Node> Transform_Intersect(ZRef<Node_Intersect> iNode);
	virtual ZRef<Node> Transform_Join(ZRef<Node_Join> iNode);
	virtual ZRef<Node> Transform_Project(ZRef<Node_Project> iNode);
	virtual ZRef<Node> Transform_Rename(ZRef<Node_Rename> iNode);
	virtual ZRef<Node> Transform_Restrict(ZRef<Node_Restrict> iNode);
	virtual ZRef<Node> Transform_Select(ZRef<Node_Select> iNode);
	virtual ZRef<Node> Transform_Union(ZRef<Node_Union> iNode);

private:
	ZRef<Node> fResult;
	};

} // namespace ZTQL

NAMESPACE_ZOOLIB_END

#endif // __ZTQL_Node__
