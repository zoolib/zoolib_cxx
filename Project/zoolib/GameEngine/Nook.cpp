// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Nook.h"

#include "zoolib/Util_STL_set.h"

namespace ZooLib {
namespace GameEngine {

using std::pair;
using std::set;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark -

NookScope::NookScope()
	{}

NookScope::~NookScope()
	{}

void NookScope::NewEra()
	{
	ZAcqMtx acq(fMtx);
	fNooks_All.insert(fNooks_JustAdded.begin(), fNooks_JustAdded.end());
	swap(fNooks_Adding, fNooks_JustAdded);
	fNooks_Adding.clear();	
	}

ZP<Nook> NookScope::GetOne(const Name& iName)
	{
	ZAcqMtx acq(fMtx);
	const NameNook theNN(iName, nullptr);

	NameNookSet::const_iterator iter = fNooks_JustAdded.lower_bound(theNN);
	if (iter != fNooks_JustAdded.end() && iter->first == iName)
		return iter->second;

	iter = fNooks_All.lower_bound(theNN);
	if (iter != fNooks_All.end() && iter->first == iName)
		return iter->second;
	
	return null;
	}

vector<ZP<Nook> > NookScope::GetAll(const Name& iName)
	{
	ZAcqMtx acq(fMtx);
	vector<ZP<Nook> > result;

	const NameNook theNN(iName, nullptr);

	for (NameNookSet::const_iterator
		iter = fNooks_JustAdded.lower_bound(theNN),
		end = fNooks_JustAdded.end();
		iter != end && iter->first == iName; ++iter)
		{ result.push_back(iter->second); }

	for (NameNookSet::const_iterator
		iter = fNooks_All.lower_bound(theNN),
		end = fNooks_All.end();
		iter != end && iter->first == iName; ++iter)
		{ result.push_back(iter->second); }
	
	return result;
	}

bool NookScope::pIsFullCycle(Nook* iNook)
	{
	ZAcqMtx acq(fMtx);
	return sContains(fNooks_All, NameNook(iNook->fName, iNook));
	}

void NookScope::pInitialize(Nook* iNook)
	{
	ZAcqMtx acq(fMtx);
	const NameNook theNN(iNook->fName, iNook);
	sInsertMust(fNooks_Adding, theNN);
	ZAssert(not sContains(fNooks_JustAdded, theNN));
	ZAssert(not sContains(fNooks_All, theNN));
	}

void NookScope::pFinalize(Nook* iNook)
	{
	ZAcqMtx acq(fMtx);
	if (iNook->FinishFinalize())
		{
		const NameNook theNN(iNook->fName, iNook);
		if (not sQErase(fNooks_Adding, theNN))
			{
			if (not sQErase(fNooks_JustAdded, theNN))
				sEraseMust(fNooks_All, theNN);
			}
		}
	}

// =================================================================================================
#pragma mark - Nook

Nook::Nook(const ZP<NookScope>& iNookScope)
:	fNookScopeWP(iNookScope)
	{}

void Nook::Initialize()
	{
	fName = typeid(*this).name();

	if (ZP<NookScope> theNM = fNookScopeWP)
		theNM->pInitialize(this);
	else
		Counted::Initialize();
	}

void Nook::Finalize()
	{
	if (ZP<NookScope> theNM = fNookScopeWP)
		theNM->pFinalize(this);
	else
		Counted::Finalize();
	}

bool Nook::IsFullCycle()
	{
	if (ZP<NookScope> theNM = fNookScopeWP)
		return theNM->pIsFullCycle(this);
	return false;
	}

ZP<NookScope> Nook::GetNookScope()
	{ return fNookScopeWP; }

} // namespace GameEngine
} // namespace ZooLib
