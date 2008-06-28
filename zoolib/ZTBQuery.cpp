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

#include "zoolib/ZTBQuery.h"
#include "zoolib/ZCompare.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZUtil_STL.h"

#ifndef kDebug_TBQuery
#	define kDebug_TBQuery 1
#endif

using std::set;
using std::string;
using std::vector;

static ZTuple sTupleFromNode(const ZRef<ZTBQueryNode>& iNode);
static ZRef<ZTBQueryNode> sNodeFromTuple(const ZTuple& iTuple);

static ZRef<ZTBQueryNode> sNodeFromStream(const ZStreamR& iStreamR);
static void sNodeToStream(const ZStreamW& iStreamW, const ZRef<ZTBQueryNode>& iNode);

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static inline void sWriteCount(const ZStreamW& iStreamW, uint32 iCount)
	{ iStreamW.WriteCount(iCount); }

static inline uint32 sReadCount(const ZStreamR& iStreamR)
	{ return iStreamR.ReadCount(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode

// Defined here so that the constructor and compare inlines are visible to the rest of this file.

inline ZTBQueryNode::ZTBQueryNode()
	{}

inline ZTBQueryNode::~ZTBQueryNode()
	{}

inline int ZTBQueryNode::Compare(const ZRef<ZTBQueryNode>& iOther)
	{
	// An extant node is greater than an absent node.
	if (!iOther)
		return 1;

	// VERY IMPORTANT. pRevCompare returns 1 if the callee is less than
	// the param. So we invoke it on iOther to get the sense that callers expect.
	return iOther.GetObject()->pRevCompare(this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQuery

/**
\class ZTBQuery
\nosubgrouping
\ingroup group_Tuplebase
\ingroup group_Tuplesoup
\sa group_Tuplebase
\sa group_Tuplesoup

\note Despite its name ZTBQuery, like ZTBSpec, knows nothing about
tuplebases, tuplesoups or tuplestores. It's simply a way to create and manage a tree of
simple queries and indirections through intermediate query results to be applied to a set
of ID/tuple pairs, as discussed in \ref group_Tuplestore. As such it probably ought to be called
ZTupleQuery or ZTQuery.

Whereas ZTBSpec is a predicate against which \em any tuple can be checked for satisfaction,
a ZTBQuery represents the selection of tuples from a \link group_Tuplestore set of ID/tuple pairs\endlink,
and potentially the use of values from the discovered tuples to execute further selection.

The simplest ZTBQuery is also the most rare; a ZTBQuery initialized with a list of IDs. Its
extension is simply the appropriate ID/tuple pairs.

The next simplest ZTBQuery is the commonest; a ZTBQuery initialized with a ZTBSpec. Its extension
is the ID/tuple pairs where the tuple satisfies the ZTBSpec.

There are two further ways to initialize a complex ZTBQuery. The first is the constructor
\link ZTBQuery(const ZTBQuery& iSourceQuery, const std::string& iSourcePropName)\endlink
whose extension is the values of ID fields named \c iSourcePropName from tuples in the
extension of \c iSourceQuery. For each tuple returned by \c iSourceQuery we take its property
named \c iSourcePropname, and if the property is an ID we add that to the result set.

The second complex constructor is
\link ZTBQuery(const std::string& iPropName, const ZTBQuery& iSourceQuery)\endlink
whose extension is those tuples with properties named \c iPropName whose value is an
ID that occurs in the extension of \c iSourceQuery.

When applied to a ZTB or a ZTSoup the extension of a ZTBQuery is a list rather than
a set, as such the order of the results is potentially meaningful. The order can be
specified by applying the method ZTBQuery::Sorted, each invocation which returns
a new ZTBQuery whose results will be primarily ordered by the given property name.
It supercedes but preserves any sort that had existed previously. So to sort primarily
by prop1, then by prop2 and finally by prop3 you would do this:
\code
  theQuery = theQuery.Sorted("prop3");
  theQuery = theQuery.Sorted("prop2");
  theQuery = theQuery.Sorted("prop1");
\endcode
or more succinctly:
\code
  theQuery.Sorted("prop3").Sorted("prop2").Sorted("prop1");
\endcode

There are three overloads of ZTBQuery::Sorted. The richest specifies the property name,
whether the sort should be ascending or descending, and what strength of comparison should
be applied to string comparisons (\ref ZTextCollator and discussion in ZTBSpec).

The other variants of ZTBQuery::Sorted take a property name and ascending flag, and a
property name only.

I will talk about ZTBQuery::First in a later revision of this documentation, when I've
updated the implementation to match the intuitive interpretation of the method.
*/

ZTBQuery::ZTBQuery()
	{}

ZTBQuery::ZTBQuery(const ZTBQuery& iOther)
:	fNode(iOther.fNode)
	{}

ZTBQuery::ZTBQuery(const ZRef<ZTBQueryNode>& iNode)
:	fNode(iNode)
	{}

ZTBQuery::ZTBQuery(ZTBQueryNode* iNode)
:	fNode(iNode)
	{}

ZTBQuery::~ZTBQuery()
	{}

ZTBQuery::ZTBQuery(const ZTuple& iTuple)
:	fNode(sNodeFromTuple(iTuple))
	{}

ZTBQuery::ZTBQuery(const ZStreamR& iStreamR)
:	fNode(sNodeFromStream(iStreamR))
	{}

ZTBQuery::ZTBQuery(uint64 iID)
	{
	if (iID)
		fNode = new ZTBQueryNode_ID_Constant(iID);
	}


/**
Occasionally you'll want to explicitly reference tuples by IDs, whilst still using
the ZTBQuery mechanism to do so.
*/
ZTBQuery::ZTBQuery(const uint64* iIDs, size_t iCount)
	{
	if (iCount)
		fNode = new ZTBQueryNode_ID_Constant(iIDs, iCount);
	}


/**
\overload
*/
ZTBQuery::ZTBQuery(const vector<uint64>& iIDs)
	{
	if (!iIDs.empty())
		fNode = new ZTBQueryNode_ID_Constant(iIDs);
	}


/**
\overload
*/
ZTBQuery::ZTBQuery(const set<uint64>& iIDs)
	{
	if (!iIDs.empty())
		fNode = new ZTBQueryNode_ID_Constant(iIDs);
	}

ZTBQuery::ZTBQuery(const ZTBSpec& iSpec)
	{
	vector<SortSpec> theSort;
	vector<ZTBQueryNode_Combo::Intersection>
		theIntersections(1, ZTBQueryNode_Combo::Intersection(iSpec, new ZTBQueryNode_All));
	fNode = new ZTBQueryNode_Combo(theSort, theIntersections);
	}

ZTBQuery::ZTBQuery(const ZTBQuery& iSourceQuery, const ZTName& iSourcePropName)
:	fNode(new ZTBQueryNode_ID_FromSource(iSourceQuery.fNode, iSourcePropName))
	{}

ZTBQuery::ZTBQuery(const ZTName& iPropName, const ZTBQuery& iSourceQuery)
:	fNode(new ZTBQueryNode_Property(iPropName, iSourceQuery.fNode))
	{}

ZTBQuery& ZTBQuery::operator=(const ZTBQuery& iOther)
	{
	fNode = iOther.fNode;
	return *this;
	}

ZTuple ZTBQuery::AsTuple() const
	{
	if (fNode)
		return fNode->AsTuple();
	return ZTuple();
	}

void ZTBQuery::ToStream(const ZStreamW& iStreamW) const
	{
	sNodeToStream(iStreamW, fNode);
	}

/**
Returns a new ZTBQuery whose extension is the current query's extension
additionally filtered by \a iSpec. Any sort or first applied to the current
query will be preserved in the new query.
*/
ZTBQuery ZTBQuery::operator&(const ZTBSpec& iSpec) const
	{
	if (!fNode || !iSpec)
		return ZTBQuery();

	if (iSpec.IsAny())
		return *this;

	if (ZRef<ZTBQueryNode_Combo> qnc = ZRefDynamicCast<ZTBQueryNode_Combo>(fNode))
		{
		vector<ZTBQueryNode_Combo::Intersection> newVectorSect;
		const vector<ZTBQueryNode_Combo::Intersection>& src = qnc->GetIntersections();
		for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator i = src.begin();
			i != src.end(); ++i)
			{
			if (ZTBSpec newSpec = (*i).fFilter & iSpec)
				newVectorSect.push_back(ZTBQueryNode_Combo::Intersection(newSpec, (*i).fNodes));
			}

		vector<SortSpec> theSort = qnc->GetSort();
		return new ZTBQueryNode_Combo(theSort, newVectorSect);
		}
	else
		{
		vector<SortSpec> theSort;
		vector<ZTBQueryNode_Combo::Intersection>
			theIntersections(1, ZTBQueryNode_Combo::Intersection(iSpec, fNode));

		return new ZTBQueryNode_Combo(theSort, theIntersections);
		}
	}

ZTBQuery& ZTBQuery::operator&=(const ZTBSpec& iSpec)
	{
	if (iSpec.IsNone())
		{
		fNode.Clear();
		}
	else if (!iSpec.IsAny())
		{
		if (ZRef<ZTBQueryNode_Combo> qnc = ZRefDynamicCast<ZTBQueryNode_Combo>(fNode))
			{
			vector<ZTBQueryNode_Combo::Intersection> newVectorSect;
			const vector<ZTBQueryNode_Combo::Intersection> src = qnc->GetIntersections();
			for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator i = src.begin();
				i != src.end(); ++i)
				{
				if (ZTBSpec newSpec = (*i).fFilter & iSpec)
					{
					newVectorSect.push_back(
						ZTBQueryNode_Combo::Intersection(newSpec, (*i).fNodes));
					}
				}
			vector<SortSpec> theSort = qnc->GetSort();
			fNode = new ZTBQueryNode_Combo(theSort, newVectorSect);
			}
		else
			{
			vector<SortSpec> theSort;
			vector<ZTBQueryNode_Combo::Intersection>
				theIntersections(1, ZTBQueryNode_Combo::Intersection(iSpec, fNode));

			fNode = new ZTBQueryNode_Combo(theSort, theIntersections);
			}
		}
	return *this;
	}

/**
Returns a new ZTBQuery whose extension is the current query's intersected with
that of of \a iQuery. Any sort or first applied to the current query will be
preserved in the new query. Any sort or first applied to \a iQuery will be ignored.
*/
ZTBQuery ZTBQuery::operator&(const ZTBQuery& iQuery) const
	{
	ZTBQuery result(fNode);
	result &= iQuery;
	return result;
	}

ZTBQuery& ZTBQuery::operator&=(const ZTBQuery& iQuery)
	{
	if (!fNode)
		{}
	else if (!iQuery.fNode)
		{
		fNode.Clear();
		}
	else if (ZRef<ZTBQueryNode_Combo> myQNC = ZRefDynamicCast<ZTBQueryNode_Combo>(fNode))
		{
		vector<ZTBQueryNode_Combo::Intersection> newVectorSect;
		if (ZRef<ZTBQueryNode_Combo> otherQNC = ZRefDynamicCast<ZTBQueryNode_Combo>(iQuery.fNode))
			{
			const vector<ZTBQueryNode_Combo::Intersection>&
				myVectorSect = myQNC->GetIntersections();

			const vector<ZTBQueryNode_Combo::Intersection>&
				otherVectorSect = otherQNC->GetIntersections();

			for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
				myIter = myVectorSect.begin(); myIter != myVectorSect.end(); ++myIter)
				{
				for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
					otherIter = otherVectorSect.begin();
					otherIter != otherVectorSect.end(); ++otherIter)
					{
					if (ZTBSpec newFilter = (*myIter).fFilter & (*otherIter).fFilter)
						{
						vector<ZRef<ZTBQueryNode> > newNodes = (*myIter).fNodes;
						newNodes.insert(newNodes.end(),
							(*otherIter).fNodes.begin(), (*otherIter).fNodes.end());

						newVectorSect.push_back(
							ZTBQueryNode_Combo::Intersection(newFilter, newNodes));
						}
					}
				}
			}
		else
			{
			newVectorSect = myQNC->GetIntersections();
			for (vector<ZTBQueryNode_Combo::Intersection>::iterator newIter = newVectorSect.begin();
				newIter != newVectorSect.end(); ++newIter)
				(*newIter).fNodes.push_back(iQuery.fNode);
			}

		if (newVectorSect.empty())
			{
			fNode.Clear();
			}
		else
			{
			vector<SortSpec> theSort = myQNC->GetSort();
			fNode = new ZTBQueryNode_Combo(theSort, newVectorSect);
			}
		}
	else if (ZRef<ZTBQueryNode_Combo> otherQNC = ZRefDynamicCast<ZTBQueryNode_Combo>(iQuery.fNode))
		{
		vector<ZTBQueryNode_Combo::Intersection> newVectorSect = otherQNC->GetIntersections();
		for (vector<ZTBQueryNode_Combo::Intersection>::iterator newIter = newVectorSect.begin();
			newIter != newVectorSect.end(); ++newIter)
			{
			(*newIter).fNodes.push_back(fNode);
			}
		vector<SortSpec> theSort;
		fNode = new ZTBQueryNode_Combo(theSort, newVectorSect);
		}
	else
		{
		ZTBQueryNode_Combo::Intersection theIntersection;
		theIntersection.fFilter = ZTBSpec::sAny();
		theIntersection.fNodes.push_back(fNode);
		theIntersection.fNodes.push_back(iQuery.fNode);
		vector<ZTBQueryNode_Combo::Intersection> newVectorSect(1, theIntersection);
		vector<SortSpec> theSort;
		fNode = new ZTBQueryNode_Combo(theSort, newVectorSect);
		}
	return *this;
	}

