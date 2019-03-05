#include "zoolib/GameEngine/Game_Render.h"

#include "zoolib/Util_STL_vector.h"

#include "zoolib/GameEngine/Draw_GL.h"
#include "zoolib/GameEngine/Draw_GL_Fixed.h"
#include "zoolib/GameEngine/Draw_GL_Shader.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"
#include "zoolib/OpenGL/Util.h"

namespace ZooLib {
namespace GameEngine {

using namespace OpenGL;

// =================================================================================================
// MARK: - Visitor_GatherSound

static inline
Rat spSquared(const Rat iRat)
	{ return iRat * iRat; }

class Visitor_GatherSound
:	public virtual Visitor_Rendered_AccumulateAlphaGainMat
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
			const Rat thePos = X(fAlphaGainMat.fMat * CVec3());
			
			const Rat distLSquared = fListenerDSquared + spSquared(fListenerL - thePos);

			const Rat distRSquared = fListenerDSquared + spSquared(fListenerR - thePos);
			
			const Gain theGain(fListenerDSquared / distLSquared, fListenerDSquared / distRSquared);

			theSound->fGain = fAlphaGainMat.fGain * theGain;

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
// MARK: - Visitor_Shader

#if not defined(__ANDROID__)

class Visitor_Shader
:	public virtual Visitor_Draw_GL_Shader
,	public virtual Visitor_Draw_GL_String
,	public virtual Visitor_GatherSound
,	public virtual Visitor_Rendered_DecomposeGroup
,	public virtual Visitor_Rendered_LineToRect
	{
public:
	Visitor_Shader(const AlphaGainMat& iAlphaGainMat,
		bool iShowBounds, bool iShowOrigin,
		const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	:	Visitor_Rendered_AccumulateAlphaGainMat(iAlphaGainMat)
	,	Visitor_Draw_GL_Shader(iShowBounds, iShowOrigin)
	,	Visitor_GatherSound(iSoundMeister, iListenerL, iListenerR, iListenerD)
		{}

	virtual void Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String)
		{
		Visitor_Draw_GL_Shader::UseFixedProgram();
		Visitor_Draw_GL_String::Visit_Rendered_String(iRendered_String);
		}

	virtual void Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel)
		{ ZUnimplemented(); }
	};

#endif // not defined(__ANDROID__)

// =================================================================================================
// MARK: - Visitor_Fixed

class Visitor_Fixed
:	public Visitor_Draw_GL_Fixed
,	public Visitor_Draw_GL_String
,	public Visitor_GatherSound
,	public Visitor_Rendered_DecomposeGroup
,	public Visitor_Rendered_LineToRect
	{
public:
	Visitor_Fixed(const AlphaGainMat& iAlphaGainMat,
		bool iShowBounds, bool iShowOrigin,
		const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	:	Visitor_Rendered_AccumulateAlphaGainMat(iAlphaGainMat)
	,	Visitor_Draw_GL_Fixed(iShowBounds, iShowOrigin)
	,	Visitor_GatherSound(iSoundMeister, iListenerL, iListenerR, iListenerD)
		{}

	virtual void Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel)
		{ ZUnimplemented(); }
	};

// =================================================================================================
// MARK: -

void sGame_Render(const ZRef<Rendered>& iRendered,
	const GPoint& iPixelSize,
	const GPoint& iGameSize,
	bool iUseShader,
	bool iShowBounds, bool iShowOrigin,
	const ZRef<SoundMeister>& iSoundMeister, Rat iListenerL, Rat iListenerR, Rat iListenerD)
	{
	// See also Visitor_Draw_GL_Shader's and Visitor_Draw_GL_Fixed's Visit_Rendered_Buffer.

	SaveSetRestore_ViewPort theSetRestore_ViewPort(0, 0, X(iPixelSize), Y(iPixelSize));

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

#if not defined(__ANDROID__)
	if (iUseShader)
		{
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
	else
#endif // not defined(__ANDROID__)
		{
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();

		#if defined(GL_VERSION_2_1)
			::glOrtho(0, X(iPixelSize), Y(iPixelSize), 0, -1000.0, 1000.0);
		#else
			::glOrthof(0, X(iPixelSize), Y(iPixelSize), 0, -1000.0, 1000.0);
		#endif

		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();

		iListenerL = X(additional * sCVec3(iListenerL,0,0));
		iListenerR = X(additional * sCVec3(iListenerR,0,0));

		iRendered->Accept(Visitor_Fixed(
			additional, iShowBounds, iShowOrigin,
			iSoundMeister, iListenerL, iListenerR, iListenerD));
		}
	}

} // namespace GameEngine
} // namespace ZooLib
