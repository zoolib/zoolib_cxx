#ifndef __ZooLib_GameEngine_Texture_GL_h__
#define __ZooLib_GameEngine_Texture_GL_h__ 1
#include "zconfig.h"

#include "zoolib/TagVal.h"

#include "zoolib/GameEngine/Texture.h"
#include "zoolib/GameEngine/Types.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

namespace ZooLib {
namespace GameEngine {

using Pixels::Pixmap;

// =================================================================================================
#pragma mark - Texture_GL

typedef TagVal<GLuint,struct Tag_TextureID> TextureID;

class Texture_GL
:	public Texture
	{
public:
	Texture_GL(PointPOD iDrawnSize);
	Texture_GL(PointPOD iTextureSize, TextureID iTextureID, bool iIsAlphaOnly);
	Texture_GL(const Pixmap& iPixmap);
	virtual ~Texture_GL();

// From Texture
	virtual PointPOD GetDrawnSize();
	virtual void Orphan();

// Our protocol
	void Get(TextureID& oTextureID, PointPOD& oTextureSize);
	TextureID GetTextureID();
	PointPOD GetTextureSize();
	bool GetIsAlphaOnly();

	static void sOrphanAll();

private:
	void pEnsureTexture();

	Pixmap fPixmap;
	PointPOD fTextureSize;
	const PointPOD fDrawnSize;
	int64 fTextureEpoch;
	TextureID fTextureID;
	bool fIsAlphaOnly;

	static int64 spTextureEpoch;
	};

ZRef<Callable_TextureFromPixmap> sCallable_TextureFromPixmap_GL();

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Texture_GL_h__