/**
Returns a new ZTBQuery whose extension is the current query's unioned
with that of of \a iQuery. Any sort or first applied to the current query or
to \a iQuery will be discarded.
*/
ZTBQuery ZTBQuery::operator|(const ZTBQuery& iQuery) const
	{
	ZTBQuery result(fNode);
	result |= iQuery;
	return result;
	}

ZTBQuery& ZTBQuery::operator|=(const ZTBQuery& iQuery)
	{
	if (!fNode)
		{
		fNode = iQuery.fNode;
		}
	else if (!iQuery.fNode)
		{}
	else
		{
		vector<ZTBQueryNode_Combo::Intersection> newVectorSect;
		if (ZRef<ZTBQueryNode_Combo> myQNC = ZRefDynamicCast<ZTBQueryNode_Combo>(fNode))
			{
			newVectorSect = myQNC->GetIntersections();
			if (ZRef<ZTBQueryNode_Combo> otherQNC
				= ZRefDynamicCast<ZTBQueryNode_Combo>(iQuery.fNode))
				{
				const vector<ZTBQueryNode_Combo::Intersection>&
					rightVectorSect = otherQNC->GetIntersections();

				newVectorSect.insert(newVectorSect.end(),
					rightVectorSect.begin(), rightVectorSect.end());
				}
			else
				{
				newVectorSect.push_back(ZTBQueryNode_Combo::Intersection(iQuery.fNode));
				}
			}
		else if (ZRef<ZTBQueryNode_Combo>otherQNC =
			ZRefDynamicCast<ZTBQueryNode_Combo>(iQuery.fNode))
			{
			newVectorSect = otherQNC->GetIntersections();
			newVectorSect.push_back(ZTBQueryNode_Combo::Intersection(fNode));
			}
		else
			{
			newVectorSect.push_back(ZTBQueryNode_Combo::Intersection(fNode));
			newVectorSect.push_back(ZTBQueryNode_Combo::Intersection(iQuery.fNode));
			}

		vector<SortSpec> theSort;
		fNode = new ZTBQueryNode_Combo(theSort, newVectorSect);
		}
	return *this;
	}

