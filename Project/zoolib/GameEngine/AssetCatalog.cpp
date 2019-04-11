#include "zoolib/GameEngine/AssetCatalog.h"

#include "zoolib/Callable_PMF.h"
#include "zoolib/DList.h"
#include "zoolib/Log.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/ThreadVal.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_unordered_map.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/GameEngine/DebugFlags.h"
#include "zoolib/GameEngine/Util.h"

namespace ZooLib {
namespace GameEngine {

using namespace Util_STL;
using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Sheet (anonymous)

namespace { // anonymous

class Sheet;

class DLink_Sheet_Active
:	public DListLink<Sheet,DLink_Sheet_Active>
	{};

class DLink_Sheet_Cached
:	public DListLink<Sheet,DLink_Sheet_Cached>
	{};

class DLink_Sheet_Load
:	public DListLink<Sheet,DLink_Sheet_Load>
	{};

class Sheet
:	public ZCounted
,	public DLink_Sheet_Active
,	public DLink_Sheet_Cached
,	public DLink_Sheet_Load
	{
public:
	Sheet(
		const ZRef<AssetCatalog::SheetCatalog>& iSC,
		const Name& iName,
		const ZRef<AssetCatalog::Callable_TextureMaker>& iTextureMaker);

	virtual ~Sheet();

// From ZCounted
	virtual void Finalize();

// Our protocol
	ZRef<Texture> GetTexture();

	ZRef<AssetCatalog::SheetCatalog> fSC;
	int fPriority;
	const Name fName;
	const ZRef<AssetCatalog::Callable_TextureMaker> fTextureMaker;
	ZRef<Texture> fTexture;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - AssetCatalog::SheetCatalog

class AssetCatalog::SheetCatalog
:	public ZCounted
	{
public:
	SheetCatalog()
		{}

	virtual ~SheetCatalog()
		{}

	virtual void Initialize()
		{
		ZCounted::Initialize();
		sStartOnNewThread(sCallable(sRef(this), &SheetCatalog::pLoad));
		}

	void Purge()
		{
		ZAcqMtx acq(fMtx);
		ZLOGTRACE(eDebug);
		
		// Gather overall size
		size_t pixelCount_Total = 0;
		for (DListIterator<Sheet,DLink_Sheet_Cached> iter = fSheets_Cached; iter; iter.Advance())
			{
			Sheet* theSheet = iter.Current();
			if (const ZRef<Texture>& theTexture = theSheet->fTexture)
				pixelCount_Total += theTexture->fPixelCount;
			}

		// TODO: Prioritize based on fPriority.

		const size_t pixelCount_ToPurge = pixelCount_Total / 2;
		size_t pixelCount_Purged = 0;
		while (fSheets_Cached.Size() && pixelCount_Purged < pixelCount_ToPurge)
			{
			Sheet* theSheet = sGetEraseBackMust<Sheet>(fSheets_Cached);
			if (const ZRef<Texture>& theTexture = theSheet->fTexture)
				pixelCount_Purged += theTexture->fPixelCount;
			sEraseMust(fSheets, theSheet->fName);			
			sQErase(fSheets_Load, theSheet);
			delete theSheet;
			}

		if (ZLOGF(w, eNotice))
			w << "Total: " << pixelCount_Total << ", Purged: " << pixelCount_Purged;
		}

	ZRef<Sheet> Get(const Name& iName, int iPriority)
		{
		ZAcqMtx acq(fMtx);

		ZRef<Sheet> result;

		if (ZQ<Sheet*> theQ = sQGet(fSheets, iName))
			{
			Sheet* theSheet = *theQ;
			if (sQErase(fSheets_Cached, theSheet))
				sInsertBackMust(fSheets_Active, theSheet);
			ZAssert(sContains(fSheets_Active, theSheet));
			ZAssert(fSheets.size() == fSheets_Cached.Size() + fSheets_Active.Size());
			result = theSheet;
			}
		else if (ZQ<ZRef<Callable_TextureMaker> > theTM = sQGet(fTextureMakers, iName))
			{
			Sheet* theSheet = new Sheet(this, iName, *theTM);
			sInsertMust(fSheets, iName, theSheet);
			sInsertBackMust(fSheets_Active, theSheet);
			ZAssert(fSheets.size() == fSheets_Cached.Size() + fSheets_Active.Size());
			result = theSheet;
			if (ZLOGF(w, eDebug + 1))
				w << iName << " needs load";
			}

		if (result)
			{
			if (not result->fTexture)
				{
				result->fPriority = sMin(iPriority, result->fPriority);
				sQInsertBack(fSheets_Load, result.Get());
				fCnd_Load.Broadcast();
				}
			}

		return result;
		}

	void pLoad()
		{
		ZThread::sSetName("Load");

		for (;;)
			{
			ZRef<Sheet> theSheet;
			ZAcqMtx acq(fMtx);

			int highestPriority = 10000;
			Sheet* highestSheet = nullptr;
			for (DListIterator<Sheet,DLink_Sheet_Load> iter = fSheets_Load; iter; iter.Advance())
				{
				Sheet* aSheet = iter.Current();
				if (highestPriority > aSheet->fPriority)
					{
					highestPriority = aSheet->fPriority;
					highestSheet = aSheet;
					}
				}

			if (not highestSheet)
				{
				fCnd_Load.Wait(fMtx);
				continue;
				}

			theSheet = highestSheet;

			ZRef<Texture> theTexture;
			{
			ZRelMtx rel(fMtx);
			theTexture = sCall(theSheet->fTextureMaker);
			}
			// Gotta be *super* careful here. Overwriting an existing texture can cause
			// us to hit OpenGL from a thread that doesn't know the context.
			ZAssert(not theSheet->fTexture);
			sEraseMust(fSheets_Load, highestSheet);
			theSheet->fTexture = theTexture;
			if (ZLOGF(w, eDebug + 1))
				w << theSheet->fName << " loaded";
			fCnd_Get.Broadcast();
			}
		}

	void pFinalize(Sheet* iSheet)
		{
		ZAcqMtx acq(fMtx);
		if (not iSheet->IsShared())
			{
			if (sQErase(fSheets_Active, iSheet))
				sInsertBackMust(fSheets_Cached, iSheet);
			ZAssert(fSheets.size() == fSheets_Cached.Size() + fSheets_Active.Size());
			}
		iSheet->FinishFinalize();
		}

	const ZRef<Callable_TextureMaker> fTextureMaker;

	ZMtx fMtx;
	ZCnd fCnd_Load;
	ZCnd fCnd_Get;

	map<Name,Sheet*> fSheets;

	DListHead<DLink_Sheet_Active> fSheets_Active;
	DListHead<DLink_Sheet_Cached> fSheets_Cached;
	DListHead<DLink_Sheet_Load> fSheets_Load;

	map<Name,ZRef<Callable_TextureMaker> > fTextureMakers;
	};

// =================================================================================================
#pragma mark - Sheet

Sheet::Sheet(
	const ZRef<AssetCatalog::SheetCatalog>& iSC,
	const Name& iName,
	const ZRef<AssetCatalog::Callable_TextureMaker>& iTextureMaker)
:	fSC(iSC)
,	fPriority(9999)
,	fName(iName)
,	fTextureMaker(iTextureMaker)
	{}

Sheet::~Sheet()
	{}

void Sheet::Finalize()
	{
	if (const ZRef<AssetCatalog::SheetCatalog>& theSC = fSC)
		theSC->pFinalize(this);
	else
		ZCounted::Finalize();
	}

ZRef<Texture> Sheet::GetTexture()
	{
	if (const ZRef<AssetCatalog::SheetCatalog>& theSC = fSC)
		{
		ZAcqMtx acq(theSC->fMtx);
		if (not fTexture && not DebugFlags::sTextureBounds)
			{
			const double time0 = Time::sSystem();

			while (not fTexture)
				theSC->fCnd_Get.Wait(theSC->fMtx);

			if (ZLOGF(w, eDebug))
				{
				w << fName << ": ";
				sEWritef(w, "%.3fms, ", (Time::sSystem()-time0) * 1e3);
				}
			}
		return fTexture;
		}
	return null;
	}

// =================================================================================================
#pragma mark - AssetCatalog

AssetCatalog::AssetCatalog()
:	fSheetCatalog(new SheetCatalog)
	{}

AssetCatalog::~AssetCatalog()
	{}

void AssetCatalog::Load(const NameFrame& iNameFrame, int iPriority)
	{
	AcqMtxF acq(fMtx);
	this->pGet(iNameFrame.fName, iNameFrame.fFrame, iPriority, nullptr);
	}

size_t AssetCatalog::FrameCount(const Name& iName)
	{
	if (ZQ<Map_Any> theAnimQ = fMap.QGet<Map_Any>(iName))
		{
		if (const ZQ<Seq_Any> theFramesQ = theAnimQ->QGet<Seq_Any>("Frames"))
			return theFramesQ->Size();
		}
	return 0;
	}

void AssetCatalog::Get(const NameFrame& iNameFrame, vector<Texture_BoundsQ_Mat>& ioResult)
	{
	const double time0 = Time::sSystem();
	AcqMtxF acq(fMtx);
	const double time1 = Time::sSystem();
	if (not this->pGet(iNameFrame.fName, iNameFrame.fFrame, false, &ioResult))
		ioResult.clear();
		
	const double time2 = Time::sSystem();
	const double duration = time2 - time0;
	if (duration > 10e-3)
		{
		if (ZLOGF(w, eDebug))
			{
			w << iNameFrame.fName << ":" << iNameFrame.fFrame;
			sEWritef(w, ", %.3fms", duration * 1e3);
			sEWritef(w, ", %.3fms", (time1-time0) * 1e3);
			}
		}
	}

void AssetCatalog::InstallSheet(const Name& iName, const ZRef<Callable_TextureMaker>& iTM)
	{ fSheetCatalog->fTextureMakers[iName] = iTM; }

void AssetCatalog::Set_Processed(const Map_Any& iMap)
	{ fMap = iMap; }

void AssetCatalog::Purge()
	{ fSheetCatalog->Purge(); }

bool AssetCatalog::pGet(const Name& iName, size_t iFrame, int iPriority,
	vector<Texture_BoundsQ_Mat>* ioResult)
	{
	ZMACRO_auto_(iterNameEntry, fMap_NameEntry.find(iName));
	if (iterNameEntry == fMap_NameEntry.end())
		{
		const Map_Any* theAnimP = fMap.PGet<Map_Any>(iName);
		if (not theAnimP)
			return false;
		
		const Map_Any& theAnim = *theAnimP;

		const Seq_Any theFramesSeq = theAnim.Get<Seq_Any>("Frames");
		if (not theFramesSeq.Size())
			return false;

		Entry theEntry;
		theEntry.fDefaultSheetName = sGet<string8>(theAnim["SheetName"]);
		theEntry.fDefaultBoundsQ = sQGRect(theAnim["Bounds"]);
		theEntry.fBaseScale = sCVec3(1, theAnim["Scale"]);
		theEntry.fBaseReg = sCVec3(0, theAnim["Reg"]);
		theEntry.fBaseInset = sQGRect(theAnim["Inset"]).DGet(GRect());
		theEntry.fFramesSeq = theFramesSeq;
		theEntry.fFramesVec.resize(theFramesSeq.Size());

		fMap_NameEntry[iName] = theEntry;
		iterNameEntry = fMap_NameEntry.find(iName);
		}

	Entry& theEntry = iterNameEntry->second;
	if (iFrame >= theEntry.fFramesVec.size())
		return false;

	Frame& theFrame = theEntry.fFramesVec[iFrame];
	if (not theFrame.fLoaded)
		{
		Seq_Any& theFramesSeq = theEntry.fFramesSeq;

		// TBM = Texture Bounds Mat

		Seq_Any* theTBMsSeqP = theFramesSeq.PMut<Seq_Any>(iFrame);
		if (not theTBMsSeqP)
			{
			theFramesSeq[iFrame] = Seq_Any().Append(theFramesSeq.Get<Map_Any>(iFrame));
			theTBMsSeqP = theFramesSeq.PMut<Seq_Any>(iFrame);
			}
		Seq_Any& theTBMsSeq = *theTBMsSeqP;

		theFrame.fSheet_BoundsQ_Mats.resize(theTBMsSeq.Size());
		if (ioResult)
			ioResult->resize(theFrame.fSheet_BoundsQ_Mats.size());

		size_t usedIndex = 0;
		for (size_t theTBMIndex = 0, theTBMCount = theTBMsSeq.Size();
			theTBMIndex < theTBMCount; ++theTBMIndex)
			{
			theFrame.fLoaded.Set();

			const Map_Any& theTBM = theTBMsSeq.Get<Map_Any>(theTBMIndex);

			const string8 theSheetName =
				theTBM.DGet<string8>(theEntry.fDefaultSheetName, "SheetName");
			
			if (ZRef<Sheet> theSheet = fSheetCatalog->Get(theSheetName, iPriority))
				{
				Sheet_BoundsQ_Mat& theSheet_BoundsQ_Mat = theFrame.fSheet_BoundsQ_Mats[theTBMIndex];
				theSheet_BoundsQ_Mat.fSheetName = theSheetName;
				const CVec3 theScale = theEntry.fBaseScale * sCVec3(1, theTBM["Scale"]);
				const CVec3 theReg = theEntry.fBaseReg + sCVec3(0, theTBM["Reg"]);

				// TODO: WTF is happening here?
				theSheet_BoundsQ_Mat.fInset = sQGRect(theTBM["Inset"]).DGet(GRect());
				theSheet_BoundsQ_Mat.fInset = sRect(
					LT(theSheet_BoundsQ_Mat.fInset) + LT(theEntry.fBaseInset),
					RB(theSheet_BoundsQ_Mat.fInset) + RB(theEntry.fBaseInset));
				theSheet_BoundsQ_Mat.fMat = sScale(theScale) * sTranslate(theReg * -1.0);

				if (ZQ<GRect> theBoundsQ = sQGRect(theTBM["Bounds"]))
					{
					theSheet_BoundsQ_Mat.fBoundsQ = *theBoundsQ;
					}
				else if (theEntry.fDefaultBoundsQ)
					{
					theSheet_BoundsQ_Mat.fBoundsQ = *theEntry.fDefaultBoundsQ;
					}

				if (ioResult)
					{
					if (ZRef<Texture> theTexture = theSheet->GetTexture())
						{
						Texture_BoundsQ_Mat& theTBM = (*ioResult)[usedIndex];
						theTBM.fTexture = theTexture;
						theTBM.fBoundsQ = theSheet_BoundsQ_Mat.fBoundsQ;
						theTBM.fInset = theSheet_BoundsQ_Mat.fInset;
						theTBM.fMat = theSheet_BoundsQ_Mat.fMat;
						++usedIndex;
						}
					else
						{
						if (ZLOGF(w,eDebug-1))
							w << theSheetName << ", previously unknown, not ready";
						}
					}
				}
			}
		if (ioResult)
			ioResult->resize(usedIndex);
		}
	else
		{
		if (ioResult)
			ioResult->resize(theFrame.fSheet_BoundsQ_Mats.size());

		size_t usedIndex = 0;

		foreachv (const Sheet_BoundsQ_Mat& theSheet_BoundsQ_Mat, theFrame.fSheet_BoundsQ_Mats)
			{
			if (ZRef<Sheet> theSheet =
				fSheetCatalog->Get(theSheet_BoundsQ_Mat.fSheetName, iPriority))
				{
				if (ioResult)
					{
					if (ZRef<Texture> theTexture = theSheet->GetTexture())
						{
						Texture_BoundsQ_Mat& theTBM = (*ioResult)[usedIndex];
						theTBM.fTexture = theSheet->GetTexture();
						theTBM.fBoundsQ = theSheet_BoundsQ_Mat.fBoundsQ;
						theTBM.fInset = theSheet_BoundsQ_Mat.fInset;
						theTBM.fMat = theSheet_BoundsQ_Mat.fMat;
						++usedIndex;
						}
					else
						{
						theTexture = theSheet->GetTexture();
						if (ZLOGF(w,eDebug-1))
							w << theSheet_BoundsQ_Mat.fSheetName << ", not ready";
						}
					}
				}
			}
		if (ioResult)
			ioResult->resize(usedIndex);
		}

	return true;
	}
// =================================================================================================
#pragma mark - Globals

size_t sToonFrameCount(const Name& iName)
	{
	return sThreadVal<ZRef<AssetCatalog>>()->FrameCount(iName);
	}

} // namespace GameEngine
} // namespace ZooLib
