// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_OpenGL_Compat_OpenGL_h__
#define __ZooLib_OpenGL_Compat_OpenGL_h__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark - defined(ZProjectHeader_OpenGL)

#if defined(ZProjectHeader_OpenGL)

#include ZProjectHeader_OpenGL

#endif // defined(ZProjectHeader_OpenGL)

// =================================================================================================
#pragma mark - not defined(ZProjectHeader_OpenGL)

#if not defined(ZProjectHeader_OpenGL)

#include "zoolib/ZCONFIG_SPI.h"

#if ZMACRO_IOS

	#define GLES_SILENCE_DEPRECATION
	#include <OpenGLES/ES2/gl.h>

#elif defined(__ANDROID__)

	#include <GLES2/gl2.h>

#else

	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl.h>

#endif

#endif // not defined(ZProjectHeader_OpenGL)

#endif // __ZooLib_OpenGL_Compat_OpenGL_h__