/**
Returns a new ZTBQuery whose extension will be primarily sorted by \a iPropName,
in ascending order if \a iAscending is true, and using a universal ZTextCollator with
strength \a iStrength for any string comparisons.
*/
ZTBQuery ZTBQuery::Sorted(const ZTName& iPropName, bool iAscending, int iStrength) const
	{
	if (!fNode)
		return *this;

	vector<SortSpec> theSort(1, SortSpec(iPropName, iAscending, iStrength));
	vector<ZTBQueryNode_Combo::Intersection> theIntersections;

	if (ZRef<ZTBQueryNode_Combo> qnc = ZRefDynamicCast<ZTBQueryNode_Combo>(fNode))
		{
		theSort.insert(theSort.end(), qnc->GetSort().begin(), qnc->GetSort().end());

		theIntersections = qnc->GetIntersections();
		}
	else
		{
		theIntersections.push_back(ZTBQueryNode_Combo::Intersection(fNode));
		}

	return new ZTBQueryNode_Combo(theSort, theIntersections);
	}

/**
\overload
*/
ZTBQuery ZTBQuery::Sorted(const ZTName& iPropName, bool iAscending) const
	{ return this->Sorted(iPropName, iAscending, 0); }

/**
\overload
*/
ZTBQuery ZTBQuery::Sorted(const ZTName& iPropName) const
	{ return this->Sorted(iPropName, true, 0); }

