#include "zoolib/tql/ZTQL_Optimize.h"
#include "zoolib/ZExpr_Query.h"

#include "zoolib/tql/ZUtil_Strim_TQL_Query.h"
#include "zoolib/tql/ZUtil_TQLConvert.h"
#include "zoolib/ZExpr_ValCondition.h"

#include "zoolib/ZUtil_Strim_Tuple.h"

#include "zoolib/tuplebase/ZTBQuery.h"

NAMESPACE_ZOOLIB_USING

using namespace ZTQL;

using std::set;
using std::string;

typedef ZExpr_Logical Spec;
typedef ZExpr_Relation Query;
typedef ZMap_Expr Map;
typedef ZRelHead RelHead;
typedef ZVal_Expr Val;
typedef ZValCondition Condition;

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL, test code

//static Query A()
//	{ return ZTQL::sAll(RelHead(true)); }

static Query A(const string& iIDName)
	{ return sAll(iIDName); }

//static Query D(const Query& iQuery1, const Query& iQuery2)
//	{ return sDifference(iQuery1, iQuery2); }

static Query E(const Val* iVals, size_t iCount)
	{ return sExplicit(iVals, iCount); }

static Query E(const std::vector<Val>& iVals)
	{ return sExplicit(iVals); }

static Query I(const Query& iQuery1, const Query& iQuery2)
	{ return sIntersect(iQuery1, iQuery2); }

static Query J(const Query& iQuery1, const Query& iQuery2)
	{ return sJoin(iQuery1, iQuery2); }

//static Query P(const string& iPropName0, const Query& iQuery)
//	{ return sProject(iQuery, iPropName0); }
static Query P(const RelHead& iRelHead, const Query& iQuery)
	{ return sProject(iRelHead, iQuery); }

static Query R(const string& iOldPropName, const string& iNewPropName, const Query& iQuery)
	{ return sRename(iOldPropName, iNewPropName, iQuery); }

static Query S(const Spec& iSpec, const Query& iQuery)
	{ return sSelect(iSpec, iQuery); }

static Query U(const Query& iQuery1, const Query& iQuery2)
	{ return sUnion(iQuery1, iQuery2); }


// No difference method
//Query operator-(const Query& iQuery1, const Query& iQuery2);
//Query operator-(const Query& iQuery1, const Query& iQuery2)
//	{ return iQuery1.Difference(iQuery2); }

// =================================================================================================

static void sTesterfsdfsdf()
	{
	Condition theCondition = CName("Object").EQ(CConst("view"));
	}

static const string sProps_note[] = { "Object", "titl", "crea", "text", "stat" };
static const RelHead sRelHead_note(sProps_note, countof(sProps_note));

static const string sProps_view[] = { "Object", "titl", "level", "lock", "stat", };
static const RelHead sRelHead_view(sProps_view, countof(sProps_view));

static Query sNotesWithIDs()
	{
	return sAllID("noteID", sRelHead_note) & (CName("Object").EQ(CConst("note")));
	}

static const string sProps_Link[] = { "Link", "from", "to" };
static const RelHead sRelHead_Link(sProps_Link, countof(sProps_Link));
static Query sLinks_Owns()
	{
	return sAll(sRelHead_Link) & (CName("Link").EQ(CConst("owns")));
	}

static void sTest()
	{
	Query notesWithIDs = sNotesWithIDs();
	
	}

static Spec sBadAuthors()
	{
	const ZValComparand a, b;
	ZValCondition theSpec = a > b;
//	Spec theSpec = operator<< <>(a << b;
//	ZExpr_ValCondition_T<ZVal_Expr>::Expr_Condition result = CName_T<ZVal_Expr>("Object") == CConst_T<ZVal_Expr>("author");
//	ZExpr_ValCondition_T<ZVal_Expr>::Expr_Condition result = CName("Object") == CConst("author");
//	ZExpr_ValCondition_T<ZVal_Expr>::Expr_Condition result = operator==<ZVal_Expr>(CName_T<ZVal_Expr>("Object"), CName_T<ZVal_Expr>("author"));
//	const Spec theSpec = operator==<ZVal_Expr>(CName_T<ZVal_Expr>("Object"), CName_T<ZVal_Expr>("author"));
//	const Spec theSpec = (CName("Object") == CConst("author"));
	
/*
	const Spec theSpec =
		CName("Object") == CConst("author")
		&	(
			CName("pass") == CName("fnam")
			| CName("pass") == CName("lnam")
			| CName("pass") == CName("unam")
			);
*/
	return ZExpr_ValCondition(theSpec);
	}

