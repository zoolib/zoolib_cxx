#include "zoolib/GameEngine/Draw_GL_Shader.h"

#if not defined(__ANDROID__)

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/Log.h"

#include "zoolib/GameEngine/Texture_GL.h"
#include "zoolib/OpenGL/Util.h"

// =================================================================================================
// MARK: -

namespace ZooLib {
namespace GameEngine {

using namespace ZooLib::OpenGL;

namespace { // anonymous

// -----

#define MACRO_ShaderPrefix \
	"#ifdef GL_ES\n" \
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
"	uniform float uStepFactor;"
"	varying vec4 ourPosition;"
"	void main()"
"		{"
"		float distFromEdge = distance(ourPosition.xy, vec2(1.0));"
"		float derivX = dFdx(distFromEdge);"
"  		float derivY = dFdy(distFromEdge);"
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
"		gl_FragColor = texture2D(uTexture, vTexCoord);"
"		gl_FragColor *= uModulation;"
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
:	public ZCounted
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
		ZCounted::Initialize();
		
		{
		VertexShaderID theVS =
			*spLoadShader<VertexShaderID>(spVertexShaderSource_Constant);

		FragmentShaderID theFS =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_Constant);

		fProgramID_Constant = ::glCreateProgram();

		::glAttachShader(fProgramID_Constant, theVS);
		::glAttachShader(fProgramID_Constant, theFS);

		spLinkAndCheckProgram(fProgramID_Constant);

		::glDetachShader(fProgramID_Constant, theVS);
		::glDetachShader(fProgramID_Constant, theFS);

		::glDeleteShader(theVS);
		::glDeleteShader(theFS);
		}

		fUniform_Constant_Projection = ::glGetUniformLocation(fProgramID_Constant, "uProjection");
		fAttribute_Constant_Pos = ::glGetAttribLocation(fProgramID_Constant, "aPos");

		fUniform_Constant_Color = ::glGetUniformLocation(fProgramID_Constant, "uColor");

		{
		VertexShaderID theVS =
			*spLoadShader<VertexShaderID>(spVertexShaderSource_Constant);

		FragmentShaderID theFS =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_RAS);

		fProgramID_RAS = ::glCreateProgram();

		::glAttachShader(fProgramID_RAS, theVS);
		::glAttachShader(fProgramID_RAS, theFS);

		spLinkAndCheckProgram(fProgramID_RAS);

		::glDetachShader(fProgramID_RAS, theVS);
		::glDetachShader(fProgramID_RAS, theFS);

		::glDeleteShader(theVS);
		::glDeleteShader(theFS);
		}

		fUniform_RAS_Projection = ::glGetUniformLocation(fProgramID_RAS, "uProjection");
		fAttribute_RAS_Pos = ::glGetAttribLocation(fProgramID_RAS, "aPos");
		fUniform_RAS_Color_Concave = ::glGetUniformLocation(fProgramID_RAS, "uColor_Concave");
		fUniform_RAS_Color_Convex = ::glGetUniformLocation(fProgramID_RAS, "uColor_Convex");
		fUniform_RAS_StepFactor = ::glGetUniformLocation(fProgramID_RAS, "uStepFactor");

		{
		VertexShaderID theVS =
			*spLoadShader<VertexShaderID>(spVertexShaderSource_Textured);

		FragmentShaderID theFS =
			*spLoadShader<FragmentShaderID>(spFragmentShaderSource_Textured);

		fProgramID_Textured = ::glCreateProgram();

		::glAttachShader(fProgramID_Textured, theVS);
		::glAttachShader(fProgramID_Textured, theFS);

		spLinkAndCheckProgram(fProgramID_Textured);

		::glDetachShader(fProgramID_Textured, theVS);
		::glDetachShader(fProgramID_Textured, theFS);
		::glDeleteShader(theVS);
		::glDeleteShader(theFS);
		}

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
		GLint fUniform_RAS_StepFactor;

	ProgramID fProgramID_Textured;
		GLint fUniform_Textured_Projection;
		GLint fAttribute_Textured_Tex;
		GLint fAttribute_Textured_Pos;

		GLint fUniform_Textured_Texture;
		GLint fUniform_Textured_Modulation;
	};

ZRef<Context> spContext()
	{
	static ZRef<Context> spContext = new Context;
	return spContext;
	}

