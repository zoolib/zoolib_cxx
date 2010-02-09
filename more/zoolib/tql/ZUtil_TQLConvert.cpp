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

#include "zoolib/tql/ZUtil_TQLConvert.h"
#include "zoolib/ZExpr_Query.h"
#include "zoolib/ZExpr_ValCondition.h"

NAMESPACE_ZOOLIB_BEGIN

typedef ZExpr_Logical Spec;
typedef ZExpr_Relation Query;
typedef ZMap_Expr Map;
typedef ZRelHead RelHead;
typedef ZVal_Expr Val;
typedef ZValCondition Condition;

namespace ZUtil_TQLConvert {

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers.

static const string sIDName = "$ID$";

static Spec sAsTSpec(const ZTBSpec& iTBSpec)
	{
	const ZTBSpec::CriterionUnion theCU = iTBSpec.GetCriterionUnion();
	bool uisfirst = true;
	Spec theTSpec;
	for (vector<ZTBSpec::CriterionSect>::const_iterator uiter = theCU.begin();
		uiter != theCU.end(); ++uiter)
		{
		Spec sect;
		bool sisfirst = true;
		for (vector<ZTBSpec::Criterion>::const_iterator siter = uiter->begin();
			siter != uiter->end(); ++siter)
			{
			ZValComparatorRep_Simple_T<Val>::EComparator theComparator;
			switch (siter->GetComparator().fRel)
				{
				case ZTBSpec::eRel_Less:
					theComparator = ZValComparatorRep_Simple_T<Val>::eLT;
					break;
				case ZTBSpec::eRel_LessEqual:
					theComparator = ZValComparatorRep_Simple_T<Val>::eLE;
					break;
				case ZTBSpec::eRel_Equal:
					theComparator = ZValComparatorRep_Simple_T<Val>::eEQ;
					break;
				case ZTBSpec::eRel_GreaterEqual:
					theComparator = ZValComparatorRep_Simple_T<Val>::eGE;
					break;
				case ZTBSpec::eRel_Greater:
					theComparator = ZValComparatorRep_Simple_T<Val>::eGT;
					break;
				default:
					ZUnimplemented();
					break;
				}

			ZValCondition_T<Val> theCondition(
				CName(siter->GetPropName()),
				new ZValComparatorRep_Simple_T<Val>(theComparator),
				CConst(siter->GetTValue()));

			if (sisfirst)
				{
				sisfirst = false;
				sect = ZExpr_Logical(new ZExprRep_ValCondition(theCondition));
				}
			else
				{
				sect = sect & theCondition;
//				sect &= theCondition;
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

static Query sConvert(ZRef<ZTBQueryNode> iNode, const string* iName, Spec* iFilter, bool iVerbose)
	{
	if (ZRefDynamicCast<ZTBQueryNode_All>(iNode))
		{
		// We must have a filter, it's not possible to construct a ZTBQueryNode_All
		// except as a clause in a combo with a filter.
		ZAssert(iFilter);

		const RelHead filterRelHead = sGetRelHead(*iFilter);

		Query theQ;
		if (iName)
			{
			theQ = sSelect(*iFilter, sAll(filterRelHead | *iName));
			if (iVerbose)
				theQ = sProject(RelHead(*iName) | "$$All_Name$$", theQ);
			else
				theQ = sProject(*iName, theQ);
			}
		else
			{
			theQ = sSelect(*iFilter, sAllID(sIDName, filterRelHead));
			if (iVerbose)
				theQ = sProject(RelHead(sIDName) | "$$All$$", theQ);
			else
				theQ = sProject(sIDName, theQ);
			}
		return theQ;
		}
	else if (ZRefDynamicCast<ZTBQueryNode_First>(iNode))
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

			Spec theFilter = sAsTSpec(theIntersection.fFilter);

			if (iFilter)
				theFilter &= *iFilter;
			
			Query innerQ;
			bool isFirst = true;			
			for (vector<ZRef<ZTBQueryNode> >::const_iterator
				iterNodes = theIntersection.fNodes.begin(), theEnd = theIntersection.fNodes.end();
				iterNodes != theEnd; ++iterNodes)
				{
				Query innermostQ = sConvert(*iterNodes, iName, &theFilter, iVerbose);
				if (isFirst)
					{
					innerQ = innermostQ;
					isFirst = false;
					}
				else
					{
					innerQ = sIntersect(innermostQ, innerQ);
					}
				}

			if (outerIsFirst)
				{
				outerIsFirst = false;
				outerQ = innerQ;
				}
			else
				{
				outerQ = sUnion(innerQ, outerQ);
				}
			}

		if (iName)
			{
			if (iVerbose)
				return sProject(RelHead(*iName) | "$$ComboReturn_Name$$", outerQ);
			else
				return outerQ;//.Project(*iName);
			}
		else
			{
			if (iVerbose)
				return sProject(RelHead(sIDName) | "$$ComboReturn$$", outerQ);
			else
				return outerQ;//.Project(sIDName);
			}
		}
	else if (ZTBQueryNode_ID_Constant* theNode_ID_Constant =
		ZRefDynamicCast<ZTBQueryNode_ID_Constant>(iNode))
		{
		const vector<uint64>& theIDs = theNode_ID_Constant->GetIDs();
		vector<Val> theVals;
		theVals.reserve(theIDs.size());
		for (vector<uint64>::const_iterator i = theIDs.begin(); i != theIDs.end(); ++i)
			{
			Map cur;
			cur.Set(sIDName, Val(*i));
			theVals.push_back(cur);
			}
		Query theQ = sExplicit(&theVals[0], theVals.size());
		return theQ;
		#if 0
		if (iFilter)
			{
			const RelHead theRelHead = sMakeRelHead(iFilter);
			theQ = sAll(sIDName, theRelHead).Join(theQ);
			theQ = theQ.Select(*iFilter);
			}

		if (iName)
			{
			#if iVerbose
				return theQ.Project(RelHead(*iName) | "$$ComboReturn$$");
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
		const string sourcePropName = theNode_ID_FromSource->GetSourcePropName();

		// Get the source tuples' sourcePropName
		Query theQ = sConvert(sourceNode, &sourcePropName, nullptr, iVerbose);

		// Rename it to sIDName.
		theQ = sRename(sourcePropName, sIDName, theQ);
		
		// Get the referenced tuples and filter them
		if (iFilter)
			{
			const RelHead filterRelHead = sGetRelHead(*iFilter);
			if (iName)
				{
				theQ = sJoin(theQ, sSelect(*iFilter, sAllID(sIDName, filterRelHead | *iName)));
				if (iVerbose)
					theQ = sProject(RelHead(*iName) | "$$FromSource_Name_Filter$$", theQ);
				else
					theQ = sProject(*iName, theQ);
				}
			else
				{
				theQ = sJoin(theQ, sSelect(*iFilter, sAllID(sIDName, filterRelHead)));
				if (iVerbose)
					theQ = sProject(RelHead(sIDName) | "$$FromSource_Filter$$", theQ);
				else
					theQ = sProject(sIDName, theQ);
				}
			}
		else
			{
			if (iName)
				{
				theQ = sJoin(theQ, sAllID(sIDName, *iName));
				if (iVerbose)
					theQ = sProject(RelHead(*iName) | "$$FromSource_Name$$", theQ);
				else
					theQ = sProject(*iName, theQ);
				}
			else
				{
				// Do nothing -- already have the IDs.
				if (iVerbose)
					theQ = sProject("$$FromSource$$", theQ);
				}
			}
		return theQ;
		}
	else if (ZTBQueryNode_Property* theNode_Property =
		ZRefDynamicCast<ZTBQueryNode_Property>(iNode))
		{
		ZRef<ZTBQueryNode> sourceNode = theNode_Property->GetSourceNode();
		const string thePropName = theNode_Property->GetPropName();

		// Get the source tuples' IDs.
		Query theQ = sConvert(sourceNode, nullptr, nullptr, iVerbose);

		// Renamed to thePropName;
		theQ = sRename(sIDName, thePropName, theQ);

		// Get tuples whose property 'thePropName' match the IDs and filter them
		if (iFilter)
			{
			const RelHead filterRelHead = sGetRelHead(*iFilter);
			if (iName)
				{
				theQ = sJoin(theQ, sAllID(sIDName, filterRelHead | *iName | thePropName));
				theQ = sProject(filterRelHead | *iName, theQ);
				theQ = sSelect(*iFilter, theQ);
				if (iVerbose)
					theQ = sProject(RelHead(*iName) | "$$Property_Name_Filter$$", theQ);
				else
					theQ = sProject(*iName, theQ);
				}
			else
				{
				theQ = sJoin(theQ, sAllID(sIDName, filterRelHead | thePropName));
				theQ = sProject(filterRelHead, theQ);
				theQ = sSelect(*iFilter, theQ);
				if (iVerbose)
					theQ = sProject(RelHead(sIDName) | "$$Property_Filter$$", theQ);
				else
					theQ = sProject(sIDName, theQ);
				}
			}
		else
			{
			if (iName)
				{
				theQ = sJoin(theQ,  sAllID(sIDName, RelHead(*iName) | thePropName));
				if (iVerbose)
					theQ = sProject(RelHead(*iName) | "$$Property_Name", theQ);
				else
					theQ = sProject(*iName, theQ);
				}
			else
				{
				theQ = sJoin(theQ, sAllID(sIDName, thePropName));
				if (iVerbose)
					theQ = sProject(RelHead(sIDName) | "$$Property", theQ);
				else
					theQ = sProject(sIDName, theQ);
				}
			}
		return theQ;
		}

	return Query();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TQLConvert

Query sConvert(const ZTBQuery& iTBQuery, bool iVerbose)
	{
	return sConvert(iTBQuery.GetNode(), nullptr, nullptr, iVerbose);
	}

} // namespace ZUtil_TQLConvert

NAMESPACE_ZOOLIB_END