/**
Some words
*/
ZTBQuery ZTBQuery::First(const ZTName& iPropName) const
	{
	if (!fNode)
		return *this;

	if (ZRef<ZTBQueryNode_First> qnf = ZRefDynamicCast<ZTBQueryNode_First>(fNode))
		{
		if (iPropName.Empty())
			return qnf->GetSourceNode();
		return new ZTBQueryNode_First(iPropName, qnf->GetSourceNode());
		}

	if (iPropName.Empty())
		return fNode;

	return new ZTBQueryNode_First(iPropName, fNode);
	}

ZRef<ZTBQueryNode> ZTBQuery::GetNode() const
	{ return fNode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQuery::SortSpec

ZTBQuery::SortSpec::SortSpec(const ZStreamR& iStreamR)
:	fPropName(iStreamR),
	fAscending(iStreamR.ReadBool()),
	fStrength(iStreamR.ReadUInt8())
	{}

int ZTBQuery::SortSpec::Compare(const SortSpec& iOther) const
	{
	if (int compare = fStrength - iOther.fStrength)
		return compare;

	if (int compare = fAscending - iOther.fAscending)
		return compare;

	return fPropName.Compare(iOther.fPropName);
	}

bool ZTBQuery::SortSpec::operator<(const SortSpec& iOther) const
	{
	if (fStrength < iOther.fStrength)
		return true;
	if (fStrength > iOther.fStrength)
		return false;

	if (fAscending < iOther.fAscending)
		return true;
	if (fAscending > iOther.fAscending)
		return false;

	return fPropName < iOther.fPropName;
	}

void ZTBQuery::SortSpec::ToStream(const ZStreamW& iStreamW) const
	{
	fPropName.ToStream(iStreamW);
	iStreamW.WriteBool(fAscending);
	iStreamW.WriteUInt8(fStrength);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_All

inline ZTBQueryNode_All::ZTBQueryNode_All(const ZStreamR& iStreamR)
	{}

ZTBQueryNode_All::ZTBQueryNode_All()
	{}

ZTBQueryNode_All::~ZTBQueryNode_All()
	{}

ZTuple ZTBQueryNode_All::AsTuple()
	{
	ZTuple theTuple;
	theTuple.SetString("Kind", "All");
	return theTuple;
	}

void ZTBQueryNode_All::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(1);
	}

int ZTBQueryNode_All::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_All(this); }

