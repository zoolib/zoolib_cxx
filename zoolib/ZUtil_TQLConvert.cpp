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

#include "ZUtil_TQLConvert.h"

#include "ZTQL_Spec.h"

using ZTQL::Query;
using ZTQL::RelHead;

#define VERBOSE 0

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers.

static const ZTName sIDName("$ID$");

static ZTQL::Spec sAsTSpec(const ZTBSpec& iTBSpec)
	{
	const ZTBSpec::CriterionUnion theCU = iTBSpec.GetCriterionUnion();
	bool uisfirst = true;
	ZTQL::Spec theTSpec;
	for (vector<ZTBSpec::CriterionSect>::const_iterator uiter = theCU.begin();
		uiter != theCU.end(); ++uiter)
		{
		ZTQL::Spec sect;
		bool sisfirst = true;
		for (vector<ZTBSpec::Criterion>::const_iterator siter = uiter->begin();
			siter != uiter->end(); ++siter)
			{
			ZRef<ZTQL::ComparatorRep> theComparatorRep;
			using ZTQL::ComparatorRep_Simple;
			switch (siter->GetComparator().fRel)
				{
				case ZTBSpec::eRel_Less:
					theComparatorRep = new ComparatorRep_Simple(ComparatorRep_Simple::eLT);
					break;
				case ZTBSpec::eRel_LessEqual:
					theComparatorRep = new ComparatorRep_Simple(ComparatorRep_Simple::eLE);
					break;
				case ZTBSpec::eRel_Equal:
					theComparatorRep = new ComparatorRep_Simple(ComparatorRep_Simple::eEQ);
					break;
				case ZTBSpec::eRel_GreaterEqual:
					theComparatorRep = new ComparatorRep_Simple(ComparatorRep_Simple::eGE);
					break;
				case ZTBSpec::eRel_Greater:
					theComparatorRep = new ComparatorRep_Simple(ComparatorRep_Simple::eGT);
					break;
				default:
					ZUnimplemented();
					break;
				}
			ZRef<ZTQL::ComparandRep> lhs = new ZTQL::ComparandRep_Name(siter->GetPropName());
			ZRef<ZTQL::ComparandRep> rhs = new ZTQL::ComparandRep_Value(siter->GetTValue());
			ZTQL::Condition theCondition(lhs, theComparatorRep, rhs);
			if (sisfirst)
				{
				sisfirst = false;
				sect = theCondition;
				}
			else
				{
				sect &= theCondition;
				}
			}
		if (uisfirst)
			{
			uisfirst = false;
			theTSpec = sect;
			}
		else
			{
			theTSpec |= sect;
			}
			
		}

	return theTSpec;;
	}

/*
sConvert returns a query whose RelHead is [$ID$], unless iName is non-null in which
case the RelHead is [*iName]. The tuples returned have been filtered by iFilter, if any.
*/

