#ifndef __ZooLib_GameEngine_Rendered_h__
#define __ZooLib_GameEngine_Rendered_h__ 1
#include "zconfig.h"

#include "zoolib/SafePtrStack.h"
#include "zoolib/Visitor.h"

#include "zoolib/GameEngine/FontCatalog.h"
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
#pragma mark - Rendered_Blush

class Rendered_Blush;

class SafePtrStackLink_Rendered_Blush
:	public SafePtrStackLink<Rendered_Blush,SafePtrStackLink_Rendered_Blush>
	{};

class Rendered_Blush
:	public Rendered
,	public SafePtrStackLink_Rendered_Blush
	{
protected:
	Rendered_Blush(const Blush& iBlush, const ZP<Rendered>& iRendered);

public:
// From Counted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const Blush& GetBlush();
	const ZP<Rendered>& GetRendered();

	static
	ZP<Rendered_Blush> spMake(const Blush& iBlush, const ZP<Rendered>& iRendered);

private:
	Blush fBlush;
	ZP<Rendered> fRendered;
	};

ZP<Rendered> sRendered_Blush(const Blush& iBlush, const ZP<Rendered>& iRendered);

// =================================================================================================
#pragma mark - Rendered_Buffer

class Rendered_Buffer
:	public Rendered
	{
public:
	Rendered_Buffer(int iWidth, int iHeight, const RGBA& iFill, const ZP<Rendered>& iRendered);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	int GetWidth();
	int GetHeight();
	RGBA GetFill();
	const ZP<Rendered>& GetRendered();

private:
	const int fWidth;
	const int fHeight;
	const RGBA fFill;
	const ZP<Rendered> fRendered;
	};

ZP<Rendered_Buffer> sRendered_Buffer(int iWidth, int iHeight, const RGBA& iFill,
	const ZP<Rendered>& iRendered);

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

ZP<Rendered_Cel> sRendered_Cel(const Cel& iCel);

// =================================================================================================
#pragma mark - Rendered_Gain

class Rendered_Gain;

class SafePtrStackLink_Rendered_Gain
:	public SafePtrStackLink<Rendered_Gain,SafePtrStackLink_Rendered_Gain>
	{};

class Rendered_Gain
:	public Rendered
,	public SafePtrStackLink_Rendered_Gain
	{
protected:
	Rendered_Gain(const Gain& iGain, const ZP<Rendered>& iRendered);

public:
// From Counted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const Gain& GetGain();
	const ZP<Rendered>& GetRendered();

	static
	ZP<Rendered_Gain> spMake(const Gain& iGain, const ZP<Rendered>& iRendered);

private:
	Gain fGain;
	ZP<Rendered> fRendered;
	};

ZP<Rendered> sRendered_Gain(const Gain& iGain, const ZP<Rendered>& iRendered);

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
// From Counted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const std::vector<ZP<Rendered> >& GetChildren();
	void Append(const ZP<Rendered>& iRendered);

	static ZP<Rendered_Group> spMake();

private:
	std::vector<ZP<Rendered> > fChildren;
	};

ZP<Rendered_Group> sRendered_Group();

// =================================================================================================
#pragma mark - Rendered_Line

class Rendered_Line
:	public Rendered
	{
public:
	Rendered_Line(const GPoint& iP0, const GPoint& iP1, Rat iWidth);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(GPoint& oP0, GPoint& oP1, Rat& oWidth);

private:
	const GPoint fP0;
	const GPoint fP1;
	Rat fWidth;
	};

ZP<Rendered> sRendered_Line(const GPoint& iP0, const GPoint& iP1, Rat iWidth);

ZP<Rendered> sRendered_Line(const RGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, Rat iWidth);

// =================================================================================================
#pragma mark - Rendered_Mat

class Rendered_Mat;

class SafePtrStackLink_Rendered_Mat
:	public SafePtrStackLink<Rendered_Mat,SafePtrStackLink_Rendered_Mat>
	{};

class Rendered_Mat
:	public Rendered
,	public SafePtrStackLink_Rendered_Mat
	{
protected:
	Rendered_Mat(const Mat& iMat, const ZP<Rendered>& iRendered);

public:
// From Counted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const Mat& GetMat();
	const ZP<Rendered>& GetRendered();

	BlushMat GetBlushMat();

	static
	ZP<Rendered_Mat> spMake(const Mat& iMat, const ZP<Rendered>& iRendered);

private:
	Mat fMat;
	ZP<Rendered> fRendered;
	};

ZP<Rendered> sRendered_Mat(const Mat& iMat, const ZP<Rendered>& iRendered);

// =================================================================================================
#pragma mark - Rendered_Rect

class Rendered_Rect
:	public Rendered
	{
public:
	Rendered_Rect(const GRect& iBounds);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(GRect& oBounds);

private:
	const GRect fBounds;
	};

