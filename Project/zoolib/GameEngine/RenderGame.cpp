// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/RenderGame.h"

#include "zoolib/Util_STL_vector.h"

#include "zoolib/GameEngine/Draw_GL.h"
#include "zoolib/GameEngine/Draw_GL_Shader.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"
#include "zoolib/OpenGL/Util.h"

namespace ZooLib {
namespace GameEngine {

using namespace OpenGL;

// =================================================================================================
#pragma mark - Visitor_GatherSound

static inline
Rat spSquared(const Rat iRat)
	{ return iRat * iRat; }

class Visitor_GatherSound
:	public virtual Visitor_Rendered_AccumulateGain
,	public virtual Visitor_Rendered_AccumulateMat
	{
public:
	Visitor_GatherSound(
		const ZP<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	:	fSoundMeister(iSoundMeister)
	,	fListenerL(iListenerL)
	,	fListenerR(iListenerR)
	,	fListenerDSquared(iListenerD * iListenerD)
		{}

	~Visitor_GatherSound()
		{
		if (fSoundMeister)
			fSoundMeister->SetSounds(Util_STL::sFirstOrNil(fSounds), fSounds.size());
		}

	virtual void Visit_Rendered_Sound(const ZP<Rendered_Sound>& iRendered_Sound)
		{
		if (ZP<Sound> theSound = iRendered_Sound->GetSound())
			{
			const Rat thePos = X(fMat * CVec3());
			
			const Rat distLSquared = fListenerDSquared + spSquared(fListenerL - thePos);

			const Rat distRSquared = fListenerDSquared + spSquared(fListenerR - thePos);
			
			const Gain theGain(fListenerDSquared / distLSquared, fListenerDSquared / distRSquared);

			theSound->fGain = fGain * theGain;

			fSounds.push_back(theSound);
			}
		}

private:
	const ZP<SoundMeister> fSoundMeister;
	const Rat fListenerL;
	const Rat fListenerR;
	const Rat fListenerDSquared;
	std::vector<ZP<Sound> > fSounds;
	};

// =================================================================================================
#pragma mark - Visitor_Shader

class Visitor_Shader
:	public virtual Visitor_Draw_GL_Shader
,	public virtual Visitor_Rendered_AccumulateBlush
,	public virtual Visitor_GatherSound // also does AccumulateGain and AccumulateMat
,	public virtual Visitor_Rendered_DecomposeGroup
,	public virtual Visitor_Rendered_LineToRect
	{
public:
	Visitor_Shader(const Mat& iMat,
		bool iShowBounds, bool iShowOrigin,
		const ZP<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	:	Visitor_Rendered_AccumulateMat(iMat)
	,	Visitor_Draw_GL_Shader(iShowBounds, iShowBounds, iShowOrigin)
	,	Visitor_GatherSound(iSoundMeister, iListenerL, iListenerR, iListenerD)
		{}

	// Cels and Strings must have been processed into Textures before now.
	virtual void Visit_Rendered_Cel(const ZP<Rendered_Cel>& iRendered_Cel)
		{ ZUnimplemented(); }

	virtual void Visit_Rendered_String(const ZP<Rendered_String>& iRendered_String)
		{ ZUnimplemented(); }
	};

// =================================================================================================
#pragma mark -

GPoint sPixelToGame(const GPoint& p, const GPoint& g, GPoint iGPoint)
	{
	Rat scaleFactor;
	if (X(p) / Y(p) < X(g) / Y(g))
		scaleFactor = X(p) / X(g);
	else
		scaleFactor = Y(p) / Y(g);

	GPoint newG = g * scaleFactor;

	iGPoint -= (p - newG) / 2;
	iGPoint /= scaleFactor;

	return iGPoint;
	}

static Mat spAdditionalMat(const GPoint& p, const GPoint& g)
	{
	Mat additional(1);

	Rat scaleFactor;
	if (X(p) / Y(p) < X(g) / Y(g))
		scaleFactor = X(p) / X(g);
	else
		scaleFactor = Y(p) / Y(g);

	additional = sScale3XY(scaleFactor, scaleFactor) * additional;

	GPoint newG = g * scaleFactor;

	additional = sTranslate3XY<Rat>(
		(X(p) - X(newG)) / 2,
		(Y(p) - Y(newG)) / 2) * additional;

	return additional;
	}

void sRenderGame(const ZP<Rendered>& iRendered,
	const GPoint& iPixelSize,
	const GPoint& iGameSize,
	bool iShowBounds, bool iShowOrigin,
	const ZP<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	{
	// See also Visitor_Draw_GL_Shader's Visit_Rendered_Buffer.

	SaveSetRestore_ViewPort ssr_ViewPort(0, 0, X(iPixelSize), Y(iPixelSize));

	::glClearColor(0, 0, 0, 1);
	::glClear(GL_COLOR_BUFFER_BIT);

	Mat theMat(1);
	// We start with -1 <= X < 1 and -1 <= Y < 1.

	// Rescale so it's -X(iPixelSize)/2 to X(iPixelSize)/2 and -Y(iPixelSize)/2
	// to Y(iPixelSize)/2, Y is flipped, and Z is scaled by 1e-3.
	theMat *= sScale3<Rat>(2.0 / X(iPixelSize), -2.0 / Y(iPixelSize), 1e-3);

	// Translate so it's 0 to X(iPixelSize) and 0 to Y(iPixelSize).
	theMat *= sTranslate3<Rat>(-X(iPixelSize)/2, -Y(iPixelSize)/2, 0);

	// Apply any 960-->480 scaling, and centering
	theMat *= spAdditionalMat(iPixelSize, iGameSize);

	iListenerL = X(theMat * sCVec3(iListenerL,0,0));
	iListenerR = X(theMat * sCVec3(iListenerR,0,0));

	iRendered->Accept(Visitor_Shader(
		theMat,
		iShowBounds, iShowOrigin,
		iSoundMeister, iListenerL, iListenerR, iListenerD / X(iPixelSize)));
	}

} // namespace GameEngine
} // namespace ZooLib
