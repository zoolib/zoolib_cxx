#ifndef __ZooLib_OpenGL_Util_h__
#define __ZooLib_OpenGL_Util_h__ 1
#include "zconfig.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

namespace ZooLib {
namespace OpenGL {

// =================================================================================================
#pragma mark - SaveSetRestore_BindTexture_2D

class SaveSetRestore_BindTexture_2D
	{
public:
	SaveSetRestore_BindTexture_2D(GLuint iTextureID);
	~SaveSetRestore_BindTexture_2D();

private:
	GLuint fPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestore_Enable

class SaveSetRestore_Enable
	{
public:
	SaveSetRestore_Enable(GLenum iEnum, bool iEnable);
	~SaveSetRestore_Enable();

private:
	GLenum fEnum;
	bool fPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestore_ActiveTexture

class SaveSetRestore_ActiveTexture
	{
public:
	SaveSetRestore_ActiveTexture(GLenum iTexture);
	~SaveSetRestore_ActiveTexture();

private:
	GLenum fPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestore_BlendFunc

class SaveSetRestore_BlendFunc
	{
public:
	SaveSetRestore_BlendFunc(GLenum sfactor, GLenum dfactor);
	~SaveSetRestore_BlendFunc();

private:
	#if defined(GL_BLEND_SRC_RGB)
		GLint fPrior_src_rgb;
		GLint fPrior_dest_rgb;
		GLint fPrior_src_alpha;
		GLint fPrior_dest_alpha;
	#else
		GLint fPrior_sfactor;
		GLint fPrior_dfactor;
	#endif
	};

// =================================================================================================
#pragma mark - SaveSetRestore_BlendEquation

class SaveSetRestore_BlendEquation
	{
public:
	SaveSetRestore_BlendEquation(GLenum mode);
	~SaveSetRestore_BlendEquation();

private:
	GLenum fPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestore_ViewPort

class SaveSetRestore_ViewPort
	{
public:
	SaveSetRestore_ViewPort(GLint x, GLint y, GLsizei width, GLsizei height);
	~SaveSetRestore_ViewPort();

private:
	GLint fPrior[4];
	};

// =================================================================================================
#pragma mark - SaveSetRestore_BindFramebuffer

class SaveSetRestore_BindFramebuffer
	{
public:
	SaveSetRestore_BindFramebuffer(GLuint iFBO);
	~SaveSetRestore_BindFramebuffer();

private:
	GLuint fPrior;
	};

} // namespace OpenGL {
} // namespace ZooLib

#endif // __ZooLib_OpenGL_Util_h__
