#include "zoolib/ZVisitor_Expr_Logic_ValCondition_DoToStrim.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_JSON.h"

#include "zoolib/sqlite/ZSQLite.h"
#include "zoolib/valbase/ZValBase.h"
#include "zoolib/valbase/ZValBase_Any.h"
#include "zoolib/valbase/ZValBase_SQLite.h"
#include "zoolib/valbase/ZValBase_YadSeqR.h"
#include "zoolib/valbase/ZValBase_YadSeqRPos.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zql/ZQL_RelOps.h"
#include "zoolib/zql/ZQL_SQL.h"
#include "zoolib/zql/ZQL_Util.h"
#include "zoolib/zql/ZQL_Util_Strim_Rel.h"
#include "zoolib/zql/ZQL_Util_Strim_RelHead.h"

NAMESPACE_ZOOLIB_USING

using namespace ZQL;

using std::map;
using std::set;
using std::string;
using std::vector;

static ZYadOptions spYadOptions(true);

typedef ZRef<ZExpr_Logic> Spec;

//typedef ZVal_Expr ZVal;
//typedef ZSeq_Expr ZSeq;
//typedef ZMap_Expr ZMap;

static void spDumpRel(const ZStrimW& s, Rel iRel)
	{
	Util_Strim_Rel::Options theOptions;
	theOptions.fDebuggingOutput = true;

	Util_Strim_Rel::sToStrim(iRel, theOptions, s);
	s << "\n";
	}

static void sDumpSFW(const ZStrimW& s, ZRef<SQL::Expr_Rel_SFW> iExpr)
	{
	ZVisitor_DoToStrim::Options theOptions;
	theOptions.fInitialIndent = 1;

	const Rename_t& theRename = iExpr->GetRename();
	s << "Renames:\n";
	for (Rename_t::const_iterator i = theRename.begin(); i != theRename.end(); ++i)
		s << "  " << (*i).second << "<--" << (*i).first << "\n" ;

	s << "Relhead:\n  ";
	Util_Strim_RelHead::sWrite_RelHead(iExpr->GetRelHead(), s);
	s << "\n";

	s << "Condition:\n  ";
	ZVisitor_Expr_Logic_ValCondition_DoToStrim().DoToStrim(theOptions, s, iExpr->GetCondition()); 
	s << "\n";

	s << "Rels:\n";
	const vector<ZRef<Expr_Rel_Concrete> >& theRels = iExpr->GetRels();
	for (vector<ZRef<Expr_Rel_Concrete> >::const_iterator i = theRels.begin(); i != theRels.end(); ++i)
		{
		s << "  ";
		Util_Strim_Rel::sToStrim(*i, theOptions, s);
		s << "\n";
		}

	s << sAsSQL(iExpr) << "\n";
	}

static void spDumpProblems(const ZStrimW& s, Rel iRel)
	{
	vector<ZQL::Util::Problem> theProblems;
	if (!sValidate(theProblems, iRel))
		{
		for (vector<ZQL::Util::Problem>::const_iterator i = theProblems.begin(); i != theProblems.end(); ++i)
			{
			s << (*i).GetDescription() << "\n";
			spDumpRel(s, (*i).GetRel());
			s << "\n";
			}
		}
	}

Rel genre = ZValBase::sConcrete(RelHead() | "genre.idGenre" | "genre.strGenre", "genre");
Rel genrelinkmovie = ZValBase::sConcrete(RelHead() | "genrelinkmovie.idGenre" | "genrelinkmovie.idMovie", "genrelinkmovie");
Rel movie = ZValBase::sConcrete(RelHead() | "movie.idMovie" | "movie.c00" , "movie");
Spec theSpec = CName("movie.idMovie") == CName("link.idMovie") & CName("movie.idGenre") == CName("link.idGenre");
//Spec theSpec = CName("movie.idMovie") == CName("link.idMovie") & CName("movie.idGenre") == CName("link.idGenre");

void sTestQL1(const ZStrimW& s)
	{
	Rel lgenre = genre;
	lgenre = sRename(lgenre, "idGenre", "genre.idGenre");
	Rel theRel = (lgenre * genrelinkmovie) & CName("idGenre") == CName("genrelinkmovie.idGenre");
	spDumpRel(s, theRel);
	spDumpProblems(s, theRel);
	ZRef<SQL::Expr_Rel_SFW> theSFW = ZQL::SQL::sConvert(theRel);
	sDumpSFW(s, theSFW);

	{
	Rel another = (theRel * movie) & (CName("movie.idMovie") == CName("genrelinkmovie.idMovie"));
	spDumpRel(s, another);
	spDumpProblems(s, another);
	sDumpSFW(s, ZQL::SQL::sConvert(another));
	}
	}

static Rel spRel()
	{
	return genre * (theSpec & movie);
	}

static Rel spRel2()
	{
	return theSpec & (genrelinkmovie * movie * genre);
	}

void sTestQL3(const ZStrimW& s)
	{
	ZRef<ZSQLite::DB> theDB = new ZSQLite::DB("/Users/ag/sqlitetest/MyVideos34.db");
	ZRef<ZValBase_SQLite::ConcreteDomain> theConcreteDomain =
		new ZValBase_SQLite::ConcreteDomain(theDB);

	Rel theRel_genre = sConcrete_SQL(theConcreteDomain, "select * from genre;", "genre.");
	theRel_genre = sRename(theRel_genre, "idGenre", "genre.idGenre");
	Rel theRel_genrelinkmovie = sConcrete_SQL(theConcreteDomain, "select * from genrelinkmovie;", "genrelinkmovie.");
	Rel theRel_movie = sConcrete_SQL(theConcreteDomain, "select * from movie;", "movie.");
	Spec theSpec = CName("movie.idMovie") == CName("genrelinkmovie.idMovie") & CName("idGenre") == CName("genrelinkmovie.idGenre");

	Rel theRel = theSpec & (theRel_genre * theRel_genrelinkmovie * theRel_movie);

	theRel = theRel & (RelHead() | "idGenre" | "genre.strGenre" | "genrelinkmovie.idGenre" | "genrelinkmovie.idMovie" | "movie.idMovie" | "movie.c00");

//	Rel theRel = spRel();
	spDumpRel(s, theRel);
	spDumpProblems(s, theRel);

//	Rel theRel2 = spRel2();
//	spDumpRel(s, theRel2);
//	spDumpProblems(s, theRel2);

	ZRef<SQL::Expr_Rel_SFW> theSFW = ZQL::SQL::sConvert(theRel);
//	ZRef<SQL::Expr_Rel_SFW> theSFW = ZQL::SQL::sConvert(theRel2);

	sDumpSFW(s, theSFW);

	for (ZRef<ZQE::Iterator> theIterator = ZValBase::sIterator(theRel);;)
		{
		if (ZRef<ZQE::Result> theZQEResult = theIterator->ReadInc())
			{
			if (ZRef<ZQE::Result_Any> theResult = theZQEResult.DynamicCast<ZQE::Result_Any>())
				{
				ZYad_JSON::sToStrim(0, spYadOptions, sMakeYadR(theResult->GetVal()), s);
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
	s << "\n";
	}
