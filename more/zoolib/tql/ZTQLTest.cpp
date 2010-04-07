#include "zoolib/tql/ZTQL_Optimize.h"
#include "zoolib/zql/ZQL_Expr_All.h"
#include "zoolib/zql/ZQL_Expr_Restrict.h"
#include "zoolib/zql/ZQL_Expr_Select.h"

#include "zoolib/zql/ZQL_Util_Strim_Query.h"

#include "zoolib/tql/ZUtil_TQLConvert.h"
#include "zoolib/ZExpr_ValCondition.h"

#include "zoolib/ZUtil_Strim_Tuple.h"

#include "zoolib/tuplebase/ZTBQuery.h"

#include "zoolib/ZStream_Buffered.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimW_ML.h"

#include "zoolib/ZFile.h"
#include "zoolib/ZYad_XMLPList.h"
#include "zoolib/ZYad_ZooLibStrim.h"
#include "zoolib/ZUtil_Yad.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_MapAsSeq.h"

#include "zoolib/valbase/ZValBase_Any.h"
#include "zoolib/valbase/ZValBase_YadSeqR.h"
#include "zoolib/valbase/ZValBase_YadSeqRPos.h"

#include "zoolib/ZYadSeqR_ExprRep_Logic.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

NAMESPACE_ZOOLIB_USING

using namespace ZQL;

using std::set;
using std::string;

typedef ZExpr_Logic Spec;
typedef Expr_Relation Query;
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

//static Query E(const Val* iVals, size_t iCount)
//	{ return sExplicit(iVals, iCount); }

//static Query E(const std::vector<Val>& iVals)
//	{ return sExplicit(iVals); }

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
	ZVisitor_ExprRep_ToStrim::Options theOptions;
	theOptions.fDebuggingOutput = true;

	s << "ZTQL::Query equivalent -----------------------\n";
	Util_Strim_Query::sToStrim(iQuery, theOptions, s);

	s << "\nZTQL::Query optimized -----------------------\n";
	
	Expr_Relation theNode = sOptimize(iQuery);
	
	Util_Strim_Query::sToStrim(theNode, theOptions, s);

	s << "\n";	
	}

#if 1
static void sTestOne(const string& iLabel, const ZStrimW& s, const ZTBQuery& iTBQ)
	{
	const Query query = ZUtil_TQLConvert::sConvert(iTBQ, false);
	s << iLabel << " -----------------------\n";
	s << "ZTBQuery AsTuple:\n" << iTBQ.AsTuple() << "\n";

	sDumpQuery(s, query);
	}
#endif
void sTestQL(const ZStrimW& s);
void sTestQL2(const ZStrimW& s)
	{
	Map m1;
	m1.Set("prop1", string("value"));
	Map m2 = m1, m3 = m1;

	m1.Set("prop2", string("value1"));

	m2.Set("prop2", string("value2"));

	m3.Set("prop2", string("value1"));

	int res0a = sCompare_T(m1, m2);
	int res0b = sCompare_T(m2, m1);
	int res0c = sCompare_T(m1, m3);
	int res0d = sCompare_T(m3, m1);

	ZVal_Expr val1 = string("Fred");
	ZVal_Expr val2 = string("bill");
	ZVal_Expr val3 = int32(27);
	ZVal_Expr val4 = int32(28);
	
	int res1 = sCompare_T(val1, val2);
	int res2 = sCompare_T(val2, val3);
	int res3 = sCompare_T(val3, val4);
	int res4 = sCompare_T(val1, val1);
	}

