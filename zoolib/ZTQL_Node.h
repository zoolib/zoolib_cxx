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

#include "ZTQL_LogOp.h"
#include "ZTQL_RelHead.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node

namespace ZTQL {

class Node : public ZRefCounted
	{
protected:
	Node();

public:
	class Transformer;

//	virtual ZRef<Node> Transform(Transformer& iTransformer) = 0;

	virtual RelHead GetEffectiveRelHead() = 0;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_All

namespace ZTQL {

class Node_All : public Node
	{
public:
	Node_All(const ZTName& iIDPropName);
	Node_All(const RelHead& iRelHead);
	Node_All(const ZTName& iIDPropName, const RelHead& iRelHead);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	const ZTName& GetIDPropName();
	const RelHead& GetRelHead();

private:
	ZTName fIDPropName;
	RelHead fRelHead;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Difference

namespace ZTQL {

class Node_Difference : public Node
	{
public:
	Node_Difference(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNodeA();
	ZRef<Node> GetNodeB();

private:
	ZRef<Node> fNodeA;
	ZRef<Node> fNodeB;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Explicit

namespace ZTQL {

class Node_Explicit : public Node
	{
public:
	Node_Explicit(const ZTuple* iTuples, size_t iCount);
	Node_Explicit(const std::vector<ZTuple>& iTuples);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	const vector<ZTuple>& GetTuples();

private:
	vector<ZTuple> fTuples;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Intersect

namespace ZTQL {

class Node_Intersect : public Node
	{
public:
	Node_Intersect(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNodeA();
	ZRef<Node> GetNodeB();

private:
	ZRef<Node> fNodeA;
	ZRef<Node> fNodeB;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Join

namespace ZTQL {

class Node_Join : public Node
	{
public:
	Node_Join(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNodeA();
	ZRef<Node> GetNodeB();

private:
	ZRef<Node> fNodeA;
	ZRef<Node> fNodeB;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Project

namespace ZTQL {

class Node_Project : public Node
	{
public:
	Node_Project(ZRef<Node> iNode, const RelHead& iRelHead);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	const RelHead& GetRelHead();

private:
	ZRef<Node> fNode;
	RelHead fRelHead;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Rename

namespace ZTQL {

class Node_Rename : public Node
	{
public:
	Node_Rename(ZRef<Node> iNode, const ZTName& iOld, const ZTName& iNew);

// From Node
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

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Restrict

namespace ZTQL {

class Node_Restrict : public Node
	{
public:
	Node_Restrict(ZRef<Node> iNode, const Condition& iCondition);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	const Condition& GetCondition();

private:
	ZRef<Node> fNode;
	Condition fCondition;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Select

namespace ZTQL {

class Node_Select : public Node
	{
public:
	Node_Select(ZRef<Node> iNode, ZRef<LogOp> iLogOp);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNode();
	ZRef<LogOp> GetLogOp();

private:
	ZRef<Node> fNode;
	ZRef<LogOp> fLogOp;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node_Union

namespace ZTQL {

class Node_Union : public Node
	{
public:
	Node_Union(ZRef<Node> iNodeA, ZRef<Node> iNodeB);

// From Node
	virtual RelHead GetEffectiveRelHead();

// Our protocol
	ZRef<Node> GetNodeA();
	ZRef<Node> GetNodeB();

private:
	ZRef<Node> fNodeA;
	ZRef<Node> fNodeB;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Node::Transformer

class ZTQL::Node::Transformer
	{
protected:
	Transformer();
	~Transformer();

public:
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
	};

#endif // __ZTQL_Node__
