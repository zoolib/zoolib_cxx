#include "zoolib/GameEngine/DrawGlyphs.h"
#include "zoolib/GameEngine/Types.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

//  Incomplete text-in-3d-api example, which draws quads properly aligned to be lossless
//

unsigned char ttf_buffer[1<<20];
unsigned char temp_bitmap[512*512];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;

void my_stbtt_initfont(const std::string& iFontPath);
void my_stbtt_initfont(const std::string& iFontPath)
	{
	static bool inited;
	if (inited)
		return;

	inited = true;
	fread(ttf_buffer, 1, 1<<20, fopen(iFontPath.c_str(), "rb"));
	stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	glGenTextures(1, &ftex);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	// can free temp_bitmap at this point
	}

namespace ZooLib {
namespace GameEngine {

using std::vector;
using std::string;

GLuint GetStuff(const string& iString, vector<GPoint>& oTexCoords, vector<GPoint>& oVertices);
GLuint GetStuff(const string& iString, vector<GPoint>& oTexCoords, vector<GPoint>& oVertices)
	{
	float x = 0;
	float y = 0;
	for (auto theChar: iString)
		{
		if (theChar >= 32 && theChar <= 127)
			{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, 512, 512, theChar - 32, &x, &y , &q, 1);//1=opengl & d3d10+,0=d3d9

			oTexCoords.push_back(sGPoint(q.s0, q.t0));
			oTexCoords.push_back(sGPoint(q.s1, q.t0));
			oTexCoords.push_back(sGPoint(q.s0, q.t1));
			oTexCoords.push_back(sGPoint(q.s1, q.t1));

			oVertices.push_back(sGPoint(q.x0, q.y0));
			oVertices.push_back(sGPoint(q.x1, q.y0));
			oVertices.push_back(sGPoint(q.x0, q.y1));
			oVertices.push_back(sGPoint(q.x1, q.y1));
			}
		}
	return ftex;
	}

} // namespace GameEngine
} // namespace ZooLib
