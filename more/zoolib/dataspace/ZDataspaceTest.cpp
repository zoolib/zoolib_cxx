
#include "zoolib/zqe/ZQE_Result_Any.h"

#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"
#include "zoolib/dataspace/ZDataspace_Source_Dummy.h"
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

static void spAddBook(ZRef<Dataset> iDataset, const string& iTitle, int32 iPublisher, int32 iAuthor)
	{
	const ZData val = spAsData(ZMap_Any()
		.Set("Type", string("Book"))
		.Set("Title", iTitle)
		.Set("book_publisherid", iPublisher)
		.Set("book_authorid", iAuthor));
	iDataset->Insert(val);
	}

void sDataspaceTest4(const ZStrimW& w)
	{
	Nombre theNombre;
	const Clock theClock;
	ZRef<Dataset> theDataset = new Dataset(Nombre(theNombre, 1), theClock);
	ZRef<Dataset> theDataset2 = theDataset->Fork();

	const ZRA::Rel rel_author =
		sConcrete(nullref, "Author", RelHead() | "Type" | "fname" | "lname" | "authorid")
		& CName("Type") == CString("Author");

	const ZRA::Rel rel_publisher =
		sConcrete(nullref, "Publisher", RelHead() | "Type" | "publishername" | "publisherid")
		& CName("Type") == CString("Publisher");

	const ZRA::Rel rel_book =
		sConcrete(nullref, "Book", RelHead() | "Type" | "Title" | "book_publisherid" | "book_authorid")
		& CName("Type") == CString("Book");
		
	Source_Dataset theSource(theDataset);

	ZData lastAuthor;
	for (int32 x = 0; x < 10; ++x)
		{
		const ZData val = spAsData(ZMap_Any()
			.Set("Type", string("Author"))
			.Set("fname", ZString::sFormat("fname_%d", x))
			.Set("lname", ZString::sFormat("fname_%d", x))
			.Set("authorid", x));
		lastAuthor = val;
		theDataset->Insert(val);
		}

	for (int32 x = 0; x < 4; ++x)
		{
		const ZData val = spAsData(ZMap_Any()
			.Set("Type", string("Publisher"))
			.Set("publishername", ZString::sFormat("publisher_%d", x))
			.Set("publisherid", x));
		theDataset->Insert(val);
		}

	spAddBook(theDataset, "Book_1_3", 1, 3);
	spAddBook(theDataset, "Book_1_4", 1, 4);
	spAddBook(theDataset, "Book_2_3", 2, 3);
	spAddBook(theDataset, "Book_3_4", 3, 4);
	


	ZRA::Rel theRel = rel_author;// & ((CName("Type") == CString("field1valuea")) | (CName("field1") == CString("field1valueb")));
//	ZRA::Rel theRel = rel_author & ((CName("Type") == CString("Author")));

	ZRA::Rel theRel_author_book = 
		(rel_author * rel_book)
		& CName("authorid") == CName("book_authorid");

	vector<AddedSearch> theAddedSearches;
//	theAddedSearches.push_back(AddedSearch(1, theRel_author_book));
	theAddedSearches.push_back(AddedSearch(1, rel_author));
//	theAddedSearches.push_back(AddedSearch(2, rel_publisher));
	vector<SearchResult> changed;
	Clock newClock;
	theSource.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);

	spDumpChanged(changed, w);

//	theSource.Dump(w);
//	w << "\n";

//	theDataset2->Insert(valc);
//	theDataset2->Insert(valb);
//	theDataset2->Erase(valb);

	theDataset->Join(theDataset2);

//	theDataset->Insert(vald);

	theDataset->Erase(lastAuthor);
	theSource.Update(false, nullptr, 0, nullptr, 0, changed, newClock);
//	spDumpChanged(changed, w);
//	theSource.Dump(w);
//	w << "\n";
	}

