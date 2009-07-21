#include "zoolib/tql/ZTQL_Query.h"

#include "zoolib/tql/ZTQL_Optimize.h"

#include "zoolib/ZUtil_Strim_TQL.h"
#include "zoolib/ZUtil_TQLConvert.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

NAMESPACE_ZOOLIB_USING

using namespace ZTQL;

using std::set;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL, test code

//static Query A()
//	{ return ZTQL::sAll(); }

static Query A(const ZTName& iIDName)
	{ return ZTQL::sAll(iIDName); }

static Query D(const Query& iQuery1, const Query& iQuery2)
	{ return sDifference(iQuery1, iQuery2); }

static Query E(const ZTuple* iTuples, size_t iCount)
	{ return sExplicit(iTuples, iCount); }

static Query E(const std::vector<ZTuple>& iTuples)
	{ return sExplicit(iTuples); }

static Query I(const Query& iQuery1, const Query& iQuery2)
	{ return sIntersect(iQuery1, iQuery2); }

static Query J(const Query& iQuery1, const Query& iQuery2)
	{ return sJoin(iQuery1, iQuery2); }

//static Query P(const ZTName& iPropName0, const Query& iQuery)
//	{ return sProject(iQuery, iPropName0); }
static Query P(const RelHead& iRelHead, const Query& iQuery)
	{ return sProject(iRelHead, iQuery); }

static Query R(const ZTName& iOldPropName, const ZTName& iNewPropName, const Query& iQuery)
	{ return sRename(iOldPropName, iNewPropName, iQuery); }

static Query S(const Spec& iSpec, const Query& iQuery)
	{ return sSelect(iSpec, iQuery); }

static Query U(const Query& iQuery1, const Query& iQuery2)
	{ return sUnion(iQuery1, iQuery2); }

// Do we even want this? Union and intersection are actually uncommon operations.
Query operator|(const Query& iQuery1, const Query& iQuery2);
Query operator|(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Union(iQuery2); }

Query operator&(const Query& iQuery1, const Query& iQuery2);
Query operator&(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Intersect(iQuery2); }

Query operator&(const Query& iQuery1, const Spec& iSpec);
Query operator&(const Query& iQuery1, const Spec& iSpec)
	{ return iQuery1.Select(iSpec); }

Query operator*(const Query& iQuery1, const Query& iQuery2);
Query operator*(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Join(iQuery2); }

// =================================================================================================

static void sTesterfsdfsdf()
	{
	Condition theCondition = CName("Object").EQ(CValue("view"));
	}

static const ZTName sProps_note[] = { "Object", "titl", "crea", "text", "stat" };
static const RelHead sRelHead_note(sProps_note, countof(sProps_note));

static const ZTName sProps_view[] = { "Object", "titl", "level", "lock", "stat", };
static const RelHead sRelHead_view(sProps_view, countof(sProps_view));

static Query sNotesWithIDs()
	{
	return Query::sAllID("noteID", sRelHead_note) & (CName("Object").EQ(CValue("note")));
	}

static const ZTName sProps_Link[] = { "Link", "from", "to" };
static const RelHead sRelHead_Link(sProps_Link, countof(sProps_Link));
static Query sLinks_Owns()
	{
	return Query::sAll(sRelHead_Link) & (CName("Link").EQ(CValue("owns")));
	}

static void sTest()
	{
	Query notesWithIDs = sNotesWithIDs();
	
	}

