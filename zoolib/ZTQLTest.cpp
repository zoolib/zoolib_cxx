#include "zoolib/ZTQL_Query.h"

#include "zoolib/ZTQL_Optimize.h"

#include "zoolib/ZUtil_Strim_TQL.h"
#include "zoolib/ZUtil_TQLConvert.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

NAMESPACE_ZOOLIB_USING

using std::set;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL, test code

using ZTQL::Condition;
using ZTQL::CName;
using ZTQL::CValue;
using ZTQL::Query;

static void sTesterfsdfsdf()
	{
	Condition theCondition = CName("Object").EQ(CValue("view"));
	}

static const ZTName sProps_note[] = { "Object", "titl", "crea", "text", "stat" };
static const ZTQL::RelHead sRelHead_note(sProps_note, countof(sProps_note));

static const ZTName sProps_view[] = { "Object", "titl", "level", "lock", "stat", };
static const ZTQL::RelHead sRelHead_view(sProps_view, countof(sProps_view));

static Query sNotesWithIDs()
	{
	return Query::sAllID("noteID", sRelHead_note).Select(CName("Object").EQ(CValue("note")));
//	return Query::sAll("noteID").Select(ZTSpec::sEQ("Object", "note"));
	}

static const ZTName sProps_Link[] = { "Link", "from", "to" };
static const ZTQL::RelHead sRelHead_Link(sProps_Link, countof(sProps_Link));
static Query sLinks_Owns()
	{
	return Query::sAll(sRelHead_Link).Select(CName("Link").EQ(CValue("owns")));
//	return Query::sAll().Select(ZTSpec::sEQ("Link", "owns"));
	}

static void sTest()
	{
	Query notesWithIDs = sNotesWithIDs();
	
	}

//static Query A()
//	{ return ZTQL::sAll(); }

static Query A(const ZTName& iIDName)
	{ return ZTQL::sAll(iIDName); }

static Query D(const Query& iQuery1, const Query& iQuery2)
	{ return sDifference(iQuery1, iQuery2); }

static Query E(const ZTuple* iTuples, size_t iCount)
	{ return ZTQL::sExplicit(iTuples, iCount); }

static Query E(const std::vector<ZTuple>& iTuples)
	{ return ZTQL::sExplicit(iTuples); }

static Query I(const Query& iQuery1, const Query& iQuery2)
	{ return sIntersect(iQuery1, iQuery2); }

static Query J(const Query& iQuery1, const Query& iQuery2)
	{ return sJoin(iQuery1, iQuery2); }

static Query P(const Query& iQuery, const ZTName& iPropName0)
	{ return sProject(iQuery, iPropName0); }

static Query R(const Query& iQuery, const ZTName& iOldPropName, const ZTName& iNewPropName)
	{ return sRename(iQuery, iOldPropName, iNewPropName); }

static Query S(const Query& iQuery, const ZTQL::Spec& iSpec)
	{ return sSelect(iQuery, iSpec); }

static Query U(const Query& iQuery1, const Query& iQuery2)
	{ return sUnion(iQuery1, iQuery2); }

// Do we even want this? Union and intersection are actually uncommon operations.
Query operator|(const Query& iQuery1, const Query& iQuery2);
Query operator|(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Union(iQuery2); }

Query operator&(const Query& iQuery1, const Query& iQuery2);
Query operator&(const Query& iQuery1, const Query& iQuery2)
	{ return iQuery1.Intersect(iQuery2); }


static ZTQL::Spec sBadAuthors()
	{
#if 1
	ZTQL::Spec theSpec = ZTQL::CName("Object").EQ(CValue("author")) & CName("pass").EQ(CName("fnam"));
//	ZTBSpec theTBSpec =
//		ZTBSpec::sEquals("Object", "author")
//		& ZTBSpec::sEquals("pass", "somethingbad");

	return theSpec;
#else
	return ZTSpec::sEQ("Object", "author") &
		(
		ZTSpec::sEQProp("fnam", "pass")
		| ZTSpec::sEQProp("lnam", "pass")
		| ZTSpec::sEQProp("unam", "pass")
		);
#endif
	}

static Query badPassword()
	{
	return
		P(
		S(A("authorID"), sBadAuthors()),
		"authorID"
		);
	}

static Query badPassword2()
	{
	return A("authorID").Select(sBadAuthors()).Project(ZTName("authorID"));
	}

// S(A("authorID"), #Object == "author" && (#fnam == #pass | #lnam == #pass | #unam == #pass));


