// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Draw_GL_Shader.h"

#if ZMACRO_CanUseShader

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/Log.h"

#include "zoolib/GameEngine/Texture_GL.h"
#include "zoolib/OpenGL/Util.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace GameEngine {

using namespace ZooLib::OpenGL;
using std::string;
using std::vector;

GLuint GetStuff(const string& iString, vector<GPoint>& oTexCoords, vector<GPoint>& oVertices);

namespace { // anonymous

// -----

#define MACRO_ShaderPrefix \
	"#ifdef GL_ES\n" \
	"	#extension GL_OES_standard_derivatives : enable\n" \
	"	precision mediump float;\n" \
	"#endif\n" \

const char spVertexShaderSource_Constant[] = ""
MACRO_ShaderPrefix
"	uniform mat4 uProjection;"
"	attribute vec4 aPos;"
"	varying vec4 ourPosition;"

"	void main()"
"		{"
"		gl_Position = uProjection * aPos;"
"		ourPosition = aPos;"
"		}"
"";

const char spFragmentShaderSource_Constant[] = ""
MACRO_ShaderPrefix
"	uniform vec4 uColor;"

"	void main()"
"		{"
"		gl_FragColor = uColor;"
"		}"
"";

const char spFragmentShaderSource_RAS[] = ""
MACRO_ShaderPrefix
"	uniform vec4 uColor_Convex;"
"	uniform vec4 uColor_Concave;"
"	varying vec4 ourPosition;"
"	void main()"
"		{"
"		float distFromEdge = distance(ourPosition.xy, vec2(1.0));"
"		float derivX = dFdx(distFromEdge);"
"		float derivY = dFdy(distFromEdge);"
"		float gradientLength = length(vec2(derivX, derivY));"
"		float thresholdWidth = 1.0 * gradientLength;"
"		float theRatio = smoothstep(1.0-thresholdWidth, 1.0+thresholdWidth, distFromEdge);"
"		gl_FragColor = mix(uColor_Concave, uColor_Convex, theRatio);"
"		}"
"";

// -----

const char spVertexShaderSource_Textured[] = ""
MACRO_ShaderPrefix
"	uniform mat4 uProjection;"
"	attribute vec2 aTex;"
"	attribute vec4 aPos;"
"	varying vec2 vTexCoord;"

"	void main()"
"		{"
"		gl_Position = uProjection * aPos;"
"		vTexCoord = aTex;"
"		}"
"";

const char spFragmentShaderSource_Textured[] = ""
MACRO_ShaderPrefix
"	uniform sampler2D uTexture;"
"	uniform vec4 uModulation;"
"	varying vec2 vTexCoord;"

"	void main()"
"		{"
"		gl_FragColor = uModulation * texture2D(uTexture, vTexCoord);"
"		}"
"";

const char spFragmentShaderSource_AlphaOnly[] = ""
MACRO_ShaderPrefix
"	uniform sampler2D uTexture;"
"	uniform vec4 uModulation;"
"	varying vec2 vTexCoord;"

"	void main()"
"		{"
"		gl_FragColor = uModulation * vec4(texture2D(uTexture, vTexCoord).a);"
"		}"
"";

// -----

typedef TagVal<GLuint,struct Tag_FragmentShaderID> FragmentShaderID;
typedef TagVal<GLuint,struct Tag_VertexShaderID> VertexShaderID;
typedef TagVal<GLuint,struct Tag_ProgramID> ProgramID;

template <class Shader_p>
GLenum spTypeOf();

template <>
GLenum spTypeOf<FragmentShaderID>() { return GL_FRAGMENT_SHADER; }

template <>
GLenum spTypeOf<VertexShaderID>() { return GL_VERTEX_SHADER; }

bool spCompileOrDeleteShader(GLuint iShaderID, const string8& iSource)
	{
	const char* srcPtr = iSource.c_str();
	::glShaderSource(iShaderID, 1, &srcPtr, nullptr);
	::glCompileShader(iShaderID);

	GLint result;
	::glGetShaderiv(iShaderID, GL_COMPILE_STATUS, &result);

	if (result)
		return true;

	if (ZLOGF(w, eNotice))
		{
		GLint length;
		::glGetShaderiv(iShaderID, GL_INFO_LOG_LENGTH, &length);

		string8 theString(length, 0);
		::glGetShaderInfoLog(iShaderID, length, nullptr, &theString[0]);
		w << theString;
		}

	::glDeleteShader(iShaderID);
	return false;
	}

template <class Shader_p>
ZQ<Shader_p> spLoadShader(const string8& iSource)
	{
	if (GLuint shaderID = ::glCreateShader(spTypeOf<Shader_p>()))
		{
		if (spCompileOrDeleteShader(shaderID, iSource))
			return shaderID;
		}
	return null;
	}

bool spLinkAndCheckProgram(ProgramID iProgram)
	{
	::glLinkProgram(iProgram);

	GLint result;
	::glGetProgramiv(iProgram, GL_LINK_STATUS, &result);
	if (result)
		return true;

	if (ZLOGF(w, eNotice))
		{
		GLint length;
		::glGetProgramiv(iProgram, GL_INFO_LOG_LENGTH, &length);
		string8 theString(length, 0);
		::glGetShaderInfoLog(iProgram, length, nullptr, &theString[0]);
		w << theString;
		}
	return false;
	}

// =================================================================================================
#pragma mark - Context

class Context
:	public Counted
	{
public:
	Context()
	:	fProgramID_Current(-1)
		{}

	ProgramID fProgramID_Current;
	
	void Use(const ProgramID& iID)
		{
		if (sQSet(fProgramID_Current, iID))
			::glUseProgram(fProgramID_Current);
		}

	virtual void Initialize()
		{
		Counted::Initialize();
		
		{
		VertexShaderID theVS_Constant =
			*spLoadShader<VertexShaderID>(spVertexShaderSource_Constant);

		FragmentShaderID theFS_Constant =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_Constant);

		fProgramID_Constant = ::glCreateProgram();

		::glAttachShader(fProgramID_Constant, theVS_Constant);
		::glAttachShader(fProgramID_Constant, theFS_Constant);

		spLinkAndCheckProgram(fProgramID_Constant);

		::glDetachShader(fProgramID_Constant, theVS_Constant);
		::glDetachShader(fProgramID_Constant, theFS_Constant);

		::glDeleteShader(theVS_Constant);
		::glDeleteShader(theFS_Constant);
		}

		fUniform_Constant_Projection = ::glGetUniformLocation(fProgramID_Constant, "uProjection");
		fAttribute_Constant_Pos = ::glGetAttribLocation(fProgramID_Constant, "aPos");

		fUniform_Constant_Color = ::glGetUniformLocation(fProgramID_Constant, "uColor");

		{
		VertexShaderID theVS_RAS =
			*spLoadShader<VertexShaderID>(spVertexShaderSource_Constant);

		FragmentShaderID theFS_RAS =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_RAS);

		fProgramID_RAS = ::glCreateProgram();

		::glAttachShader(fProgramID_RAS, theVS_RAS);
		::glAttachShader(fProgramID_RAS, theFS_RAS);

		spLinkAndCheckProgram(fProgramID_RAS);

		::glDetachShader(fProgramID_RAS, theVS_RAS);
		::glDetachShader(fProgramID_RAS, theFS_RAS);

		::glDeleteShader(theVS_RAS);
		::glDeleteShader(theFS_RAS);
		}

