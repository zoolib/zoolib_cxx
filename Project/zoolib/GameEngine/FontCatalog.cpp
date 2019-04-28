#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Texture_GL.h"

#include "zoolib/Pixels/Formats.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_STL_map.h"

#define STB_TRUETYPE_IMPLEMENTATION // force following include to generate implementation
#include "stb_truetype.h"

namespace ZooLib {
namespace GameEngine {

using Pixels::Pixmap;

using std::map;
using std::pair;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark -

namespace { // anonymous


} // anonymous namespace

GRect sMeasure(const ZRef<FontInfo>& iFontInfo, Rat iScale, const string8& iString)
	{
	GRect theBounds;
	Rat accumulatedX = 0;
	bool isFirst = true;
	for (string8::const_iterator iter = iString.begin(), end = iString.end();
		/*no test*/; /*no inc*/)
		{
		UTF32 theCP;
		if (not Unicode::sReadInc(iter, end, theCP))
			break;

		GRect theRect;
		Rat xAdvance;
		iFontInfo->Measure(iScale, theCP, theRect, xAdvance);
		if (not sIsEmpty(theRect))
			{
			if (isFirst)
				{
				isFirst = false;
				theBounds = sOffsettedX(theRect, accumulatedX);
				}
			else
				{
				theBounds |= sOffsettedX(theRect, accumulatedX);
				}
			}
		accumulatedX += xAdvance;
		}
	return theBounds;
	}

// =================================================================================================
#pragma mark - FontStrike

Rat FontStrike::Ascent()
	{
	Rat theAscent, theDescent, theLeading;
	this->VMetrics(theAscent, theDescent, theLeading);
	return theAscent;
	}

Rat FontStrike::Decent()
	{
	Rat theAscent, theDescent, theLeading;
	this->VMetrics(theAscent, theDescent, theLeading);
	return theDescent;
	}

Rat FontStrike::Leading()
	{
	Rat theAscent, theDescent, theLeading;
	this->VMetrics(theAscent, theDescent, theLeading);
	return theLeading;
	}

// =================================================================================================
#pragma mark - FontInfo

Rat FontInfo::Ascent(Rat iScale)
	{
	Rat theAscent, theDescent, theLeading;
	this->VMetrics(iScale, theAscent, theDescent, theLeading);
	return theAscent;
	}

Rat FontInfo::Decent(Rat iScale)
	{
	Rat theAscent, theDescent, theLeading;
	this->VMetrics(iScale, theAscent, theDescent, theLeading);
	return theDescent;
	}

Rat FontInfo::Leading(Rat iScale)
	{
	Rat theAscent, theDescent, theLeading;
	this->VMetrics(iScale, theAscent, theDescent, theLeading);
	return theLeading;
	}

// =================================================================================================
#pragma mark - FontStrike_TT declaration

class FontInfo_TT;

class FontStrike_TT
:	public FontStrike
	{
public:
	FontStrike_TT(const ZRef<FontInfo_TT>& iFontInfo, Rat iScale,
		ZRef<Texture> iTexture, const map<UTF32,stbtt_bakedchar>& iBakedChar);

	virtual ZRef<Texture> GetGlyphTexture(UTF32 iCP,
		GRect& oGlyphBoundsInTexture, GPoint& oOffset, Rat& oXAdvance);

	virtual GRect Measure(UTF32 iCP);
	virtual void VMetrics(Rat& oAscent, Rat& oDescent, Rat& oLeading);

	const ZRef<FontInfo_TT> fFontInfo;
	const Rat fScale;
	const ZRef<Texture> fTexture;
	const map<UTF32,stbtt_bakedchar> fBakedChar;
	};

// =================================================================================================
#pragma mark - FontInfo_TT declaration

class FontInfo_TT
:	public FontInfo
	{
public:
	FontInfo_TT(const Data_Any& iTTData);
	virtual ~FontInfo_TT();

	virtual Rat GetScaleForEmHeight(Rat iEmHeight);
	virtual Rat GetScaleForPixelHeight(Rat iPixelHeight);
	virtual ZRef<FontStrike> GetStrikeForScale(Rat iScale);

	virtual void Measure(Rat iScale, UTF32 iCP, GRect& oBounds, Rat& oXAdvance);
	virtual void VMetrics(Rat iScale, Rat& oAscent, Rat& oDescent, Rat& oLeading);

	const Data_Any fTTData;
	unsigned char* fTTPtr;

	stbtt_fontinfo f_fontinfo;
	map<Rat,ZRef<FontStrike_TT>> fStrikes;
	};

// =================================================================================================
#pragma mark - FontCatalog_TT declaration

class FontCatalog_TT
:	public FontCatalog
	{
public:
	FontCatalog_TT(const FileSpec& iFileSpec);
	virtual ~FontCatalog_TT();

	virtual ZRef<FontInfo> GetFontInfo(const string8& iName);

	FileSpec fFileSpec;
	map<string8,ZRef<FontInfo_TT>> fInfos;
	};

// =================================================================================================
#pragma mark - FontStrike_TT definition

FontStrike_TT::FontStrike_TT(const ZRef<FontInfo_TT>& iFontInfo, Rat iScale,
	ZRef<Texture> iTexture, const map<UTF32,stbtt_bakedchar>& iBakedChar)
:	fFontInfo(iFontInfo)
,	fScale(iScale)
,	fTexture(iTexture)
,	fBakedChar(iBakedChar)
	{}

ZRef<Texture> FontStrike_TT::GetGlyphTexture(UTF32 iCP,
	GRect& oGlyphBoundsInTexture, GPoint& oOffset, Rat& oXAdvance)
	{
	if (const stbtt_bakedchar* theP = sPGet(fBakedChar, iCP))
		{
		L(oGlyphBoundsInTexture) = theP->x0;
		T(oGlyphBoundsInTexture) = theP->y0;
		R(oGlyphBoundsInTexture) = theP->x1;
		B(oGlyphBoundsInTexture) = theP->y1;
		X(oOffset) = theP->xoff;
		Y(oOffset) = theP->yoff;
		oXAdvance = theP->xadvance;
		return fTexture;
		}
	return null;
	}

GRect FontStrike_TT::Measure(UTF32 iCP)
	{
	if (const stbtt_bakedchar* theP = sPGet(fBakedChar, iCP))
		{
		GRect theRect = sGRect(theP->x1 - theP->x0, theP->y1 - theP->y0);
		theRect += sGPoint(theP->xoff, theP->yoff);
		return theRect;
		}
	return sGRect();
	}

void FontStrike_TT::VMetrics(Rat& oAscent, Rat& oDescent, Rat& oLeading)
	{ fFontInfo->VMetrics(fScale, oAscent, oDescent, oLeading); }

// =================================================================================================
#pragma mark - FontInfo_TT definition

static int spBakeForScale(stbtt_fontinfo& f,
	float scale,
	unsigned char* pixels, int pw, int ph,
	int first_char, int num_chars,
	map<UTF32,stbtt_bakedchar>& oMap)
	{
	int x=1;
	int y=1;
	int bottom_y = 1;

	for (int i=0; i < num_chars; ++i)
		{
		int advance, lsb, x0,y0,x1,y1,gw,gh;
		int g = stbtt_FindGlyphIndex(&f, first_char + i);
		stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
		stbtt_GetGlyphBitmapBox(&f, g, scale,scale, &x0, &y0, &x1, &y1);
		gw = x1-x0;
		gh = y1-y0;
		if (x + gw + 1 >= pw)
			{
			// advance to next row
			y = bottom_y;
			x = 1;
			}

		if (y + gh + 1 >= ph) // check if it fits vertically AFTER potentially moving to next row
			return -i;

		STBTT_assert(x+gw < pw);
		STBTT_assert(y+gh < ph);

		stbtt_MakeGlyphBitmap(&f,
			pixels+x+y*pw, // base address
			gw, // remaining horizontal space
			gh, // remaining vertical space
			pw, // Row stride
			scale,scale, // x and y scale
			g // glyph index
			);

		stbtt_bakedchar& theBakedChar = sMut(oMap, first_char+i);

		theBakedChar.x0 = (stbtt_int16) x;
		theBakedChar.y0 = (stbtt_int16) y;
		theBakedChar.x1 = (stbtt_int16) (x + gw);
		theBakedChar.y1 = (stbtt_int16) (y + gh);
		theBakedChar.xadvance = scale * advance;
		theBakedChar.xoff     = (float) x0;
		theBakedChar.yoff     = (float) y0;
		x = x + gw + 1;
		if (y+gh+1 > bottom_y)
			bottom_y = y+gh+1;
		}
	return bottom_y;
	}

FontInfo_TT::FontInfo_TT(const Data_Any& iTTData)
:	fTTData(iTTData)
,	fTTPtr((unsigned char*)fTTData.GetPtr())
	{
	stbtt_InitFont(&f_fontinfo, fTTPtr, stbtt_GetFontOffsetForIndex(fTTPtr,0));
	}

FontInfo_TT::~FontInfo_TT()
	{}

Rat FontInfo_TT::GetScaleForEmHeight(Rat iEmHeight)
	{ return stbtt_ScaleForMappingEmToPixels(&f_fontinfo, iEmHeight); }

Rat FontInfo_TT::GetScaleForPixelHeight(Rat iPixelHeight)
	{ return stbtt_ScaleForPixelHeight(&f_fontinfo, iPixelHeight); }

ZRef<FontStrike> FontInfo_TT::GetStrikeForScale(Rat iScale)
	{
	if (ZRef<FontStrike> theStrike = sGet(fStrikes, iScale))
		return theStrike;

	Pixmap thePixmap = sPixmap(sPointPOD(512,512), Pixels::EFormatStandard::Alpha_8, sRGBA(0,0));

	const UTF32 firstCP = 32;
	const UTF32 lastCP = 127;

	map<UTF32,stbtt_bakedchar> theMap;
	/*int heightConsumed = */spBakeForScale(f_fontinfo,
		iScale,
		(unsigned char*)thePixmap.GetBaseAddress(), thePixmap.Width(), thePixmap.Height(),
		firstCP, lastCP - firstCP + 1,
		theMap);

	ZRef<Texture> theTexture = new Texture_GL(thePixmap);

	ZRef<FontStrike_TT> theStrike = new FontStrike_TT(this, iScale, theTexture, theMap);
	fStrikes[iScale] = theStrike;
	return theStrike;
	}

void FontInfo_TT::Measure(Rat iScale, UTF32 iCP, GRect& oBounds, Rat& oXAdvance)
	{
	int index = stbtt_FindGlyphIndex(&f_fontinfo, iCP);

	int advance, lsb;
	stbtt_GetGlyphHMetrics(&f_fontinfo, index, &advance, &lsb);
	oXAdvance = advance * iScale;

	int  x0, y0, x1, y1;
	stbtt_GetGlyphBitmapBox(&f_fontinfo, index, iScale, iScale, &x0, &y0, &x1, &y1);

	oBounds = sGRect(x0, y0, x1, y1);
	}

void FontInfo_TT::VMetrics(Rat iScale, Rat& oAscent, Rat& oDescent, Rat& oLeading)
	{
	int ascent, descent, linegap;
	stbtt_GetFontVMetrics(&f_fontinfo, &ascent, &descent, &linegap);
	oAscent = ascent * iScale;
	oDescent = descent * iScale;
	oLeading = linegap * iScale;
	}

// =================================================================================================
#pragma mark - FontCatalog_TT definition

FontCatalog_TT::FontCatalog_TT(const FileSpec& iFileSpec)
:	fFileSpec(iFileSpec)
	{}

FontCatalog_TT::~FontCatalog_TT()
	{}

ZRef<FontInfo> FontCatalog_TT::GetFontInfo(const string8& iName)
	{
	if (ZRef<FontInfo_TT> theInfo = sGet(fInfos, iName))
		return theInfo;

	if (ZRef<ChannerR_Bin> theChannerR = fFileSpec.Child(iName).OpenR())
		{
		Data_Any theTTData = sReadAll_T<Data_Any>(*theChannerR);
		ZRef<FontInfo_TT> theFontInfo = new FontInfo_TT(theTTData);
		fInfos[iName] = theFontInfo;
		return theFontInfo;
		}
	return null;
	}

// =================================================================================================
#pragma mark - FontCatalog

ZRef<FontCatalog> sMakeFontCatalog(const FileSpec& iFileSpec)
	{ return new FontCatalog_TT(iFileSpec); }

} // namespace GameEngine
} // namespace ZooLib