void spDrawTexture(
	TextureID iTextureID,
	GPoint iSize,
	const GRect& iBounds,
	const AlphaMat& iAlphaMat)
	{
	ZRef<Context> theContext = spContext();
	
	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_Textured);

	::glUniform4f(theContext->fUniform_Textured_Modulation,
		iAlphaMat.fAlpha, iAlphaMat.fAlpha, iAlphaMat.fAlpha, iAlphaMat.fAlpha);

	::glUniformMatrix4fv(theContext->fUniform_Textured_Projection,
		1, false, &iAlphaMat.fMat.fE[0][0]);

	const GRect insetBounds = sInsetted(iBounds, 0.5, 0.5);
	GPoint texCoords[4];
	texCoords[0] = LT(insetBounds) / iSize;
	texCoords[1] = RT(insetBounds) / iSize;
	texCoords[2] = LB(insetBounds) / iSize;
	texCoords[3] = RB(insetBounds) / iSize;

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

void spSetUniform_RGBA(GLint uniform, const ZRGBA& iRGBA, Alpha iAlpha)
	{
	float theAlpha = iRGBA.floatAlpha() * sGet(iAlpha);
	::glUniform4f(
		uniform,
		iRGBA.floatRed() * theAlpha,
		iRGBA.floatGreen() * theAlpha,
		iRGBA.floatBlue() * theAlpha,
		theAlpha);
	}

void spDrawRect(const AlphaMat& iAlphaMat,
	const ZRGBA& iRGBA,
	const GRect& iRect)
	{
	ZRef<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_Constant);

	spSetUniform_RGBA(theContext->fUniform_Constant_Color, iRGBA, iAlphaMat.fAlpha);

	::glUniformMatrix4fv(
		theContext->fUniform_Constant_Projection,
		1, false, &iAlphaMat.fMat.fE[0][0]);

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

void spDrawRightAngleSegment(const AlphaMat& iAlphaMat,
	const ZRGBA& iRGBA_Convex, const ZRGBA& iRGBA_Concave, float iStepFactor)
	{
	ZRef<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_RAS);

	spSetUniform_RGBA(theContext->fUniform_RAS_Color_Concave, iRGBA_Convex, iAlphaMat.fAlpha);
	spSetUniform_RGBA(theContext->fUniform_RAS_Color_Convex, iRGBA_Concave, iAlphaMat.fAlpha);
	::glUniform1f(theContext->fUniform_Constant_Color, iStepFactor);

	::glUniformMatrix4fv(
		theContext->fUniform_RAS_Projection,
		1, false, &iAlphaMat.fMat.fE[0][0]);

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