		fUniform_RAS_Projection = ::glGetUniformLocation(fProgramID_RAS, "uProjection");
		fAttribute_RAS_Pos = ::glGetAttribLocation(fProgramID_RAS, "aPos");
		fUniform_RAS_Color_Concave = ::glGetUniformLocation(fProgramID_RAS, "uColor_Concave");
		fUniform_RAS_Color_Convex = ::glGetUniformLocation(fProgramID_RAS, "uColor_Convex");

		{
		VertexShaderID theVS_Textured =
			*spLoadShader<VertexShaderID>(spVertexShaderSource_Textured);

		FragmentShaderID theFS_Textured =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_Textured);

		fProgramID_Textured = ::glCreateProgram();

		::glAttachShader(fProgramID_Textured, theVS_Textured);
		::glAttachShader(fProgramID_Textured, theFS_Textured);

		spLinkAndCheckProgram(fProgramID_Textured);

		::glDetachShader(fProgramID_Textured, theVS_Textured);
		::glDetachShader(fProgramID_Textured, theFS_Textured);


		FragmentShaderID theFS_AlphaOnly =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_AlphaOnly);

		fProgramID_AlphaOnly = ::glCreateProgram();

		::glAttachShader(fProgramID_AlphaOnly, theVS_Textured);
		::glAttachShader(fProgramID_AlphaOnly, theFS_AlphaOnly);

		spLinkAndCheckProgram(fProgramID_AlphaOnly);

		::glDetachShader(fProgramID_AlphaOnly, theVS_Textured);
		::glDetachShader(fProgramID_AlphaOnly, theFS_AlphaOnly);

		::glDeleteShader(theVS_Textured);
		::glDeleteShader(theFS_Textured);
		::glDeleteShader(theFS_AlphaOnly);
		}

		fUniform_AlphaOnly_Projection = ::glGetUniformLocation(fProgramID_AlphaOnly, "uProjection");
		fAttribute_AlphaOnly_Tex = ::glGetAttribLocation(fProgramID_AlphaOnly, "aTex");
		fAttribute_AlphaOnly_Pos = ::glGetAttribLocation(fProgramID_AlphaOnly, "aPos");

		fUniform_AlphaOnly_Texture = ::glGetUniformLocation(fProgramID_AlphaOnly, "uTexture");
		fUniform_AlphaOnly_Modulation = ::glGetUniformLocation(fProgramID_AlphaOnly, "uModulation");

		fUniform_Textured_Projection = ::glGetUniformLocation(fProgramID_Textured, "uProjection");
		fAttribute_Textured_Tex = ::glGetAttribLocation(fProgramID_Textured, "aTex");
		fAttribute_Textured_Pos = ::glGetAttribLocation(fProgramID_Textured, "aPos");

		fUniform_Textured_Texture = ::glGetUniformLocation(fProgramID_Textured, "uTexture");
		fUniform_Textured_Modulation = ::glGetUniformLocation(fProgramID_Textured, "uModulation");
		}

	ProgramID fProgramID_Constant;
		GLint fUniform_Constant_Projection;
		GLint fAttribute_Constant_Pos;
		GLint fUniform_Constant_Color;

	ProgramID fProgramID_RAS;
		GLint fUniform_RAS_Projection;
		GLint fAttribute_RAS_Pos;
		GLint fUniform_RAS_Color_Concave;
		GLint fUniform_RAS_Color_Convex;

	ProgramID fProgramID_Textured;
		GLint fUniform_Textured_Projection;
		GLint fAttribute_Textured_Tex;
		GLint fAttribute_Textured_Pos;

		GLint fUniform_Textured_Texture;
		GLint fUniform_Textured_Modulation;

	ProgramID fProgramID_AlphaOnly;
		GLint fUniform_AlphaOnly_Projection;
		GLint fAttribute_AlphaOnly_Tex;
		GLint fAttribute_AlphaOnly_Pos;

		GLint fUniform_AlphaOnly_Texture;
		GLint fUniform_AlphaOnly_Modulation;
	};

