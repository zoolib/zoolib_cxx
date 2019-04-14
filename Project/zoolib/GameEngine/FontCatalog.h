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

	virtual GRect Measure(const UTF32* iCPs, size_t iCount) = 0;

//	void GetVMetrics(float* oAscent, float* oDescent, float* oLineGap);
//	void GetHMetrics(const UTF32 iCP, float* oAdvance, float* oLeftSideBearing);
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
