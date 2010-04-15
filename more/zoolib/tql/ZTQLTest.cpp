#include "zoolib/ZVisitor_Expr_Logic_ValCondition_DoToStrim.h"

#include "zoolib/sqlite/ZSQLite.h"
#include "zoolib/valbase/ZValBase.h"
#include "zoolib/valbase/ZValBase_Any.h"
#include "zoolib/valbase/ZValBase_SQLite.h"
#include "zoolib/valbase/ZValBase_YadSeqR.h"
#include "zoolib/valbase/ZValBase_YadSeqRPos.h"
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
	ZVisitor_Expr_DoToStrim::Options theOptions;
	theOptions.fInitialIndent = 1;

	const map<string8, string8>& theMap = iExpr->GetRenameMap();
	s << "Renames:\n";
	for (map<string8, string8>::const_iterator i = theMap.begin(); i != theMap.end(); ++i)
		s << "  " << (*i).first << "<--" << (*i).second << "\n" ;

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
Rel movie = ZValBase::sConcrete(RelHead() | "movie.idMovie" | "movie.file" | "movie.idGenre", "movie");
Spec theSpec = CName("movie.idMovie") == CName("link.idMovie") & CName("movie.idGenre") == CName("link.idGenre");

void sTestQL3(const ZStrimW& s)
	{
	Rel theRel = (genre * genrelinkmovie) & CName("genre.idGenre") == CName("genrelinkmovie.idGenre");
	theRel = sRename(theRel, "idGenre", "genre.idGenre");
	spDumpRel(s, theRel);
	spDumpProblems(s, theRel);
	sDumpSFW(s, ZQL::SQL::sConvert(theRel));
	}

static Rel spRel()
	{
	return genre * (theSpec & movie);
	}

static Rel spRel2()
	{
	return theSpec & (genrelinkmovie * movie * genre);
	}

void sTestQL1(const ZStrimW& s)
	{
	ZRef<ZSQLite::DB> theDB = new ZSQLite::DB("/Users/ag/sqlitetest/MyVideos34.db");
	ZRef<ZValBase_SQLite::ConcreteDomain> theConcreteDomain =
		new ZValBase_SQLite::ConcreteDomain(theDB);

//	Rel theRel = sConcrete_SQL(theConcreteDomain, "select * from genre;");
	Rel theRel_genre = sConcrete_SQL(theConcreteDomain, "select * from genre;", "genre.");
	Rel theRel_genrelinkmovie = sConcrete_SQL(theConcreteDomain, "select * from genrelinkmovie;", "genrelinkmovie.");
	Rel theRel_movie = sConcrete_SQL(theConcreteDomain, "select * from movie;", "movie.");
	Spec theSpec = CName("movie.idMovie") == CName("genrelinkmovie.idMovie") & CName("movie.idGenre") == CName("link.idGenre");

	Rel theRel = theSpec & (theRel_genre * theRel_genrelinkmovie * theRel_movie);

//	Rel theRel = spRel();
	spDumpRel(s, theRel);
	spDumpProblems(s, theRel);

//	Rel theRel2 = spRel2();
//	spDumpRel(s, theRel2);
//	spDumpProblems(s, theRel2);

	ZRef<SQL::Expr_Rel_SFW> theSFW = ZQL::SQL::sConvert(theRel);
//	ZRef<SQL::Expr_Rel_SFW> theSFW = ZQL::SQL::sConvert(theRel2);

	sDumpSFW(s, theSFW);
	}
