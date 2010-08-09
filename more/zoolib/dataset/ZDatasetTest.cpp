#include "zoolib/dataset/ZDataset.h"

#include "zoolib/ZStdIO.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Strim_Data.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"

namespace ZooLib {
namespace ZDataset {

using namespace std;

// =================================================================================================
#pragma mark -
#pragma mark *

static void spDumpData(const ZData& iData, const ZStrimW& s)
	{
	const char* theSource = static_cast<const char*>(iData.GetData());
	const size_t theSize = iData.GetSize();
	ZStreamW_HexStrim(" ", "", 0, s).Write(theSource, theSize);
	s.Write(" // ");
	for (size_t x = 0; x < theSize; ++x)
		{
		const char theChar = theSource[x];
		if (theChar < 0x20 || theChar > 0x7E)
			s.WriteCP('.');
		else
			s.WriteCP(theChar);
		}
	}

static void spDump(const set<Daton>& iSet, const ZStrimW& s)
	{
	if (!iSet.empty())
		{
		for (set<Daton>::const_iterator i = iSet.begin();
			i != iSet.end(); ++i)
			{
			spDumpData(i->GetData(), s);
			s << "\n";
			}
		}
	}

static void spDump(const map<Daton, bool>& iStatements, const ZStrimW& s)
	{
	for (map<Daton, bool>::const_iterator j = iStatements.begin();
		j != iStatements.end(); ++j)
		{
		if (!j->second)
			s << "~";
		spDumpData(j->first.GetData(), s);
		s << "\n";
		}
	}

static void spDump(ZRef<Delta> iDelta, const ZStrimW& s)
	{ spDump(iDelta->GetStatements(), s); }

static void spDump(ZRef<Deltas> iSD, const ZStrimW& s)
	{
	const Map_NamedEvent_Delta_t& theMap = iSD->GetMap();
	for (Map_NamedEvent_Delta_t::const_iterator i = theMap.begin(); i != theMap.end(); ++i)
		{
		s << i->first.GetEvent() << "\n";
		spDump(i->second, s);
		}
	}

static void spDump(ZRef<DeltasChain> iSDC, const ZStrimW& s)
	{
	if (iSDC)
		{
		spDump(iSDC->GetParent(), s);
		s << "  --\n";
		spDump(iSDC->GetDeltas(), s);
		}
	}

static void spDump(const string& iName, ZRef<Dataset> iDataset, const ZStrimW& s)
	{
	s << "\n------------------\n------------------\n" << iName << "\n";
	s << iDataset->GetNombre().AsString() << "\n";

	s << "Contents\n";
	spDump(iDataset->GetComposed(), s);

	// There won't be any pending statements after calling GetComposed
//	s << "Pending\n";
//	spDump(iDataset->fPendingStatements, s);

	s << "History\n";
	spDump(iDataset->GetDeltasChain(), s);
	}

static void dumptspair(const ZRef<Event>& a, const ZRef<Event>& b)
	{
	ZStdIO::strim_err << "a:" << a << "\nb:" << b << "\n";
	if (a->LessEqual(b))
		ZStdIO::strim_err << "a LE b\n";
	if (b->LessEqual(a))
		ZStdIO::strim_err << "b LE a\n";
	}

static void dumptspair(const NamedEvent& a, const NamedEvent& b)
	{ dumptspair(a.GetEvent(), b.GetEvent()); }

// =================================================================================================
#pragma mark -
#pragma mark *

static void spJoin(ZRef<Dataset> iTarget, ZRef<Dataset> iSource)
	{ iTarget->Join(iSource); }

template <class T>
ZData sData_T(const T& iT)
	{ return ZData(&iT, sizeof iT); }

template <>
ZData sData_T<string>(const string& iT)
	{ return ZData(iT.c_str(), iT.length()); }

void sTest(const ZStrimW& s)
	{
	Nombre theNombre;
	ZRef<Stamp> theClock = ZIntervalTreeClock::Stamp::sSeed();
	ZRef<Dataset> dataset = new Dataset(Nombre(theNombre, 1), theClock);

	ZRef<Dataset> datasetA = dataset->Fork();
	ZRef<Dataset> datasetC = dataset->Fork();

	datasetA->Insert(sData_T(string("A1")));
	datasetA->Insert(sData_T(string("A2")));

	datasetC->Insert(sData_T(string("C1")));
	datasetC->Insert(sData_T(string("C2")));

	spJoin(datasetA, datasetC);

	datasetA->Erase(sData_T(string("A1")));
	datasetA->Insert(sData_T(string("A3")));
	datasetC->Insert(sData_T(string("C3")));

	spDump("datasetA", datasetA, s);
	spDump("datasetC", datasetC, s);

	spJoin(datasetA, datasetC);
	spDump("datasetA", datasetA, s);

	spJoin(datasetC, datasetA);
	spDump("datasetC", datasetC, s);
	}

void sTest3(const ZStrimW& s)
	{
#if 0
	using ZIntervalTreeClock::Identity;
	ZRef<Identity> theID = Identity::sOne();
	s << theID << "\n";

	ZRef<Identity> theID1, theID2;
	theID->Split(theID1, theID2);
	s << "theID1 " << theID1 << "\n";
//	s << "theID2 " << theID2 << "\n";

//	s << "theID1+theID2 " << theID1->Summed(theID2) << "\n\n";

	ZRef<Identity> theID3, theID4;
	theID1->Split(theID3, theID4);
	s << "theID3 " << theID3 << "\n";
//	s << "theID4 " << theID4 << "\n\n";
//	s << "theID3+theID4 " << theID3->Summed(theID4) << "\n\n";
	s << "theID1+theID3 " << theID1->Summed(theID3) << "\n\n";
//	s << "theID1+theID4 " << theID1->Summed(theID4) << "\n\n";


	return;
#endif
	
	
	Nombre theNombre;
	ZRef<Stamp> theClock = ZIntervalTreeClock::Stamp::sSeed();
	ZRef<Dataset> dataset = new Dataset(Nombre(theNombre, 1), theClock);

	ZRef<Dataset> datasetA = dataset->Fork();
	ZRef<Dataset> datasetC = dataset->Fork();

	ZStdIO::strim_err << "#1\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());

	datasetA->Insert(sData_T(string("A1")));

	datasetC->Insert(sData_T(string("C1")));
	datasetC->Insert(sData_T(string("C2")));

	//ZStdIO::strim_err << "#2\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());
	spJoin(datasetA, datasetC);
	//ZStdIO::strim_err << "#3\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());

	datasetA->Insert(sData_T(string("A2")));
	datasetA->Erase(sData_T(string("A1")));
//	datasetA->Fork();
	spDump("datasetA", datasetA, s);

	datasetC->Insert(sData_T(string("C3")));
	spDump("datasetC", datasetC, s);

	//ZStdIO::strim_err << "#4\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());
	spJoin(datasetC, datasetA);
	//ZStdIO::strim_err << "#5\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());

	//ZStdIO::strim_err << "#6\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());
	spJoin(datasetA, datasetC);
	//ZStdIO::strim_err << "#7\n";
	//dumptspair(datasetA->GetEvent(), datasetC->GetEvent());

	spDump("datasetC", datasetC, s);
	}

void sTest2(const ZStrimW& s)
	{
	Nombre theNombre;
	ZRef<Stamp> theClock;
	ZRef<Dataset> dataset = new Dataset(Nombre(theNombre, 1), theClock);

	ZRef<Dataset> datasetA = dataset->Fork();
	ZRef<Dataset> datasetC = dataset->Fork();

//	dumptspair(datasetA->GetNamedEvent(), datasetC->GetNamedEvent());

	datasetA->Insert(sData_T(string("A1")));

	datasetC->Insert(sData_T(string("C1")));
	datasetC->Insert(sData_T(string("C2")));

	spJoin(datasetA, datasetC);
//	datasetA->Insert(sData_T(string("A2")));
	datasetA->Erase(sData_T(string("A1")));
	datasetA->Fork();
	spDump("datasetA", datasetA, s);

	spJoin(datasetC, datasetA);
	spDump("datasetC", datasetC, s);
	}

} // namespace ZDataset
} // namespace ZooLib
