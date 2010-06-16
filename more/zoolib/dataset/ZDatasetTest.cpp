#include "zoolib/dataset/ZDataset.h"

#include "zoolib/ZStdIO.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Strim_Data.h"

namespace ZooLib {
namespace ZDataset {

// =================================================================================================
#pragma mark -
#pragma mark *

static void spDumpData(const ZData& iData, const ZStrimW& s)
	{
	const char* theSource = static_cast<const char*>(iData.GetData());
	const size_t theSize = iData.GetSize();
	ZStreamW_HexStrim(" ", "", 0, s) .Write(theSource, theSize);
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
			spDumpData((*i).GetData(), s);
			s << "\n";
			}
		}
	}

static void spDump(const map<Daton, bool>& iStatements, const ZStrimW& s)
	{
	for (map<Daton, bool>::const_iterator j = iStatements.begin();
		j != iStatements.end(); ++j)
		{
		if (!(*j).second)
			s <<"  ";
		spDumpData((*j).first.GetData(), s);
		s << "\n";
		}
	}

static void spDump(ZRef<Delta> iDelta, const ZStrimW& s)
	{ spDump(iDelta->GetStatements(), s); }

static void spDump(ZRef<ClockedDeltas> iSD, const ZStrimW& s)
	{
	const Map_NamedClock_Delta_t& theMap = iSD->GetMap();
	for (Map_NamedClock_Delta_t::const_iterator i = theMap.begin(); i != theMap.end(); ++i)
		{
		printStamp((*i).first.GetClock().Peek().GetStamp());
		spDump((*i).second, s);
		}
	}

static void spDump(ZRef<ClockedDeltasChain> iSDC, const ZStrimW& s)
	{
	if (iSDC)
		{
		spDump(iSDC->GetParent(), s);
		s << "--------\n";
		spDump(iSDC->GetClockedDeltas(), s);
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
	spDump(iDataset->GetClockedDeltasChain(), s);
	}

static void dumptspair(const Clock& a, const Clock& b)
	{
	ZStdIO::strim_err << "a:";
	printStamp(a.Peek().GetStamp());
	ZStdIO::strim_err << "b:";
	printStamp(b.Peek().GetStamp());
	if (a.LE(b))
		ZStdIO::strim_err << "a LE b\n";
	if (b.LE(a))
		ZStdIO::strim_err << "b LE a\n";
	}

static void dumptspair(const NamedClock& a, const NamedClock& b)
	{ dumptspair(a.GetClock(), b.GetClock()); }

// =================================================================================================
#pragma mark -
#pragma mark *

static void spJoin(ZRef<Dataset> iTarget, ZRef<Dataset> iSource)
	{iTarget->Join(iSource); }

template <class T>
ZData sData_T(const T& iT)
	{ return ZData(&iT, sizeof iT); }

template <>
ZData sData_T<string>(const string& iT)
	{ return ZData(iT.c_str(), iT.length()); }

void sTest(const ZStrimW& s)
	{
	Nombre theNombre;
	Clock theClock;
	ZRef<Dataset> dataset = new Dataset(Nombre(theNombre, 1), theClock);

	ZRef<Dataset> datasetA = dataset->Fork();
	ZRef<Dataset> datasetC = dataset->Fork();

//	dumptspair(datasetA->GetNamedClock(), datasetC->GetNamedClock());

	datasetA->Insert(sData_T(string("A1")));

	datasetC->Insert(sData_T(string("C1")));
	datasetC->Insert(sData_T(string("C2")));

	spJoin(datasetA, datasetC);
	datasetA->Insert(sData_T(string("A2")));
	datasetA->Erase(sData_T(string("A1")));
//	datasetA->Fork();
	spDump("datasetA", datasetA, s);

	datasetC->Insert(sData_T(string("C3")));

	spJoin(datasetC, datasetA);
	spDump("datasetC", datasetC, s);
	}

void sTest2(const ZStrimW& s)
	{
	Nombre theNombre;
	Clock theClock;
	ZRef<Dataset> dataset = new Dataset(Nombre(theNombre, 1), theClock);

	ZRef<Dataset> datasetA = dataset->Fork();
	ZRef<Dataset> datasetC = dataset->Fork();

//	dumptspair(datasetA->GetNamedClock(), datasetC->GetNamedClock());

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