static ZP<Context> spContextObject;

ZP<Context> spContext()
	{
	if (not spContextObject)
		spContextObject = new Context;
	return spContextObject;
	}

void spSetUniform_RGBA(GLint uniform, RGBA iRGBA)
	{
	::glUniform4f(
		uniform,
		sRed(iRGBA) * sAlpha(iRGBA),
		sGreen(iRGBA) * sAlpha(iRGBA),
		sBlue(iRGBA) * sAlpha(iRGBA),
		sAlpha(iRGBA));
	}

void spDrawRect(const Blush& iBlush, const Mat& iMat, const GRect& iRect)
	{
	ZP<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_Constant);

	spSetUniform_RGBA(theContext->fUniform_Constant_Color, iBlush);

	::glUniformMatrix4fv(
		theContext->fUniform_Constant_Projection,
		1, false, &iMat.fE[0][0]);

	GPoint vertices[4];
	vertices[0] = LT(iRect);
	vertices[1] = RT(iRect);
	vertices[2] = LB(iRect);
	vertices[3] = RB(iRect);

	::glEnableVertexAttribArray(theContext->fAttribute_Constant_Pos);
 	::glVertexAttribPointer(theContext->fAttribute_Constant_Pos,
		2, GL_FLOAT, GL_FALSE, 0, vertices);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
	}

