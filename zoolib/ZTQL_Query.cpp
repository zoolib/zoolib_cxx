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

#include "ZTQL_Query.h"

#include "ZUtil_STL.h"

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL

namespace ZTQL {

Query::Query()
	{}

Query::Query(const Query& iOther)
:	fNode(iOther.fNode)
	{}

Query::Query(ZRef<Node> iNode)
:	fNode(iNode)
	{}

Query& Query::operator=(const Query& iOther)
	{
	fNode = iOther.fNode;
	return *this;
	}

Query::~Query()
	{}

ZRef<Node> Query::GetNode() const
	{ return fNode; }

Query::Query(const ZTuple* iTuples, size_t iCount)
:	fNode(new Node_Explicit(iTuples, iCount))
	{}

Query::Query(const std::vector<ZTuple>& iTuples)
:	fNode(new Node_Explicit(iTuples))
	{}

Query Query::sAllID(const ZTName& iIDPropName)
	{
	return Query(new Node_All(iIDPropName));
	}

Query Query::sAllID(const ZTName& iIDPropName, const RelHead& iRelHead)
	{
	return Query(new Node_All(iIDPropName, iRelHead));
	}

Query Query::sAll(const RelHead& iRelHead)
	{
	return Query(new Node_All(iRelHead));
	}

Query Query::Intersect(const Query& iOther) const
	{
	return Query(new Node_Intersect(fNode, iOther.fNode));
	}

Query Query::Join(const Query& iOther) const
	{
	return Query(new Node_Join(fNode, iOther.fNode));
	}

Query Query::Project(const RelHead& iRelHead) const
	{
	return Query(new Node_Project(fNode, iRelHead));
	}

Query Query::Rename(const ZTName& iOld, const ZTName& iNew) const
	{
	return Query(new Node_Rename(fNode, iOld, iNew));
	}

Query Query::Select(const Spec& iSpec) const
	{
	return Query(new Node_Select(fNode, iSpec.GetLogOp()));
	}

Query Query::Union(const Query& iOther) const
	{
	return Query(new Node_Union(fNode, iOther.fNode));
	}

RelHead Query::GetRelHead() const
	{
	if (fNode)
		return fNode->GetEffectiveRelHead();
	return RelHead();
	}

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL, static functions

namespace ZTQL {

Query sAllID(const ZTName& iIDName)
	{ return Query::sAllID(iIDName); }

Query sAllID(const ZTName& iIDName, const RelHead& iRelHead)
	{ return Query::sAllID(iIDName, iRelHead); }

Query sAll(const RelHead& iRelHead)
	{ return Query::sAll(iRelHead); }

//##Query sDifference(const Query& iQuery1, const Query& iQuery2)
//##	{ return iQuery1.Difference(iQuery2); }

Query sExplicit(const ZTuple* iTuples, size_t iCount)
	{ return Query(iTuples, iCount); }

Query sExplicit(const std::vector<ZTuple>& iTuples)
	{ return Query(iTuples); }

Query sIntersect(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Intersect(iQuery2); }

Query sJoin(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Join(iQuery2); }

Query sProject(const Query& iQuery, const RelHead& iRelHead)
	{ return iQuery.Project(iRelHead); }

Query sRename(const Query& iQuery, const ZTName& iOldPropName, const ZTName& iNewPropName)
	{ return iQuery.Rename(iOldPropName, iNewPropName); }

Query sSelect(const Query& iQuery, const Spec& iSpec)
	{ return iQuery.Select(iSpec); }

Query sUnion(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Union(iQuery2); }

} // namespace ZTQL