void sDataspaceTest(const ZStrimW& w)
	{
	Nombre theNombre;
	const Clock theClock;
	ZRef<Dataset> theDataset = new Dataset(Nombre(theNombre, 1), theClock);
	ZRef<Dataset> theDataset2 = theDataset->Fork();

	const ZRA::Rel rel_author =
		sConcrete(nullref, "Author", RelHead() | "AuthorType" | "fname" | "lname" | "authorid")
		& CName("AuthorType") == CString("Author");
//		sConcrete(nullref, "Author", RelHead("fname") | "lname" | "authorid");

	const ZRA::Rel rel_publisher =
//		sConcrete(nullref, "Publisher", RelHead() | "Type" | "publishername" | "publisherid")
//		& CName("Type") == CString("Publisher");
		sConcrete(nullref, "Publisher", RelHead("publishername") | "publisherid");

	const ZRA::Rel rel_book =
		sConcrete(nullref, "Book", RelHead() | "Type" | "Title" | "book_publisherid" | "book_authorid")
		& CName("Type") == CString("Book");
		
	Source_Dataset theSource(theDataset);
	set<RelHead> emptySet;
	Source_Dummy sourceEmpty(emptySet);

	Source_Union sourceU;
	sourceU.InsertSource(&theSource);
//	sourceU.InsertSource(&sourceEmpty);

	ZData lastAuthor;
	for (int32 x = 0; x < 10; ++x)
		{
		const ZData val = spAsData(ZMap_Any()
			.Set("AuthorType", string("Author"))
			.Set("fname", ZString::sFormat("fname_%d", x))
			.Set("lname", ZString::sFormat("fname_%d", x))
			.Set("authorid", x));
		lastAuthor = val;
		theDataset->Insert(val);
		}

	for (int32 x = 0; x < 4; ++x)
		{
		const ZData val = spAsData(ZMap_Any()
//			.Set("Type", string("Publisher"))
			.Set("publishername", ZString::sFormat("publisher_%d", x))
			.Set("publisherid", x));
		theDataset->Insert(val);
		}

	spAddBook(theDataset, "Book_1_3", 1, 3);
	spAddBook(theDataset, "Book_1_4", 1, 4);
	spAddBook(theDataset, "Book_2_3", 2, 3);
	spAddBook(theDataset, "Book_3_4", 3, 4);
	

//	ZRA::Rel theRel = rel_author;// & ((CName("Type") == CString("field1valuea")) | (CName("field1") == CString("field1valueb")));
//	ZRA::Rel theRel = rel_author & ((CName("Type") == CString("Author")));

	ZRA::Rel theRel_author_book = 
		(rel_author * rel_book)
		& CName("authorid") == CName("book_authorid");



	vector<AddedSearch> theAddedSearches;
//	theAddedSearches.push_back(AddedSearch(1, theRel_author_book));
//	theAddedSearches.push_back(AddedSearch(1, rel_author * rel_book));
	theAddedSearches.push_back(AddedSearch(2, rel_publisher));
	vector<SearchResult> changed;
	Clock newClock;
	sourceU.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);

	spDumpChanged(changed, w);

//	theSource.Dump(w);
//	w << "\n";

//	theDataset2->Insert(valc);
//	theDataset2->Insert(valb);
//	theDataset2->Erase(valb);

	theDataset->Join(theDataset2);

//	theDataset->Insert(vald);

	theDataset->Erase(lastAuthor);
	theSource.Update(false, nullptr, 0, nullptr, 0, changed, newClock);
//	spDumpChanged(changed, w);
//	theSource.Dump(w);
//	w << "\n";
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