static Query badPassword()
	{
	return A("authorID") & sBadAuthors();

	return P(RelHead("authorID"),
		A("authorID") & sBadAuthors());
	}

static Query badPassword2()
	{
//	return (A("authorID") & sBadAuthors()).Project("authorID");
	return sProject(string("authorID"), A("authorID") & sBadAuthors());
	}

// S(A(@authorID), @Object == "author" & (@fnam == @pass | @lnam == @pass | @unam == @pass));


static ZTBQuery sTBQuery()
	{
	ZTBQuery allViews = ZTBSpec::sEquals("Object", "view");
	ZTBQuery allContains = ZTBSpec::sEquals("Link", "contains");
	ZTBQuery allNotes = ZTBSpec::sEquals("Object", "note");
	ZTBQuery result = ZTBQuery(allContains & ZTBQuery("from", allViews), "to") & allNotes;
	return result;
	}

static Query sPrefix(const string& iPrefix, const RelHead& iIgnore, Query iQuery)
	{
	if (iPrefix.empty())
		return iQuery;

	bool universal;
	set<string> theNames;
	iQuery.GetRelHead().GetNames(universal, theNames);
	for (set<string>::iterator i = theNames.begin(); i != theNames.end(); ++i)
		{
		if (iIgnore.Contains(*i))
			continue;
		iQuery = sRename(*i, iPrefix + *i, iQuery);
		}
	return iQuery;
	}

static Query sSuperJoin(
	const string& iPrefix1, Query iQuery1,
	const RelHead& iJoinOn,
	Query iQuery2, const string& iPrefix2)
	{
	Query newQuery1 = sPrefix(iPrefix1, iJoinOn, iQuery1);
	Query newQuery2 = sPrefix(iPrefix2, iJoinOn, iQuery2);
	return newQuery1 * newQuery2;
	}

static Query sDrop(Query iQuery, const string& iTName)
	{
	RelHead theRelHead = iQuery.GetRelHead();
	if (theRelHead.Contains(iTName))
		return sProject(theRelHead - iTName, iQuery);
	return iQuery;	
	}

static Query sAllIDs()
	{
	return sAllID("$ID$");
	}

static Query sAllNotes()
	{
	Query theQuery = sAllID("$ID$", sRelHead_note) & (CName("Object") == CConst("note"));
	return sDrop(theQuery, "Object");
	}

static Query sAllNotesNoHead()
	{
	return sAllID("$ID$") & (CName("Object").EQ(CConst("note")));
	}

static Query sAllViews()
	{
	Query theQuery = sAllID("$ID$", sRelHead_view) & (CName("Object") == CConst("view"));
	return sDrop(theQuery, "Object");
	}

static Query sAllViewsNoHead()
	{
//	return sAllID("$ID$", sRelHead_view) & Spec(true);
	return sAllID("$ID$") & (CName("Object").EQ(CConst("view")));
	}

static Query sAllContains()
	{
	Query theQuery = sAll(sRelHead_Link) & (CName("Link") == CConst("contains"));
//	Query theQuery = sAll(sRelHead_Link) & (CName("Link").EQ(CConst("contains")));
	return sDrop(theQuery, "Link");
	}

static Query sQueryNoHead()
	{
	Query allViews = sRename("$ID$", "from", sAllViewsNoHead());
	Query allNotes = sRename("$ID$", "to", sAllNotesNoHead());
	Query allContains = sAllContains();
	return allViews * (allContains * allNotes);
	}

