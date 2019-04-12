#ifndef __ZooLib_GameEngine_Draw_GL_Shader_h__
#define __ZooLib_GameEngine_Draw_GL_Shader_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#define ZMACRO_CanUseShader 1

#if ZMACRO_CanUseShader

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark -

class Visitor_Draw_GL_Shader
:	public virtual Visitor_Rendered_AccumulateBlush
,	public virtual Visitor_Rendered_AccumulateMat
	{
public:
	Visitor_Draw_GL_Shader(bool iShowBounds, bool iShowOrigin);

	virtual void Visit_Rendered_Buffer(const ZRef<Rendered_Buffer>& iRendered_Buffer);

	virtual void Visit_Rendered_Rect(const ZRef<Rendered_Rect>& iRendered_Rect);

	virtual void Visit_Rendered_RightAngleSegment(
		const ZRef<Rendered_RightAngleSegment>& iRendered_RightAngleSegment);

	virtual void Visit_Rendered_Texture(const ZRef<Rendered_Texture>& iRendered_Texture);

	virtual void Visit_Rendered_Triangle(const ZRef<Rendered_Triangle>& iRendered_Triangle);

private:
	bool fShowBounds;
	bool fShowOrigin;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // ZMACRO_CanUseShader

#endif // __ZooLib_GameEngine_Draw_GL_Shader_h__
