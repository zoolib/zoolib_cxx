#ifndef __ZooLib_OpenGL_Util_h__
#define __ZooLib_OpenGL_Util_h__ 1
#include "zconfig.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

namespace ZooLib {
namespace OpenGL {

// =================================================================================================
// MARK: - SaveSetRestore_BindTexture_2D

class SaveSetRestore_BindTexture_2D
	{
public:
	SaveSetRestore_BindTexture_2D(GLuint iTextureID);
	~SaveSetRestore_BindTexture_2D();

private:
	GLuint fPrior;
	};

// =================================================================================================
// MARK: - SaveSetRestore_Enable

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
// MARK: - SaveSetRestore_EnableClientState

class SaveSetRestore_EnableClientState
	{
public:
	SaveSetRestore_EnableClientState(GLenum iEnum, bool iEnable);
	~SaveSetRestore_EnableClientState();

private:
	GLenum fEnum;
	bool fPrior;
	};

// =================================================================================================
// MARK: - SaveSetRestore_ActiveTexture

class SaveSetRestore_ActiveTexture
	{
public:
	SaveSetRestore_ActiveTexture(GLenum iTexture);
	~SaveSetRestore_ActiveTexture();

private:
	GLenum fPrior;
	};

// =================================================================================================
// MARK: - SaveSetRestore_BlendFunc

class SaveSetRestore_BlendFunc
	{
public:
	SaveSetRestore_BlendFunc(GLenum sfactor, GLenum dfactor);
	~SaveSetRestore_BlendFunc();

private:
	GLenum fPrior_sfactor;
	GLenum fPrior_dfactor;
	};

// =================================================================================================
// MARK: - SaveSetRestore_BlendEquation

class SaveSetRestore_BlendEquation
	{
public:
	SaveSetRestore_BlendEquation(GLenum mode);
	~SaveSetRestore_BlendEquation();

private:
	GLenum fPrior;
	};

// =================================================================================================
// MARK: - SaveSetRestore_ViewPort

class SaveSetRestore_ViewPort
	{
public:
	SaveSetRestore_ViewPort(GLint x, GLint y, GLsizei width, GLsizei height);
	~SaveSetRestore_ViewPort();

private:
	GLint fPrior[4];
	};

// =================================================================================================
// MARK: - SaveSetRestore_BindFramebuffer

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