int ZTBQueryNode_All::pCompare_All(ZTBQueryNode_All* iAll)
	{ return 0; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Combo::Intersection

ZTBQueryNode_Combo::Intersection::Intersection(const ZStreamR& iStreamR)
	{
	fFilter = ZTBSpec(iStreamR);
	if (uint32 theCount = sReadCount(iStreamR))
		{
		fNodes.reserve(theCount);
		while (theCount--)
			fNodes.push_back(sNodeFromStream(iStreamR));
		}
	}

ZTBQueryNode_Combo::Intersection::Intersection(const ZTuple& iTuple)
	{
	fFilter = ZTBSpec(iTuple.GetTuple("Filter"));
	const vector<ZTValue>& nodes = iTuple.GetVector("Nodes");
	for (vector<ZTValue>::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
		{
		if (ZRef<ZTBQueryNode> aNode = sNodeFromTuple((*i).GetTuple()))
			fNodes.push_back(aNode);
		}
	}

ZTuple ZTBQueryNode_Combo::Intersection::AsTuple() const
	{
	ZTuple result;
	result.SetTuple("Filter", fFilter.AsTuple());
	vector<ZTValue>& destVec = result.SetMutableVector("Nodes");
	for (vector<ZRef<ZTBQueryNode> >::const_iterator i = fNodes.begin(); i != fNodes.end(); ++i)
		destVec.push_back(sTupleFromNode(*i));
	return result;
	}

void ZTBQueryNode_Combo::Intersection::ToStream(const ZStreamW& iStreamW) const
	{
	fFilter.ToStream(iStreamW);

	sWriteCount(iStreamW, fNodes.size());
	for (vector<ZRef<ZTBQueryNode> >::const_iterator i = fNodes.begin(); i != fNodes.end(); ++i)
		sNodeToStream(iStreamW, *i);
	}

int ZTBQueryNode_Combo::Intersection::Compare(const Intersection& iOther) const
	{
	if (int compare = fFilter.Compare(iOther.fFilter))
		return compare;

	return ZooLib::sCompare_T(fNodes, iOther.fNodes);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Combo

static void sSimplify(vector<ZTBQueryNode_Combo::Intersection>& ioIntersections)
	{
	bool gotSimpleOnes = false;
	ZTBSpec simpleFilter;
	for (vector<ZTBQueryNode_Combo::Intersection>::iterator i = ioIntersections.begin();
		i != ioIntersections.end(); /*no inc*/)
		{
		bool gotAnAll = false;
		for (vector<ZRef<ZTBQueryNode> >::iterator j = (*i).fNodes.begin();
			j != (*i).fNodes.end(); /*no incr*/)
			{
			if (ZRef<ZTBQueryNode_All> theAll = ZRefDynamicCast<ZTBQueryNode_All>(*j))
				{
				j = (*i).fNodes.erase(j);
				gotAnAll = true;
				}
			else
				{
				++j;
				}
			}

		if ((*i).fNodes.empty())
			{
			if (gotAnAll)	
				{
				gotSimpleOnes = true;
				simpleFilter |= (*i).fFilter;
				}
			i = ioIntersections.erase(i);
			}
		else
			{
			++i;
			}
		}
	if (gotSimpleOnes)
		{
		ioIntersections.push_back(
			ZTBQueryNode_Combo::Intersection(simpleFilter, new ZTBQueryNode_All));
		}
	}

ZTBQueryNode_Combo::ZTBQueryNode_Combo(const ZStreamR& iStreamR)
	{
	if (uint32 theCount = sReadCount(iStreamR))
		{
		fSort.reserve(theCount);
		while (theCount--)
			fSort.push_back(ZTBQuery::SortSpec(iStreamR));
		}

	if (uint32 theCount = sReadCount(iStreamR))
		{
		fIntersections.reserve(theCount);
		while (theCount--)
			fIntersections.push_back(Intersection(iStreamR));
		}
	}

ZTBQueryNode_Combo::ZTBQueryNode_Combo(
	vector<ZTBQuery::SortSpec>& ioSort, vector<Intersection>& ioIntersections)
	{
	fSort.swap(ioSort),
	fIntersections.swap(ioIntersections);
	sSimplify(fIntersections);
	}

ZTBQueryNode_Combo::~ZTBQueryNode_Combo()
	{}

ZTuple ZTBQueryNode_Combo::AsTuple()
	{
	ZTuple result;
	result.SetString("Kind", "Combo");

	if (!fSort.empty())
		{
		vector<ZTValue>& sortVector = result.SetMutableVector("Sort");
		for (vector<ZTBQuery::SortSpec>::iterator i = fSort.begin(); i != fSort.end(); ++i)
			{
			ZTuple theTuple;
			theTuple.SetString("PropName", (*i).fPropName.AsString());
			theTuple.SetBool("Ascending", (*i).fAscending);
			theTuple.SetInt32("Strength", (*i).fStrength);
			sortVector.push_back(theTuple);
			}
		}

	vector<ZTValue>& sectVector = result.SetMutableVector("Intersections");
	for (vector<Intersection>::iterator i = fIntersections.begin(); i != fIntersections.end(); ++i)
		sectVector.push_back((*i).AsTuple());

	return result;
	}

void ZTBQueryNode_Combo::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(2);

	sWriteCount(iStreamW, fSort.size());
	for (vector<ZTBQuery::SortSpec>::iterator i = fSort.begin(); i != fSort.end(); ++i)
		(*i).ToStream(iStreamW);

	sWriteCount(iStreamW, fIntersections.size());
	for (vector<Intersection>::iterator i = fIntersections.begin(); i != fIntersections.end(); ++i)
		(*i).ToStream(iStreamW);
	}

int ZTBQueryNode_Combo::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_Combo(this); }

int ZTBQueryNode_Combo::pCompare_Combo(ZTBQueryNode_Combo* iOther)
	{
	if (int compare = ZooLib::sCompare_T(fSort, iOther->fSort))
		return compare;

	return ZooLib::sCompare_T(fIntersections, iOther->fIntersections);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Difference

inline ZTBQueryNode_Difference::ZTBQueryNode_Difference(const ZStreamR& iStreamR)
	{
	fLeftNode = sNodeFromStream(iStreamR);
	fRightNode = sNodeFromStream(iStreamR);
	}

ZTBQueryNode_Difference::ZTBQueryNode_Difference(
	const ZRef<ZTBQueryNode>& iLeftNode, const ZRef<ZTBQueryNode>& iRightNode)
:	fLeftNode(iLeftNode),
	fRightNode(iRightNode)
	{}

ZTBQueryNode_Difference::~ZTBQueryNode_Difference()
	{}

ZTuple ZTBQueryNode_Difference::AsTuple()
	{
	ZTuple theTuple;
	theTuple.SetString("Kind", "Difference");
	if (fLeftNode)
		theTuple.SetTuple("LeftNode", fLeftNode->AsTuple());
	if (fRightNode)
		theTuple.SetTuple("RightNode", fRightNode->AsTuple());
	return theTuple;
	}

void ZTBQueryNode_Difference::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(3);
	sNodeToStream(iStreamW, fLeftNode);
	sNodeToStream(iStreamW, fRightNode);
	}

int ZTBQueryNode_Difference::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_Difference(this); }