static Spec sBadAuthors()
	{
	Spec theSpec = CName("Object").EQ(CValue("author"))
		&
		(CName("pass").EQ(CName("fnam"))
		| CName("pass").EQ(CName("lnam"))
		| CName("pass").EQ(CName("unam")));
	return theSpec;
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
	return (A("authorID") & sBadAuthors()).Project(ZTName("authorID"));
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

static Query sPrefix(const ZTName& iPrefix, const RelHead& iIgnore, Query iQuery)
	{
	if (iPrefix.Empty())
		return iQuery;

	set<ZTName> theNames;
	iQuery.GetRelHead().GetNames(theNames);
	for (set<ZTName>::iterator i = theNames.begin(); i != theNames.end(); ++i)
		{
		if (iIgnore.Contains(*i))
			continue;
		iQuery = iQuery.Rename(*i, iPrefix.AsString() + i->AsString());
		}
	return iQuery;
	}

static Query sSuperJoin(const ZTName& iPrefix1,
	Query iQuery1, const RelHead& iJoinOn, Query iQuery2, const ZTName& iPrefix2)
	{
	Query newQuery1 = sPrefix(iPrefix1, iJoinOn, iQuery1);
	Query newQuery2 = sPrefix(iPrefix2, iJoinOn, iQuery2);
	return newQuery1 * newQuery2;
	}

static Query sDrop(Query iQuery, const ZTName& iTName)
	{
	RelHead theRelHead = iQuery.GetRelHead();
	if (theRelHead.Contains(iTName))
		return iQuery.Project(theRelHead - iTName);
	return iQuery;	
	}

static Query sAllIDs()
	{
	return Query::sAllID("$ID$");
	}

static Query sAllNotes()
	{
	Query theQuery = Query::sAllID("$ID$", sRelHead_note) & (CName("Object").EQ(CValue("note")));
	return sDrop(theQuery, "Object");
	}

static Query sAllNotesNoHead()
	{
	return Query::sAllID("$ID$") & (CName("Object").EQ(CValue("note")));
	}

static Query sAllViews()
	{
	Query theQuery = Query::sAllID("$ID$", sRelHead_view) & (CName("Object").EQ(CValue("view")));
	return sDrop(theQuery, "Object");
	}

static Query sAllViewsNoHead()
	{
	return Query::sAllID("$ID$", sRelHead_view) & Spec(true);
//	return Query::sAllID("$ID$") & (CName("Object").EQ(CValue("view")));
	}

static Query sAllContains()
	{
	Query theQuery = Query::sAll(sRelHead_Link) & (CName("Link").EQ(CValue("contains")));
	return sDrop(theQuery, "Link");
	}

static Query sQueryNoHead()
	{
	Query allViews = sAllViewsNoHead().Rename("$ID$", "from");
	Query allNotes = sAllNotesNoHead().Rename("$ID$", "to");
	Query allContains = sAllContains();
	return allViews * (allContains * allNotes);
	}

static Query sQuery()
	{
	Query allViews = sAllViews().Rename("$ID$", "from");
	Query allNotes = sAllNotes().Rename("$ID$", "to");
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

	return sAllViews().Rename("$ID$", "from") * (sAllContains().Rename("to", "$ID$")) * (sAllNotes().Rename("titl", "noteTitle"));
	}

static void sDumpQuery(const ZStrimW& s, Query iQuery)
	{
	s << "ZTQL::Query equivalent -----------------------\n";
	ZUtil_Strim_TQL::sToStrim(s, iQuery.GetNode());

	s << "\nZTQL::Query optimized -----------------------\n";
	
	ZRef<Node> theNode = sOptimize(iQuery.GetNode());
	
	ZUtil_Strim_TQL::sToStrim(s, theNode);

	s << "\n";	
	}

static void sTestOne(const string& iLabel, const ZStrimW& s, const ZTBQuery& iTBQ)
	{
	const Query query = ZUtil_TQLConvert::sConvert(iTBQ, false);
	s << iLabel << " -----------------------\n";
	s << "ZTBQuery AsTuple:\n" << iTBQ.AsTuple() << "\n";

	sDumpQuery(s, query);
	}

//Query sGetVCN();
void sTestQL(const ZStrimW& s);
void sTestQL(const ZStrimW& s)
	{
//	sDumpQuery(s, sGetVCN());
	sDumpQuery(s, sQueryNoHead());
//	sDumpQuery(s, ZUtil_TQLConvert::sConvert(sTBQuery(), false));

//	sDumpQuery(s, badPassword());
	return;
	
	const ZTBQuery allViews
		= ZTBSpec::sEquals("Object", "view") & ZTBSpec::sEquals("titl", "something");
	sTestOne("allviews", s, allViews);

	const ZTBQuery allContains = ZTBSpec::sEquals("Link", "contains");
//	sTestOne("allContains", s, allContains);

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