void spDrawTriangle(const AlphaMat& iAlphaMat,
	const ZRGBA& iRGBA,
	const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{
	ZRef<Context> theContext = spContext();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	theContext->Use(theContext->fProgramID_Constant);

	spSetUniform_RGBA(theContext->fUniform_Constant_Color, iRGBA, iAlphaMat.fAlpha);

	::glUniformMatrix4fv(
		theContext->fUniform_Constant_Projection,
		1, false, &iAlphaMat.fMat.fE[0][0]);

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

// =================================================================================================
#pragma mark - Visitor_Draw_GL_Shader

Visitor_Draw_GL_Shader::Visitor_Draw_GL_Shader(bool iShowBounds, bool iShowOrigin)
:	fShowBounds(iShowBounds)
,	fShowOrigin(iShowOrigin)
	{
	#if not ZMACRO_IOS
		ZAssert(strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_framebuffer_object"));
	#endif
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Buffer(const ZRef<Rendered_Buffer>& iRendered_Buffer)
	{
	// See also GameEngine_Game_Render.cpp/sGame_Render and
	// Visitor_Draw_GL_Fixed::Visit_Rendered_Buffer.

	ZRef<Texture_GL> theTexture_GL =
		new Texture_GL(sPointPOD(iRendered_Buffer->GetWidth(), iRendered_Buffer->GetHeight()));

	const ZPointPOD theTextureSize = theTexture_GL->GetTextureSize();

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
				const ZRGBA theRGBA = iRendered_Buffer->GetRGBA();
				::glClearColor(
					theRGBA.floatRed(), theRGBA.floatGreen(), theRGBA.floatBlue(), theRGBA.floatAlpha());
				::glClear(GL_COLOR_BUFFER_BIT);

				Mat theMat(1);
				// We start with -1 <= X < 1 and -1 <= Y < 1.

				// Rescale so it's -X(theTextureSize)/2 to X(theTextureSize)/2 and -Y(theTextureSize)/2
				// to Y(theTextureSize)/2, and Z is scaled by 1e-3.
				theMat *= sScale3<Rat>(2.0 / X(theTextureSize), 2.0 / Y(theTextureSize), 1e-3);

				// Translate so it's 0 to X(theTextureSize) and 0 to Y(theTextureSize).
				theMat *= sTranslate3<Rat>(-X(theTextureSize)/2, -Y(theTextureSize)/2, 0);

				SaveSetRestore<AlphaGainMat> theSSR(fAlphaGainMat, theMat);
				if (ZRef<Rendered> theRendered = iRendered_Buffer->GetRendered())
					theRendered->Accept_Rendered(*this);

				::glFlush();

				}} // ssr_ViewPort
			}} // ssr_BindFramebuffer

		::glDeleteFramebuffers(1, &fbo);
		}} // ssr_ActiveTexture

	ZRef<Rendered> theRendered = sRendered_Texture(theTexture_GL,
		sRect<GRect>(X(theTextureSize), Y(theTextureSize)));

	theRendered->Accept(*this);

	::glFlush();
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Rect(const ZRef<Rendered_Rect>& iRendered_Rect)
	{
	ZRGBA theRGBA;
	GRect theRect;
	iRendered_Rect->Get(theRGBA, theRect);
	spDrawRect(sAlphaMat(fAlphaGainMat), theRGBA, theRect);
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_RightAngleSegment(
	const ZRef<Rendered_RightAngleSegment>& iRendered_RightAngleSegment)
	{
	ZRGBA theRGBA_Convex, theRGBA_Concave;
	iRendered_RightAngleSegment->Get(theRGBA_Convex, theRGBA_Concave);
	spDrawRightAngleSegment(sAlphaMat(fAlphaGainMat), theRGBA_Convex, theRGBA_Concave, 0.005);
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Texture(const ZRef<Rendered_Texture>& iRendered_Texture)
	{
	if (const ZRef<Texture_GL>& theTexture_GL =
		iRendered_Texture->GetTexture().DynamicCast<Texture_GL>())
		{
		TextureID theTextureID;
		ZPointPOD theSize;
		theTexture_GL->Get(theTextureID, theSize);
		spDrawTexture(theTextureID, sPoint<GPoint>(theSize),
			iRendered_Texture->GetBounds(), sAlphaMat(fAlphaGainMat));

		if (fShowBounds)
			{
			const GRect theBounds = sAlignedLT(iRendered_Texture->GetBounds(), 0, 0);

			// Red = left/top
			// Green = right/bottom
			// Blue = Horizontal

			// Left/Port/Izquierda
			sRef(new Rendered_Line(ZRGBA::sRed, LT(theBounds), LB(theBounds), 1.0))
				->Accept(*this);

			// Right/Starboard/Derecha
			sRef(new Rendered_Line(ZRGBA::sGreen, RT(theBounds), RB(theBounds), 1.0))
				->Accept(*this);

			// Top/Above/Aloft/Arriba/Apex
			sRef(new Rendered_Line(ZRGBA::sRed + ZRGBA::sBlue, LT(theBounds), RT(theBounds), 1.0))
				->Accept(*this);

			// Bottom/Below/Beneath/Abajo/Base
			sRef(new Rendered_Line(ZRGBA::sGreen + ZRGBA::sBlue, LB(theBounds), RB(theBounds), 1.0))
				->Accept(*this);
			}
		}
	}

void Visitor_Draw_GL_Shader::Visit_Rendered_Triangle(
	const ZRef<Rendered_Triangle>& iRendered_Triangle)
	{
	ZRGBA theRGBA;
	GPoint theP0, theP1, theP2;
	iRendered_Triangle->Get(theRGBA, theP0, theP1, theP2);
	spDrawTriangle(sAlphaMat(fAlphaGainMat), theRGBA, theP0, theP1, theP2);
	}

void Visitor_Draw_GL_Shader::UseFixedProgram()
	{
	spContext()->Use(0);
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // not defined(__ANDROID__)