void spDrawRightAngleSegment(const Blush& iBlush, const Mat& iMat,
	const RGBA& iRGBA_Convex, const RGBA& iRGBA_Concave)
	{
	ZP<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_RAS);

	spSetUniform_RGBA(theContext->fUniform_RAS_Color_Concave, iBlush * iRGBA_Convex);
	spSetUniform_RGBA(theContext->fUniform_RAS_Color_Convex, iBlush * iRGBA_Concave);

	::glUniformMatrix4fv(
		theContext->fUniform_RAS_Projection,
		1, false, &iMat.fE[0][0]);

	GPoint vertices[4];
	vertices[0] = sGPoint(0,0);
	vertices[1] = sGPoint(1,0);
	vertices[2] = sGPoint(0,1);
	vertices[3] = sGPoint(1,1);

	::glEnableVertexAttribArray(theContext->fAttribute_RAS_Pos);
	::glVertexAttribPointer(theContext->fAttribute_RAS_Pos,
		2, GL_FLOAT, GL_FALSE, 0, vertices);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

void spDrawTexture_AlphaOnly(const Blush& iBlush, const Mat& iMat,
	TextureID iTextureID, GPoint iTextureSize,
	const GRect& iBounds)
	{
	ZP<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_AlphaOnly);

	spSetUniform_RGBA(theContext->fUniform_AlphaOnly_Modulation, iBlush);

	::glUniformMatrix4fv(theContext->fUniform_AlphaOnly_Projection,
		1, false, &iMat.fE[0][0]);

	const GRect insetBounds = iBounds;//sInsetted(iBounds, 0.5, 0.5);
	GPoint texCoords[4];
	texCoords[0] = LT(insetBounds) / iTextureSize;
	texCoords[1] = RT(insetBounds) / iTextureSize;
	texCoords[2] = LB(insetBounds) / iTextureSize;
	texCoords[3] = RB(insetBounds) / iTextureSize;

	::glEnableVertexAttribArray(theContext->fAttribute_AlphaOnly_Tex);
	::glVertexAttribPointer(theContext->fAttribute_AlphaOnly_Tex,
		2, GL_FLOAT, GL_FALSE, 0, texCoords);

	GPoint vertices[4];
	vertices[0] = sPoint<GPoint>(0,0);
	vertices[1] = sPoint<GPoint>(W(iBounds),0);
	vertices[2] = sPoint<GPoint>(0, H(iBounds));
	vertices[3] = WH(iBounds);

	::glEnableVertexAttribArray(theContext->fAttribute_AlphaOnly_Pos);
 	::glVertexAttribPointer(theContext->fAttribute_AlphaOnly_Pos,
		2, GL_FLOAT, GL_FALSE, 0, vertices);

	SaveSetRestore_ActiveTexture ssr_ActiveTexture(GL_TEXTURE0);
	SaveSetRestore_BindTexture_2D ssr_BindTexture_2D(iTextureID);

	::glUniform1i(theContext->fUniform_AlphaOnly_Texture, 0);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

