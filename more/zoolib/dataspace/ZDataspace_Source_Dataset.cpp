/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZLog.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"

#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

namespace ZooLib {
namespace ZDataspace {

using ZDataset::Dataset;
using ZDataset::Daton;
using ZDataset::Deltas;
using ZDataset::Map_NamedEvent_Delta_t;
using ZDataset::NamedEvent;

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

enum { kDebug = 1 };

/*
Minimally, keep an index of property names in contents -- that way we can efficiently
identify those entities that satisfy a RelHead.
*/

// =================================================================================================
#pragma mark -
#pragma mark * spAsVal (anonymous)

namespace { // anonymous

ZVal_Any spAsVal(const ZData_Any& iData)
	{
	try
		{
		ZRef<ZStreamerR> theStreamerR =
			new ZStreamerRPos_T<ZStreamRPos_Data_T<ZData_Any> >(iData);

		ZRef<ZStrimmerR> theStrimmerR_StreamUTF8 =
			new ZStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(theStreamerR);

		ZRef<ZStrimmerU> theStrimmerU_Unreader =
			new ZStrimmerU_FT<ZStrimU_Unreader>(theStrimmerR_StreamUTF8);

		ZRef<ZYadR> theYadR = ZYad_ZooLibStrim::sMakeYadR(theStrimmerU_Unreader);

		return sFromYadR(ZVal_Any(), theYadR);
		}
	catch (...)
		{
		return ZVal_Any();
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Daton/val conversion.

ZVal_Any sAsVal(const Daton& iDaton)
	{ return spAsVal(iDaton.GetData()); }

Daton sAsDaton(const ZVal_Any& iVal)
	{
	ZData_Any theData;
	ZYad_ZooLibStrim::sToStrim(sMakeYadR(iVal),
		ZStrimW_StreamUTF8(ZStreamRWPos_Data_T<ZData_Any>(theData)));
	return theData;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker

class Source_Dataset::Visitor_DoMakeWalker
:	public virtual ZQE::Visitor_DoMakeWalker
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
	{
public:
	Visitor_DoMakeWalker(Source_Dataset* iSource)
	:	fSource(iSource)
		{}

	virtual void Visit_Expr_Rel_Concrete(ZRef<ZRA::Expr_Rel_Concrete> iExpr)
		{ this->pSetResult(fSource->pMakeWalker(iExpr->GetConcreteRelHead())); }

	Source_Dataset* fSource;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::Walker

class Source_Dataset::Walker : public ZQE::Walker
	{
public:
	Walker(Source_Dataset* iSource,
		const vector<string8>& iNames,
		Map_Main::const_iterator iCurrent_Main,
		Map_Pending::const_iterator iCurrent_Pending);

	virtual ~Walker();

// From ZQE::Walker
	virtual size_t NameCount();
	virtual string8 NameAt(size_t iIndex);

	virtual ZRef<ZQE::Walker> Clone();
	virtual ZRef<ZQE::Row> ReadInc(ZMap_Any iBindings);

	Source_Dataset* fSource;
	vector<string8> fNames;
	Map_Main::const_iterator fCurrent_Main;
	Map_Pending::const_iterator fCurrent_Pending;
	};

Source_Dataset::Walker::Walker(Source_Dataset* iSource,
	const vector<string8>& iNames,
	Map_Main::const_iterator iCurrent_Main,
	Map_Pending::const_iterator iCurrent_Pending)
:	fSource(iSource)
,	fNames(iNames)
,	fCurrent_Main(iCurrent_Main)
,	fCurrent_Pending(iCurrent_Pending)
	{}

Source_Dataset::Walker::~Walker()
	{}

size_t Source_Dataset::Walker::NameCount()
	{ return fNames.size(); }

string8 Source_Dataset::Walker::NameAt(size_t iIndex)
	{
	if (iIndex < fNames.size())
		return fNames[iIndex];
	return string8();
	}

ZRef<ZQE::Walker> Source_Dataset::Walker::Clone()
	{ return new Walker(fSource, fNames, fCurrent_Main, fCurrent_Pending); }

ZRef<ZQE::Row> Source_Dataset::Walker::ReadInc(ZMap_Any iBindings)
	{ return fSource->pReadInc(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset::PQuery

class Source_Dataset::PQuery
	{
public:
	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset

Source_Dataset::Source_Dataset(ZRef<Dataset> iDataset)
:	fDataset(iDataset)
,	fEvent(Event::sZero())
,	fStackChanged(false)
	{}

Source_Dataset::~Source_Dataset()
	{}

set<RelHead> Source_Dataset::GetRelHeads()
	{ return set<RelHead>(); }

void Source_Dataset::Update(
	bool iLocalOnly,
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	ZRef<Event>& oEvent)
	{
	oChanged.clear();

	const bool anyChanges = fStackChanged || iAddedCount || iRemovedCount;
	fStackChanged = false;

	while (iAddedCount--)
		{
		PQuery* thePQuery = new PQuery;
		thePQuery->fRefcon = iAdded->fRefcon;
		thePQuery->fRel = iAdded->fRel;
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fMap_RefconToPQuery, thePQuery->fRefcon, thePQuery);

		++iAdded;
		}

	while (iRemovedCount--)
		delete ZUtil_STL::sEraseAndReturn(kDebug, fMap_RefconToPQuery, *iRemoved++);

	// Pick up (and index) values from dataset
	if (this->pPull() || anyChanges)
		{
		for (map<int64, PQuery*>::iterator iter_RefconToPQuery = fMap_RefconToPQuery.begin();
			iter_RefconToPQuery != fMap_RefconToPQuery.end(); ++iter_RefconToPQuery)
			{
			ZRef<ZQE::Walker> theWalker =
				Visitor_DoMakeWalker(this).Do(iter_RefconToPQuery->second->fRel);

			vector<string8> theRowHead;
			for (size_t x = 0, count = theWalker->NameCount(); x < count; ++x)
				theRowHead.push_back(theWalker->NameAt(x));

			vector<ZRef<ZQE::Row> > theRows;
			for (ZRef<ZQE::Row> theRow; theRow = theWalker->ReadInc(ZMap_Any()); /*no inc*/)
				theRows.push_back(theRow);

			SearchResult theSearchResult;
			theSearchResult.fRefcon = iter_RefconToPQuery->first;
			theSearchResult.fResultSet = new ZQE::ResultSet(&theRowHead, new ZQE::RowVector(&theRows));
			oChanged.push_back(theSearchResult);
			}
		}

	oEvent = fEvent;
	}

ZRef<ZDataset::Dataset> Source_Dataset::GetDataset()
	{
	return fDataset;
	}

void Source_Dataset::Insert(const Daton& iDaton)
	{ this->pModify(iDaton, sAsVal(iDaton), true); }

void Source_Dataset::Erase(const Daton& iDaton)
	{ this->pModify(iDaton, sAsVal(iDaton), false); }

void Source_Dataset::Insert(const ZVal_Any& iVal)
	{ this->pModify(sAsDaton(iVal), iVal, true); }

void Source_Dataset::Erase(const ZVal_Any& iVal)
	{ this->pModify(sAsDaton(iVal), iVal, false); }

size_t Source_Dataset::OpenTransaction()
	{
	fStackChanged = true;
	fStack.push_back(fMap_Pending);
	return fStack.size();
	}

void Source_Dataset::ClearTransaction(size_t iIndex)
	{
	fStackChanged = true;
	ZAssert(iIndex == fStack.size());
	fMap_Pending = fStack.back();

	this->pInvokeCallable();
	}

void Source_Dataset::CloseTransaction(size_t iIndex)
	{
	fStackChanged = true;
	ZAssert(iIndex == fStack.size());
	fStack.pop_back();
	if (fStack.empty())
		{
		for (Map_Pending::iterator i = fMap_Pending.begin(), end = fMap_Pending.end();
			i != end; ++i)
			{
			if (i->second.second)
				fDataset->Insert(i->first);
			else
				fDataset->Erase(i->first);			
			}
		fMap_Pending.clear();
		}
	this->pInvokeCallable();
	}

void Source_Dataset::pModify(const ZDataset::Daton& iDaton, const ZVal_Any& iVal, bool iSense)
	{
	fStackChanged = true;
	Map_Pending::iterator i = fMap_Pending.find(iDaton);
	if (fMap_Pending.end() == i)
		{
		fMap_Pending.insert(make_pair(iDaton, make_pair(iVal, iSense)));
		}
	else
		{
		ZAssert(i->second.second == !iSense);
		fMap_Pending.erase(i);
		}
	this->pInvokeCallable();	
	}

ZRef<ZQE::Walker> Source_Dataset::pMakeWalker(const RelHead& iRelHead)
	{
	vector<string8> theNames(iRelHead.begin(), iRelHead.end());
	return new Walker(this, theNames, fMap.begin(), fMap_Pending.begin());
	}

static ZRef<ZQE::Row_ValVector> spMakeRow(
	const ZMap_Any& iMap, const vector<string8>& iNames)
	{
	const size_t nameCount = iNames.size();
	vector<ZVal_Any> theVals;
	theVals.resize(nameCount);
	bool allGood = true;
	for (size_t x = 0; x < nameCount; ++x)
		{
		if (const ZVal_Any* theVal = iMap.PGet(iNames[x]))
			{
			theVals[x] = *theVal;
			}
		else
			{
			allGood = false;
			break;
			}
		}

	if (allGood)
		return new ZQE::Row_ValVector(&theVals);

	return null;
	}

ZRef<ZQE::Row> Source_Dataset::pReadInc(ZRef<Walker> iWalker)
	{
	while (iWalker->fCurrent_Main != fMap.end())
		{
		// Ignore anything that's in pending (for now).
		if (fMap_Pending.end() == fMap_Pending.find(iWalker->fCurrent_Main->first))
			{
			if (const ZMap_Any* theMap = iWalker->fCurrent_Main->second.second.PGet<ZMap_Any>())
				{
				if (ZRef<ZQE::Row_ValVector> theRow = spMakeRow(*theMap, iWalker->fNames))
					{
					theRow->AddAnnotation(new Annotation_Daton(iWalker->fCurrent_Main->first));
					++iWalker->fCurrent_Main;
					return theRow;
					}
				}
			}
		++iWalker->fCurrent_Main;
		}

	// Handle anything pending
	while (iWalker->fCurrent_Pending != fMap_Pending.end())
		{
		if (iWalker->fCurrent_Pending->second.second)
			{
			if (const ZMap_Any* theMap = iWalker->fCurrent_Pending->second.first.PGet<ZMap_Any>())
				{
				if (ZRef<ZQE::Row_ValVector> theRow = spMakeRow(*theMap, iWalker->fNames))
					{
					theRow->AddAnnotation(new Annotation_Daton(iWalker->fCurrent_Pending->first));
					++iWalker->fCurrent_Pending;
					return theRow;
					}
				}
			}
		++iWalker->fCurrent_Pending;
		}

	return null;
	}

bool Source_Dataset::pPull()
	{
	// Get our map in sync with fDataset
	ZLOGF(s, eDebug);
	ZRef<Deltas> theDeltas;
	fEvent = fDataset->GetDeltas(theDeltas, fEvent);
	const Map_NamedEvent_Delta_t& theMNED = theDeltas->GetMap();
	s.Writef("theMNED.size()=%zu", theMNED.size());
	bool anyChanges = false;
	for (Map_NamedEvent_Delta_t::const_iterator
		iterMNED = theMNED.begin(), endMNED = theMNED.end();
		iterMNED != endMNED; ++iterMNED)
		{
		const NamedEvent& theNamedEvent = iterMNED->first;
		const map<Daton, bool>& theStatements = iterMNED->second->GetStatements();
		s.Writef("\ntheStatements.size()=%zu", theStatements.size());
		for (map<Daton, bool>::const_iterator
			iterStmts = theStatements.begin(), endStmts = theStatements.end();
			iterStmts != endStmts; ++iterStmts)
			{
			const Daton& theDaton = iterStmts->first;
			const pair<NamedEvent, ZVal_Any> newPair(theNamedEvent, sAsVal(theDaton));

			map<Daton, pair<NamedEvent, ZVal_Any> >::iterator iterMap = fMap.find(theDaton);

			if (iterMap == fMap.end() || iterMap->first != theDaton)
				{
				if (iterStmts->second)
					{
					anyChanges = true;
					fMap.insert(iterMap, make_pair(theDaton, newPair));
					}
				}
			else if (iterMap->second.first < theNamedEvent)
				{
				// theNamedEvent is more recent than what we've got and thus supersedes it.
				anyChanges = true;

				if (iterStmts->second)
					iterMap->second = newPair;
				else
					fMap.erase(iterMap);
				}
			}
		}
	return true;//anyChanges;
	}

} // namespace ZDataspace
} // namespace ZooLib
