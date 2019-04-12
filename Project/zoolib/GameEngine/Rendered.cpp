#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

using std::vector;

// =================================================================================================
#pragma mark - Rendered

Rendered::Rendered()
	{}

void Rendered::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Rendered* theVisitor = sDynNonConst<Visitor_Rendered>(&iVisitor))
		this->Accept_Rendered(*theVisitor);
	else
		Visitee::Accept(iVisitor);
	}

void Rendered::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered(this); }

// =================================================================================================
#pragma mark - Rendered_BlushGainMat

namespace {

SafePtrStack_WithDestroyer<Rendered_BlushGainMat,SafePtrStackLink_Rendered_BlushGainMat>
	spSafePtrStack_BlushGainMat;

} // anonymous namespace

Rendered_BlushGainMat::Rendered_BlushGainMat(const BlushGainMat& iBlushGainMat,
	const ZRef<Rendered>& iRendered)
:	fBlushGainMat(iBlushGainMat)
,	fRendered(iRendered)
	{}

void Rendered_BlushGainMat::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fRendered.Clear();
	
	spSafePtrStack_BlushGainMat.Push(this);
	}

void Rendered_BlushGainMat::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_BlushGainMat(this); }

const BlushGainMat& Rendered_BlushGainMat::GetBlushGainMat()
	{ return fBlushGainMat; }

const ZRef<Rendered>& Rendered_BlushGainMat::GetRendered()
	{ return fRendered; }

BlushMat Rendered_BlushGainMat::GetBlushMat()
	{ return sBlushMat(fBlushGainMat); }

ZRef<Rendered_BlushGainMat> Rendered_BlushGainMat::spMake(const BlushGainMat& iBlushGainMat,
	const ZRef<Rendered>& iRendered)
	{
	if (Rendered_BlushGainMat* result =
		spSafePtrStack_BlushGainMat.PopIfNotEmpty<Rendered_BlushGainMat>())
		{
		result->fBlushGainMat = iBlushGainMat;
		result->fRendered = iRendered;
		return result;
		}

	return new Rendered_BlushGainMat(iBlushGainMat, iRendered);
	}

ZRef<Rendered> sRendered_BlushGainMat(
	const BlushGainMat& iBlushGainMat, const ZRef<Rendered>& iRendered)
	{ return Rendered_BlushGainMat::spMake(iBlushGainMat, iRendered); }

// =================================================================================================
#pragma mark - Rendered_Buffer

Rendered_Buffer::Rendered_Buffer(int iWidth, int iHeight, const RGBA& iFill,
	const ZRef<Rendered>& iRendered)
:	fWidth(iWidth)
,	fHeight(iHeight)
,	fFill(iFill)
,	fRendered(iRendered)
	{}

void Rendered_Buffer::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Buffer(this); }

int Rendered_Buffer::GetWidth()
	{ return fWidth; }

int Rendered_Buffer::GetHeight()
	{ return fHeight; }

RGBA Rendered_Buffer::GetFill()
	{ return fFill; }

const ZRef<Rendered>& Rendered_Buffer::GetRendered()
	{ return fRendered; }

ZRef<Rendered_Buffer> sRendered_Buffer(int iWidth, int iHeight, const RGBA& iFill,
	const ZRef<Rendered>& iRendered)
	{ return new Rendered_Buffer(iWidth, iHeight, iFill, iRendered); }

// =================================================================================================
#pragma mark - Rendered_Cel

Rendered_Cel::Rendered_Cel(const Cel& iCel)
:	fCel(iCel)
	{}

void Rendered_Cel::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Cel(this); }

const Cel& Rendered_Cel::GetCel()
	{ return fCel; }

ZRef<Rendered_Cel> sRendered_Cel(const Cel& iCel)
	{ return new Rendered_Cel(iCel); }

// =================================================================================================
#pragma mark - Rendered_Group

namespace { // anonymous

SafePtrStack_WithDestroyer<Rendered_Group,SafePtrStackLink_Rendered_Group> spSafePtrStack_Group;

} // anonymous namespace

Rendered_Group::Rendered_Group()
	{}

void Rendered_Group::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fChildren.clear();
	
	spSafePtrStack_Group.Push(this);
	}

void Rendered_Group::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Group(this); }

const vector<ZRef<Rendered> >& Rendered_Group::GetChildren()
	{ return fChildren; }