void spDrawTexture(const Blush& iBlush, const Mat& iMat,
	TextureID iTextureID, GPoint iTextureSize,
	const GRect& iBounds)
	{
	ZP<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_Textured);

	spSetUniform_RGBA(theContext->fUniform_Textured_Modulation, iBlush);

	::glUniformMatrix4fv(theContext->fUniform_Textured_Projection,
		1, false, &iMat.fE[0][0]);

	const GRect insetBounds = iBounds; // sInsetted(iBounds, 0.5, 0.5);
	GPoint texCoords[4];
	texCoords[0] = LT(insetBounds) / iTextureSize;
	texCoords[1] = RT(insetBounds) / iTextureSize;
	texCoords[2] = LB(insetBounds) / iTextureSize;
	texCoords[3] = RB(insetBounds) / iTextureSize;

	::glEnableVertexAttribArray(theContext->fAttribute_Textured_Tex);
	::glVertexAttribPointer(theContext->fAttribute_Textured_Tex,
		2, GL_FLOAT, GL_FALSE, 0, texCoords);

	GPoint vertices[4];
	vertices[0] = sPoint<GPoint>(0,0);
	vertices[1] = sPoint<GPoint>(W(iBounds),0);
	vertices[2] = sPoint<GPoint>(0, H(iBounds));
	vertices[3] = WH(iBounds);

	::glEnableVertexAttribArray(theContext->fAttribute_Textured_Pos);
 	::glVertexAttribPointer(theContext->fAttribute_Textured_Pos,
		2, GL_FLOAT, GL_FALSE, 0, vertices);

	SaveSetRestore_ActiveTexture ssr_ActiveTexture(GL_TEXTURE0);
	SaveSetRestore_BindTexture_2D ssr_BindTexture_2D(iTextureID);

	::glUniform1i(theContext->fUniform_Textured_Texture, 0);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

void spDrawTriangle(const Blush& iBlush,
	const Mat& iMat,
	const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{
	ZP<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_Constant);

	spSetUniform_RGBA(theContext->fUniform_Constant_Color, iBlush);

	::glUniformMatrix4fv(
		theContext->fUniform_Constant_Projection,
		1, false, &iMat.fE[0][0]);

	GPoint vertices[3];
	vertices[0] = iP0;
	vertices[1] = iP1;
	vertices[2] = iP2;

	::glEnableVertexAttribArray(theContext->fAttribute_Constant_Pos);
	::glVertexAttribPointer(theContext->fAttribute_Constant_Pos,
		2, GL_FLOAT, GL_FALSE, 0, vertices);

	::glDrawArrays(GL_TRIANGLES, 0, 3);
	}

} // anonymous namespace

void sKillContext()
	{
	spContextObject.Clear();
	}

// =================================================================================================
#pragma mark - Visitor_Draw_GL_Shader

Visitor_Draw_GL_Shader::Visitor_Draw_GL_Shader(
	bool iShowBounds, bool iShowBounds_AlphaOnly, bool iShowOrigin)
