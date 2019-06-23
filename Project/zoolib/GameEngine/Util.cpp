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

#include "zoolib/Apple/CGData_Channer.h"
#include "zoolib/Apple/Pixmap_CGImage.h"

#include "zoolib/Pixels/Blit.h"
#include "zoolib/Pixels/Formats.h"
#include "zoolib/Pixels/PixmapCoder_PNG.h"

namespace ZooLib {
namespace GameEngine {

using namespace Util_string;

using Pixels::Ord;

// =================================================================================================
#pragma mark -

ZQ<GPoint> sQGPoint(const ZQ<Val>& iValQ)
	{
	if (iValQ)
		return sQGPoint(*iValQ);
	return null;
	}

ZQ<GPoint> sQGPoint(const Val& iVal)
	{
	if (ZQ<CVec3> theCVecQ = sQCVec3(1, iVal))
		return sCartesian(*theCVecQ);
	return null;
	}

ZQ<GPoint> sQGPoint(const ZQ<Val_Any>& iVal_AnyQ)
	{
	if (iVal_AnyQ)
		return sQGPoint(*iVal_AnyQ);
	return null;
	}

ZQ<GPoint> sQGPoint(const Val_Any& iVal_Any)
	{
	if (ZQ<CVec3> theCVecQ = sQCVec3(1, iVal_Any))
		return sCartesian(*theCVecQ);
	return null;
	}

// =================================================================================================
#pragma mark -

template <class Rect_p, class Seq_p>
ZQ<Rect_p> spQGRect_T(const Seq_p& iSeq)
	{
	if (iSeq.Count() == 4)
		{
		if (false) {}
		else if (NotQ<Rat> l = sQRat(iSeq[0])) {}
		else if (NotQ<Rat> t = sQRat(iSeq[1])) {}
		else if (NotQ<Rat> r = sQRat(iSeq[2])) {}
		else if (NotQ<Rat> b = sQRat(iSeq[3])) {}
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

// =================================================================================================
#pragma mark -

static ZP<ChannerR_Bin> spChanner_Buffered(const ZP<ChannerR_Bin>& iChanner)
	{ return sChannerR_Buffered(iChanner, 4096); }

ZP<ChannerW_Bin> sCreateW_Truncate(const FileSpec& iFS)
	{
	if (ZP<ChannerWPos_Bin> theChannerWPos = iFS.CreateWPos(true, true))
		{
		sSizeSet(*theChannerWPos, 0);
		sPosSet(*theChannerWPos, 0);
		return theChannerWPos;
		}
	return null;
	}

ZP<ChannerR_Bin> sOpenR_Buffered(const FileSpec& iFS)
	{
	if (ZP<ChannerR_Bin> theChannerR = iFS.OpenR())
		return spChanner_Buffered(theChannerR);
	return null;
	}

void sWriteBin(const Val_Any& iVal, const ChanW_Bin& w)
	{ Util_Any_JSONB::sWrite(iVal, w); }

Val_Any sReadBin(const ChanR_Bin& iChanR)
	{
	ThreadVal_NameUniquifier theNU;
	if (ZQ<Any> theQ = Util_Any_JSONB::sQRead(iChanR))
		return *theQ;
	return Val_Any();
	}

static ZQ<Map_Any> spQReadMap(const ChanRU_UTF& iChanRU)
	{
	Util_Chan_JSON::PullTextOptions_JSON theOptions;
	theOptions.fAllowUnquotedPropertyNames = true;
	theOptions.fAllowEquals = true;
	theOptions.fAllowSemiColons = true;
	theOptions.fAllowTerminators = true;
	theOptions.fLooseSeparators = true;
	if (ZQ<Any> theQ = Util_Any_JSON::sQRead(iChanRU, theOptions))
		return theQ->QGet<Map_Any>();

	return null;
	}

ZQ<Map_Any> sQReadMap_Any(const ChanR_Bin& iChanR, const string8* iName)
	{
	ThreadVal_NameUniquifier theNU;

	ChanR_XX_Buffered<ChanR_Bin> theChanR_Buffered(iChanR, 4096);
	ChanR_UTF_Chan_Bin_UTF8 theChanR_UTF(theChanR_Buffered);
	ChanRU_UTF_Std theChanRU_UTF(theChanR_UTF);

	try { return spQReadMap(theChanRU_UTF); }
	catch (std::exception& ex)
		{
		if (ZLOGF(w, eErr))
			{
			w << "Caught exception: " << ex.what();
			if (iName)
				w << " in file: " << *iName;
			w << ", line: " << theChanRU_UTF.GetLine() + 1;
			w << ", char: " << theChanRU_UTF.GetColumn() + 1;
			}
		}
	return null;
	}

ZQ<Map_Any> sQReadMap_Any(const ChanR_Bin& iChanR, const string8& iName)
	{ return sQReadMap_Any(iChanR, &iName); }

ZQ<Map_Any> sQReadMap_Any(const FileSpec& iFS)
	{
	if (Util_string::sQWithoutSuffix(".zootext", iFS.Name())
		|| Util_string::sQWithoutSuffix(".txt", iFS.Name()))
		{
		if (ZP<ChannerR_Bin> channerR = iFS.OpenR())
			return sQReadMap_Any(*channerR, iFS.Name());
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
		else if (iFS.Name().empty() || ispunct(iFS.Name().at(0)))
			{}
		else
			{
			ZQ<string8> theQ = sQWithoutSuffix(".txt", theName);

			if (not theQ)
				theQ = sQWithoutSuffix(".vals", theName);

			if (theQ)
				{
				if (ZP<ChannerR_Bin> channerR = iter.Current().OpenR())
					{
					// To handle multiple maps in a single file, sQReadMap_Any needs to be
					// refactored so we use the *same* buffer between invocations
					if (ZQ<Map_Any> theQ = sQReadMap_Any(*channerR, theName))
						theMap = sAugmented(theMap, *theQ);
					}
				}
			}
		}
	return theMap;
	}

void sDump(const ChanW_UTF& w, const Val& iVal)
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

void sDump(const ChanW_UTF& w, const Any& iAny)
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
#pragma mark -

uint64 sNextID()
	{
	static uint64 spID;
	return ++spID;
	}

// =================================================================================================

#if ZMACRO_IOS
#else
static bool spPremultiply(Ord iH, Ord iV, RGBA& ioColor, void* iRefcon)
	{
	sRed(ioColor) *= sAlpha(ioColor);
	sGreen(ioColor) *= sAlpha(ioColor);
	sBlue(ioColor) *= sAlpha(ioColor);
	return true;
	}
#endif

Pixmap sPixmap_PNG(const ZP<ChannerR_Bin>& iChannerR)
	{
	#if ZMACRO_IOS

		if (ZP<CGDataProviderRef> theProvider_File = CGData_Channer::sProvider(iChannerR))
			{
			if (ZP<CGImageRef> theImageRef = sAdopt& ::CGImageCreateWithPNGDataProvider(
				theProvider_File, nullptr, true, kCGRenderingIntentDefault))
				{
				//CGImageAlphaInfo theInfo = ::CGImageGetAlphaInfo(theImageRef);
				return sPixmap(theImageRef);
				}
			}
	
	#else

		if (Pixmap thePixmap = Pixels::sReadPixmap_PNG(*iChannerR))
			{
			if (thePixmap.GetRasterDesc().fPixvalDesc.fDepth != 32)
				{
				const PointPOD theSize = thePixmap.Size();
				Pixmap target = sPixmap(theSize, Pixels::EFormatStandard::RGBA_32, sRGBA(0,0,0,0));
				sBlit(thePixmap, sRect<RectPOD>(theSize), target, sPointPOD(0,0));
				thePixmap = target;
				}
			sMunge(thePixmap, spPremultiply, nullptr);
			return thePixmap;
			}

	#endif

	return null;
	}

} // namespace GameEngine
} // namespace ZooLib
