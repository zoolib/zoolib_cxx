#ifndef __ZooLib_GameEngine_Rendered_h__
#define __ZooLib_GameEngine_Rendered_h__ 1
#include "zconfig.h"

#include "zoolib/SafePtrStack.h"
#include "zoolib/Visitor.h"

#include "zoolib/GameEngine/Sound.h"
#include "zoolib/GameEngine/Texture.h"
#include "zoolib/GameEngine/Types.h"

#include <vector>

namespace ZooLib {
namespace GameEngine {

class Visitor_Rendered;

// =================================================================================================
#pragma mark - Rendered

class Rendered
:	public Visitee
	{
protected:
	Rendered();

public:
// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// Our protocol
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);
	};

// =================================================================================================
#pragma mark - Rendered_AlphaGainMat

class Rendered_AlphaGainMat;

class SafePtrStackLink_Rendered_AlphaGainMat
:	public SafePtrStackLink<Rendered_AlphaGainMat,SafePtrStackLink_Rendered_AlphaGainMat>
	{};

class Rendered_AlphaGainMat
:	public Rendered
,	public SafePtrStackLink_Rendered_AlphaGainMat
	{
protected:
	Rendered_AlphaGainMat();
	Rendered_AlphaGainMat(const AlphaGainMat& iAlphaGainMat, const ZRef<Rendered>& iRendered);

public:
// From ZCounted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const AlphaGainMat& GetAlphaGainMat();
	const ZRef<Rendered>& GetRendered();

	AlphaMat GetAlphaMat();

	static
	ZRef<Rendered_AlphaGainMat> spMake(
		const AlphaGainMat& iAlphaGainMat, const ZRef<Rendered>& iRendered);

private:
	AlphaGainMat fAlphaGainMat;
	ZRef<Rendered> fRendered;
	};

ZRef<Rendered_AlphaGainMat> sRendered_AlphaGainMat(
	const AlphaGainMat& iAlphaGainMat, const ZRef<Rendered>& iRendered);

// =================================================================================================
#pragma mark - Rendered_Buffer

class Rendered_Buffer
:	public Rendered
	{
public:
	Rendered_Buffer(int iWidth, int iHeight, const ZRGBA& iRGBA, const ZRef<Rendered>& iRendered);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	int GetWidth();
	int GetHeight();
	ZRGBA GetRGBA();
	const ZRef<Rendered>& GetRendered();

private:
	const int fWidth;
	const int fHeight;
	const ZRGBA fRGBA;
	const ZRef<Rendered> fRendered;
	};

ZRef<Rendered_Buffer> sRendered_Buffer(
	int iWidth, int iHeight, const ZRGBA& iRGBA, const ZRef<Rendered>& iRendered);

// =================================================================================================
#pragma mark - Rendered_Cel

class Rendered_Cel
:	public Rendered
	{
public:
	Rendered_Cel(const Cel& iCel);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const Cel& GetCel();

private:
	const Cel fCel;
	};

ZRef<Rendered_Cel> sRendered_Cel(const Cel& iCel);

// =================================================================================================
#pragma mark - Rendered_Group

class Rendered_Group;

class SafePtrStackLink_Rendered_Group
:	public SafePtrStackLink<Rendered_Group,SafePtrStackLink_Rendered_Group>
	{};

class Rendered_Group
:	public Rendered
,	public SafePtrStackLink_Rendered_Group
	{
protected:
	Rendered_Group();

public:
// From ZCounted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const std::vector<ZRef<Rendered> >& GetChildren();
	void Append(const ZRef<Rendered>& iRendered);

	static ZRef<Rendered_Group> spMake();

private:
	std::vector<ZRef<Rendered> > fChildren;
	};

ZRef<Rendered_Group> sRendered_Group();

// =================================================================================================
#pragma mark - Rendered_Line

class Rendered_Line
:	public Rendered
	{
public:
	Rendered_Line(const ZRGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, Rat iWidth);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(ZRGBA& oRGBA, GPoint& oP0, GPoint& oP1, Rat& oWidth);

private:
	const ZRGBA fRGBA;
	const GPoint fP0;
	const GPoint fP1;
	Rat fWidth;
	};

// =================================================================================================
#pragma mark - Rendered_Rect

class Rendered_Rect
:	public Rendered
	{
public:
	Rendered_Rect(const ZRGBA& iRGBA, const GRect& iBounds);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(ZRGBA& oRGBA, GRect& oBounds);

private:
	const ZRGBA fRGBA;
	const GRect fBounds;
	};

ZRef<Rendered_Rect> sRendered_Rect(const ZRGBA& iRGBA, const GRect& iBounds);

// =================================================================================================
#pragma mark - Rendered_RightAngleSegment

class Rendered_RightAngleSegment
:	public Rendered
	{
public:
	Rendered_RightAngleSegment(const ZRGBA& iRGBA_Convex, const ZRGBA& iRGBA_Concave);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(ZRGBA& oRGBA_Convex, ZRGBA& oRGBA_Concave);

private:
	const ZRGBA fRGBA_Convex;
	const ZRGBA fRGBA_Concave;
	};

ZRef<Rendered_RightAngleSegment> sRendered_RightAngleSegment(
	const ZRGBA& iRGBA_Convex, const ZRGBA& iRGBA_Concave);

// =================================================================================================
#pragma mark - Rendered_Sound

class Rendered_Sound
:	public Rendered
	{
public:
	Rendered_Sound(const ZRef<Sound>& iSound);
	
	const ZRef<Sound>& GetSound();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

private:
	const ZRef<Sound> fSound;
	};

// =================================================================================================
#pragma mark - Rendered_String

class Rendered_String
:	public Rendered
	{
public:
	Rendered_String(const ZRGBA& iRGBA, const string8& iString);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const ZRGBA& GetRGBA();
	const string8& GetString();

private:
	const ZRGBA fRGBA;
	const string8 fString;
	};

// =================================================================================================
#pragma mark - Rendered_Texture

class Rendered_Texture;

class SafePtrStackLink_Rendered_Texture
:	public SafePtrStackLink<Rendered_Texture,SafePtrStackLink_Rendered_Texture>
	{};

class Rendered_Texture
:	public Rendered
,	public SafePtrStackLink_Rendered_Texture
	{
public:
	Rendered_Texture(const ZRef<Texture>& iTexture, const GRect& iBounds);

// From ZCounted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const ZRef<Texture>& GetTexture();
	const GRect& GetBounds();

	static
	ZRef<Rendered_Texture> spMake(
		const ZRef<Texture>& iTexture, const GRect& iBounds);

private:
	ZRef<Texture> fTexture;
	GRect fBounds;
	};

ZRef<Rendered_Texture> sRendered_Texture(const ZRef<Texture>& iTexture, const GRect& iBounds);

// =================================================================================================
#pragma mark - Rendered_Triangle

class Rendered_Triangle
:	public Rendered
	{
public:
	Rendered_Triangle(const ZRGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(ZRGBA& oRGBA, GPoint& oP0, GPoint& oP1, GPoint& oP2);

private:
	const ZRGBA fRGBA;
	const GPoint fP0;
	const GPoint fP1;
	const GPoint fP2;
	};

ZRef<Rendered_Triangle> sRendered_Triangle(
	const ZRGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2);

// =================================================================================================
#pragma mark - Visitor_Rendered

class Visitor_Rendered
:	public virtual Visitor
	{
public:
	virtual void Visit_Rendered(const ZRef<Rendered>& iRendered);

	virtual void Visit_Rendered_AlphaGainMat(
		const ZRef<Rendered_AlphaGainMat>& iRendered_AlphaGainMat);
	virtual void Visit_Rendered_Buffer(const ZRef<Rendered_Buffer>& iRendered_Buffer);
	virtual void Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel);
	virtual void Visit_Rendered_Group(const ZRef<Rendered_Group>& iRendered_Group);
	virtual void Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line);
	virtual void Visit_Rendered_Rect(const ZRef<Rendered_Rect>& iRendered_Rect);
	virtual void Visit_Rendered_RightAngleSegment(
		const ZRef<Rendered_RightAngleSegment>& iRendered_RightAngleSegment);
	virtual void Visit_Rendered_Sound(const ZRef<Rendered_Sound>& iRendered_Sound);
	virtual void Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String);
	virtual void Visit_Rendered_Texture(const ZRef<Rendered_Texture>& iRendered_Texture);
	virtual void Visit_Rendered_Triangle(const ZRef<Rendered_Triangle>& iRendered_Triangle);
	};

// =================================================================================================
#pragma mark -

ZRef<Rendered> sFrontmost(const ZRef<Rendered>& iRendered);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Rendered_h__
