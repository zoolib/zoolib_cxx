#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace GameEngine {

using std::vector;

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateAlphaGainMat

Visitor_Rendered_AccumulateAlphaGainMat::Visitor_Rendered_AccumulateAlphaGainMat()
	{}

Visitor_Rendered_AccumulateAlphaGainMat::Visitor_Rendered_AccumulateAlphaGainMat(
	const AlphaGainMat& iAlphaGainMat)
:	fAlphaGainMat(iAlphaGainMat)
	{}

void Visitor_Rendered_AccumulateAlphaGainMat::Visit_Rendered_AlphaGainMat(
	const ZRef<Rendered_AlphaGainMat>& iRendered_AlphaGainMat)
	{
	SaveSetRestore<AlphaGainMat> theSSR(
		fAlphaGainMat, fAlphaGainMat * iRendered_AlphaGainMat->GetAlphaGainMat());

	if (ZRef<Rendered> theRendered = iRendered_AlphaGainMat->GetRendered())
		theRendered->Accept_Rendered(*this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeCel

Visitor_Rendered_DecomposeCel::Visitor_Rendered_DecomposeCel(
	const ZRef<AssetCatalog>& iAssetCatalog, bool iShowNameFrame)
:	fAssetCatalog(iAssetCatalog)
,	fShowNameFrame(iShowNameFrame)
	{}

void Visitor_Rendered_DecomposeCel::Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel)
	{
	const Cel& theCel = iRendered_Cel->GetCel();

	vector<Texture_BoundsQ_Mat> theTBMs;
	
	fAssetCatalog->Get(theCel.fNameFrame, theTBMs);

	foreachv (const Texture_BoundsQ_Mat& theTBM, theTBMs)
		{
		GRect theBounds;
		if (theTBM.fBoundsQ)
			theBounds = *theTBM.fBoundsQ;
		else
			theBounds = sRect<GRect>(theTBM.fTexture->GetDrawnSize());
		LT(theBounds) += LT(theTBM.fInset);
		RB(theBounds) -= RB(theTBM.fInset);

		ZRef<Rendered> theRendered = sRendered_Texture(theTBM.fTexture, theBounds);
		theRendered = sRendered_AlphaGainMat(
			AlphaGainMat(theCel.fAlphaMat.fAlpha, theCel.fAlphaMat.fMat * theTBM.fMat),
			theRendered);
		theRendered->Accept(*this);
		}

	if (fShowNameFrame)
		{
		// Box at origin
		sRef(new Rendered_Rect(ZRGBA::sWhite, sGRect(0,0,2,2)))->Accept_Rendered(*this);
		
		string8 theString8;
		ChanW_UTF_string8(&theString8)
			<< theCel.fNameFrame.fName << "/" << theCel.fNameFrame.fFrame;

		sRef(new Rendered_String(ZRGBA::sWhite, theString8))->Accept(*this);
		}
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeGroup

void Visitor_Rendered_DecomposeGroup::Visit_Rendered_Group(
	const ZRef<Rendered_Group>& iRendered_Group)
	{
	foreacha (entry, iRendered_Group->GetChildren())
		entry->Accept_Rendered(*this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_LineToRect

void Visitor_Rendered_LineToRect::Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line)
	{
	ZRGBA theRGBA;
	GPoint theP0, theP1;
	Rat theWidth;
	iRendered_Line->Get(theRGBA, theP0, theP1, theWidth);

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

	ZRef<Rendered> theRendered = new Rendered_Rect(theRGBA, theRect);
	theRendered = sRendered_AlphaGainMat(theMat, theRendered);
	theRendered->Accept(*this);
	}

} // namespace GameEngine
} // namespace ZooLib
