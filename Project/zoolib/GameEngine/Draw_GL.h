#ifndef __ZooLib_GameEngine_Draw_GL_h__
#define __ZooLib_GameEngine_Draw_GL_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Rendered_Visitors.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Visitor_Draw_GL_String

class Visitor_Draw_GL_String
:	public virtual Visitor_Rendered_AccumulateAlphaGainMat
	{
public:
	virtual void Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Draw_GL_h__