ZP<Rendered> sRendered_Rect(const GRect& iBounds);

ZP<Rendered> sRendered_Rect(const RGBA& iRGBA, const GRect& iBounds);

// =================================================================================================
#pragma mark - Rendered_RightAngleSegment

class Rendered_RightAngleSegment
:	public Rendered
	{
public:
	Rendered_RightAngleSegment(const RGBA& iRGBA_Convex, const RGBA& iRGBA_Concave);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(RGBA& oRGBA_Convex, RGBA& oRGBA_Concave);

private:
	const RGBA fRGBA_Convex;
	const RGBA fRGBA_Concave;
	};

ZP<Rendered> sRendered_RightAngleSegment(const RGBA& iRGBA_Convex, const RGBA& iRGBA_Concave);

// =================================================================================================
#pragma mark - Rendered_Sound

class Rendered_Sound
:	public Rendered
	{
public:
	Rendered_Sound(const ZP<Sound>& iSound);
	
	const ZP<Sound>& GetSound();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

private:
	const ZP<Sound> fSound;
	};

// =================================================================================================
#pragma mark - Rendered_String

class Rendered_String
:	public Rendered
	{
public:
	Rendered_String(const ZP<FontStrike>& iFontStrike, const string8& iString);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const ZP<FontStrike>& GetFontStrike();
	const string8& GetString();

private:
	const ZP<FontStrike> fFontStrike;
	const string8 fString;
	};

ZP<Rendered> sRendered_String(const ZP<FontStrike>& iFontStrike, const string8& iString);

ZP<Rendered> sRendered_String(const RGBA& iRGBA,
	const ZP<FontStrike>& iFontStrike, const string8& iString);

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
	Rendered_Texture(const ZP<Texture>& iTexture, const GRect& iBounds);

// From Counted
	virtual void Finalize();

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	const ZP<Texture>& GetTexture();
	const GRect& GetBounds();

	static
	ZP<Rendered_Texture> spMake(
		const ZP<Texture>& iTexture, const GRect& iBounds);

private:
	ZP<Texture> fTexture;
	GRect fBounds;
	};

ZP<Rendered_Texture> sRendered_Texture(const ZP<Texture>& iTexture, const GRect& iBounds);

// =================================================================================================
#pragma mark - Rendered_Triangle

class Rendered_Triangle
:	public Rendered
	{
public:
	Rendered_Triangle(const GPoint& iP0, const GPoint& iP1, const GPoint& iP2);

// From Rendered
	virtual void Accept_Rendered(Visitor_Rendered& iVisitor);

// Our protocol
	void Get(GPoint& oP0, GPoint& oP1, GPoint& oP2);

private:
	const GPoint fP0;
	const GPoint fP1;
	const GPoint fP2;
	};

ZP<Rendered> sRendered_Triangle(const GPoint& iP0, const GPoint& iP1, const GPoint& iP2);

ZP<Rendered> sRendered_Triangle(
	const RGBA& iRGBA, const GPoint& iP0, const GPoint& iP1, const GPoint& iP2);

// =================================================================================================
#pragma mark - Visitor_Rendered

class Visitor_Rendered
:	public virtual Visitor
	{
public:
	virtual void Visit_Rendered(const ZP<Rendered>& iRendered);

	virtual void Visit_Rendered_Blush(const ZP<Rendered_Blush>& iRendered_Blush) = 0;
	virtual void Visit_Rendered_Buffer(const ZP<Rendered_Buffer>& iRendered_Buffer);
	virtual void Visit_Rendered_Cel(const ZP<Rendered_Cel>& iRendered_Cel);
	virtual void Visit_Rendered_Gain(const ZP<Rendered_Gain>& iRendered_Gain) = 0;
	virtual void Visit_Rendered_Group(const ZP<Rendered_Group>& iRendered_Group);
	virtual void Visit_Rendered_Line(const ZP<Rendered_Line>& iRendered_Line);
	virtual void Visit_Rendered_Mat(const ZP<Rendered_Mat>& iRendered_Mat) = 0;
	virtual void Visit_Rendered_Rect(const ZP<Rendered_Rect>& iRendered_Rect);
	virtual void Visit_Rendered_RightAngleSegment(
		const ZP<Rendered_RightAngleSegment>& iRendered_RightAngleSegment);
	virtual void Visit_Rendered_Sound(const ZP<Rendered_Sound>& iRendered_Sound);
	virtual void Visit_Rendered_String(const ZP<Rendered_String>& iRendered_String);
	virtual void Visit_Rendered_Texture(const ZP<Rendered_Texture>& iRendered_Texture);
	virtual void Visit_Rendered_Triangle(const ZP<Rendered_Triangle>& iRendered_Triangle);
	};

// =================================================================================================
#pragma mark -

ZP<Rendered> sFrontmost(const ZP<Rendered>& iRendered);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Rendered_h__