int ZTBQueryNode_Difference::pCompare_Difference(ZTBQueryNode_Difference* iOther)
	{
	if (int compare = fLeftNode->Compare(iOther->fLeftNode))
		return compare;

	return fRightNode->Compare(iOther->fRightNode);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_First

ZTBQueryNode_First::ZTBQueryNode_First(const ZStreamR& iStreamR)
:	fPropName(iStreamR),
	fSourceNode(sNodeFromStream(iStreamR))
	{}

ZTBQueryNode_First::ZTBQueryNode_First(
	const ZTName& iPropName, const ZRef<ZTBQueryNode>& iSourceNode)
:	fPropName(iPropName),
	fSourceNode(iSourceNode)
	{}

ZTBQueryNode_First::~ZTBQueryNode_First()
	{}

ZTuple ZTBQueryNode_First::AsTuple()
	{
	ZTuple theTuple;
	theTuple.SetString("Kind", "First");
	theTuple.SetString("PropName", fPropName.AsString());
	if (fSourceNode)
		theTuple.SetTuple("SourceNode", fSourceNode->AsTuple());
	return theTuple;
	}

void ZTBQueryNode_First::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(4);
	fPropName.ToStream(iStreamW);
	sNodeToStream(iStreamW, fSourceNode);
	}

int ZTBQueryNode_First::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_First(this); }

int ZTBQueryNode_First::pCompare_First(ZTBQueryNode_First* iOther)
	{
	if (int compare = fPropName.Compare(iOther->fPropName))
		return compare;

	return fSourceNode->Compare(iOther->fSourceNode);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_ID_Constant

ZTBQueryNode_ID_Constant::ZTBQueryNode_ID_Constant(const ZStreamR& iStreamR)
	{
	if (uint32 theCount = sReadCount(iStreamR))
		{
		fIDs.reserve(theCount);
		while (theCount--)
			fIDs.push_back(iStreamR.ReadUInt64());
		}
	}

ZTBQueryNode_ID_Constant::ZTBQueryNode_ID_Constant(uint64 iID)
	{
	if (iID)
		fIDs.push_back(iID);
	}

ZTBQueryNode_ID_Constant::ZTBQueryNode_ID_Constant(const set<uint64>& iIDs)
:	fIDs(iIDs.begin(), iIDs.end())
	{}

ZTBQueryNode_ID_Constant::ZTBQueryNode_ID_Constant(const vector<uint64>& iIDs)
:	fIDs(iIDs)
	{}

ZTBQueryNode_ID_Constant::ZTBQueryNode_ID_Constant(vector<uint64>& ioIDs, bool iKnowWhatImDoing)
	{
	ZAssertStop(kDebug_TBQuery, iKnowWhatImDoing);
	fIDs.swap(ioIDs);
	}

ZTBQueryNode_ID_Constant::ZTBQueryNode_ID_Constant(const uint64* iIDs, size_t iCount)
:	fIDs(iIDs, iIDs + iCount)
	{}

ZTBQueryNode_ID_Constant::~ZTBQueryNode_ID_Constant()
	{}

ZTuple ZTBQueryNode_ID_Constant::AsTuple()
	{
	ZTuple theTuple;
	theTuple.SetString("Kind", "ID_Constant");
	theTuple.SetVector_T("IDs", fIDs.begin(), fIDs.end());
	return theTuple;
	}

void ZTBQueryNode_ID_Constant::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(5);
	sWriteCount(iStreamW, fIDs.size());
	for (vector<uint64>::iterator i = fIDs.begin(), theEnd = fIDs.end();
		i != theEnd; ++i)
		{
		iStreamW.WriteUInt64(*i);
		}
	}