void Rendered_Group::Append(const ZRef<Rendered>& iRendered)
	{ fChildren.push_back(iRendered); }

ZRef<Rendered_Group> Rendered_Group::spMake()
	{
	if (Rendered_Group* result = spSafePtrStack_Group.PopIfNotEmpty<Rendered_Group>())
		return result;

	return new Rendered_Group;
	}

ZRef<Rendered_Group> sRendered_Group()
	{ return Rendered_Group::spMake(); }

// =================================================================================================
#pragma mark - Rendered_Line

Rendered_Line::Rendered_Line(const GPoint& iP0, const GPoint& iP1, Rat iWidth)
:	fP0(iP0)
,	fP1(iP1)
,	fWidth(iWidth)
	{}

void Rendered_Line::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Line(this); }

void Rendered_Line::Get(GPoint& oP0, GPoint& oP1, Rat& oWidth)
	{
	oP0 = fP0;
	oP1 = fP1;
	oWidth = fWidth;
	}

ZRef<Rendered> sRendered_Line(const GPoint& iP0, const GPoint& iP1, Rat iWidth)
	{ return new Rendered_Line(iP0, iP1, iWidth); }

ZRef<Rendered> sRendered_Line(const RGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, Rat iWidth)
	{ return sRendered_BlushGainMat(iRGBA, sRendered_Line(iP0, iP1, iWidth)); }

// =================================================================================================
#pragma mark - Rendered_Rect

Rendered_Rect::Rendered_Rect(const GRect& iBounds)
:	fBounds(iBounds)
	{}

void Rendered_Rect::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Rect(this); }

void Rendered_Rect::Get(GRect& oBounds)
	{
	oBounds = fBounds;
	}

ZRef<Rendered> sRendered_Rect(const GRect& iBounds)
	{ return new Rendered_Rect(iBounds); }

ZRef<Rendered> sRendered_Rect(const RGBA& iRGBA, const GRect& iBounds)
	{ return sRendered_BlushGainMat(iRGBA, new Rendered_Rect(iBounds)); }

// =================================================================================================
#pragma mark - Rendered_RightAngleSegment

Rendered_RightAngleSegment::Rendered_RightAngleSegment(
	const RGBA& iRGBA_Convex, const RGBA& iRGBA_Concave)
:	fRGBA_Convex(iRGBA_Convex)
,	fRGBA_Concave(iRGBA_Concave)
	{}

void Rendered_RightAngleSegment::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_RightAngleSegment(this); }

void Rendered_RightAngleSegment::Get(RGBA& oRGBA_Convex, RGBA& oRGBA_Concave)
	{
	oRGBA_Convex = fRGBA_Convex;
	oRGBA_Concave = fRGBA_Concave;
	}

ZRef<Rendered> sRendered_RightAngleSegment(
	const RGBA& iRGBA_Convex, const RGBA& iRGBA_Concave)
	{ return new Rendered_RightAngleSegment(iRGBA_Convex, iRGBA_Concave); }

// =================================================================================================
#pragma mark - Rendered_Sound

Rendered_Sound::Rendered_Sound(const ZRef<Sound>& iSound)
:	fSound(iSound)
	{}

const ZRef<Sound>& Rendered_Sound::GetSound()
	{ return fSound; }

void Rendered_Sound::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Sound(this); }

// =================================================================================================
#pragma mark - Rendered_String

Rendered_String::Rendered_String(const FontSpec& iFontSpec, const string8& iString)
:	fFontSpec(iFontSpec)
,	fString(iString)
	{}

void Rendered_String::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_String(this); }

const FontSpec& Rendered_String::GetFontSpec()
	{ return fFontSpec; }

const string8& Rendered_String::GetString()
	{ return fString; }

ZRef<Rendered> sRendered_String(const FontSpec& iFontSpec, const string8& iString)
	{ return new Rendered_String(iFontSpec, iString); }

ZRef<Rendered> sRendered_String(const RGBA& iRGBA, const FontSpec& iFontSpec, const string8& iString)
	{ return sRendered_BlushGainMat(iRGBA, sRendered_String(iFontSpec, iString)); }

// =================================================================================================
#pragma mark - Rendered_Texture

namespace {

SafePtrStack_WithDestroyer<Rendered_Texture,SafePtrStackLink_Rendered_Texture>
	spSafePtrStack_Texture;

} // anonymous namespace