void sTestQL4(const ZStrimW& s)
	{
	ZSeq_Any theSeq;
	for (int x = 0; x < 20; ++x)
		{
		ZMap_Any innerMap;
		innerMap.Set("field", x);
		ZMap_Any outerMap;
		outerMap.Set("inner", innerMap);
		theSeq.Append(outerMap);
		}

	Expr_Relation thePhys(new ZValBase_Any::ExprRep_Concrete(theSeq));
//	ZExpr_ValCondition theSpec1 = CVal() > CConst(10);
//	Spec theSpec = Spec(false) | (CVal() > CConst(10));
//	Spec theSpec = ();
	Spec theSpec = ZExpr_ValCondition(CTrail("inner/field") < CConst(10));
	thePhys = thePhys & theSpec;

	Util_Strim_Query::sToStrim(thePhys, s);
	s << "\n";

	thePhys = Expr_Relation(sOptimize(thePhys));

	ZRef<ZQE::Iterator> theIterator =
		ZValBase_Any::Visitor_ExprRep_Concrete_MakeIterator().MakeIterator(thePhys);

	for (;;)
		{
		if (ZRef<ZQE::Result> theResult = theIterator->ReadInc())
			{
			s.Writef("%08X, ", theResult.Get());
			}
		else
			{
			break;
			}
		}

	}

void sTestQL3(const ZStrimW& s)
	{
	Spec theSpec = CVar("TestVar1") == CConst(1) | CVar("TestVar2") == CConst(2);
	Expr_Relation theExp = sSelect(theSpec, sAll(ZRelHead(true)));
	sDumpQuery(s, theExp);

	sDumpQuery(s, theExp * theExp);

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


//	return;
	
//	Query theQuery = sAllID("$ID$") & theSpec;
//	sDumpQuery(s, theQuery);

	sDumpQuery(s, sQuery());
#if 0
//	sDumpQuery(s, sQueryNoHead());
//	sDumpQuery(s, ZUtil_TQLConvert::sConvert(sTBQuery(), false));

//	sDumpQuery(s, badPassword());
//	return;
	
	const ZTBQuery allViews
		= ZTBSpec::sEquals("Object", "view") & ZTBSpec::sEquals("titl", "something");
	sTestOne("allviews", s, allViews);

	const ZTBQuery allContains = ZTBSpec::sEquals("Link", "contains");
	sTestOne("allContains", s, allContains);

	const ZTBQuery allNotes
		= ZTBSpec::sEquals("Object", "note") | ZTBSpec::sEquals("Object", "attachment");
	sTestOne("allNotes", s, allNotes);

	const ZTBQuery partial1 = ZTBQuery("from", allViews);
//	sTestOne("partial1", s, partial1);

	const ZTBQuery partial2 = allContains & partial1;
	sTestOne("partial2", s, partial2);
	
	const ZTBQuery partial3(partial2, "to");
	sTestOne("partial3", s, partial3);

	const ZTBQuery partial4 = partial3 & allNotes;
	sTestOne("partial4", s, partial4);


//	Query query = sQuery();
//	ZUtil_StrimTQL::sToStrim(s, query);
//	s << "\n";
#endif
	}


void sTestQL5(const ZStrimW& s)
	{
	ZSeq_Any s1;
	for (int32 x = 0; x < 10; ++x)
		{
		ZMap_Any aMap;
		aMap.Set("field", x);
		aMap.Set("field1", string("I'm in seq1"));
		s1.Append(aMap);
		}

	ZSeq_Any s2;
	for (int32 x = 0; x < 10; x += 2)
		{
		ZMap_Any aMap;
		aMap.Set("field", x);
		aMap.Set("field2", string("I'm in seq2"));
		s2.Append(aMap);
		}

	ZRef<ZYadSeqR> yad1 = sMakeYadR(s1);
	ZRef<ZYadSeqR> yad2 = sMakeYadR(s2);

	Expr_Concrete thePhys1 = ZValBase_YadSeqR::sConcrete(yad1);
	Expr_Concrete thePhys2 = ZValBase_YadSeqR::sConcrete(yad2);

//	Expr_Relation sect = sJoin(thePhys1, thePhys2);
	Expr_Relation sect = thePhys1 * thePhys2;
	
	ZRef<ZQE::Iterator> theIterator = ZValBase_YadSeqR::sIterator(sect);
		

	ZYadOptions theYadOptions(true);

	for (;;)
		{
		if (ZRef<ZQE::Result> theZQEResult = theIterator->ReadInc())
			{
			if (ZRef<ZQE::Result_Any> theResult = theZQEResult.DynamicCast<ZQE::Result_Any>())
				{
				ZYad_ZooLibStrim::sToStrim(0, theYadOptions, sMakeYadR(theResult->GetVal()), s);
				s << "\n";
				}
			else
				{
				s.Writef("%08X, ", theZQEResult.Get());
				}
			}
		else
			{
			break;
			}
		}
	}

