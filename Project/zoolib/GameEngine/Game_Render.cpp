#include "zoolib/GameEngine/Game_Render.h"

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
		const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
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

	virtual void Visit_Rendered_Sound(const ZRef<Rendered_Sound>& iRendered_Sound)
		{
		if (ZRef<Sound> theSound = iRendered_Sound->GetSound())
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
	const ZRef<SoundMeister> fSoundMeister;
	const Rat fListenerL;
	const Rat fListenerR;
	const Rat fListenerDSquared;
	std::vector<ZRef<Sound> > fSounds;
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
		const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	:	Visitor_Rendered_AccumulateMat(iMat)
	,	Visitor_Draw_GL_Shader(iShowBounds, iShowOrigin)
	,	Visitor_GatherSound(iSoundMeister, iListenerL, iListenerR, iListenerD)
		{}

	virtual void Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel)
		{ ZUnimplemented(); }

	virtual void Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String)
		{ ZUnimplemented(); }
	};

// =================================================================================================
#pragma mark -

void sGame_Render(const ZRef<Rendered>& iRendered,
	const GPoint& iPixelSize,
	const GPoint& iGameSize,
	bool iUseShader,
	bool iShowBounds, bool iShowOrigin,
	const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	{
	// See also Visitor_Draw_GL_Shader's Visit_Rendered_Buffer.

	SaveSetRestore_ViewPort ssr_ViewPort(0, 0, X(iPixelSize), Y(iPixelSize));

	::glClearColor(0, 0, 0, 1);
	::glClear(GL_COLOR_BUFFER_BIT);

	Mat additional(1);

	if (X(iPixelSize) < X(iGameSize))
		{
		// We're on a screen narrower than the game, so scale to 50%. AssetCatalog will
		// have loaded low-res versions of artwork with compensating 200% scale.
		additional *= sScale3<Rat>(0.5, 0.5, 1.0);

		// And center. Note, this expression is *not* the same...
		additional *= sTranslate3<Rat>(
			X(iPixelSize) - (X(iGameSize) / 2),
			Y(iPixelSize) - (Y(iGameSize) / 2),
			0);
		}
	else
		{
		// ... as this expression.
		additional *= sTranslate3<Rat>(
			(X(iPixelSize) - X(iGameSize)) / 2,
			(Y(iPixelSize) - Y(iGameSize)) / 2,
			0);
		}

	Mat theMat(1);
	// We start with -1 <= X < 1 and -1 <= Y < 1.

	// Rescale so it's -X(iPixelSize)/2 to X(iPixelSize)/2 and -Y(iPixelSize)/2
	// to Y(iPixelSize)/2, Y is flipped, and Z is scaled by 1e-3.
	theMat *= sScale3<Rat>(2.0 / X(iPixelSize), -2.0 / Y(iPixelSize), 1e-3);

	// Translate so it's 0 to X(iPixelSize) and 0 to Y(iPixelSize).
	theMat *= sTranslate3<Rat>(-X(iPixelSize)/2, -Y(iPixelSize)/2, 0);

	// Apply any 960-->480 scaling, and centering
	theMat *= additional;

	iListenerL = X(theMat * sCVec3(iListenerL,0,0));
	iListenerR = X(theMat * sCVec3(iListenerR,0,0));

	iRendered->Accept(Visitor_Shader(
		theMat, iShowBounds, iShowOrigin,
		iSoundMeister, iListenerL, iListenerR, iListenerD / X(iPixelSize)));
	}

} // namespace GameEngine
} // namespace ZooLib
