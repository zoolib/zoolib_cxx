#ifndef __ZooLib_GameEngine_Texture_GL_h__
#define __ZooLib_GameEngine_Texture_GL_h__ 1
#include "zconfig.h"

#include "zoolib/TagVal.h"

#include "zoolib/GameEngine/Texture.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Texture_GL

typedef TagVal<GLuint,struct Tag_TextureID> TextureID;

class Texture_GL
:	public Texture
	{
public:
	Texture_GL(ZPointPOD iDrawnSize);
	Texture_GL(ZPointPOD iTextureSize, TextureID iTextureID);
	Texture_GL(const ZDCPixmap& iPixmap);
	virtual ~Texture_GL();

// From Texture
	virtual ZPointPOD GetDrawnSize();
	
// Our protocol
	void Get(TextureID& oTextureID, ZPointPOD& oTextureSize);
	TextureID GetTextureID();
	ZPointPOD GetTextureSize();

private:
	void pMakeTexture();

	ZDCPixmap fPixmap;
	ZPointPOD fTextureSize;
	const ZPointPOD fDrawnSize;
	TextureID fTextureID;
	};

ZRef<Callable_TextureFromPixmap> sCallable_TextureFromPixmap_GL();

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Texture_GL_h__
