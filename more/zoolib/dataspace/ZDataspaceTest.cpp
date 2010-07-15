
#include "zoolib/zqe/ZQE_Result_Any.h"

#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"
#include "zoolib/dataspace/ZDataspace_Source_Dummy.h"
#include "zoolib/dataspace/ZDataspace_Source_SQLite.h"
#include "zoolib/dataspace/ZDataspace_Source_Union.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZValPred.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/zra/ZRA_Util_RelOperators.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"

#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

using namespace ZooLib;
using namespace ZRA;
using namespace ZDataspace;
using namespace ZDataset;

static ZData_Any spAsData(const ZVal_Any& iVal)
	{
	ZData_Any theData;
	ZStreamRWPos_Data_T<ZData_Any> theStreamW(theData);
	ZStrimW_StreamUTF8 theStrimW(theStreamW);
	ZYad_ZooLibStrim::sToStrim(sMakeYadR(iVal), theStrimW);
	return theStreamW.GetData();
	}

static void spDumpChanged(const vector<SearchResult>& iChanged, const ZStrimW& w)
	{
	for (vector<SearchResult>::const_iterator i = iChanged.begin(); i != iChanged.end(); ++i)
		{
		for (vector<ZRef<ZQE::Result> >::const_iterator j = (*i).fResults.begin(); j != (*i).fResults.end(); ++j)
			{
			if (ZRef<ZQE::Result_Any> theResult = (*j).DynamicCast<ZQE::Result_Any>())
				{
				ZYad_ZooLibStrim::sToStrim(0, ZYadOptions(true), sMakeYadR(theResult->GetVal()), w);
				const set<ZRef<ZCounted> >& theAnnotations = theResult->GetAnnotations();
				for (set<ZRef<ZCounted> >::const_iterator k = theAnnotations.begin(); k != theAnnotations.end(); ++k)
					{
					w << typeid(*(*k).Get()).name() << ", ";
					}
				}
			else
				{
				w << "???";
				}
			w << "\n";
			}
		}
	w << "---------------------\n";
	}

static void spDump(const RelHead& iRelHead, const ZStrimW& w)
	{
	Util_Strim_RelHead::sWrite_RelHead(iRelHead, w);
	w << "\n";
	}

static void spDump(const vector<RelHead>& iRelHeads, const ZStrimW& w)
	{
	for (vector<RelHead>::const_iterator i = iRelHeads.begin(); i != iRelHeads.end(); ++i)
		spDump(*i, w);
	}

static void spDump(const set<RelHead>& iRelHeads, const ZStrimW& w)
	{
	for (set<RelHead>::const_iterator i = iRelHeads.begin(); i != iRelHeads.end(); ++i)
		spDump(*i, w);
	}

