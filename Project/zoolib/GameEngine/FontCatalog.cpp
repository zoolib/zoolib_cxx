// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Texture_GL.h"

#include "zoolib/Pixels/Formats.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_STL_map.h"

// ----------

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"

	#define STB_TRUETYPE_IMPLEMENTATION // force following include to generate implementation
	#include "stb_truetype.h"

#pragma clang diagnostic pop

// ----------

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

GRect sMeasure(const ZP<FontInfo>& iFontInfo, Rat iScale, const string8& iString)
	{
	GRect theBounds;
	Rat accumulatedX = 0;
	UTF32 priorCP;
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
				if (ZQ<Rat> theKernQ = iFontInfo->QKern(iScale, priorCP, theCP))
					accumulatedX += *theKernQ;
				theBounds |= sOffsettedX(theRect, accumulatedX);
				}
			priorCP = theCP;
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

ZQ<Rat> FontStrike::QKern(UTF32 iCP, UTF32 iCPNext)
	{ return null; }

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

ZQ<Rat> FontInfo::QKern(Rat iScale, UTF32 iCP, UTF32 iCPNext)
	{ return null; }

// =================================================================================================
#pragma mark - FontStrike_TT declaration

class FontInfo_TT;

class FontStrike_TT
:	public FontStrike
	{
public:
	FontStrike_TT(const ZP<FontInfo_TT>& iFontInfo, Rat iScale,
		ZP<Texture> iTexture, const map<UTF32,stbtt_bakedchar>& iBakedChar);

	virtual ZP<Texture> GetGlyphTexture(UTF32 iCP,
		GRect& oGlyphBoundsInTexture, GPoint& oOffset, Rat& oXAdvance);

	virtual GRect Measure(UTF32 iCP);

	virtual void VMetrics(Rat& oAscent, Rat& oDescent, Rat& oLeading);

	virtual ZQ<Rat> QKern(UTF32 iCP, UTF32 iCPNext);

	const ZP<FontInfo_TT> fFontInfo;
	const Rat fScale;
	const ZP<Texture> fTexture;
	const map<UTF32,stbtt_bakedchar> fBakedChar;
	};

GRect sMeasure(const ZP<FontStrike>& iFontStrike, const string8& iString)
	{
	ZP<FontStrike_TT> theFS = iFontStrike.StaticCast<FontStrike_TT>();
	return sMeasure(theFS->fFontInfo, theFS->fScale, iString);
	}

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
	virtual ZP<FontStrike> GetStrikeForScale(Rat iScale);

	virtual void Measure(Rat iScale, UTF32 iCP, GRect& oBounds, Rat& oXAdvance);

	virtual void VMetrics(Rat iScale, Rat& oAscent, Rat& oDescent, Rat& oLeading);

	virtual ZQ<Rat> QKern(Rat iScale, UTF32 iCP, UTF32 iCPNext);

	const Data_Any fTTData;
	const unsigned char* fTTPtr;

	stbtt_fontinfo f_fontinfo;
	map<Rat,ZP<FontStrike_TT>> fStrikes;
	};

// =================================================================================================
#pragma mark - FontCatalog_TT declaration

class FontCatalog_TT
:	public FontCatalog
	{
public:
	FontCatalog_TT(const FileSpec& iFileSpec);
	virtual ~FontCatalog_TT();

	virtual ZP<FontInfo> GetFontInfo(const string8& iName);

	FileSpec fFileSpec;
	map<string8,ZP<FontInfo_TT>> fInfos;
	};

// =================================================================================================
#pragma mark - FontStrike_TT definition

FontStrike_TT::FontStrike_TT(const ZP<FontInfo_TT>& iFontInfo, Rat iScale,
	ZP<Texture> iTexture, const map<UTF32,stbtt_bakedchar>& iBakedChar)
:	fFontInfo(iFontInfo)
,	fScale(iScale)
,	fTexture(iTexture)
,	fBakedChar(iBakedChar)
	{}

ZP<Texture> FontStrike_TT::GetGlyphTexture(UTF32 iCP,
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

ZQ<Rat> FontStrike_TT::QKern(UTF32 iCP, UTF32 iCPNext)
	{ return fFontInfo->QKern(fScale, iCP, iCPNext); }

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
,	fTTPtr((const unsigned char*)fTTData.GetPtr())
	{
	stbtt_InitFont(&f_fontinfo, fTTPtr, stbtt_GetFontOffsetForIndex(fTTPtr, 0));
	}

FontInfo_TT::~FontInfo_TT()
	{}

Rat FontInfo_TT::GetScaleForEmHeight(Rat iEmHeight)
	{ return stbtt_ScaleForMappingEmToPixels(&f_fontinfo, iEmHeight); }

Rat FontInfo_TT::GetScaleForPixelHeight(Rat iPixelHeight)
	{ return stbtt_ScaleForPixelHeight(&f_fontinfo, iPixelHeight); }

ZP<FontStrike> FontInfo_TT::GetStrikeForScale(Rat iScale)
	{
	if (ZP<FontStrike> theStrike = sGet(fStrikes, iScale))
		return theStrike;

	Pixmap thePixmap = sPixmap(sPointPOD(1024,1024), Pixels::EFormatStandard::Alpha_8, sRGBA(0,0));

	const UTF32 firstCP = 32;
	const UTF32 lastCP = 127;

	map<UTF32,stbtt_bakedchar> theMap;
	/*int heightConsumed = */spBakeForScale(f_fontinfo,
		iScale,
		(unsigned char*)thePixmap.GetBaseAddress(), thePixmap.Width(), thePixmap.Height(),
		firstCP, lastCP - firstCP + 1,
		theMap);

	ZP<Texture> theTexture = new Texture_GL(thePixmap);

	ZP<FontStrike_TT> theStrike = new FontStrike_TT(this, iScale, theTexture, theMap);
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

ZQ<Rat> FontInfo_TT::QKern(Rat iScale, UTF32 iCP, UTF32 iCPNext)
	{
	int kern = stbtt_GetCodepointKernAdvance(&f_fontinfo, iCP, iCPNext);
	return kern * iScale;
	}

// =================================================================================================
#pragma mark - FontCatalog_TT definition

FontCatalog_TT::FontCatalog_TT(const FileSpec& iFileSpec)
:	fFileSpec(iFileSpec)
	{}

FontCatalog_TT::~FontCatalog_TT()
	{}

ZP<FontInfo> FontCatalog_TT::GetFontInfo(const string8& iName)
	{
	if (ZP<FontInfo_TT> theInfo = sGet(fInfos, iName))
		return theInfo;

	if (ZP<ChannerR_Bin> theChannerR = fFileSpec.Child(iName).OpenR())
		{
		Data_Any theTTData = sReadAll_T<Data_Any>(*theChannerR);
		ZP<FontInfo_TT> theFontInfo = new FontInfo_TT(theTTData);
		fInfos[iName] = theFontInfo;
		return theFontInfo;
		}
	return null;
	}

// =================================================================================================
#pragma mark - FontCatalog

ZP<FontCatalog> sMakeFontCatalog(const FileSpec& iFileSpec)
	{ return new FontCatalog_TT(iFileSpec); }

} // namespace GameEngine
} // namespace ZooLib
