#ifndef __ZooLib_GameEngine_FontCatalog_h__
#define __ZooLib_GameEngine_FontCatalog_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

#include "zoolib/GameEngine/Texture.h"
#include "zoolib/GameEngine/Types.h" // For FontSpec, GPoint, GRect, Rat

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - FontStrike

class FontStrike
:	public ZCounted
	{
public:
	virtual ZRef<Texture> GetGlyphTexture(UTF32 iCP,
	GRect& oGlyphBounds, GPoint& oOffset, Rat& oXAdvance) = 0;
	};

// =================================================================================================
#pragma mark - FontCatalog

class FontStrike;

class FontCatalog
:	public ZCounted
	{
public:
	virtual ZRef<FontStrike> GetFontStrike(const FontSpec& iFontSpec) = 0;
	};

ZRef<FontCatalog> sMakeFontCatalog(const FileSpec& iFileSpec);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_FontCatalog_h__