Rendered_Texture::Rendered_Texture(const ZRef<Texture>& iTexture, const GRect& iBounds)
:	fTexture(iTexture)
,	fBounds(iBounds)
	{}

void Rendered_Texture::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fTexture.Clear();
	
	spSafePtrStack_Texture.Push(this);
	}

void Rendered_Texture::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Texture(this); }

const ZRef<Texture>& Rendered_Texture::GetTexture()
	{ return fTexture; }

const GRect& Rendered_Texture::GetBounds()
	{ return fBounds; }

ZRef<Rendered_Texture> Rendered_Texture::spMake(
	const ZRef<Texture>& iTexture, const GRect& iBounds)
	{
	if (Rendered_Texture* result = spSafePtrStack_Texture.PopIfNotEmpty<Rendered_Texture>())
		{
		result->fTexture = iTexture;
		result->fBounds = iBounds;
		return result;
		}

	return new Rendered_Texture(iTexture, iBounds);
	}

ZRef<Rendered_Texture> sRendered_Texture(const ZRef<Texture>& iTexture, const GRect& iBounds)
	{ return Rendered_Texture::spMake(iTexture, iBounds); }

// =================================================================================================
#pragma mark - Rendered_Triangle

Rendered_Triangle::Rendered_Triangle(const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
:	fP0(iP0)
,	fP1(iP1)
,	fP2(iP2)
	{}

void Rendered_Triangle::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Triangle(this); }

void Rendered_Triangle::Get(GPoint& oP0, GPoint& oP1, GPoint& oP2)
	{
	oP0 = fP0;
	oP1 = fP1;
	oP2 = fP2;
	}

ZRef<Rendered> sRendered_Triangle(const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{ return new Rendered_Triangle(iP0, iP1, iP2); }

ZRef<Rendered> sRendered_Triangle(
	const RGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{ return sRendered_BlushGainMat(iRGBA, sRendered_Triangle(iP0, iP1, iP2)); }

// =================================================================================================
#pragma mark - Visitor_Rendered

void Visitor_Rendered::Visit_Rendered(const ZRef<Rendered>& iRendered)
	{ this->Visit(iRendered); }

void Visitor_Rendered::Visit_Rendered_BlushGainMat(
	const ZRef<Rendered_BlushGainMat>& iRendered_BlushGainMat)
	{ this->Visit_Rendered(iRendered_BlushGainMat); }

void Visitor_Rendered::Visit_Rendered_Buffer(const ZRef<Rendered_Buffer>& iRendered_Buffer)
	{ this->Visit_Rendered(iRendered_Buffer); }

void Visitor_Rendered::Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel)
	{ this->Visit_Rendered(iRendered_Cel); }

void Visitor_Rendered::Visit_Rendered_Group(const ZRef<Rendered_Group>& iRendered_Group)
	{ this->Visit_Rendered(iRendered_Group); }

void Visitor_Rendered::Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line)
	{ this->Visit_Rendered(iRendered_Line); }

void Visitor_Rendered::Visit_Rendered_Rect(const ZRef<Rendered_Rect>& iRendered_Rect)
	{ this->Visit_Rendered(iRendered_Rect); }

void Visitor_Rendered::Visit_Rendered_RightAngleSegment(
	const ZRef<Rendered_RightAngleSegment>& iRendered_RightAngleSegment)
	{ this->Visit_Rendered(iRendered_RightAngleSegment); }

void Visitor_Rendered::Visit_Rendered_Sound(const ZRef<Rendered_Sound>& iRendered_Sound)
	{ this->Visit_Rendered(iRendered_Sound); }

void Visitor_Rendered::Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String)
	{ this->Visit_Rendered(iRendered_String); }

void Visitor_Rendered::Visit_Rendered_Texture(const ZRef<Rendered_Texture>& iRendered_Texture)
	{ this->Visit_Rendered(iRendered_Texture); }

void Visitor_Rendered::Visit_Rendered_Triangle(const ZRef<Rendered_Triangle>& iRendered_Triangle)
	{ this->Visit_Rendered(iRendered_Triangle); }

// =================================================================================================
#pragma mark -

ZRef<Rendered> sFrontmost(const ZRef<Rendered>& iRendered)
	{ return sRendered_BlushGainMat(sTranslate3Z<Rat>(128), iRendered); }

} // namespace GameEngine
} // namespace ZooLib
