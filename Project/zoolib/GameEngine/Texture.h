#ifndef __ZooLib_GameEngine_Texture_h__
#define __ZooLib_GameEngine_Texture_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/ZDCPixmap.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Texture

class Texture
:	public ZCounted
	{
protected:
	Texture();

public:
	virtual ~Texture();

	virtual ZPointPOD GetDrawnSize() = 0;
	
	size_t fPixelCount;
	};

typedef Callable<ZRef<Texture>(const ZDCPixmap&)> Callable_TextureFromPixmap;

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Texture_h__
