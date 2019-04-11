#include "zoolib/GameEngine/Draw_GL_Fixed.h"

#include "zoolib/OpenGL/Util.h"

#include "zoolib/GameEngine/Texture_GL.h"

namespace ZooLib {
namespace GameEngine {

using namespace OpenGL;

// =================================================================================================
#pragma mark -

static void spBefore(const AlphaMat& iAlphaMat,
	const ZRGBA& iRGBA)
	{
	::glMultMatrixf(&iAlphaMat.fMat[0][0]);
//###########
	const float theAlpha = iRGBA.floatAlpha() * sGet(iAlphaMat.fAlpha);
	::glColor4f(
		iRGBA.floatRed() * theAlpha,
		iRGBA.floatGreen() * theAlpha,
		iRGBA.floatBlue() * theAlpha,
		theAlpha);
	}

namespace { // anonymous

void spDrawTexture(
	TextureID iTextureID,
	GPoint iSize,
	const GRect& iBounds,
	const AlphaMat& iAlphaMat)
	{
	::glPushMatrix();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	spBefore(iAlphaMat, ZRGBA::sWhite);

	SaveSetRestore_Enable ssr_a(GL_TEXTURE_2D, true);

	SaveSetRestore_BindTexture_2D ssr_BindTexture_2D(iTextureID);

	::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//??

	GPoint texCoords[4];
	texCoords[0] = LT(iBounds) / iSize;
	texCoords[1] = RT(iBounds) / iSize;
	texCoords[2] = LB(iBounds) / iSize;
	texCoords[3] = RB(iBounds) / iSize;
	SaveSetRestore_EnableClientState ssr_EnableClientState_a(GL_TEXTURE_COORD_ARRAY, true);
	::glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

	GPoint vertices[4];
	vertices[0] = sPoint<GPoint>(0,0);
	vertices[1] = sPoint<GPoint>(W(iBounds),0);
	vertices[2] = sPoint<GPoint>(0, H(iBounds));
	vertices[3] = WH(iBounds);
	SaveSetRestore_EnableClientState ssr_EnableClientState_b(GL_VERTEX_ARRAY, true);
	::glVertexPointer(2, GL_FLOAT, 0, vertices);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	::glPopMatrix();
	}

void spDrawRect(const AlphaMat& iAlphaMat,
	const ZRGBA& iRGBA,
	const GRect& iRect)
	{
	::glPushMatrix();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	spBefore(iAlphaMat, iRGBA);

	SaveSetRestore_EnableClientState ssr_EnableClientState_b(GL_VERTEX_ARRAY, true);
	SaveSetRestore_EnableClientState ssr_EnableClientState_a(GL_TEXTURE_COORD_ARRAY, false);

	GPoint vertices[4];
	vertices[0] = LT(iRect);
	vertices[1] = RT(iRect);
	vertices[2] = LB(iRect);
	vertices[3] = RB(iRect);

	::glVertexPointer(2, GL_FLOAT, 0, vertices);

	::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	

	::glPopMatrix();
	}

void spDrawTriangle(const AlphaMat& iAlphaMat,
	const ZRGBA& iRGBA,
	const GPoint& iP0, const GPoint& iP1, const GPoint& iP2)
	{
	::glPushMatrix();

	SaveSetRestore_Enable ssr_Enable_BLEND(GL_BLEND, true);
	SaveSetRestore_BlendEquation ssr_BlendEquation(GL_FUNC_ADD);
	SaveSetRestore_BlendFunc ssr_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	spBefore(iAlphaMat, iRGBA);

	SaveSetRestore_EnableClientState ssr_EnableClientState_b(GL_VERTEX_ARRAY, true);
	SaveSetRestore_EnableClientState ssr_EnableClientState_a(GL_TEXTURE_COORD_ARRAY, false);

	GPoint vertices[3];
	vertices[0] = iP0;
	vertices[1] = iP1;
	vertices[2] = iP2;

	::glVertexPointer(2, GL_FLOAT, 0, vertices);

	::glDrawArrays(GL_TRIANGLES, 0, 3);

	::glPopMatrix();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -

Visitor_Draw_GL_Fixed::Visitor_Draw_GL_Fixed(bool iShowBounds, bool iShowOrigin)
:	fShowBounds(iShowBounds)
,	fShowOrigin(iShowOrigin)
	{
	ZAssert(strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_framebuffer_object"));
	}

void Visitor_Draw_GL_Fixed::Visit_Rendered_Buffer(const ZRef<Rendered_Buffer>& iRendered_Buffer)
	{
	// See also GameEngine_Game_Render.cpp/sGame_Render and
	// Visitor_Draw_GL_Shader::Visit_Rendered_Buffer.

	ZRef<Texture_GL> theTexture_GL =
		new Texture_GL(sPointPOD(iRendered_Buffer->GetWidth(), iRendered_Buffer->GetHeight()));

	const ZPointPOD theTextureSize = theTexture_GL->GetTextureSize();

	if (true)
		{
		SaveSetRestore_ActiveTexture ssr_ActiveTexture(GL_TEXTURE0);

		GLuint fbo;
		::glGenFramebuffers(1, &fbo);

		if (true)
			{
			SaveSetRestore_BindFramebuffer ssr_BindFramebuffer(fbo);
			::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
				theTexture_GL->GetTextureID(), 0);

			ZAssert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

			if (true)
				{
				SaveSetRestore_ViewPort ssr_ViewPort(0, 0, X(theTextureSize), Y(theTextureSize));

				::glMatrixMode(GL_PROJECTION);
				::glPushMatrix();
				::glLoadIdentity();
				#if defined(GL_VERSION_2_1)
					::glOrtho(0, X(theTextureSize), 0, Y(theTextureSize), -1000, 1000);
				#else
					::glOrthof(0, X(theTextureSize), 0, Y(theTextureSize), -1000, 1000);
				#endif
				::glMatrixMode(GL_MODELVIEW);

				const ZRGBA theRGBA = iRendered_Buffer->GetRGBA();
				::glClearColor(
					theRGBA.floatRed(), theRGBA.floatGreen(), theRGBA.floatBlue(), theRGBA.floatAlpha());
				::glClear(GL_COLOR_BUFFER_BIT);

				if (true)
					{
					SaveSetRestore<AlphaGainMat> theSSR(fAlphaGainMat, AlphaGainMat());
					if (ZRef<Rendered> theRendered = iRendered_Buffer->GetRendered())
						theRendered->Accept_Rendered(*this);
					} // theSSR

				::glFlush();

				::glMatrixMode(GL_PROJECTION);
				::glPopMatrix();
				::glMatrixMode(GL_MODELVIEW);
				} // ssr_ViewPort
			} // ssr_BindFramebuffer

		::glDeleteFramebuffers(1, &fbo);
		} // ssr_ActiveTexture

	ZRef<Rendered> theRendered = sRendered_Texture(theTexture_GL,
		sRect<GRect>(X(theTextureSize), Y(theTextureSize)));

	theRendered->Accept(*this);

	::glFlush();
	}

void Visitor_Draw_GL_Fixed::Visit_Rendered_Rect(const ZRef<Rendered_Rect>& iRendered_Rect)
	{
	ZRGBA theRGBA;
	GRect theRect;
	iRendered_Rect->Get(theRGBA, theRect);
	spDrawRect(sAlphaMat(fAlphaGainMat), theRGBA, theRect);
	}

void Visitor_Draw_GL_Fixed::Visit_Rendered_Texture(const ZRef<Rendered_Texture>& iRendered_Texture)
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

			// Left
			sRef(new Rendered_Line(ZRGBA::sRed, LB(theBounds), LT(theBounds), 1.0))
				->Accept(*this);

			// Right
			sRef(new Rendered_Line(ZRGBA::sGreen, RT(theBounds), RB(theBounds), 1.0))
				->Accept(*this);

			// Top
			sRef(new Rendered_Line(ZRGBA::sRed + ZRGBA::sBlue, LT(theBounds), RT(theBounds), 1.0))
				->Accept(*this);

			// Bottom
			sRef(new Rendered_Line(ZRGBA::sGreen + ZRGBA::sBlue, RB(theBounds), LB(theBounds), 1.0))
				->Accept(*this);
			}
		}
	}

void Visitor_Draw_GL_Fixed::Visit_Rendered_Triangle(
	const ZRef<Rendered_Triangle>& iRendered_Triangle)
	{
	ZRGBA theRGBA;
	GPoint theP0, theP1, theP2;
	iRendered_Triangle->Get(theRGBA, theP0, theP1, theP2);
	spDrawTriangle(sAlphaMat(fAlphaGainMat), theRGBA, theP0, theP1, theP2);
	}

} // namespace GameEngine
} // namespace ZooLib