static void spDump(const NameMap& iNameMap, const ZStrimW& w)
	{
	const set<NameMap::Elem_t>& theElems = iNameMap.GetElems();

	w.Write("[");

	bool isFirst = true;
	for (set<NameMap::Elem_t>::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
		{
		if (!isFirst)
			w.Write(", ");
		isFirst = false;
		Util_Strim_RelHead::sWrite_PropName((*i).first, w);
		if ((*i).first != (*i).second)
			{
			w << "/";
			Util_Strim_RelHead::sWrite_PropName((*i).second, w);
			}
		}
	w.Write("]");
	w << "\n";
	}

static void spDump(const vector<NameMap>& iNameMaps, const ZStrimW& w)
	{
	for (vector<NameMap>::const_iterator i = iNameMaps.begin(); i != iNameMaps.end(); ++i)
		spDump(*i, w);
	}

static void spDump(const SearchThing& iSearchThing, const ZStrimW& w)
	{
	spDump(iSearchThing.fNameMaps, w);
	Util_Strim_Rel::sToStrim(sAsRel(iSearchThing), w);
	w << "\n";
//	spDump(sAsRel(iSearchThing.fValPredCompound), w);
	}

static void spDump(const Rel& iRel, const ZStrimW& w)
	{
	Util_Strim_Rel::sToStrim(iRel, w);
	w << "\n";
	}

static ZRA::Rel spPrefix(const RelName& iPrefix, ZRA::Rel iRel)
	{
	RelHead theRelHead = iRel->GetRelHead();
	for (RelHead::const_iterator i = theRelHead.begin(); i != theRelHead.end(); ++i)
		iRel = sRename(iRel, iPrefix + *i, *i);
	return iRel;
	}

/*
SQLite, we need to look at the NameMaps, determine which table each maps to. When the same table is referenced by more than one NameMap, we have no choice but to do a FROM xxx AS yyy, and write the condition as yyy.something AS xxx.ORIGINAL, xxx.ORIGINAL == "somestring".

2010-07-07 00:04:46.149647   a0f/00007FFF701CCBE0 P7 void ZooLib::ZDataspace::Source_Union::pUpdate(ZooLib::ZDataspace::AddedSearch*, size_t, ZStdInt::int64*, size_t, std::vector<ZooLib::ZDataspace::SearchResult, std::allocator<ZooLib::ZDataspace::SearchResult> >&, ZooLib::ZDataspace::Clock&) - [@"sql.movie.c00", @"sql.movie.idFile", @"sql.movie.idMovie"]
2010-07-07 00:04:46.150189   a0f/00007FFF701CCBE0 P7 virtual void ZooLib::ZDataspace::Source_SQLite::Update(bool, ZooLib::ZDataspace::AddedSearch*, size_t, ZStdInt::int64*, size_t, std::vector<ZooLib::ZDataspace::SearchResult, std::allocator<ZooLib::ZDataspace::SearchResult> >&, ZooLib::ZDataspace::Clock&) - ((sql.movie.c00<--movie.c00, sql.movie.idFile<--movie.idFile, sql.movie.idMovie<--movie.idMovie), (IDMOVIE<--movie.idFile, b_sql.movie.c00<--movie.c00, b_sql.movie.idMovie<--movie.idMovie))
(false | (@IDMOVIE == "somestring" & true))
Bug id_sim i1.n 1 - i2.n 1
2010-07-07 00:04:46.150443   a0f/00007FFF701CCBE0 P7                                                                                                                                                                                                    void ZooLib::ZDataspace::Source_Union::PQuery::Regenerate() - ((sql.movie.c00<--sql.movie.c00, sql.movie.idFile<--sql.movie.idFile, sql.movie.idMovie<--sql.movie.idMovie), (IDMOVIE<--sql.movie.idFile, b_sql.movie.c00<--sql.movie.c00, b_sql.movie.idMovie<--sql.movie.idMovie))
(false | (@IDMOVIE == "somestring" & true))

*/

// =================================================================================================

void sDataspaceTest(const ZStrimW& w)
	{
	ZRef<ZSQLite::DB> theDB = new ZSQLite::DB("/Users/ag/sqlitetest/MyVideos34.db");

	const string movieArray[] = {"movie_idFile", "movie_idMovie", "movie_idMovie", "movie_c00"};

	RelHead rhMovie = RelHead(movieArray, countof(movieArray));

	ZRA::Rel relMovieA = sConcrete(rhMovie);
	ZRA::Rel relMovieB = spPrefix("movieB_", sConcrete(rhMovie));


	relMovieA = sRename(relMovieA, "MOVIE_IDFILE", "movie_idFile");

	Source_SQLite theSource(theDB);


//	spDump(theSource.GetRelHeads(), w);

	ZRA::Rel theRel = relMovieA;
	theRel = theRel * relMovieB;
	theRel = theRel & (CName("MOVIE_IDFILE") == CName("movieB_movie_idMovie"));
//	theRel = theRel & (CName("MOVIE_IDFILE") == CString("2"));

	{
	vector<AddedSearch> theAddedSearches;
	theAddedSearches.push_back(AddedSearch(1, sAsSearchThing(theRel)));
	vector<SearchResult> changed;
	Clock newClock;
	theSource.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);

	spDumpChanged(changed, w);
	}
	}

void sDataspaceTest2(const ZStrimW& w)
	{
	ZRef<ZSQLite::DB> theDB = new ZSQLite::DB("/Users/ag/sqlitetest/MyVideos34.db");

	const string movieArray[] = {"movie.idFile", "movie.idMovie", "movie.idMovie", "movie.c00"};
	const RelHead rhMovie = RelHead(movieArray, countof(movieArray));
	const RelHead rhMovie_sql = sPrefixAdd("sql.", rhMovie);


	ZRA::Rel relMovieA = sConcrete(rhMovie_sql);
	ZRA::Rel relMovieB = spPrefix("b_", relMovieA);
	relMovieB = sRename(relMovieB, "IDMOVIE", "b_sql.movie.idFile");

	Source_SQLite theSource_SQLite(theDB);

	Source_Union theSource;
	theSource.InsertSource(&theSource_SQLite, "sql.");

//	spDump(theSource.GetRelHeads(), w);

	ZRA::Rel theRel = relMovieA * relMovieB;
	theRel = theRel & (CName("IDMOVIE") == CString("somestring"));
//	theRel = theRel & (CName("b_sql.movie.idFile") == CName("sql.movie.idFile"));

	{
	vector<AddedSearch> theAddedSearches;
	theAddedSearches.push_back(AddedSearch(1, sAsSearchThing(theRel)));
	vector<SearchResult> changed;
	Clock newClock;
	theSource.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);

	spDumpChanged(changed, w);
	}
	}