:	fShowBounds(iShowBounds)
,	fShowBounds_AlphaOnly(iShowBounds_AlphaOnly)
,	fShowOrigin(iShowOrigin)
	{
	#if not ZMACRO_IOS and not defined(ANDROID)
		ZAssert(strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_framebuffer_object"));
	#endif
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Buffer(const ZP<Rendered_Buffer>& iRendered_Buffer)
	{
	// See also GameEngine_Game_Render.cpp/sGame_Render and
	// Visitor_Draw_GL_Fixed::Visit_Rendered_Buffer.

	const PointPOD theTextureSize =
		sPointPOD(iRendered_Buffer->GetWidth(), iRendered_Buffer->GetHeight());

	ZP<Texture_GL> theTexture_GL = new Texture_GL(theTextureSize);

	{SaveSetRestore_ActiveTexture ssr_ActiveTexture(GL_TEXTURE0);
		{
		GLuint fbo;
		::glGenFramebuffers(1, &fbo);

		{SaveSetRestore_BindFramebuffer ssr_BindFramebuffer(fbo);
			{
			::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
				theTexture_GL->GetTextureID(), 0);

			ZAssert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

			{SaveSetRestore_ViewPort ssr_ViewPort(0, 0, X(theTextureSize), Y(theTextureSize));
				{
				const RGBA theFill = iRendered_Buffer->GetFill();
				::glClearColor(sRed(theFill) * sAlpha(theFill),
					sGreen(theFill) * sAlpha(theFill),
					sBlue(theFill) * sAlpha(theFill),
					sAlpha(theFill));
				::glClear(GL_COLOR_BUFFER_BIT);

				Mat theMat(1);
				// We start with -1 <= X < 1 and -1 <= Y < 1.

				// Rescale so it's -X(theTextureSize)/2 to X(theTextureSize)/2 and -Y(theTextureSize)/2
				// to Y(theTextureSize)/2, and Z is scaled by 1e-3.
				theMat *= sScale3<Rat>(2.0 / X(theTextureSize), 2.0 / Y(theTextureSize), 1e-3);

				// Translate so it's 0 to X(theTextureSize) and 0 to Y(theTextureSize).
				theMat *= sTranslate3<Rat>(-X(theTextureSize)/2, -Y(theTextureSize)/2, 0);

				SaveSetRestore<Mat> theSSR(fMat, theMat);
				if (ZP<Rendered> theRendered = iRendered_Buffer->GetRendered())
					theRendered->Accept_Rendered(*this);

				::glFlush();
				}} // ssr_ViewPort
			}} // ssr_BindFramebuffer

		::glDeleteFramebuffers(1, &fbo);
		}} // ssr_ActiveTexture

	ZP<Rendered> theRendered = sRendered_Texture(theTexture_GL, sRect<GRect>(theTextureSize));

	theRendered->Accept(*this);

	::glFlush();
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Rect(const ZP<Rendered_Rect>& iRendered_Rect)
	{
	GRect theRect;
	iRendered_Rect->Get(theRect);
	spDrawRect(fBlush, fMat, theRect);
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_RightAngleSegment(
	const ZP<Rendered_RightAngleSegment>& iRendered_RightAngleSegment)
	{
	RGBA theRGBA_Convex, theRGBA_Concave;
	iRendered_RightAngleSegment->Get(theRGBA_Convex, theRGBA_Concave);
	spDrawRightAngleSegment(fBlush, fMat, theRGBA_Convex, theRGBA_Concave);
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Texture(const ZP<Rendered_Texture>& iRendered_Texture)
	{
	if (const ZP<Texture_GL>& theTexture_GL =
		iRendered_Texture->GetTexture().DynamicCast<Texture_GL>())
		{
		TextureID theTextureID;
		PointPOD theTextureSize;
		theTexture_GL->Get(theTextureID, theTextureSize);

		bool showBounds;
		if (theTexture_GL->GetIsAlphaOnly())
			{
			spDrawTexture_AlphaOnly(fBlush, fMat,
				theTextureID, sPoint<GPoint>(theTextureSize),
				iRendered_Texture->GetBounds());
			showBounds = fShowBounds_AlphaOnly;
			}
		else
			{
			spDrawTexture(fBlush, fMat,
				theTextureID, sPoint<GPoint>(theTextureSize),
				iRendered_Texture->GetBounds());
			showBounds = fShowBounds;
			}

		if (showBounds)
			{
			SaveRestore<Blush> srBlush(fBlush);

			const GRect theBounds = sAlignedLT(iRendered_Texture->GetBounds(), 0, 0);

			// Red = left/top
			// Green = right/bottom
			// Blue = if horizontal

			// Left/Port/Izquierda
			fBlush = sRGBA(1,0,0,1);
			sAccept(sRendered_Line(LT(theBounds), LB(theBounds), 1.0), *this);

			// Right/Starboard/Derecha
			fBlush = sRGBA(0,1,0,1);
			sAccept(sRendered_Line(RT(theBounds), RB(theBounds), 1.0), *this);

			// Top/Above/Aloft/Arriba/Apex
			fBlush = sRGBA(1,0,1,1);
			sAccept(sRendered_Line(LT(theBounds), RT(theBounds), 1.0), *this);

			// Bottom/Below/Beneath/Abajo/Base
			fBlush = sRGBA(0,1,1,1);
			sAccept(sRendered_Line(LB(theBounds), RB(theBounds), 1.0), *this);
			}
		}
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Triangle(
	const ZP<Rendered_Triangle>& iRendered_Triangle)
	{
	GPoint theP0, theP1, theP2;
	iRendered_Triangle->Get(theP0, theP1, theP2);
	spDrawTriangle(fBlush, fMat, theP0, theP1, theP2);
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // ZMACRO_CanUseShader
