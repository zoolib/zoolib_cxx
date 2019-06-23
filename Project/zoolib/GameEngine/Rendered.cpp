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
#pragma mark - Rendered_Blush

namespace {

SafePtrStack_WithDestroyer<Rendered_Blush,SafePtrStackLink_Rendered_Blush>
	spSafePtrStack_Blush;

} // anonymous namespace

Rendered_Blush::Rendered_Blush(const Blush& iBlush, const ZP<Rendered>& iRendered)
:	fBlush(iBlush)
,	fRendered(iRendered)
	{}

void Rendered_Blush::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fRendered.Clear();
	
	spSafePtrStack_Blush.Push(this);
	}

void Rendered_Blush::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Blush(this); }

const Blush& Rendered_Blush::GetBlush()
	{ return fBlush; }

const ZP<Rendered>& Rendered_Blush::GetRendered()
	{ return fRendered; }

ZP<Rendered_Blush> Rendered_Blush::spMake(const Blush& iBlush, const ZP<Rendered>& iRendered)
	{
	if (Rendered_Blush* result = spSafePtrStack_Blush.PopIfNotEmpty<Rendered_Blush>())
		{
		result->fBlush = iBlush;
		result->fRendered = iRendered;
		return result;
		}

	return new Rendered_Blush(iBlush, iRendered);
	}

ZP<Rendered> sRendered_Blush(const Blush& iBlush, const ZP<Rendered>& iRendered)
	{
	if (iBlush == sRGBA(1,1,1,1))
		return iRendered;

	return Rendered_Blush::spMake(iBlush, iRendered);
	}

// =================================================================================================
#pragma mark - Rendered_Buffer

Rendered_Buffer::Rendered_Buffer(int iWidth, int iHeight, const RGBA& iFill,
	const ZP<Rendered>& iRendered)
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

const ZP<Rendered>& Rendered_Buffer::GetRendered()
	{ return fRendered; }

ZP<Rendered_Buffer> sRendered_Buffer(int iWidth, int iHeight, const RGBA& iFill,
	const ZP<Rendered>& iRendered)
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

ZP<Rendered_Cel> sRendered_Cel(const Cel& iCel)
	{ return new Rendered_Cel(iCel); }

// =================================================================================================
#pragma mark - Rendered_Gain

namespace {

SafePtrStack_WithDestroyer<Rendered_Gain,SafePtrStackLink_Rendered_Gain>
	spSafePtrStack_Gain;

} // anonymous namespace

Rendered_Gain::Rendered_Gain(const Gain& iGain, const ZP<Rendered>& iRendered)
:	fGain(iGain)
,	fRendered(iRendered)
	{}

void Rendered_Gain::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fRendered.Clear();

	spSafePtrStack_Gain.Push(this);
	}

void Rendered_Gain::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Gain(this); }

const Gain& Rendered_Gain::GetGain()
	{ return fGain; }

const ZP<Rendered>& Rendered_Gain::GetRendered()
	{ return fRendered; }

ZP<Rendered_Gain> Rendered_Gain::spMake(const Gain& iGain, const ZP<Rendered>& iRendered)
	{
	if (Rendered_Gain* result = spSafePtrStack_Gain.PopIfNotEmpty<Rendered_Gain>())
		{
		result->fGain = iGain;
		result->fRendered = iRendered;
		return result;
		}

	return new Rendered_Gain(iGain, iRendered);
	}

ZP<Rendered> sRendered_Gain(const Gain& iGain, const ZP<Rendered>& iRendered)
	{
	if (iGain.fL == 1 && iGain.fR == 1)
		return iRendered;

	return Rendered_Gain::spMake(iGain, iRendered);
	}

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

const vector<ZP<Rendered> >& Rendered_Group::GetChildren()
	{ return fChildren; }

void Rendered_Group::Append(const ZP<Rendered>& iRendered)
	{ fChildren.push_back(iRendered); }

ZP<Rendered_Group> Rendered_Group::spMake()
	{
	if (Rendered_Group* result = spSafePtrStack_Group.PopIfNotEmpty<Rendered_Group>())
		return result;

	return new Rendered_Group;
	}

ZP<Rendered_Group> sRendered_Group()
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

