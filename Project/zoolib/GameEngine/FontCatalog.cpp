#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Texture_GL.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/OpenGL/Util.h"

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

namespace ZooLib {
namespace GameEngine {

using std::map;
using std::pair;
using std::vector;

using namespace OpenGL;
using namespace Util_STL;

// =================================================================================================
#pragma mark -

namespace { // anonymous


} // anonymous namespace

// =================================================================================================
#pragma mark - FontStrike_TT declaration

class FontStrike_TT
:	public FontStrike
	{
public:
	FontStrike_TT(ZRef<Texture> iTexture, const map<UTF32,stbtt_bakedchar>& iBakedChar);

	virtual ZRef<Texture> GetGlyphTexture(UTF32 iCP,
		GRect& oGlyphBounds, GPoint& oOffset, Rat& oXAdvance);

	ZRef<Texture> fTexture;
	map<UTF32,stbtt_bakedchar> fBakedChar;
	};

// =================================================================================================
#pragma mark - FontCatalog_TT declaration

class FontCatalog_TT
:	public FontCatalog
	{
public:
	FontCatalog_TT(const FileSpec& iFileSpec);
	virtual ~FontCatalog_TT();

	virtual ZRef<FontStrike> GetFontStrike(const FontSpec& iFontSpec);

	const FileSpec fFileSpec;
	map<pair<Name,Rat>,ZRef<FontStrike_TT>> fStrikes;
	};

// =================================================================================================
#pragma mark - FontCatalog_TT definition

FontCatalog_TT::FontCatalog_TT(const FileSpec& iFileSpec)
:	fFileSpec(iFileSpec)
	{}

FontCatalog_TT::~FontCatalog_TT()
	{}

ZRef<FontStrike> FontCatalog_TT::GetFontStrike(const FontSpec& iFontSpec)
	{
	pair<Name,Rat> theKey(iFontSpec.fName, iFontSpec.fSize);

	if (ZRef<FontStrike> theStrike = sGet(fStrikes, theKey))
		return theStrike;

	if (ZRef<ChannerR_Bin> theChannerR = fFileSpec.Child(iFontSpec.fName).OpenR())
		{
		Data_Any theTTData = sReadAll_T<Data_Any>(*theChannerR);

		vector<uint8> tempBitmap(512*512, 0);

		vector<stbtt_bakedchar> theVec;
		theVec.resize(96);

		stbtt_BakeFontBitmap((unsigned char*)theTTData.GetPtr(), 0, iFontSpec.fSize,
			&tempBitmap[0], 512, 512, 32, theVec.size(), &theVec[0]);

		SaveSetRestore_ActiveTexture ssr_ActiveTexture(GL_TEXTURE0);

		GLuint theTextureID;
		::glGenTextures(1, &theTextureID);

		SaveSetRestore_BindTexture_2D ssr_BindTexture_2D(theTextureID);

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#if defined(GL_VERSION_2_1)
			// GL_CLAMP_TO_BORDER_EXT ??
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		#else
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		#endif

		::glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
			512, 512, 0,
			GL_ALPHA, GL_UNSIGNED_BYTE, &tempBitmap[0]);

		ZRef<Texture> theTexture = new Texture_GL(sPoint<ZPointPOD>(512, 512), theTextureID, true);
		map<UTF32,stbtt_bakedchar> theMap;
		for (size_t xx = 0; xx < theVec.size(); ++xx)
			theMap[UTF32(32) + xx] = theVec[xx];

		ZRef<FontStrike_TT> theStrike = new FontStrike_TT(theTexture, theMap);
		fStrikes[theKey] = theStrike;
		return theStrike;
		}
	return null;
	}

// =================================================================================================
#pragma mark - FontStrike_TT definition

FontStrike_TT::FontStrike_TT(ZRef<Texture> iTexture, const map<UTF32,stbtt_bakedchar>& iBakedChar)
:	fTexture(iTexture)
,	fBakedChar(iBakedChar)
	{}

ZRef<Texture> FontStrike_TT::GetGlyphTexture(UTF32 iCP,
	GRect& oGlyphBounds, GPoint& oOffset, Rat& oXAdvance)
	{
	if (const stbtt_bakedchar* theP = sPGet(fBakedChar, iCP))
		{
		L(oGlyphBounds) = theP->x0;
		T(oGlyphBounds) = theP->y0;
		R(oGlyphBounds) = theP->x1;
		B(oGlyphBounds) = theP->y1;
		X(oOffset) = theP->xoff;
		Y(oOffset) = theP->yoff;
		oXAdvance = theP->xadvance;
		return fTexture;
		}
	return null;
	}

// =================================================================================================
#pragma mark - FontCatalog

ZRef<FontCatalog> sMakeFontCatalog(const FileSpec& iFileSpec)
	{ return new FontCatalog_TT(iFileSpec); }

} // namespace GameEngine
} // namespace ZooLib
