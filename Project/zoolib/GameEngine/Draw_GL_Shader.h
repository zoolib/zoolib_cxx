#ifndef __ZooLib_GameEngine_Draw_GL_Shader_h__
#define __ZooLib_GameEngine_Draw_GL_Shader_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#if not defined(__ANDROID__)

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: -

class Visitor_Draw_GL_Shader
:	public virtual Visitor_Rendered_AccumulateAlphaGainMat
	{
public:
	Visitor_Draw_GL_Shader(bool iShowBounds, bool iShowOrigin);

	virtual void Visit_Rendered_Buffer(const ZRef<Rendered_Buffer>& iRendered_Buffer);
	virtual void Visit_Rendered_Rect(const ZRef<Rendered_Rect>& iRendered_Rect);
	virtual void Visit_Rendered_RightAngleSegment(
		const ZRef<Rendered_RightAngleSegment>& iRendered_RightAngleSegment);
	virtual void Visit_Rendered_Texture(const ZRef<Rendered_Texture>& iRendered_Texture);
	virtual void Visit_Rendered_Triangle(const ZRef<Rendered_Triangle>& iRendered_Triangle);

	void UseFixedProgram();

private:
	bool fShowBounds;
	bool fShowOrigin;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // not defined(__ANDROID__)

#endif // __ZooLib_GameEngine_Draw_GL_Shader_h__