ZP<Rendered> sRendered_Line(const GPoint& iP0, const GPoint& iP1, Rat iWidth)
	{ return new Rendered_Line(iP0, iP1, iWidth); }

ZP<Rendered> sRendered_Line(const RGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, Rat iWidth)
	{ return sRendered_Blush(iRGBA, sRendered_Line(iP0, iP1, iWidth)); }

// =================================================================================================
#pragma mark - Rendered_Mat

namespace {

SafePtrStack_WithDestroyer<Rendered_Mat,SafePtrStackLink_Rendered_Mat>
	spSafePtrStack_Mat;

} // anonymous namespace

Rendered_Mat::Rendered_Mat(const Mat& iMat, const ZP<Rendered>& iRendered)
:	fMat(iMat)
,	fRendered(iRendered)
	{}

void Rendered_Mat::Finalize()
	{
	bool finalized = this->FinishFinalize();
	ZAssert(finalized);
	ZAssert(not this->IsReferenced());
	fRendered.Clear();

	spSafePtrStack_Mat.Push(this);
	}

void Rendered_Mat::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Mat(this); }

const Mat& Rendered_Mat::GetMat()
	{ return fMat; }

const ZP<Rendered>& Rendered_Mat::GetRendered()
	{ return fRendered; }

ZP<Rendered_Mat> Rendered_Mat::spMake(const Mat& iMat, const ZP<Rendered>& iRendered)
	{
	if (Rendered_Mat* result = spSafePtrStack_Mat.PopIfNotEmpty<Rendered_Mat>())
		{
		result->fMat = iMat;
		result->fRendered = iRendered;
		return result;
		}

	return new Rendered_Mat(iMat, iRendered);
	}

ZP<Rendered> sRendered_Mat(const Mat& iMat, const ZP<Rendered>& iRendered)
	{ return Rendered_Mat::spMake(iMat, iRendered); }

// =================================================================================================
#pragma mark - Rendered_Rect

Rendered_Rect::Rendered_Rect(const GRect& iBounds)
:	fBounds(iBounds)
	{}

void Rendered_Rect::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Rect(this); }

void Rendered_Rect::Get(GRect& oBounds)
	{ oBounds = fBounds; }

ZP<Rendered> sRendered_Rect(const GRect& iBounds)
	{ return new Rendered_Rect(iBounds); }

ZP<Rendered> sRendered_Rect(const RGBA& iRGBA, const GRect& iBounds)
	{ return sRendered_Blush(iRGBA, new Rendered_Rect(iBounds)); }

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

ZP<Rendered> sRendered_RightAngleSegment(
	const RGBA& iRGBA_Convex, const RGBA& iRGBA_Concave)
	{ return new Rendered_RightAngleSegment(iRGBA_Convex, iRGBA_Concave); }

// =================================================================================================
#pragma mark - Rendered_Sound

Rendered_Sound::Rendered_Sound(const ZP<Sound>& iSound)
:	fSound(iSound)
	{}

const ZP<Sound>& Rendered_Sound::GetSound()
	{ return fSound; }

void Rendered_Sound::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_Sound(this); }

// =================================================================================================
#pragma mark - Rendered_String

Rendered_String::Rendered_String(const ZP<FontStrike>& iFontStrike, const string8& iString)
:	fFontStrike(iFontStrike)
,	fString(iString)
	{}

void Rendered_String::Accept_Rendered(Visitor_Rendered& iVisitor)
	{ iVisitor.Visit_Rendered_String(this); }

const ZP<FontStrike>& Rendered_String::GetFontStrike()
	{ return fFontStrike; }

const string8& Rendered_String::GetString()
	{ return fString; }

ZP<Rendered> sRendered_String(const ZP<FontStrike>& iFontStrike, const string8& iString)
	{ return new Rendered_String(iFontStrike, iString); }

ZP<Rendered> sRendered_String(const RGBA& iRGBA, const ZP<FontStrike>& iFontStrike, const string8& iString)
	{ return sRendered_Blush(iRGBA, sRendered_String(iFontStrike, iString)); }

// =================================================================================================
#pragma mark - Rendered_Texture

