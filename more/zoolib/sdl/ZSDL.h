/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZSDL_h__
#define __ZSDL_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZDCPixmap.h"
#include "zoolib/ZThread.h"

#include "SDL.h"

#include <set>

namespace ZooLib {
namespace ZSDL {

extern ZMtxR sMtxR;

class Texture;
class Texture_Mutable;
class Surface;

// =================================================================================================
// MARK: - Renderer

class Renderer
:	public ZCounted
	{
public:
	Renderer(SDL_Renderer* iRenderer);
	virtual ~Renderer();

	SDL_Renderer* GetSDL();

	ZRef<Texture> Make_Texture(Uint32 format, int w, int h);
	ZRef<Texture_Mutable> Make_Texture_Mutable(Uint32 format, int w, int h);
	ZRef<Texture> Make_Texture(ZRef<Surface> iSurface);

private:
	SDL_Renderer* fRenderer;
	};

// =================================================================================================
// MARK: - Texture

class Texture
:	public ZCounted
	{
protected:
	Texture(ZRef<Renderer> iRenderer, SDL_Texture* iTexture);

public:
	virtual ~Texture();

	SDL_Texture* GetSDL();

	ZRef<Renderer> GetRenderer();

private:
	SDL_Texture* fTexture;
	ZWeakRef<Renderer> fRenderer;
	friend class Renderer;
	};

// =================================================================================================
// MARK: - Texture_Mutable

class Texture_Mutable
:	public Texture
	{
protected:
	Texture_Mutable(ZRef<Renderer> iRenderer, SDL_Texture* iTexture);

public:
	friend class Renderer;
	};

// =================================================================================================
// MARK: - Surface

class Surface
:	public ZCounted
	{
public:
	Surface(SDL_Surface* iSurface);
	virtual ~Surface();

	SDL_Surface* GetSDL();

private:
	SDL_Surface* fSurface;
	};

ZDCPixmap sPixmap(ZRef<Surface> iSurface);

// =================================================================================================
// MARK: - RWops

class RWops
:	public ZCounted
	{
public:
	RWops(SDL_RWops* iRWops);
	virtual ~RWops();

	SDL_RWops* GetSDL();

private:
	SDL_RWops* fRWops;
	};

// =================================================================================================
// MARK: - Mixer

class Mixer
:	public ZCounted
	{
public:
	Mixer();
	virtual ~Mixer();

	SDL_AudioSpec GetSpec();

	typedef ZCallable<size_t(void*,size_t)> Callable;

	void Insert(ZRef<Callable> iCallable);
	void Erase(ZRef<Callable> iCallable);

private:
	void pCallback(Uint8* oDest, size_t iCount);
	static void spCallback(void* iRefcon, Uint8* oDest, int iLen);

	ZMtxR fMtxR;
	SDL_AudioDeviceID fDeviceID;
	SDL_AudioSpec fSpec;
	std::set<ZRef<Callable> > fCallables;
	};

// =================================================================================================
// MARK: -

/*
SDL_Surface
SDL_Texture
SDL_Renderer
SDL_RWops

*/
#if 0

template <class T, class dtor>
class Wrap
	{
public:
	~Wrap()
		{
		fDtor(fT);
		}

	T fT;
	dtor fDtor;
	};

#endif
} // namespace ZooLib
} // namespace ZSDL

#endif // __ZSDL_h__
