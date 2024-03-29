// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Types.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Coerce_Any.h" // For sQCoerceRat and sQCoerceInt
#include "zoolib/Util_Chan_UTF.h"

#include <stdlib.h> // For arc4random

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - NameFrame

NameFrame::NameFrame()
:	fFrame(0)
	{}

NameFrame::NameFrame(const Name& iName)
:	fName(iName)
,	fFrame(0)
	{}

NameFrame::NameFrame(const Name& iName, size_t iFrame)
:	fName(iName)
,	fFrame(iFrame)
	{}

// =================================================================================================
#pragma mark - Cel

Cel::Cel()
	{}

Cel::Cel(const NameFrame& iNameFrame, const BlushMat& iBlushMat)
:	fNameFrame(iNameFrame)
,	fBlushMat(iBlushMat)
	{}

// =================================================================================================
#pragma mark - Map Get

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0)
	{ return iMap.QGet(iName0); }

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0, const Name& iName1)
	{
	if (ZQ<Val> theQ = sQGetNamed(iMap, iName0))
		return *theQ;
	return iMap.QGet(iName1);
	}

ZQ<Val> sQGetNamed(const Map& iMap, const Name& iName0, const Name& iName1, const Name& iName2)
	{
	if (ZQ<Val> theQ = sQGetNamed(iMap, iName0, iName1))
		return *theQ;
	return iMap.QGet(iName2);
	}

Val sGetNamed(const Map& iMap, const Name& iName0, const Name& iName1)
	{
	if (ZQ<Val> theQ = sQGetNamed(iMap, iName0, iName1))
		return *theQ;
	return Val();
	}

// =================================================================================================
#pragma mark - Rat, from Any

ZQ<Rat> sQRat(const AnyBase& iAnyBase)
	{
	if (ZQ<double> theQ = sQCoerceRat(iAnyBase))
		return *theQ;
	else if (ZQ<int64> theQ = sQCoerceInt(iAnyBase))
		return *theQ;
	return null;
	}

Rat sRat(const AnyBase& iAnyBase)
	{
	if (ZQ<Rat> theQ = sQRat(iAnyBase))
		return *theQ;
	return 0;
	}

// =================================================================================================
#pragma mark - Rat, from ZQ<Val>

ZQ<Rat> sQRat(const ZQ<Val>& iValQ)
	{
	if (iValQ)
		return sQRat(*iValQ);
	return null;
	}

Rat sRat(const ZQ<Val>& iValQ)
	{
	if (ZQ<Rat> theQ = sQRat(iValQ))
		return *theQ;
	return 0;
	}

// =================================================================================================
#pragma mark - Coerce, CVec

ZQ<CVec3> sQCVec3(Rat iIdentity, const AnyBase& iAnyBase)
	{
	if (const CVec3* theCVecP = iAnyBase.PGet<CVec3>())
		return *theCVecP;

	if (const Seq* theSeqP = iAnyBase.PGet<Seq>())
		{
		CVec3 result = sCVec3<>(iIdentity);
		if (theSeqP->Count() > 0)
			{
			if (ZQ<Rat> theQ = sQRat(theSeqP->Get(0)))
				result[0] = *theQ;
			else
				return null;
			}
		if (theSeqP->Count() > 1)
			{
			if (ZQ<Rat> theQ = sQRat(theSeqP->Get(1)))
				result[1] = *theQ;
			else
				return null;
			}
		if (theSeqP->Count() > 2)
			{
			if (ZQ<Rat> theQ = sQRat(theSeqP->Get(2)))
				result[2] = *theQ;
			else
				return null;
			}
		return result;
		}

	if (const Seq_ZZ* theSeqP = iAnyBase.PGet<Seq_ZZ>())
		{
		CVec3 result(iIdentity);
		if (theSeqP->Count() > 0)
			{
			if (ZQ<Rat> theQ = sQRat(theSeqP->Get(0)))
				result[0] = *theQ;
			else
				return null;
			}
		if (theSeqP->Count() > 1)
			{
			if (ZQ<Rat> theQ = sQRat(theSeqP->Get(1)))
				result[1] = *theQ;
			else
				return null;
			}
		if (theSeqP->Count() > 2)
			{
			if (ZQ<Rat> theQ = sQRat(theSeqP->Get(2)))
				result[2] = *theQ;
			else
				return null;
			}
		return result;
		}

	if (ZQ<Rat> theQ = sQRat(iAnyBase))
		return sCVec3<Rat>(*theQ, iIdentity, iIdentity);

	return null;
	}

CVec3 sCVec3(Rat iIdentity, const AnyBase& iAnyBase)
	{
	if (ZQ<CVec3> theQ = sQCVec3(iIdentity, iAnyBase))
		return *theQ;
	return CVec3(iIdentity);
	}

ZQ<CVec3> sQCVec3(Rat iIdentity, const ZQ<Val>& iValQ)
	{
	if (iValQ)
		return sQCVec3(iIdentity, *iValQ);
	return null;
	}

CVec3 sCVec3(Rat iIdentity, const ZQ<Val>& iValQ)
	{
	if (iValQ)
		return sCVec3(iIdentity, *iValQ);
	return sCVec3(iIdentity);
	}

CVec3 sCVec3(Rat iX, Rat iY, Rat iZ)
	{ return sCVec3<Rat>(iX, iY, iZ); }

// =================================================================================================
#pragma mark - RGBA

ZQ<RGBA> sQRGBA(const string8& iString)
	{
	using namespace std;
	using namespace Util_Chan;

	ChanRU_UTF_string8 theChan(iString);
	vector<float> comps;
	for (;;)
		{
		if (NotQ<int> high = sQRead_HexDigit(theChan))
			break;
		else if (NotQ<int> low = sQRead_HexDigit(theChan))
			break;
		else
			comps.push_back(((*high<<4) | *low) / 255.0);
		}

	if (comps.size() == 1) // Gray
		return sRGBA(comps[0]);

	if (comps.size() == 2) // Gray, Alpha
		return sRGBA(comps[0], comps[1]);

	if (comps.size() == 3) // RGB
		return sRGBA(comps[0], comps[1], comps[2]);

	if (comps.size() == 4) // RGBA
		return sRGBA(comps[0], comps[1], comps[2], comps[3]);

	return null;
	}

ZQ<RGBA> sQRGBA(const AnyBase& iAnyBase)
	{
	if (const RGBA* theRGBAP = sPGet<RGBA>(iAnyBase))
		return *theRGBAP;

	if (const string8* theStringP = sPGet<string8>(iAnyBase))
		return sQRGBA(*theStringP);

	return null;
	}

ZQ<RGBA> sQRGBA(const ZQ<Val>& iValQ)
	{
	if (iValQ)
		return sQRGBA(*iValQ);
	return null;
	}

// =================================================================================================
#pragma mark - sRandomInRange

double sRandomInRange(double iMin, double iMax)
	{ return (double(arc4random()) / 0x100000000) * (iMax - iMin) + iMin; }

} // namespace GameEngine
} // namespace ZooLib
