#ifndef __ZooLib_GameEngine_Draw_GL_Shader_h__
#define __ZooLib_GameEngine_Draw_GL_Shader_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#define ZMACRO_CanUseShader 1

#if ZMACRO_CanUseShader

namespace ZooLib {
namespace GameEngine {

void sKillContext();

// =================================================================================================
#pragma mark -

class Visitor_Draw_GL_Shader
:	public virtual Visitor_Rendered_AccumulateBlush
,	public virtual Visitor_Rendered_AccumulateMat
	{
public:
	Visitor_Draw_GL_Shader(bool iShowBounds, bool iShowBounds_AlphaOnly, bool iShowOrigin);

	virtual void Visit_Rendered_Buffer(const ZP<Rendered_Buffer>& iRendered_Buffer);

	virtual void Visit_Rendered_Rect(const ZP<Rendered_Rect>& iRendered_Rect);

	virtual void Visit_Rendered_RightAngleSegment(
		const ZP<Rendered_RightAngleSegment>& iRendered_RightAngleSegment);

	virtual void Visit_Rendered_Texture(const ZP<Rendered_Texture>& iRendered_Texture);

	virtual void Visit_Rendered_Triangle(const ZP<Rendered_Triangle>& iRendered_Triangle);

private:
	bool fShowBounds;
	bool fShowBounds_AlphaOnly;
	bool fShowOrigin;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // ZMACRO_CanUseShader

#endif // __ZooLib_GameEngine_Draw_GL_Shader_h__
