
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

static void spDump(const RelRename& iRelRename, const ZStrimW& w)
	{
	const set<RelRename::Elem_t>& theElems = iRelRename.GetElems();

	w.Write("[");

	bool isFirst = true;
	for (set<RelRename::Elem_t>::const_iterator i = theElems.begin(); i != theElems.end(); ++i)
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

static void spDump(const vector<RelRename>& iRelRenames, const ZStrimW& w)
	{
	for (vector<RelRename>::const_iterator i = iRelRenames.begin(); i != iRelRenames.end(); ++i)
		spDump(*i, w);
	}

static void spDump(const SearchThing& iSearchThing, const ZStrimW& w)
	{
	spDump(iSearchThing.fRelRenames, w);
	Util_Strim_Rel::sToStrim(sAsRel(iSearchThing), w);
	w << "\n";
//	spDump(sAsRel(iSearchThing.fValPredCompound), w);
	}

static void spDump(const Rel& iRel, const ZStrimW& w)
	{
	Util_Strim_Rel::sToStrim(iRel, w);
	w << "\n";
	}

// =================================================================================================

void sDataspaceTest(const ZStrimW& w)
	{
	ZRef<ZSQLite::DB> theDB = new ZSQLite::DB("/Users/ag/sqlitetest/MyVideos34.db");

	const string movieArray[] = {"movie.idFile", "movie.idMovie", "movie.idMovie", "movie.c00"};
	const RelHead rhMovie(movieArray, countof(movieArray));

	Source_SQLite theSource_SQLite(theDB);

	Source_Union theSource;
	theSource.InsertSource(&theSource_SQLite);

//	spDump(theSource.GetRelHeads(), w);

	ZRA::Rel relMovie = sConcrete(null, "movie", rhMovie);

	{
	vector<AddedSearch> theAddedSearches;
	theAddedSearches.push_back(AddedSearch(1, relMovie));
	vector<SearchResult> changed;
	Clock newClock;
	theSource.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);

	spDumpChanged(changed, w);
	}
	}
