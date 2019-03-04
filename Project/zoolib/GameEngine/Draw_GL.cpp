#include "zoolib/GameEngine/Draw_GL.h"

#include "zoolib/Unicode.h"

#include "zoolib/ZMACRO_foreach.h"

#if not ZMACRO_IOS && not defined(__ANDROID__)
	#include <GLUT/glut.h>
#endif

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Visitor_Draw_GL_String

void Visitor_Draw_GL_String::Visit_Rendered_String(
	const ZRef<Rendered_String>& iRendered_String)
	{
	#if defined(GLUT_API_VERSION)
		const ZRGBA theRGBA = iRendered_String->GetRGBA();

		::glColor4f(
			theRGBA.floatRed(),
			theRGBA.floatGreen(),
			theRGBA.floatBlue(),
			Alpha(theRGBA.floatAlpha()) * fAlphaGainMat.fAlpha);
		
		const string32 theString32 = Unicode::sAsUTF32(iRendered_String->GetString());
		::glPushMatrix();
		::glMultMatrixf(&fAlphaGainMat.fMat[0][0]);
		::glRasterPos3f(0, 0, 0);
		foreacha (cp, theString32)
			::glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, cp);
		::glPopMatrix();
	#endif
	}

} // namespace GameEngine
} // namespace ZooLib
