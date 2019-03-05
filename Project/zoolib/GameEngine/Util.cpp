#include "zoolib/GameEngine/Util.h"

#include "zoolib/Callable_Function.h"
#include "zoolib/Cartesian_Matrix.h"
#include "zoolib/Chan_XX_Buffered.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/ChanRU_UTF_Std.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For ThreadVal_NameUniquifier
#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_Any_JSONB.h"
#include "zoolib/Util_Chan_JSON.h"
#include "zoolib/Util_string.h"

#include "zoolib/ZMACRO_auto.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/ZDCPixmap_CGImage.h"

#include "zoolib/Apple/CGData_Channer.h"

#if 1 // not ZCONFIG_SPI_Enabled(CoreGraphics)
	#include "zoolib/ZDCPixmapCoder_PNG.h"
#endif

namespace ZooLib {
namespace GameEngine {

using namespace Util_string;

// =================================================================================================
// MARK: -

template <class Rect_p, class Seq_p>
ZQ<Rect_p> spQGRect_T(const Seq_p& iSeq)
	{
	if (iSeq.Count() == 4)
		{
		if (false) {}
		else if (ZQ<Rat,false> l = sQRat(iSeq[0])) {}
		else if (ZQ<Rat,false> t = sQRat(iSeq[1])) {}
		else if (ZQ<Rat,false> r = sQRat(iSeq[2])) {}
		else if (ZQ<Rat,false> b = sQRat(iSeq[3])) {}
		else { return sRect<Rect_p>(*l, *t, *r, *b); }
		}

	if (ZMACRO_auto_(theFirstQ, sQCVec3(0, iSeq[0])))
		{
		if (ZMACRO_auto_(theSecondQ, sQCVec3(0, iSeq[1])))
			return sRect(*theFirstQ, *theFirstQ + *theSecondQ);
		else
			return sRect<Rect_p>(*theFirstQ);
		}
	return null;
	}

ZQ<GRect> sQGRect(const ZQ<Val>& iValQ)
	{
	if (iValQ)
		return sQGRect(*iValQ);
	return null;
	}

ZQ<GRect> sQGRect(const ZQ<Val_Any>& iVal_AnyQ)
	{
	if (iVal_AnyQ)
		return sQGRect(*iVal_AnyQ);
	return null;
	}

ZQ<GRect> sQGRect(const Val& iVal)
	{
	if (ZMACRO_auto_(theSeqQ, iVal.QGet<Seq>()))
		return spQGRect_T<GRect>(*theSeqQ);
	else if (ZMACRO_auto_(theSeqQ, iVal.QGet<Seq_Any>()))
		return spQGRect_T<GRect>(*theSeqQ);
	return null;
	}

ZQ<GRect> sQGRect(const Val_Any& iVal_Any)
	{
	if (ZMACRO_auto_(theSeqQ, iVal_Any.QGet<Seq>()))
		return spQGRect_T<GRect>(*theSeqQ);
	else if (ZMACRO_auto_(theSeqQ, iVal_Any.QGet<Seq_Any>()))
		return spQGRect_T<GRect>(*theSeqQ);
	return null;
	}

static ZRef<ChannerR_Bin> spChanner_Buffered(const ZRef<ChannerR_Bin>& iChanner)
	{ return sChannerR_Buffered(iChanner, 4096); }

ZRef<ChannerW_Bin> sCreateW_Truncate(const FileSpec& iFS)
	{
	if (ZRef<ChannerWPos_Bin> theChannerWPos = iFS.CreateWPos(true, true))
		{
		sSizeSet(*theChannerWPos, 0);
		sPosSet(*theChannerWPos, 0);
		return theChannerWPos;
		}
	return null;
	}

ZRef<ChannerR_Bin> sOpenR_Buffered(const FileSpec& iFS)
	{
	if (ZRef<ChannerR_Bin> theChannerR = iFS.OpenR())
		return spChanner_Buffered(theChannerR);
	return null;
	}

void sWriteBin(const Val_Any& iVal, const ChanW_Bin& w)
	{ Util_Any_JSONB::sWrite(iVal, w); }

Val_Any sReadBin(const ZRef<ChannerR_Bin>& iChannerR)
	{
	ThreadVal_NameUniquifier theNU;
	if (ZQ<Any> theQ = Util_Any_JSONB::sQRead(*iChannerR))
		return *theQ;
	return Val_Any();
	}

static ZQ<Map_Any> spQReadMap(const ZRef<ChannerRU_UTF>& iStrimmerRU)
	{
	Util_Chan_JSON::ReadOptions theRO;
	theRO.fAllowUnquotedPropertyNames = true;
	theRO.fAllowEquals = true;
	theRO.fAllowSemiColons = true;
	theRO.fAllowTerminators = true;
	theRO.fLooseSeparators = true;
	if (ZQ<Any> theQ = Util_Any_JSON::sQRead(*iStrimmerRU, theRO))
		return theQ->QGet<Map_Any>();

	return null;
	}

ZQ<Map_Any> sQReadMap_Any(const ZRef<ChannerR_Bin>& iChannerR, const string8* iName)
	{
	ThreadVal_NameUniquifier theNU;

	if (iChannerR)
		{
		const ZRef<ChannerR_Bin> buffered = spChanner_Buffered(iChannerR);

		ZRef<ChannerR_UTF> theChannerR_UTF =
			sChanner_Channer_T<ChanR_UTF_Chan_Bin_UTF8>(buffered);

		ZRef<Channer_T<ChanRU_UTF_Std>> theChannerRU_UTF = sChanner_Channer_T<ChanRU_UTF_Std>(theChannerR_UTF);

		try { return spQReadMap(theChannerRU_UTF); }
		catch (std::exception& ex)
			{
			if (ZLOGF(w, eErr))
				{
				w << "Caught exception: " << ex.what();
				if (iName)
					w << " in file: " << *iName;
				w << ", line: " << theChannerRU_UTF->GetLine() + 1;
				w << ", char: " << theChannerRU_UTF->GetColumn() + 1;
				}
			}
		}
	return null;
	}

ZQ<Map_Any> sQReadMap_Any(const ZRef<ChannerR_Bin>& iChannerR, const string8& iName)
	{ return sQReadMap_Any(iChannerR, &iName); }

ZQ<Map_Any> sQReadMap_Any(const FileSpec& iFS)
	{
	if (Util_string::sQWithoutSuffix(".zootext", iFS.Name())
		|| Util_string::sQWithoutSuffix(".txt", iFS.Name()))
		{
		if (ZRef<ChannerR_Bin> channerR = iFS.OpenR())
			return sQReadMap_Any(channerR, iFS.Name());
		}
	return null;
	}

Map_Any sReadTextData(const FileSpec& iFS)
	{
	// For now we trust FileIter to return files in alpha order. Important if we have overrides.
	Map_Any theMap;
	for (FileIter iter = iFS; iter; iter.Advance())
		{
		const string theName = iter.CurrentName();
		if (false)
			{}
		else if (ZQ<string8,false> theQ = sQWithoutSuffix(".txt", theName))
			{}
		else if (ZRef<ChannerR_Bin> channerR = iter.Current().OpenR())
			{
			// To handle multiple maps in a single file, sQReadMap_Any needs to be
			// refactored so we use the *same* buffer between invocations
			if (ZQ<Map_Any> theQ = sQReadMap_Any(channerR, theName))
				theMap = sAugmented(theMap, *theQ);
			}
		}
	return theMap;
	}


static Util_Chan_JSON::WriteOptions spWO()
	{
	Util_Chan_JSON::WriteOptions theWO = YadOptions(true);
	theWO.fUseExtendedNotation = true;
	return theWO;
	}

static ZRef<YadR> spFilter(const ZRef<YadR>& iYadR)
	{
	if (ZRef<YadAtomR> theAtom = iYadR.DynamicCast<YadAtomR>())
		{
		if (ZQ<CVec3> theQ = theAtom->AsAny().QGet<CVec3>())
			{
			Map_Any theMap;
			theMap["!"] = "CVec3";
			Seq_Any& theSeq = theMap.Mut<Seq_Any>("E");
			theSeq.Append(theQ->Get(0));
			theSeq.Append(theQ->Get(1));
			theSeq.Append(theQ->Get(2));
			return sYadR(theMap);
			}
		}
	return iYadR;
	}

void sDump(const ZStrimW& w, const Val& iVal)
	{
	if (iVal.IsNull())
		{
		w << "NULL";
		}
	else
		{
		w << "\n";
		Util_Any_JSON::sWrite(true, iVal, w);
		}
	}

void sDump(const Val& iVal)
	{
	if (ZLOGF(w, eDebug-1))
		sDump(w, iVal);
	}

void sDump(const ZStrimW& w, const Any& iAny)
	{
	w << "\n";
	Util_Any_JSON::sWrite(true, iAny, w);
	}

void sDump(const Any& iAny)
	{
	if (ZLOGF(w, eDebug-1))
		sDump(w, iAny);
	}

// =================================================================================================
// MARK: -

uint64 sNextID()
	{
	static uint64 spID;
	return ++spID;
	}

// =================================================================================================

static bool spPremultiply(ZCoord iLocationH, ZCoord iLocationV,
	ZRGBA_POD& ioColor, void* iRefcon)
	{
	const float alpha = ioColor.floatAlpha();
	ioColor.red = 65535 * (ioColor.red / 65535.0) * alpha;
	ioColor.green = 65535 * (ioColor.green / 65535.0) * alpha;
	ioColor.blue = 65535 * (ioColor.blue / 65535.0) * alpha;
	return true;
	}

ZDCPixmap sPixmap_PNG(const ZRef<ChannerR_Bin>& iChannerR)
	{
	#if ZMACRO_IOS

		if (ZRef<CGDataProviderRef> theProvider_File = CGData_Channer::sProvider(iChannerR))
			{
			if (ZRef<CGImageRef> theImageRef = sAdopt& ::CGImageCreateWithPNGDataProvider(
				theProvider_File, nullptr, true, kCGRenderingIntentDefault))
				{
				//CGImageAlphaInfo theInfo = ::CGImageGetAlphaInfo(theImageRef);
				return ZDCPixmap_CGImage::sPixmap(theImageRef);
				}
			}
	
	#else

		if (ZDCPixmap thePixmap = ZDCPixmapDecoder_PNG::sReadPixmap(*iChannerR))
			{
			if (thePixmap.GetRasterDesc().fPixvalDesc.fDepth != 32)
				{
				const ZPointPOD theSize = thePixmap.Size();
				ZDCPixmap target(theSize, ZDCPixmapNS::eFormatStandard_RGBA_32, ZRGBA(0,0,0,0));
				target.CopyFrom(sPoint<ZPointPOD>(0,0), thePixmap, sRect<ZRectPOD>(theSize));
				thePixmap = target;
				}
			thePixmap.Munge(spPremultiply, nullptr);
			return thePixmap;
			}

	#endif

	return null;
	}

} // namespace GameEngine
} // namespace ZooLib