int ZTBQueryNode_ID_Constant::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_ID_Constant(this); }

int ZTBQueryNode_ID_Constant::pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iOther)
	{ return ZooLib::sCompare_T(fIDs, iOther->fIDs); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_ID_FromSource

inline ZTBQueryNode_ID_FromSource::ZTBQueryNode_ID_FromSource(const ZStreamR& iStreamR)
:	fSourcePropName(iStreamR),
	fSourceNode(sNodeFromStream(iStreamR))
	{}

ZTBQueryNode_ID_FromSource::ZTBQueryNode_ID_FromSource(
	const ZRef<ZTBQueryNode>& iSourceNode, const ZTName& iSourcePropName)
:	fSourcePropName(iSourcePropName),
	fSourceNode(iSourceNode)
	{}

ZTBQueryNode_ID_FromSource::~ZTBQueryNode_ID_FromSource()
	{}

ZTuple ZTBQueryNode_ID_FromSource::AsTuple()
	{
	ZTuple theTuple;
	theTuple.SetString("Kind", "ID_FromSource");
	theTuple.SetString("SourcePropName", fSourcePropName.AsString());
	if (fSourceNode)
		theTuple.SetTuple("SourceNode", fSourceNode->AsTuple());
	return theTuple;
	}

void ZTBQueryNode_ID_FromSource::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(6);
	fSourcePropName.ToStream(iStreamW);
	sNodeToStream(iStreamW, fSourceNode);
	}

int ZTBQueryNode_ID_FromSource::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_ID_FromSource(this); }

int ZTBQueryNode_ID_FromSource::pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iOther)
	{
	if (int compare = fSourcePropName.Compare(iOther->fSourcePropName))
		return compare;

	return fSourceNode->Compare(iOther->fSourceNode);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Property

inline ZTBQueryNode_Property::ZTBQueryNode_Property(const ZStreamR& iStreamR)
:	fPropName(iStreamR),
	fSourceNode(sNodeFromStream(iStreamR))
	{}

ZTBQueryNode_Property::ZTBQueryNode_Property(
	const ZTName& iPropName, const ZRef<ZTBQueryNode>& iSourceNode)
:	fPropName(iPropName),
	fSourceNode(iSourceNode)
	{}

ZTBQueryNode_Property::~ZTBQueryNode_Property()
	{}

ZTuple ZTBQueryNode_Property::AsTuple()
	{
	ZTuple theTuple;
	theTuple.SetString("Kind", "Property");
	theTuple.SetString("PropName", fPropName.AsString());
	if (fSourceNode)
		theTuple.SetTuple("SourceNode", fSourceNode->AsTuple());
	return theTuple;
	}

void ZTBQueryNode_Property::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt8(7);
	fPropName.ToStream(iStreamW);
	sNodeToStream(iStreamW, fSourceNode);
	}

int ZTBQueryNode_Property::pRevCompare(ZTBQueryNode* iOther)
	{ return iOther->pCompare_Property(this); }

