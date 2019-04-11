#include "zoolib/GameEngine/Draw_GL.h"
#include "zoolib/OpenGL/Util.h"

#include "zoolib/Unicode.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace GameEngine {

using namespace OpenGL;

// =================================================================================================
#pragma mark - Visitor_Draw_GL_String

void Visitor_Draw_GL_String::Visit_Rendered_String(
	const ZRef<Rendered_String>& iRendered_String)
	{
//	my_stbtt_initfont();
//
//	::glPushMatrix();
//
//	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
//	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
//	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//
//	spBefore(sAlphaMat(fAlphaGainMat), iRendered_String->GetRGBA());
//
//	SaveSetRestore_Enable ssr_a(GL_TEXTURE_2D, true);
//
//	SaveSetRestore_BindTexture_2D ssr_BindTexture_2D(ftex);
//
//	::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//??
//
//	string8 theString8 = iRendered_String->GetString();
//
//	my_stbtt_print(0, 0, theString8.c_str());
//
//	::glPopMatrix();
//
	}

} // namespace GameEngine
} // namespace ZooLib