void sTestQL(const ZStrimW& s)
	{
	ZRef<ZStreamerR> theStreamerR = ZFileSpec("/Users/ag/Music/iTunes/iTunes Music Library.xml").OpenR();
	theStreamerR = new ZStreamerR_Buffered(4096, theStreamerR);

	ZRef<ZStrimmerR> theStrimmerR_StreamUTF8 =
		new ZStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(theStreamerR);

	ZRef<ZStrimmerU> theStrimmerU_Unreader =
		new ZStrimmerU_FT<ZStrimU_Unreader>(theStrimmerR_StreamUTF8);

	ZRef<ZML::StrimmerU> theStrimmerU_ML = new ZML::StrimmerU(theStrimmerU_Unreader);

	ZRef<ZYadR> theYadR = ZYad_XMLPList::sMakeYadR(theStrimmerU_ML);
	theYadR = ZUtil_Yad::sWalk(theYadR, "Tracks");

	ZYadOptions theYadOptions(true);

	ZExpr_Logic theCondition =
		CName("Disc Number") == CConst(int32(2)) & CName("Track Number") > CConst(int32(10));

	ZRef<ZYadSeqR> theYadSeqR;
	if (ZRef<ZYadMapR> theYadMapR = theYadR.DynamicCast<ZYadMapR>())
		theYadSeqR = sMapAsSeq("Dummy", theYadMapR);

	ZTime start = ZTime::sNow();
#if 1
	if (theYadSeqR)
		{
//		const ZSeq_Any theSeq = sFromYadR(ZVal_Any(), theYadSeqR).GetSeq();
		
		s.Writef("\nElapsed, read: %gms\n", 1000.0 * (ZTime::sNow() - start));

		start = ZTime::sNow();
		Expr_Relation thePhys = ZValBase_YadSeqR::sConcrete(theYadSeqR);
		thePhys = thePhys & theCondition;
		thePhys = thePhys & thePhys;

		Util_Strim_Query::sToStrim(thePhys, s);
		s << "\n";

//		thePhys = Expr_Relation(sOptimize(thePhys));

		ZRef<ZQE::Iterator> theIterator = ZValBase_YadSeqR::sIterator(thePhys);

		for (;;)
			{
			if (ZRef<ZQE::Result> theZQEResult = theIterator->ReadInc())
				{
				if (ZRef<ZQE::Result_Any> theResult = theZQEResult.DynamicCast<ZQE::Result_Any>())
					{
					ZYad_ZooLibStrim::sToStrim(0, theYadOptions, sMakeYadR(theResult->GetVal()), s);
					s << "\n";
					}
				else
					{
					s.Writef("%08X, ", theZQEResult.Get());
					}
				}
			else
				{
				break;
				}
			}
		}
#elif 1
	if (theYadSeqR)
		{
		theYadSeqR = new ZYadSeqR_ExprRep_Logic(theYadSeqR, theCondition);
		theYadSeqR->SkipAll();
//		ZYad_ZooLibStrim::sToStrim(0, theYadOptions, theYadSeqR, s);
		}
#endif
	s.Writef("\nElapsed: %gms\n", 1000.0 * (ZTime::sNow() - start));
	}