static Query sConvert(ZRef<ZTBQueryNode> iNode, const ZTName* iName, ZTQL::Spec* iFilter)
	{
	if (ZTBQueryNode_All* theNode_All =
		ZRefDynamicCast<ZTBQueryNode_All>(iNode))
		{
		// We must have a filter, it's not possible to construct a ZTBQueryNode_All
		// except as a clause in a combo with a filter.
		ZAssert(iFilter);

		const RelHead filterRelHead = iFilter->GetRelHead();

		Query theQ;
		if (iName)
			{
			theQ = ZTQL::sAll(filterRelHead | *iName).Select(*iFilter);
			if (VERBOSE)
				theQ = theQ.Project(*iName | ZTName("$$All_Name$$"));
			else
				theQ = theQ.Project(*iName);
			}
		else
			{
			theQ = ZTQL::sAllID(sIDName, filterRelHead).Select(*iFilter);
			if (VERBOSE)
				theQ = theQ.Project(sIDName | ZTName("$$All$$"));
			else
				theQ = theQ.Project(sIDName);
			}
		return theQ;
		}
	else if (ZTBQueryNode_First* theNode_First =
		ZRefDynamicCast<ZTBQueryNode_First>(iNode))
		{
		ZDebugStop(0);
		// Don't support first, yet, if ever.
		}
	else if (ZTBQueryNode_Combo* theNode_Combo =
		ZRefDynamicCast<ZTBQueryNode_Combo>(iNode))
		{
		ZAssert(theNode_Combo->GetSort().empty());
		// Don't support sorting, yet.

		const vector<ZTBQueryNode_Combo::Intersection>& theIntersections
			= theNode_Combo->GetIntersections();

		Query outerQ;
		bool outerIsFirst = true;
		for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
			i = theIntersections.begin(); i != theIntersections.end(); ++i)
			{
			const ZTBQueryNode_Combo::Intersection& theIntersection = *i;

			ZTQL::Spec theFilter = sAsTSpec(theIntersection.fFilter);

			if (iFilter)
				theFilter &= *iFilter;
			
			Query innerQ;
			bool isFirst = true;			
			for (vector<ZRef<ZTBQueryNode> >::const_iterator
				iterNodes = theIntersection.fNodes.begin(), theEnd = theIntersection.fNodes.end();
				iterNodes != theEnd; ++iterNodes)
				{
				Query innermostQ = sConvert(*iterNodes, iName, &theFilter);
				if (isFirst)
					{
					innerQ = innermostQ;
					isFirst = false;
					}
				else
					{
					innerQ = innerQ.Intersect(innermostQ);
					}
				}

			if (outerIsFirst)
				{
				outerIsFirst = false;
				outerQ = innerQ;
				}
			else
				{
				outerQ = outerQ.Union(innerQ);
				}
			}

		if (iName)
			{
			if (VERBOSE)
				return outerQ.Project(*iName | ZTName("$$ComboReturn_Name$$"));
			else
				return outerQ;//.Project(*iName);
			}
		else
			{
			if (VERBOSE)
				return outerQ.Project(sIDName | ZTName("$$ComboReturn"));
			else
				return outerQ;//.Project(sIDName);
			}
		}
	else if (ZTBQueryNode_ID_Constant* theNode_ID_Constant =
		ZRefDynamicCast<ZTBQueryNode_ID_Constant>(iNode))
		{
		const vector<uint64>& theIDs = theNode_ID_Constant->GetIDs();
		vector<ZTuple> theTuples;
		theTuples.reserve(theIDs.size());
		for (vector<uint64>::const_iterator i = theIDs.begin(); i != theIDs.end(); ++i)
			{
			ZTuple cur;
			cur.SetID(sIDName, *i);
			theTuples.push_back(cur);
			}
		Query theQ = ZTQL::sExplicit(theTuples);
		return theQ;
		#if 0
		if (iFilter)
			{
			const ZTQL::RelHead theRelHead = sMakeRelHead(iFilter);
			theQ = ZTQL::sAll(sIDName, theRelHead).Join(theQ);
			theQ = theQ.Select(*iFilter);
			}

		if (iName)
			{
			#if VERBOSE
				return theQ.Project(*iName | ZTName("$$ComboReturn$$"));
			#else
				return theQ.Project(*iName);
			#endif
			}
		else
			{
			return theQ;
			}
		#endif
		}
	else if (ZTBQueryNode_ID_FromSource* theNode_ID_FromSource =
		ZRefDynamicCast<ZTBQueryNode_ID_FromSource>(iNode))
		{
		ZRef<ZTBQueryNode> sourceNode = theNode_ID_FromSource->GetSourceNode();
		const ZTName sourcePropName = theNode_ID_FromSource->GetSourcePropName();

		// Get the source tuples' sourcePropName
		Query theQ = sConvert(sourceNode, &sourcePropName, nil);

		// Rename it to sIDName.
		theQ = theQ.Rename(sourcePropName, sIDName);
		
		// Get the referenced tuples and filter them
		if (iFilter)
			{
			const RelHead filterRelHead = iFilter->GetRelHead();
			if (iName)
				{
				theQ = ZTQL::sAllID(sIDName, filterRelHead | *iName).Join(theQ).Select(*iFilter);
				if (VERBOSE)
					theQ = theQ.Project(*iName | ZTName("$$FromSource_Name_Filter$$"));
				else
					theQ = theQ.Project(*iName);
				}
			else
				{
				theQ = ZTQL::sAllID(sIDName, filterRelHead).Join(theQ).Select(*iFilter);
				if (VERBOSE)
					theQ = theQ.Project(sIDName | ZTName("$$FromSource_Filter$$"));
				else
					theQ = theQ.Project(sIDName);
				}
			}
		else
			{
			if (iName)
				{
				theQ = ZTQL::sAllID(sIDName, *iName).Join(theQ);
				if (VERBOSE)
					theQ = theQ.Project(*iName | ZTName("$$FromSource_Name$$"));
				else
					theQ = theQ.Project(*iName);
				}
			else
				{
				// Do nothing -- already have the IDs.
				if (VERBOSE)
					theQ = theQ.Project(ZTName("$$FromSource$$"));
				}
			}
		return theQ;
		}
	else if (ZTBQueryNode_Property* theNode_Property =
		ZRefDynamicCast<ZTBQueryNode_Property>(iNode))
		{
		ZRef<ZTBQueryNode> sourceNode = theNode_Property->GetSourceNode();
		const ZTName thePropName = theNode_Property->GetPropName();

		// Get the source tuples' IDs.
		Query theQ = sConvert(sourceNode, nil, nil);

		// Renamed to thePropName;
		theQ = theQ.Rename(sIDName, thePropName);

		// Get tuples whose property 'thePropName' match the IDs and filter them
		if (iFilter)
			{
			const RelHead filterRelHead = iFilter->GetRelHead();
			if (iName)
				{
				theQ = ZTQL::sAllID(sIDName, filterRelHead | *iName | thePropName).Join(theQ);
				theQ = theQ.Project(filterRelHead | *iName);
				theQ = theQ.Select(*iFilter);
				if (VERBOSE)
					theQ = theQ.Project(*iName | ZTName("$$Property_Name_Filter$$"));
				else
					theQ = theQ.Project(*iName);
				}
			else
				{
				theQ = ZTQL::sAllID(sIDName, filterRelHead | thePropName).Join(theQ);
				theQ = theQ.Project(filterRelHead);
				theQ = theQ.Select(*iFilter);
				if (VERBOSE)
					theQ = theQ.Project(sIDName | ZTName("$$Property_Filter$$"));
				else
					theQ = theQ.Project(sIDName);
				}
			}
		else
			{
			if (iName)
				{
				theQ = ZTQL::sAllID(sIDName, *iName | thePropName).Join(theQ);
				if (VERBOSE)
					theQ = theQ.Project(*iName | ZTName("$$Property_Name"));
				else
					theQ = theQ.Project(*iName);
				}
			else
				{
				theQ = ZTQL::sAllID(sIDName, thePropName).Join(theQ);
				if (VERBOSE)
					theQ = theQ.Project(sIDName | ZTName("$$Property"));
				else
					theQ = theQ.Project(sIDName);
				}
			}
		return theQ;
		}

	return Query();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TQLConvert

ZTQL::Query ZUtil_TQLConvert::sConvert(const ZTBQuery& iTBQuery)
	{
	return sConvert(iTBQuery.GetNode(), nil, nil);
	}