static ZTBQuery sTBQuery()
	{
	ZTBQuery allViews = ZTBSpec::sEquals("Object", "view");
	ZTBQuery allContains = ZTBSpec::sEquals("Link", "contains");
	ZTBQuery allNotes = ZTBSpec::sEquals("Object", "note");
	ZTBQuery result = ZTBQuery(allContains & ZTBQuery("from", allViews), "to") & allNotes;
	return result;
	}

static Query sPrefix(const ZTName& iPrefix, const ZTQL::RelHead& iIgnore, Query iQuery)
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

static Query sSuperJoin(const ZTName& iPrefix1, Query iQuery1, const ZTQL::RelHead& iJoinOn, Query iQuery2, const ZTName& iPrefix2)
	{
	using ZTQL::RelHead;
	Query newQuery1 = sPrefix(iPrefix1, iJoinOn, iQuery1);
	Query newQuery2 = sPrefix(iPrefix2, iJoinOn, iQuery2);
	return newQuery1.Join(newQuery2);
	}

static Query sDrop(Query iQuery, const ZTName& iTName)
	{
	ZTQL::RelHead theRelHead = iQuery.GetRelHead();
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
	Query theQuery = Query::sAllID("$ID$", sRelHead_note).Select(CName("Object").EQ(CValue("note")));
	return sDrop(theQuery, "Object");
	}

static Query sAllViews()
	{
	Query theQuery = Query::sAllID("$ID$", sRelHead_view).Select(CName("Object").EQ(CValue("view")));
	return sDrop(theQuery, "Object");
	}

static Query sAllContains()
	{
	Query theQuery = Query::sAll(sRelHead_Link).Select(CName("Link").EQ(CValue("contains")));
	return sDrop(theQuery, "Link");
	}

static Query sQuery()
	{
	using ZTQL::RelHead;
	Query allViews = sAllViews().Rename("$ID$", "from");
	Query allNotes = sAllNotes().Rename("$ID$", "to");
	Query allContains = sAllContains();

	Query result1 = sSuperJoin("view.", allViews, RelHead("from"), allContains, "");
	Query result2 = sSuperJoin("note.", allNotes, RelHead("to"), result1, "");
	return result2;

//	return sAllViews().Rename("$ID$", "from").Join(sAllContains().Rename("to", "$ID$")).Join(sAllNotes().Rename("titl", "noteTitle"));
	}

static void sDumpQuery(const ZStrimW& s, Query iQuery)
	{
	s << "ZTQL::Query equivalent -----------------------\n";
	ZUtil_Strim_TQL::sToStrim(s, iQuery.GetNode());

	s << "\nZTQL::Query optimized -----------------------\n";
	
	ZRef<ZTQL::Node> theNode = ZTQL::sOptimize(iQuery.GetNode());
	
	ZUtil_Strim_TQL::sToStrim(s, theNode);

	s << "\n";	
	}

static void sTestOne(const string& iLabel, const ZStrimW& s, const ZTBQuery& iTBQ)
	{
	const ZTQL::Query query = ZUtil_TQLConvert::sConvert(iTBQ);
	s << iLabel << " -----------------------\n";
	s << "ZTBQuery AsTuple:\n" << iTBQ.AsTuple() << "\n";

	sDumpQuery(s, query);
	}

//Query sGetVCN();
void sTestQL(const ZStrimW& s);
void sTestQL(const ZStrimW& s)
	{
//	sDumpQuery(s, sGetVCN());
//	sDumpQuery(s, sQuery());
	return;
	
	const ZTBQuery allViews = ZTBSpec::sEquals("Object", "view") & ZTBSpec::sEquals("titl", "something");
//	sTestOne("allviews", s, allViews);

	const ZTBQuery allContains = ZTBSpec::sEquals("Link", "contains");
//	sTestOne("allContains", s, allContains);

	const ZTBQuery allNotes = ZTBSpec::sEquals("Object", "note") | ZTBSpec::sEquals("Object", "attachment");
//	sTestOne("allNotes", s, allNotes);

	const ZTBQuery partial1 = ZTBQuery("from", allViews);
//	sTestOne("partial1", s, partial1);

	const ZTBQuery partial2 = allContains & partial1;
//	sTestOne("partial2", s, partial2);
	
	const ZTBQuery partial3(partial2, "to");
//	sTestOne("partial3", s, partial3);

	const ZTBQuery partial4 = partial3 & allNotes;
	sTestOne("partial4", s, partial4);


//	ZTQL::Query query = ZUtil_TQLConvert::sConvert(sTBQuery());
//	Query query = sQuery();
//	ZUtil_StrimTQL::sToStrim(s, query);
//	s << "\n";
	}