static Query sQuery()
	{
	Query allViews = sRename("$ID$", "from", sAllViews());
	Query allNotes = sRename("$ID$", "to", sAllNotes());
	Query allContains = sAllContains();

	return sPrefix("view.", RelHead("from"), allViews)
		* allContains
		* sPrefix("note.", RelHead("to"), allNotes);

//	Query result2 = sPrefix("note.", RelHead("to"), allNotes) * result1;
//	Query result1 = sSuperJoin("view.", allViews, RelHead("from"), allContains, "");
//	Query result2 = sSuperJoin("note.", allNotes, RelHead("to"), result1, "");
//	return result2;

// & Intersect, or Select
// | Union
// * Join
// /
// +
// -
// %
// ^

	return sRename("$ID$", "from", sAllViews()) * sRename("to", "$ID$", sAllContains()) * sRename("titl", "noteTitle", sAllNotes());
	}

static void sDumpQuery(const ZStrimW& s, Query iQuery)
	{
	s << "ZTQL::Query equivalent -----------------------\n";
	ZUtil_Strim_TQL::sToStrim(iQuery, s);

	s << "\nZTQL::Query optimized -----------------------\n";
	
	ZExpr_Relation theNode = sOptimize(iQuery);
	
	ZUtil_Strim_TQL::sToStrim(theNode, s);

	s << "\n";	
	}

static void sTestOne(const string& iLabel, const ZStrimW& s, const ZTBQuery& iTBQ)
	{
	const Query query = ZUtil_TQLConvert::sConvert(iTBQ, false);
	s << iLabel << " -----------------------\n";
	s << "ZTBQuery AsTuple:\n" << iTBQ.AsTuple() << "\n";

	sDumpQuery(s, query);
	}

void sTestQL(const ZStrimW& s);
void sTestQL(const ZStrimW& s)
	{
	ZMap_Expr theMap;
	theMap.Set("name", ZMap_Expr().Set("last", string("fred")));
//	Spec theSpec = CTrail("name/last") < CConst("fred1");
	
	if (sMatches(ZExpr_ValCondition(CTrail("name/last") < CConst("fred1")), theMap))
		s << "Matches\n";
	else
		s << "Doesn't\n";

	if (sMatches(ZExpr_ValCondition(CTrail("name/last") >= CConst("fred1")), theMap))
		s << "Matches\n";
	else
		s << "Doesn't\n";

	ZExpr_Relation theExp = sSelect(CVar("TestVar1") == CConst(1) & CVar("TestVar2") == CConst(2), sAll(ZRelHead(true)));
	sDumpQuery(s, theExp);


//	return;
	
//	Query theQuery = sAllID("$ID$") & theSpec;
//	sDumpQuery(s, theQuery);
	sDumpQuery(s, sQuery());
//	sDumpQuery(s, sQueryNoHead());
//	sDumpQuery(s, ZUtil_TQLConvert::sConvert(sTBQuery(), false));

//	sDumpQuery(s, badPassword());
	return;
	
	const ZTBQuery allViews
		= ZTBSpec::sEquals("Object", "view") & ZTBSpec::sEquals("titl", "something");
//	sTestOne("allviews", s, allViews);

	const ZTBQuery allContains = ZTBSpec::sEquals("Link", "contains");
	sTestOne("allContains", s, allContains);

	const ZTBQuery allNotes
		= ZTBSpec::sEquals("Object", "note") | ZTBSpec::sEquals("Object", "attachment");
//	sTestOne("allNotes", s, allNotes);

	const ZTBQuery partial1 = ZTBQuery("from", allViews);
//	sTestOne("partial1", s, partial1);

	const ZTBQuery partial2 = allContains & partial1;
//	sTestOne("partial2", s, partial2);
	
	const ZTBQuery partial3(partial2, "to");
//	sTestOne("partial3", s, partial3);

	const ZTBQuery partial4 = partial3 & allNotes;
//	sTestOne("partial4", s, partial4);


//	Query query = sQuery();
//	ZUtil_StrimTQL::sToStrim(s, query);
//	s << "\n";
	}
