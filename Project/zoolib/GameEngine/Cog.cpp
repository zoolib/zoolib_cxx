#include "zoolib/GameEngine/Cog.h"

#include "zoolib/PairwiseCombiner_T.h"
#include "zoolib/Log.h"
#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/GameEngine/DebugFlags.h"
#include "zoolib/GameEngine/Util.h"

#include <list>

namespace ZooLib {
namespace GameEngine {

using namespace Util_STL;
using std::map;
using std::vector;

// =================================================================================================
#pragma mark - InChannel

InChannel::InChannel(GPoint iGameSize, const std::vector<int>* iKeyDowns, const ZRef<NookScope>& iNookScope)
:	fPrior(nullptr)
,	fGameSize(iGameSize)
,	fKeyDowns(iKeyDowns)
,	fNookScope(iNookScope)
,	fMat(1)
	{}

InChannel::InChannel(const InChannel& iParent, const ZRef<NookScope>& iNookScope)
:	fPrior(&iParent)
,	fGameSize(iParent.fGameSize)
,	fKeyDowns(iParent.fKeyDowns)
,	fNookScope(iNookScope)
,	fMat(iParent.fMat)
	{}

InChannel::InChannel(const InChannel& iParent, const Mat& iMat)
:	fPrior(&iParent)
,	fGameSize(iParent.fGameSize)
,	fKeyDowns(iParent.fKeyDowns)
,	fMat(iParent.fMat * iMat)
	{}

const InChannel* InChannel::GetPrior() const
	{ return fPrior; }

GPoint InChannel::GetGameSize() const
	{ return fGameSize; }

const std::vector<int>* InChannel::GetKeyDowns() const
	{ return fKeyDowns; }

const ZRef<NookScope>& InChannel::GetActiveNookScope() const
	{
	if (fNookScope)
		return fNookScope;
	else if (fPrior)
		return fPrior->GetActiveNookScope();
	else
		return sDefault();
	}

const ZRef<NookScope>& InChannel::GetOwnNookScope() const
	{ return fNookScope; }

const Mat& InChannel::GetMat() const
	{ return fMat; }

// =================================================================================================
#pragma mark -

OutChannel::OutChannel(
	const Map& iRootMap,
	const ZRef<AssetCatalog>& iAssetCatalog,
	const ZRef<FontCatalog>& iFontCatalog,
	const ZRef<SoundMeister>& iSoundMeister)
:	fRootMap(iRootMap)
,	fAssetCatalog(iAssetCatalog)
,	fFontCatalog(iFontCatalog)
,	fSoundMeister(iSoundMeister)
,	fGroup(sRendered_Group())
	{}

OutChannel::~OutChannel()
	{}

vector<ZRef<TouchListener> >& OutChannel::GetTLs() const
	{ return fTLs; }

ZRef<Rendered_Group>& OutChannel::GetGroup() const
	{ return fGroup; }

void OutChannel::RegisterTouchListener(ZRef<TouchListener> iListener) const
	{
	ZAssert(iListener);
	ZAssert(not Util_STL::sContains(fTLs, iListener));
	fTLs.push_back(iListener);

	if (DebugFlags::sTouches)
		fGroup->Append(sRendered_Rect(sRGBA(1, 0, 0, 0.5), iListener->GetBoundsForDebug()));
	}

// =================================================================================================
#pragma mark -

ZRef<Nook> sGetOneNook(const Param& iParam, const Name& iName)
	{
	for (const InChannel* cur = &iParam.fInChannel; cur; cur = cur->GetPrior())
		{
		if (const ZRef<NookScope>& theNS = cur->GetOwnNookScope())
			{
			if (ZRef<Nook> theNook = theNS->GetOne(iName))
				return theNook;
			}
		}
	return null;
	}

std::vector<ZRef<Nook> > sGetAllNooks(const Param& iParam, const Name& iName)
	{
	std::vector<ZRef<Nook> > result;
	for (const InChannel* cur = &iParam.fInChannel; cur; cur = cur->GetPrior())
		{
		if (const ZRef<NookScope>& theNS = cur->GetOwnNookScope())
			{
			std::vector<ZRef<Nook> > temp = theNS->GetAll(iName);
			if (sNotEmpty(temp))
				result.insert(result.end(), temp.begin(), temp.end());
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark - CogRegistration

CogRegistration::CogRegistration(const string8& iCtorName, Fun iFun)
	{ sSingleton<map<string8,Fun> >()[iCtorName] = iFun; }

Cog CogRegistration::sCtor(const string8& iCtorName, const Map& iMap)
	{
	ZQ<Fun> theFun = Util_STL::sQGet(sSingleton<map<string8,Fun> >(), iCtorName);

	if (not theFun || not *theFun)
		{
		if (ZLOGPF(w, eErr))
			w << "Couldn't find ctor for " << iCtorName;
		}

	// Assertion will trip if theFun wasn't found (detail logged above)
	return (*theFun)(iMap);
	}

// =================================================================================================
#pragma mark - CogAccumulatorCombiner

void CogAccumulatorCombiner_Plus::operator()(Cog& io0, const Cog& i1) const
	{ io0 += i1; }

void CogAccumulatorCombiner_Minus::operator()(Cog& io0, const Cog& i1) const
	{
	if (sIsPending(io0))
		io0 -= i1;
	else
		io0 = i1;
	}

void CogAccumulatorCombiner_Each::operator()(Cog& io0, const Cog& i1) const
	{ io0 ^= i1; }

void CogAccumulatorCombiner_And::operator()(Cog& io0, const Cog& i1) const
	{
	if (sIsPending(io0))
		io0 &= i1;
	else
		io0 = i1;
	}

void CogAccumulatorCombiner_Or::operator()(Cog& io0, const Cog& i1) const
	{ io0 |= i1; }

// =================================================================================================
#pragma mark - sCog

namespace { // anonymous

template <class AccumulatorCombiner>
Cog spCogs(size_t iStart, const Seq& iSeq)
	{
	PairwiseCombiner_T<Cog, AccumulatorCombiner, std::list<Cog> > theAcc;

	for (size_t index = iStart, count = iSeq.Count(); index < count; ++index)
		theAcc.Include(sCog(iSeq.Get(index)));

	return theAcc.Get();
	}

Cog spCogs(const Seq& iSeq)
	{
	if (ZQ<string8> theQ = iSeq.QGet<string8>(0))
		{
		if (*theQ == "^")
			return spCogs<CogAccumulatorCombiner_Each>(1, iSeq);

		if (*theQ == "+")
			return spCogs<CogAccumulatorCombiner_Plus>(1, iSeq);

		if (*theQ == "-")
			return spCogs<CogAccumulatorCombiner_Minus>(1, iSeq);

		if (*theQ == "&")
			return spCogs<CogAccumulatorCombiner_And>(1, iSeq);

		if (*theQ == "|")
			return spCogs<CogAccumulatorCombiner_Or>(1, iSeq);

		if (*theQ == "%")
			return sCog(iSeq.Get(1)) % sCog(iSeq.Get(2));

		if (*theQ == "/")
			return sCog(iSeq.Get(1)) % sCog(iSeq.Get(2));

		if (*theQ == "*")
			return sCog_Repeat(sCog(iSeq.Get(1)));
			
		if (ZLOGPF(w, eDebug))
			w << "Unhandled operator: " << *theQ;

		ZUnimplemented();
		}

	return spCogs<CogAccumulatorCombiner_Plus>(0, iSeq);
	}

} // anonymous namespace

Cog sCog(const ZQ<Val>& iValQ)
	{
	if (not iValQ)
		{
		return null;
		}
	else if (iValQ->IsNull())
		{
		return null;
		}
	else if (const ZQ<bool> theBoolQ = iValQ->QGet<bool>())
		{
		return *theBoolQ;
		}
	else if (const ZQ<string8> theStringQ = iValQ->QGet<string8>())
		{
		// There's a good chance something is calling sCog_Toon.
		// Just quietly fail which will let sCog_Toon work.
		return null;
		}
	else if (ZQ<Seq> theSeqQ = iValQ->QGet<Seq>())
		{
		return spCogs(*theSeqQ);
		}
	else if (ZQ<Map> theMapQ = iValQ->QGet<Map>())
		{
		if (ZQ<string8> theCogCtor = theMapQ->QGet<string8>("CogCtor"))
			return CogRegistration::sCtor(*theCogCtor, *theMapQ);
		
		// Special forms?
		return null;
		}

	sDump(*iValQ);

	ZUnimplemented();

	return null;
	}

static
Cog spCogCtor_Cog(const Map& iMap)
	{ return sCog(iMap.QGet("Cog")); }

static
CogRegistration spCogRegistration_Cog("CogCtor_Cog", spCogCtor_Cog);

// =================================================================================================
#pragma mark - sCog_WithEpoch

class Callable_Cog_WithEpoch
:	public Cog::Callable
	{
public:
	Callable_Cog_WithEpoch(double iEpochOffset, const Cog& iChild)
	:	fEpochOffset(iEpochOffset)
	,	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{
		if (const ZQ<Cog> newChildQ = fChild->QCall(
			fChild,
			Param(
				iParam.fInChannel,
				iParam.fOutChannel,
				iParam.fEra,
				iParam.fElapsed - fEpochOffset,
				iParam.fInterval,
				iParam.fSubsequentTime - fEpochOffset)))
			{
			const Cog& newChild = *newChildQ;
			if (newChild == fChild)
				return iSelf;
		
			if (sIsFinished(newChild))
				return newChild;

			return new Callable_Cog_WithEpoch(fEpochOffset, newChild);
			}
		return false;
		}

private:
	const double fEpochOffset;
	const Cog fChild;
	};

class Callable_Cog_NewEpoch
:	public Cog::Callable
	{
public:
	const Cog fChild;

	Callable_Cog_NewEpoch(const Cog& iChild)
	:	fChild(iChild)
		{}

	virtual ZQ<Cog> QCall(const Cog& iSelf, const Param& iParam)
		{ return sCallCog<const Param&>(new Callable_Cog_WithEpoch(iParam.fElapsed, fChild), iParam); }
	};

Cog sCog_NewEpoch(const Cog& iChild)
	{
	if (not iChild)
		return null;
	return new Callable_Cog_NewEpoch(iChild);
	}

// =================================================================================================
#pragma mark - Touches

static
Cog spCogFun_UpdateTouchListener(const Cog& iSelf, const Param& iParam,
	const ZRef<TouchListener>& iTouchListener)
	{
	iTouchListener->SetInverseMat(iParam.fInChannel.GetMat());
	
	iParam.fOutChannel.RegisterTouchListener(iTouchListener);

	return true;
	}

Cog sCog_UpdateTouchListener(const ZRef<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_UpdateTouchListener);
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_TouchNothing(const Cog& iSelf, const Param& iParam,
	const ZRef<TouchListener>& iTouchListener)
	{ return iTouchListener->fDowns.empty(); }

Cog sCog_TouchNothing(const ZRef<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchNothing);
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_TouchDown(const Cog& iSelf, const Param& iParam,
	const ZRef<TouchListener>& iTouchListener)
	{ return not iTouchListener->fDowns.empty(); }

Cog sCog_TouchDown(const ZRef<TouchListener>& iTouchListener)
	{
	GEMACRO_Callable(spCallable, spCogFun_TouchDown);
	return sBindR(spCallable, iTouchListener);
	}

static
Cog spCogFun_DisableTouches(const Cog& iSelf, const Param& iParam,
	const Cog& iChild)
	{
	vector<ZRef<TouchListener> > dummy;
	iParam.fOutChannel.GetTLs().swap(dummy);

	Cog newChild = iChild;
	const bool unchanged = sCallUpdate_Cog_Unchanged(newChild, iParam);

	iParam.fOutChannel.GetTLs().swap(dummy);

	if (unchanged)
		return iSelf;

	if (sIsFinished(newChild))
		return newChild;

	return sCog_DisableTouches(newChild);
	}

Cog sCog_DisableTouches(const Cog& iCog)
	{
	GEMACRO_Callable(spCallable, spCogFun_DisableTouches);
	return sBindR(spCallable, iCog);
	}

// =================================================================================================
#pragma mark - Noop

Cog sCog_Noop()
	{ return sCog_Pending<Cog::Param>(); }

// =================================================================================================
#pragma mark -

static
Cog spCogFun_Log1(const Cog& iSelf, const Param& iParam,
	int iLevel, const string8& iMessage)
	{
	if (const Log::S& w = Log::S(iLevel, "COG1"))
		w << iMessage;
	return true;
	}

Cog sCog_Log1(int iLevel, const string8& iMessage)
	{
	GEMACRO_Callable(spCallable, spCogFun_Log1);	
	return sBindR(spCallable, iLevel, iMessage);	
	}

static
Cog spCogCtor_Log1(const Map& iMap)
	{ return sCog_Log1(sDRat(7, iMap.QGet("Level")), iMap.Get<string8>("Message")); }

static
CogRegistration spCogRegistration_Log("CogCtor_Log", spCogCtor_Log1);

Cog sCog_Log1(const string8& iMessage)
	{
	GEMACRO_Callable(spCallable, spCogFun_Log1);	
	return sBindR(spCallable, Log::eDebug, iMessage);	
	}

static
Cog spCogFun_Log(const Cog& iSelf, const Param& iParam,
	const string8& iMessage)
	{
	if (ZLOG(w, eDebug, "COG"))
		w << iMessage;
	return iSelf;
	}

Cog sCog_Log(const string8& iMessage)
	{
	GEMACRO_Callable(spCallable, spCogFun_Log);	
	return sBindR(spCallable, iMessage);	
	}

// =================================================================================================
#pragma mark - Larger collections of cogs

Cog sCogs_Plus(vector<Cog>* ioCogs)
	{
	if (ioCogs->size())
		{
		PairwiseCombiner_T<Cog, CogAccumulatorCombiner_Plus, std::list<Cog> > theAcc;

		foreacha (entry, *ioCogs)
			theAcc.Include(entry);

		return theAcc.Get();
		}
	return null;
	}

Cog sCogs_Each(vector<Cog>* ioCogs)
	{
	if (ioCogs->size())
		{
		PairwiseCombiner_T<Cog, CogAccumulatorCombiner_Each, std::list<Cog> > theAcc;

		foreacha (entry, *ioCogs)
			theAcc.Include(entry);

		return theAcc.Get();
		}
	return null;
	}

} // namespace GameEngine
} // namespace ZooLib
