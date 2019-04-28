#ifndef __ZooLib_GameEngine_FontCatalog_h__
#define __ZooLib_GameEngine_FontCatalog_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

#include "zoolib/GameEngine/Texture.h"
#include "zoolib/GameEngine/Types.h" // For GPoint, GRect, Rat

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - FontStrike

class FontStrike
:	public ZCounted
	{
public:
	virtual ZRef<Texture> GetGlyphTexture(UTF32 iCP,
		GRect& oGlyphBoundsInTexture, GPoint& oOffset, Rat& oXAdvance) = 0;

	virtual GRect Measure(UTF32 iCP) = 0;

	virtual void VMetrics(Rat& oAscent, Rat& oDescent, Rat& oLeading) = 0;

	Rat Ascent();
	Rat Decent();
	Rat Leading();
	};

GRect sMeasure(const ZRef<FontStrike>& iFontStrike, const string8& iString);

// =================================================================================================
#pragma mark - FontInfo

class FontInfo
:	public ZCounted
	{
public:
	virtual Rat GetScaleForEmHeight(Rat iEmHeight) = 0;
	virtual Rat GetScaleForPixelHeight(Rat iPixelHeight) = 0;
	virtual ZRef<FontStrike> GetStrikeForScale(Rat iScale) = 0;

	virtual void Measure(Rat iScale, UTF32 iCP, GRect& oBounds, Rat& oXAdvance) = 0;

	virtual void VMetrics(Rat iScale, Rat& oAscent, Rat& oDescent, Rat& oLeading) = 0;

	Rat Ascent(Rat iScale);
	Rat Decent(Rat iScale);
	Rat Leading(Rat iScale);
	};

GRect sMeasure(const ZRef<FontInfo>& iFontInfo, Rat iScale, const string8& iString);

// =================================================================================================
#pragma mark - FontCatalog

class FontCatalog
:	public ZCounted
	{
public:
	virtual ZRef<FontInfo> GetFontInfo(const string8& iName) = 0;
	};

ZRef<FontCatalog> sMakeFontCatalog(const FileSpec& iFileSpec);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_FontCatalog_h__
