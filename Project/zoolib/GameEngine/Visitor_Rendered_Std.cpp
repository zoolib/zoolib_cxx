#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace GameEngine {

using std::vector;

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateBlushGainMat

Visitor_Rendered_AccumulateBlushGainMat::Visitor_Rendered_AccumulateBlushGainMat()
	{}

Visitor_Rendered_AccumulateBlushGainMat::Visitor_Rendered_AccumulateBlushGainMat(
	const BlushGainMat& iBlushGainMat)
:	fBlushGainMat(iBlushGainMat)
	{}

void Visitor_Rendered_AccumulateBlushGainMat::Visit_Rendered_BlushGainMat(
	const ZRef<Rendered_BlushGainMat>& iRendered_BlushGainMat)
	{
	SaveSetRestore<BlushGainMat> theSSR(
		fBlushGainMat, fBlushGainMat * iRendered_BlushGainMat->GetBlushGainMat());

	if (ZRef<Rendered> theRendered = iRendered_BlushGainMat->GetRendered())
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
		theRendered = sRendered_BlushGainMat(
			theCel.fBlushMat * theTBM.fMat,
			theRendered);
		sAccept(theRendered, *this);
		}

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
		entry->Accept_Rendered(*this);
	}

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeString

Visitor_Rendered_DecomposeString::Visitor_Rendered_DecomposeString(
	const ZRef<FontCatalog>& iFontCatalog)
:	fFontCatalog(iFontCatalog)
	{}

void Visitor_Rendered_DecomposeString::Visit_Rendered_String(
	const ZRef<Rendered_String>& iRendered_String)
	{
	// Turn iRendered_String into a bunch of textures etc.
	const FontSpec theFontSpec = iRendered_String->GetFontSpec();
	const string8& theString = iRendered_String->GetString();

//	Given the FontSpec and the string, we ask the font catalog for a Texture, a list
//	of bounding rectangles and post-draw offsets, and a list of indices.
//	The textures in this case will be Texture_GL, with the "useOnlyAlpha flag set".
//
//	Then we can turn that into a suite of BGMs and Textured.

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

	sAccept(sRendered_BlushGainMat(theMat, sRendered_Rect(theRect)), *this);
	}

} // namespace GameEngine
} // namespace ZooLib
