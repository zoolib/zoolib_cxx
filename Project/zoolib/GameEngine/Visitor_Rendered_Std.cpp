#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace GameEngine {

using std::vector;

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateBlush

Visitor_Rendered_AccumulateBlush::Visitor_Rendered_AccumulateBlush()
:	fBlush(1)
	{}

void Visitor_Rendered_AccumulateBlush::Visit_Rendered_Blush(
	const ZRef<Rendered_Blush>& iRendered_Blush)
	{
	SaveSetRestore<Blush> theSSR(fBlush, fBlush * iRendered_Blush->GetBlush());

	if (ZRef<Rendered> theRendered = iRendered_Blush->GetRendered())
		theRendered->Accept_Rendered(*this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateGain

Visitor_Rendered_AccumulateGain::Visitor_Rendered_AccumulateGain()
:	fGain(1)
	{}

void Visitor_Rendered_AccumulateGain::Visit_Rendered_Gain(
	const ZRef<Rendered_Gain>& iRendered_Gain)
	{
	SaveSetRestore<Gain> theSSR(fGain, fGain * iRendered_Gain->GetGain());

	if (ZRef<Rendered> theRendered = iRendered_Gain->GetRendered())
		theRendered->Accept_Rendered(*this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateMat

Visitor_Rendered_AccumulateMat::Visitor_Rendered_AccumulateMat()
:	fMat(1)
	{}

Visitor_Rendered_AccumulateMat::Visitor_Rendered_AccumulateMat(const Mat& iMat)
:	fMat(iMat)
	{}

void Visitor_Rendered_AccumulateMat::Visit_Rendered_Mat(const ZRef<Rendered_Mat>& iRendered_Mat)
	{
	SaveSetRestore<Mat> theSSR(fMat, fMat * iRendered_Mat->GetMat());

	if (ZRef<Rendered> theRendered = iRendered_Mat->GetRendered())
		theRendered->Accept_Rendered(*this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_CelToTextures

Visitor_Rendered_CelToTextures::Visitor_Rendered_CelToTextures(
	const ZRef<AssetCatalog>& iAssetCatalog, bool iShowNameFrame)
:	fAssetCatalog(iAssetCatalog)
,	fShowNameFrame(iShowNameFrame)
	{}

void Visitor_Rendered_CelToTextures::Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel)
	{
	const Cel& theCel = iRendered_Cel->GetCel();

	vector<Texture_BoundsQ_Mat> theTBMs;
	
	fAssetCatalog->Get(theCel.fNameFrame, theTBMs);

	ZRef<Rendered_Group> theGroup = sRendered_Group();
	foreachv (const Texture_BoundsQ_Mat& theTBM, theTBMs)
		{
		GRect theBounds;
		if (theTBM.fBoundsQ)
			theBounds = *theTBM.fBoundsQ;
		else
			theBounds = sRect<GRect>(theTBM.fTexture->GetDrawnSize());
		LT(theBounds) += LT(theTBM.fInset);
		RB(theBounds) -= RB(theTBM.fInset);

		theGroup->Append(sRendered_Mat(theTBM.fMat, sRendered_Texture(theTBM.fTexture, theBounds)));
		}

	ZRef<Rendered> theRendered =
		sRendered_Blush(theCel.fBlushMat.fBlush,
			sRendered_Mat(theCel.fBlushMat.fMat, theGroup));

	sAccept(theRendered, *this);

	if (fShowNameFrame)
		{
		// Box at origin
		sAccept(sRendered_Rect(sRGBA(1,1), sGRect(0,0,2,2)), *this);
		
		string8 theString8;
		ChanW_UTF_string8(&theString8)
			<< theCel.fNameFrame.fName << "/" << theCel.fNameFrame.fFrame;

		sAccept(sRendered_String(RGBA(1), FontSpec(), theString8), *this);
		}
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeGroup

void Visitor_Rendered_DecomposeGroup::Visit_Rendered_Group(
	const ZRef<Rendered_Group>& iRendered_Group)
	{
	foreacha (entry, iRendered_Group->GetChildren())
		sAccept(entry, *this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_LineToRect

void Visitor_Rendered_LineToRect::Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line)
	{
	GPoint theP0, theP1;
	Rat theWidth;
	iRendered_Line->Get(theP0, theP1, theWidth);

	// Build a rectangle
	const GPoint delta = theP1 - theP0;

	const Rat halfWidth = theWidth / 2;

	const Rat theLength = sqrt(X(delta) * X(delta) + Y(delta) * Y(delta));

	const GRect theRect = sRect<GRect>(-halfWidth, 0, halfWidth, theLength);

	Mat theMat(1);

	// Offset to align with P0
	// w/o translate, the lines will be arranged around topleft (I think)
	theMat *= sTranslate3<Rat>(X(theP0), Y(theP0), 0);

	const Rat s = -X(delta)/theLength;
	const Rat c = Y(delta)/theLength;

	// Rotate it appropriately
	Mat theRot;
	theRot[0][0] = c;
	theRot[0][1] = s;
	theRot[1][0] = -s;
	theRot[1][1] = c;
	theRot[2][2] = 1;
	theRot[3][3] = 1;
	theMat *= theRot;

	sAccept(sRendered_Mat(theMat, sRendered_Rect(theRect)), *this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_StringToTextures

Visitor_Rendered_StringToTextures::Visitor_Rendered_StringToTextures(
	const ZRef<FontCatalog>& iFontCatalog)
:	fFontCatalog(iFontCatalog)
	{}

void Visitor_Rendered_StringToTextures::Visit_Rendered_String(
	const ZRef<Rendered_String>& iRendered_String)
	{
	if (ZRef<FontStrike> theStrike = fFontCatalog->GetFontStrike(iRendered_String->GetFontSpec()))
		{
		ZRef<Rendered_Group> theGroup = sRendered_Group();

		const string8& theString = iRendered_String->GetString();

		Mat theMat(1);
		for (string8::const_iterator iter = theString.begin(), end = theString.end();
			/*no test*/; /*no inc*/)
			{
			UTF32 theCP;
			if (not Unicode::sReadInc(iter, end, theCP))
				break;

			GRect glyphBounds;
			GPoint glyphOffset;
			Rat xAdvance;
			if (ZRef<Texture> theTexture = theStrike->GetGlyphTexture(theCP,
				glyphBounds, glyphOffset, xAdvance))
				{
				theGroup->Append(
					sRendered_Mat(sTranslate3XY(X(glyphOffset), Y(glyphOffset)) * theMat,
						sRendered_Texture(theTexture, glyphBounds)));

				theMat = sTranslate3X(xAdvance) * theMat;
				}
			}

		sAccept(theGroup, *this);
		}
	}

} // namespace GameEngine
} // namespace ZooLib