namespace {

SafePtrStack_WithDestroyer<Rendered_Texture,SafePtrStackLink_Rendered_Texture>
	spSafePtrStack_Texture;

} // anonymous namespace

Rendered_Texture::Rendered_Texture(const ZP<Texture>& iTexture, const GRect& iBounds)
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

const ZP<Texture>& Rendered_Texture::GetTexture()
	{ return fTexture; }

const GRect& Rendered_Texture::GetBounds()
	{ return fBounds; }

ZP<Rendered_Texture> Rendered_Texture::spMake(
	const ZP<Texture>& iTexture, const GRect& iBounds)
	{
	if (Rendered_Texture* result = spSafePtrStack_Texture.PopIfNotEmpty<Rendered_Texture>())
		{
		result->fTexture = iTexture;
		result->fBounds = iBounds;
		return result;
		}

	return new Rendered_Texture(iTexture, iBounds);
	}

ZP<Rendered_Texture> sRendered_Texture(const ZP<Texture>& iTexture, const GRect& iBounds)
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

ZP<Rendered> sRendered_Triangle(const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{ return new Rendered_Triangle(iP0, iP1, iP2); }

ZP<Rendered> sRendered_Triangle(
	const RGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{ return sRendered_Blush(iRGBA, sRendered_Triangle(iP0, iP1, iP2)); }

// =================================================================================================
#pragma mark - Visitor_Rendered

void Visitor_Rendered::Visit_Rendered(const ZP<Rendered>& iRendered)
	{ this->Visit(iRendered); }

void Visitor_Rendered::Visit_Rendered_Blush(const ZP<Rendered_Blush>& iRendered_Blush)
	{ this->Visit_Rendered(iRendered_Blush); }

void Visitor_Rendered::Visit_Rendered_Buffer(const ZP<Rendered_Buffer>& iRendered_Buffer)
	{ this->Visit_Rendered(iRendered_Buffer); }

void Visitor_Rendered::Visit_Rendered_Cel(const ZP<Rendered_Cel>& iRendered_Cel)
	{ this->Visit_Rendered(iRendered_Cel); }

void Visitor_Rendered::Visit_Rendered_Gain(const ZP<Rendered_Gain>& iRendered_Gain)
	{ this->Visit_Rendered(iRendered_Gain); }

void Visitor_Rendered::Visit_Rendered_Group(const ZP<Rendered_Group>& iRendered_Group)
	{ this->Visit_Rendered(iRendered_Group); }

void Visitor_Rendered::Visit_Rendered_Line(const ZP<Rendered_Line>& iRendered_Line)
	{ this->Visit_Rendered(iRendered_Line); }

void Visitor_Rendered::Visit_Rendered_Mat(const ZP<Rendered_Mat>& iRendered_Mat)
	{ this->Visit_Rendered(iRendered_Mat); }

void Visitor_Rendered::Visit_Rendered_Rect(const ZP<Rendered_Rect>& iRendered_Rect)
	{ this->Visit_Rendered(iRendered_Rect); }

void Visitor_Rendered::Visit_Rendered_RightAngleSegment(
	const ZP<Rendered_RightAngleSegment>& iRendered_RightAngleSegment)
	{ this->Visit_Rendered(iRendered_RightAngleSegment); }

void Visitor_Rendered::Visit_Rendered_Sound(const ZP<Rendered_Sound>& iRendered_Sound)
	{ this->Visit_Rendered(iRendered_Sound); }

void Visitor_Rendered::Visit_Rendered_String(const ZP<Rendered_String>& iRendered_String)
	{ this->Visit_Rendered(iRendered_String); }

void Visitor_Rendered::Visit_Rendered_Texture(const ZP<Rendered_Texture>& iRendered_Texture)
	{ this->Visit_Rendered(iRendered_Texture); }

void Visitor_Rendered::Visit_Rendered_Triangle(const ZP<Rendered_Triangle>& iRendered_Triangle)
	{ this->Visit_Rendered(iRendered_Triangle); }

// =================================================================================================
#pragma mark -

ZP<Rendered> sFrontmost(const ZP<Rendered>& iRendered)
	{ return sRendered_Mat(sTranslate3Z<Rat>(128), iRendered); }

} // namespace GameEngine
} // namespace ZooLib
