#include "zoolib/OpenGL/Util.h"

namespace ZooLib {
namespace OpenGL {

// =================================================================================================
#pragma mark - SaveSetRestore_BindTexture_2D

SaveSetRestore_BindTexture_2D::SaveSetRestore_BindTexture_2D(GLuint iTextureID)
	{
	::glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&fPrior);
	::glBindTexture(GL_TEXTURE_2D, iTextureID);
	}

SaveSetRestore_BindTexture_2D::~SaveSetRestore_BindTexture_2D()
	{ ::glBindTexture(GL_TEXTURE_2D, fPrior); }

// =================================================================================================
#pragma mark - SaveSetRestore_Enable

SaveSetRestore_Enable::SaveSetRestore_Enable(GLenum iEnum, bool iEnable)
:	fEnum(iEnum)
,	fPrior(::glIsEnabled(iEnum))
	{
	if (iEnable)
		::glEnable(fEnum);
	else
		::glDisable(fEnum);
	}

SaveSetRestore_Enable::~SaveSetRestore_Enable()
	{
	if (fPrior)
		::glEnable(fEnum);
	else
		::glDisable(fEnum);
	}

// =================================================================================================
#pragma mark - SaveSetRestore_ActiveTexture

SaveSetRestore_ActiveTexture::SaveSetRestore_ActiveTexture(GLenum iTexture)
	{
	::glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&fPrior);
	::glActiveTexture(iTexture);
	}

SaveSetRestore_ActiveTexture::~SaveSetRestore_ActiveTexture()
	{ ::glActiveTexture(fPrior); }

// =================================================================================================
#pragma mark - SaveSetRestore_BlendFunc

SaveSetRestore_BlendFunc::SaveSetRestore_BlendFunc(GLenum sfactor, GLenum dfactor)
	{
	#if defined(GL_BLEND_SRC_RGB)

		::glGetIntegerv(GL_BLEND_SRC_RGB, &fPrior_src_rgb);
		::glGetIntegerv(GL_BLEND_DST_RGB, &fPrior_dest_rgb);
		::glGetIntegerv(GL_BLEND_SRC_ALPHA, &fPrior_src_alpha);
		::glGetIntegerv(GL_BLEND_DST_ALPHA, &fPrior_dest_alpha);

	#else

		::glGetIntegerv(GL_BLEND_SRC, &fPrior_sfactor);
		::glGetIntegerv(GL_BLEND_DST, &fPrior_dfactor);

	#endif

	::glBlendFunc(sfactor, dfactor);
	}



SaveSetRestore_BlendFunc::~SaveSetRestore_BlendFunc()
	{
	#if defined(GL_BLEND_SRC_RGB)

		::glBlendFuncSeparate(fPrior_src_rgb, fPrior_dest_rgb, fPrior_src_alpha, fPrior_dest_alpha);

	#else

		::glBlendFunc(fPrior_sfactor, fPrior_dfactor);

	#endif
	}

// =================================================================================================
#pragma mark - SaveSetRestore_BlendEquation

SaveSetRestore_BlendEquation::SaveSetRestore_BlendEquation(GLenum mode)
	{
	::glGetIntegerv(GL_BLEND_EQUATION, (GLint*)&fPrior);
	::glBlendEquation(mode);
	}

SaveSetRestore_BlendEquation::~SaveSetRestore_BlendEquation()
	{ ::glBlendEquation(fPrior); }

// =================================================================================================
#pragma mark - SaveSetRestore_ViewPort

SaveSetRestore_ViewPort::SaveSetRestore_ViewPort(GLint x, GLint y, GLsizei width, GLsizei height)
	{
	::glGetIntegerv(GL_VIEWPORT, fPrior);
	::glViewport(x, y, width, height);
	}

SaveSetRestore_ViewPort::~SaveSetRestore_ViewPort()
	{ ::glViewport(fPrior[0], fPrior[1], fPrior[2], fPrior[3]); }

// =================================================================================================
#pragma mark - SaveSetRestore_BindFramebuffer

SaveSetRestore_BindFramebuffer::SaveSetRestore_BindFramebuffer(GLuint iFBO)
	{
	::glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fPrior);
	::glBindFramebuffer(GL_FRAMEBUFFER, iFBO);
	}

SaveSetRestore_BindFramebuffer::~SaveSetRestore_BindFramebuffer()
	{ ::glBindFramebuffer(GL_FRAMEBUFFER, fPrior); }

} // namespace OpenGL
} // namespace ZooLib