void sDataspaceTest3(const ZStrimW& w)
	{
	const string author[] = {"authorid", "authorname"};
	const RelHead rhAuthor(author, countof(author));

	const string book[] = {"book_authorid", "title", "book_publisherid"};
	const RelHead rhBook(book, countof(book));

	const string publisher[] = {"publishername", "publisherid"};
	const RelHead rhPublisher(publisher, countof(publisher));

	const string publisherAddress[] = {"publisherAddress", "pa_publisherid"};
	const RelHead rhPublisherAddress(publisherAddress, countof(publisherAddress));

	set<RelHead> raset1;
	raset1.insert(rhAuthor);
	raset1.insert(rhBook);

	Source_Dummy source1(raset1);

//	set<RelHead> raset2 = raset1;
	set<RelHead> raset2;
	raset2.insert(rhPublisher);
	raset2.insert(rhPublisherAddress);

	Source_Dummy source2(raset2);
	
	const Rel relAuthor = sConcrete(nullref, "Author", rhAuthor);
	const Rel relBook = sConcrete(nullref, "Book", rhBook);
	const Rel relPublisher = sConcrete(nullref, "Publisher", rhPublisher);
	const Rel relPublisherAddress = sConcrete(nullref, "PublisherAddress", rhPublisherAddress);

	Source_Union sourceU;
	sourceU.InsertSource(&source1);
	sourceU.InsertSource(&source2);

//	spDump(sourceU.GetRelHeads(), w);

	const Rel authorSearch = (CName("book_publisherid") == CName("publisherid")) & relPublisher * relBook * (relAuthor & (CName("authorname") == CString("fred")));
//	const Rel authorSearch = (relAuthor & (CName("authorname") == CString("fred")));
//	spDump(authorSearch, w);
//	spDump(sAsSearchThing(authorSearch), w);

//	spDump(sAsSearchThing((relAuthor * relBook) & CName("authorname") == CString("fred")), w);


	{
	vector<AddedSearch> theAddedSearches;
	theAddedSearches.push_back(AddedSearch(1, authorSearch));
//	theAddedSearches.push_back(AddedSearch(1, relAuthor));
	vector<SearchResult> changed;
	Clock newClock;
	sourceU.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);
	}
	}

void sDataspaceTest5(const ZStrimW& w)
	{
	const string author[] = {"id", "name"};
	const RelHead rhAuthor(author, countof(author));

	const string book[] = {"authorid", "title", "publisherid"};
	const RelHead rhBook(book, countof(book));

	const string publisher[] = {"name", "id"};
	const RelHead rhPublisher(publisher, countof(publisher));

	const string publisherAddress[] = {"address", "publisherid"};
	const RelHead rhPublisherAddress(publisherAddress, countof(publisherAddress));

	set<RelHead> raset1;
	raset1.insert(rhAuthor);
	raset1.insert(rhBook);

	Source_Dummy source1(raset1);

	set<RelHead> raset2;
	Source_Dummy source2(raset2);

	const Rel relAuthor = sConcrete(nullref, "Author", rhAuthor);
	const Rel relBook = sConcrete(nullref, "Book", rhBook);

	Source_Union sourceU;
	sourceU.InsertSource(&source1);
	sourceU.InsertSource(&source2);
//	Nombre theNombre;
//	const Clock theClock;
//	ZRef<Dataset> theDataset = new Dataset(Nombre(theNombre, 1), theClock);
//	Source_Dataset theSource_Dataset(theDataset);
//	sourceU.InsertSource(&theSource_Dataset);

	const ZRef<ZExpr_Logic> thePred = 
//		true & CName("authorname") == CName("title");
		true & CName("authorname") == CString("andy");
	
	Rel authorRenamed = sRename(relAuthor, "authorname", "name");
	
	const Rel authorSearch = thePred & (authorRenamed );//* relBook);

	{
	vector<AddedSearch> theAddedSearches;
	theAddedSearches.push_back(AddedSearch(1, authorSearch));
//	theAddedSearches.push_back(AddedSearch(1, relAuthor));
	vector<SearchResult> changed;
	Clock newClock;
	sourceU.Update(false,
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		nullptr, 0,
		changed, newClock);
	}
	}
