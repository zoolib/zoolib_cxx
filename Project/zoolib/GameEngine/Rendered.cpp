#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

using std::vector;

// =================================================================================================
// MARK: - Rendered

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
// MARK: - Rendered_AlphaGainMat

namespace {

SafePtrStack_WithDestroyer<Rendered_AlphaGainMat,SafePtrStackLink_Rendered_AlphaGainMat>
	spSafePtrStack_AlphaGainMat;

} // anonymous namespace

Rendered_AlphaGainMat::Rendered_AlphaGainMat()
	{}

Rendered_AlphaGainMat::Rendered_AlphaGainMat(
	const AlphaGainMat& iAlphaGainMat, const ZRef<Rendered>& iRendered)
:	fAlphaGainMat(iAlphaGainMat)
,	fRendered(iRendered)
	{}

void Rendered_AlphaGainMat::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fRendered.Clear();
	
	spSafePtrStack_AlphaGainMat.Push(this);
	}

void Rendered_AlphaGainMat::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_AlphaGainMat(this); }

const AlphaGainMat& Rendered_AlphaGainMat::GetAlphaGainMat()
	{ return fAlphaGainMat; }

const ZRef<Rendered>& Rendered_AlphaGainMat::GetRendered()
	{ return fRendered; }

AlphaMat Rendered_AlphaGainMat::GetAlphaMat()
	{ return AlphaMat(fAlphaGainMat.fAlpha, fAlphaGainMat.fMat); }

ZRef<Rendered_AlphaGainMat> Rendered_AlphaGainMat::spMake(
	const AlphaGainMat& iAlphaGainMat, const ZRef<Rendered>& iRendered)
	{
	if (Rendered_AlphaGainMat* result =
		spSafePtrStack_AlphaGainMat.PopIfNotEmpty<Rendered_AlphaGainMat>())
		{
		result->fAlphaGainMat = iAlphaGainMat;
		result->fRendered = iRendered;
		return result;
		}

	return new Rendered_AlphaGainMat(iAlphaGainMat, iRendered);
	}

ZRef<Rendered_AlphaGainMat> sRendered_AlphaGainMat(
	const AlphaGainMat& iAlphaGainMat, const ZRef<Rendered>& iRendered)
	{ return Rendered_AlphaGainMat::spMake(iAlphaGainMat, iRendered); }

// =================================================================================================
// MARK: - Rendered_Buffer

Rendered_Buffer::Rendered_Buffer(
	int iWidth, int iHeight, const ZRGBA& iRGBA, const ZRef<Rendered>& iRendered)
:	fWidth(iWidth)
,	fHeight(iHeight)
,	fRGBA(iRGBA)
,	fRendered(iRendered)
	{}

void Rendered_Buffer::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Buffer(this); }

int Rendered_Buffer::GetWidth()
	{ return fWidth; }

int Rendered_Buffer::GetHeight()
	{ return fHeight; }

ZRGBA Rendered_Buffer::GetRGBA()
	{ return fRGBA; }

const ZRef<Rendered>& Rendered_Buffer::GetRendered()
	{ return fRendered; }

ZRef<Rendered_Buffer> sRendered_Buffer(
	int iWidth, int iHeight, const ZRGBA& iRGBA, const ZRef<Rendered>& iRendered)
	{ return new Rendered_Buffer(iWidth, iHeight, iRGBA, iRendered); }

// =================================================================================================
// MARK: - Rendered_Cel

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
// MARK: - Rendered_Group

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
// MARK: - Rendered_Line

Rendered_Line::Rendered_Line(const ZRGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, Rat iWidth)
:	fRGBA(iRGBA)
,	fP0(iP0)
,	fP1(iP1)
,	fWidth(iWidth)
	{}

void Rendered_Line::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Line(this); }

void Rendered_Line::Get(ZRGBA& oRGBA, GPoint& oP0, GPoint& oP1, Rat& oWidth)
	{
	oRGBA = fRGBA;
	oP0 = fP0;
	oP1 = fP1;
	oWidth = fWidth;
	}

// =================================================================================================
// MARK: - Rendered_Rect

Rendered_Rect::Rendered_Rect(const ZRGBA& iRGBA, const GRect& iBounds)
:	fRGBA(iRGBA)
,	fBounds(iBounds)
	{}

void Rendered_Rect::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Rect(this); }

void Rendered_Rect::Get(ZRGBA& oRGBA, GRect& oBounds)
	{
	oRGBA = fRGBA;
	oBounds = fBounds;
	}

ZRef<Rendered_Rect> sRendered_Rect(const ZRGBA& iRGBA, const GRect& iBounds)
	{ return new Rendered_Rect(iRGBA, iBounds); }

// =================================================================================================
// MARK: - Rendered_RightAngleSegment

Rendered_RightAngleSegment::Rendered_RightAngleSegment(
	const ZRGBA& iRGBA, bool iConcave)
:	fRGBA(iRGBA)
,	fConcave(iConcave)
	{}

void Rendered_RightAngleSegment::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_RightAngleSegment(this); }

void Rendered_RightAngleSegment::Get(ZRGBA& oRGBA, bool& oConcave)
	{
	oRGBA = fRGBA;
	oConcave = fConcave;
	}

ZRef<Rendered_RightAngleSegment> sRendered_RightAngleSegment(
	const ZRGBA& iRGBA, bool iConcave)
	{ return new Rendered_RightAngleSegment(iRGBA, iConcave); }

// =================================================================================================
// MARK: - Rendered_Sound

Rendered_Sound::Rendered_Sound(const ZRef<Sound>& iSound)
:	fSound(iSound)
	{}

const ZRef<Sound>& Rendered_Sound::GetSound()
	{ return fSound; }

void Rendered_Sound::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Sound(this); }

// =================================================================================================
// MARK: - Rendered_String

Rendered_String::Rendered_String(const ZRGBA& iRGBA, const string8& iString)
:	fRGBA(iRGBA)
,	fString(iString)
	{}

void Rendered_String::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_String(this); }

const ZRGBA& Rendered_String::GetRGBA()
	{ return fRGBA; }

const string8& Rendered_String::GetString()
	{ return fString; }

// =================================================================================================
// MARK: - Rendered_Texture

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
// MARK: - Rendered_Triangle

Rendered_Triangle::Rendered_Triangle(
	const ZRGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
:	fRGBA(iRGBA)
,	fP0(iP0)
,	fP1(iP1)
,	fP2(iP2)
	{}

void Rendered_Triangle::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Triangle(this); }

void Rendered_Triangle::Get(ZRGBA& oRGBA, GPoint& oP0, GPoint& oP1, GPoint& oP2)
	{
	oRGBA = fRGBA;
	oP0 = fP0;
	oP1 = fP1;
	oP2 = fP2;
	}

ZRef<Rendered_Triangle> sRendered_Triangle(
	const ZRGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{ return new Rendered_Triangle(iRGBA, iP0, iP1, iP2); }

// =================================================================================================
// MARK: - Visitor_Rendered

void Visitor_Rendered::Visit_Rendered(const ZRef<Rendered>& iRendered)
	{ this->Visit(iRendered); }

void Visitor_Rendered::Visit_Rendered_AlphaGainMat(
	const ZRef<Rendered_AlphaGainMat>& iRendered_AlphaGainMat)
	{ this->Visit_Rendered(iRendered_AlphaGainMat); }

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
// MARK: -

ZRef<Rendered> sFrontmost(const ZRef<Rendered>& iRendered)
	{ return sRendered_AlphaGainMat(sTranslate3Z<Rat>(128), iRendered); }

} // namespace GameEngine
} // namespace ZooLib