int ZTBQueryNode_Property::pCompare_Property(ZTBQueryNode_Property* iOther)
	{
	if (int compare = fPropName.Compare(iOther->fPropName))
		return compare;

	return fSourceNode->Compare(iOther->fSourceNode);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static ZTuple sTupleFromNode(const ZRef<ZTBQueryNode>& iNode)
	{
	if (iNode)
		return iNode->AsTuple();
	return ZTuple();
	}

static ZRef<ZTBQueryNode> sNodeFromTuple(const ZTuple& iTuple)
	{
	string nodeKind = iTuple.GetString("Kind");
	if (false)
		{}
	else if (nodeKind == "All")
		{
		return new ZTBQueryNode_All;
		}
	else if (nodeKind == "Combo")
		{
		vector<ZTBQuery::SortSpec> theSort;
		const vector<ZTValue>& vectorSort = iTuple.GetVector("Sort");
		for (vector<ZTValue>::const_iterator i = vectorSort.begin(); i != vectorSort.end(); ++i)
			{
			const ZTuple& temp = (*i).GetTuple();
			theSort.push_back(ZTBQuery::SortSpec(
				ZTName(temp.GetString("PropName")), temp.GetBool("Ascending"), temp.GetInt32("Strength")));
			}

		const vector<ZTValue>& sourceSect = iTuple.GetVector("Intersections");
		vector<ZTBQueryNode_Combo::Intersection> theIntersections;
		for (vector<ZTValue>::const_iterator i = sourceSect.begin(); i != sourceSect.end(); ++i)
			theIntersections.push_back(ZTBQueryNode_Combo::Intersection((*i).GetTuple()));

		return new ZTBQueryNode_Combo(theSort, theIntersections);
		}
	else if (nodeKind == "Difference")
		{
		ZRef<ZTBQueryNode> leftNode = sNodeFromTuple(iTuple.GetTuple("LeftNode"));
		ZRef<ZTBQueryNode> rightNode = sNodeFromTuple(iTuple.GetTuple("RightNode"));
		return new ZTBQueryNode_Difference(leftNode, rightNode);
		}
	else if (nodeKind == "First")
		{
		string propName = iTuple.GetString("PropName");
		ZRef<ZTBQueryNode> sourceNode = sNodeFromTuple(iTuple.GetTuple("SourceNode"));
		return new ZTBQueryNode_First(ZTName(propName), sourceNode);
		}
	else if (nodeKind == "ID_Constant")
		{
		vector<uint64> theIDs;
		iTuple.GetVector_T("IDs", back_inserter(theIDs), uint64());
		return new ZTBQueryNode_ID_Constant(theIDs, true);
		}
	else if (nodeKind == "ID_FromSource")
		{
		ZRef<ZTBQueryNode> sourceNode = sNodeFromTuple(iTuple.GetTuple("SourceNode"));
		string sourcePropName = iTuple.GetString("SourcePropName");
		return new ZTBQueryNode_ID_FromSource(sourceNode, ZTName(sourcePropName));
		}
	else if (nodeKind == "Property")
		{
		string propName = iTuple.GetString("PropName");
		ZRef<ZTBQueryNode> sourceNode = sNodeFromTuple(iTuple.GetTuple("SourceNode"));
		return new ZTBQueryNode_Property(ZTName(propName), sourceNode);
		}
	throw std::runtime_error(string("ZTBQuery, sNodeFromTuple, unknown nodeKind: " + nodeKind));
	}

static ZRef<ZTBQueryNode> sNodeFromStream(const ZStreamR& iStreamR)
	{
	uint8 theType = iStreamR.ReadUInt8();
	switch (theType)
		{
		case 0: return ZRef<ZTBQueryNode>();
		case 1: return new ZTBQueryNode_All(iStreamR);
		case 2: return new ZTBQueryNode_Combo(iStreamR);
		case 3: return new ZTBQueryNode_Difference(iStreamR);
		case 4: return new ZTBQueryNode_First(iStreamR);
		case 5: return new ZTBQueryNode_ID_Constant(iStreamR);
		case 6: return new ZTBQueryNode_ID_FromSource(iStreamR);
		case 7: return new ZTBQueryNode_Property(iStreamR);
		}
	throw std::runtime_error("ZTBQuery, sNodeFromStream, unknown node type");
	}

static void sNodeToStream(const ZStreamW& iStreamW, const ZRef<ZTBQueryNode>& iNode)
	{
	if (iNode)
		iNode->ToStream(iStreamW);
	else
		iStreamW.WriteUInt8(0);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT

int ZTBQueryNode_GT::pCompare_All(ZTBQueryNode_All* iAll)
	{ return -1; }

int ZTBQueryNode_GT::pCompare_Combo(ZTBQueryNode_Combo* iCombo)
	{ return -1; }

int ZTBQueryNode_GT::pCompare_Difference(ZTBQueryNode_Difference* iDifference)
	{ return -1; }

int ZTBQueryNode_GT::pCompare_First(ZTBQueryNode_First* iFirst)
	{ return -1; }

int ZTBQueryNode_GT::pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iID_Constant)
	{ return -1; }

int ZTBQueryNode_GT::pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iID_FromSource)
	{ return -1; }

int ZTBQueryNode_GT::pCompare_Property(ZTBQueryNode_Property* iProperty)
	{ return -1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_All

int ZTBQueryNode_GT_All::pCompare_All(ZTBQueryNode_All* iAll)
	{ return 1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_Combo

int ZTBQueryNode_GT_Combo::pCompare_Combo(ZTBQueryNode_Combo* iCombo)
	{ return 1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_Difference

int ZTBQueryNode_GT_Difference::pCompare_Difference(ZTBQueryNode_Difference* iDifference)
	{ return 1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_First

int ZTBQueryNode_GT_First::pCompare_First(ZTBQueryNode_First* iFirst)
	{ return 1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_ID_Constant

int ZTBQueryNode_GT_ID_Constant::pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iID_Constant)
	{ return 1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_ID_FromSource

int ZTBQueryNode_GT_ID_FromSource::pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iID_FromSource)
	{ return 1; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_Property

int ZTBQueryNode_GT_Property::pCompare_Property(ZTBQueryNode_Property* iProperty)
	{ return 1; }
