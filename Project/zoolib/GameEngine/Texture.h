// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Texture_h__
#define __ZooLib_GameEngine_Texture_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/Pixels/Pixmap.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Texture

class Texture
:	public Counted
	{
protected:
	Texture();

public:
	virtual ~Texture();

	virtual Pixels::PointPOD GetDrawnSize() = 0;

	virtual void Orphan() = 0;
	
	size_t fPixelCount;
	};

typedef Callable<ZP<Texture>(const Pixels::Pixmap&)> Callable_TextureFromPixmap;

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Texture_h__
