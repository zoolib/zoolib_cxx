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

#include "zoolib/sdl/ZSDL.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

namespace ZooLib {
namespace ZSDL {

//using std::set;
using std::vector;

ZMtxR sMtxR;

// =================================================================================================
// MARK: -

Renderer::Renderer(SDL_Renderer* iRenderer)
:	fRenderer(iRenderer)
	{
	ZAcqMtxR acq(sMtxR);
	}

Renderer::~Renderer()
	{
	ZAcqMtxR acq(sMtxR);
	::SDL_DestroyRenderer(fRenderer);
	}

SDL_Renderer* Renderer::GetSDL()
	{ return fRenderer; }

ZRef<Texture> Renderer::Make_Texture(Uint32 format, int w, int h)
	{
	ZAcqMtxR acq(sMtxR);

	if (SDL_Texture* theTexture = ::SDL_CreateTexture
		(fRenderer, format, SDL_TEXTUREACCESS_STATIC, w, h))
		{ return new Texture(this, theTexture); }

	return null;
	}

ZRef<Texture_Mutable> Renderer::Make_Texture_Mutable(Uint32 format, int w, int h)
	{
	ZAcqMtxR acq(sMtxR);

	if (SDL_Texture* theTexture = ::SDL_CreateTexture
		(fRenderer, format, SDL_TEXTUREACCESS_STREAMING, w, h))
		{ return new Texture_Mutable(this, theTexture); }

	return null;
	}

ZRef<Texture> Renderer::Make_Texture(ZRef<Surface> iSurface)
	{
	ZAcqMtxR acq(sMtxR);

	if (SDL_Texture* theTexture = ::SDL_CreateTextureFromSurface
		(fRenderer, iSurface->GetSDL()))
		{ return new Texture(this, theTexture); }

	return null;
	}

// =================================================================================================
// MARK: -

Texture::Texture(ZRef<Renderer> iRenderer, SDL_Texture* iTexture)
:	fRenderer(iRenderer)
,	fTexture(iTexture)
	{}

Texture::~Texture()
	{
	ZAcqMtxR acq(sMtxR);
	::SDL_DestroyTexture(fTexture);
	}

SDL_Texture* Texture::GetSDL()
	{ return fTexture; }

ZRef<Renderer> Texture::GetRenderer()
	{ return fRenderer; }

// =================================================================================================
// MARK: - Texture_Mutable

Texture_Mutable::Texture_Mutable(ZRef<Renderer> iRenderer, SDL_Texture* iTexture)
:	Texture(iRenderer, iTexture)
	{}

// =================================================================================================
// MARK: -

Surface::Surface(SDL_Surface* iSurface)
:	fSurface(iSurface)
	{}

Surface::~Surface()
	{
	ZAcqMtxR acq(sMtxR);
	::SDL_FreeSurface(fSurface);
	}

SDL_Surface* Surface::GetSDL()
	{ return fSurface; }

// =================================================================================================
// MARK: - RWops

RWops::RWops(SDL_RWops* iRWops)
:	fRWops(iRWops)
	{}

RWops::~RWops()
	{
	ZAcqMtxR acq(sMtxR);
	::SDL_FreeRW(fRWops);
	}

SDL_RWops* RWops::GetSDL()
	{ return fRWops; }

// =================================================================================================
// MARK: - sPixmap

class PixmapRaster : public ZDCPixmapRaster
	{
public:
	PixmapRaster(const ZDCPixmapNS::RasterDesc& iRasterDesc, ZRef<Surface> iSurface);
	virtual ~PixmapRaster();

private:
	ZRef<Surface> fSurface;
	};

PixmapRaster::PixmapRaster(const ZDCPixmapNS::RasterDesc& iRasterDesc, ZRef<Surface> iSurface)
:	ZDCPixmapRaster(iRasterDesc)
,	fSurface(iSurface)
	{
	fBaseAddress = iSurface->GetSDL()->pixels;
	}

PixmapRaster::~PixmapRaster()
	{
	fBaseAddress = nullptr;
	}

// =================================================================================================
// MARK: - sPixmap

ZDCPixmap sPixmap(ZRef<Surface> iSurface)
	{
	using namespace ZDCPixmapNS;

	SDL_Surface* theSurface = iSurface->GetSDL();
	SDL_PixelFormat* theFormat = theSurface->format;

	PixelDesc thePixelDesc(new PixelDescRep_Color
		(theFormat->Rmask, theFormat->Gmask, theFormat->Bmask, theFormat->Amask));

	int32 theDepth;
	if (theFormat->BitsPerPixel)
		theDepth = theFormat->BitsPerPixel;
	else
		theDepth = theFormat->BytesPerPixel;
	PixvalDesc thePixvalDesc(theDepth, false);

	const size_t theRowBytes = theSurface->pitch;
	const size_t theHeight = theSurface->h;
	const size_t theWidth = theSurface->w;

	const RasterDesc theRasterDesc(thePixvalDesc, theRowBytes, theHeight, false);

	ZRef<PixmapRaster> thePMR = new PixmapRaster(theRasterDesc, iSurface);

	return ZDCPixmapRep::sCreate(thePMR, sRect<ZRectPOD>(theWidth, theHeight), thePixelDesc);
	}

// =================================================================================================
// MARK: - Mixer

Mixer::Mixer()
:	fDeviceID(0)
	{
	ZMemZero_T(fSpec);

	SDL_AudioSpec theSpec = {0};
    theSpec.freq = 44100;
    theSpec.format = AUDIO_S16LSB;
    theSpec.channels = 2;
    theSpec.samples = 256;
    theSpec.callback = spCallback;
    theSpec.userdata = this;	

	if (0 == ::SDL_OpenAudio(&theSpec, &fSpec))
		fDeviceID = 1;
	}

Mixer::~Mixer()
	{
	ZAcqMtxR acq(fMtxR);
	if (!fDeviceID)
		return;

	::SDL_CloseAudioDevice(fDeviceID);
	}

SDL_AudioSpec Mixer::GetSpec()
	{ return fSpec; }

void Mixer::Insert(ZRef<Callable> iCallable)
	{
	ZAcqMtxR acq(fMtxR);
	if (!fDeviceID)
		return;

	ZUtil_STL::sInsertMustNotContain(fCallables, iCallable);

	if (fCallables.size() == 1)
		::SDL_PauseAudioDevice(fDeviceID, 0);
	}

void Mixer::Erase(ZRef<Callable> iCallable)
	{
	ZAcqMtxR acq(fMtxR);

	if (!fDeviceID)
		return;

	ZUtil_STL::sEraseMustContain(fCallables, iCallable);

	if (fCallables.empty())
		::SDL_PauseAudioDevice(fDeviceID, 1);
	}

void Mixer::pCallback(Uint8* oDest, size_t iCount)
	{
	ZGuardRMtxR guard(fMtxR);
	vector<ZRef<Callable> > vec(fCallables.begin(), fCallables.end());
	guard.Release();

	foreachv (ZRef<Callable> callable, vec)
		{
		if (not callable->Call(oDest, iCount))
			this->Erase(callable);
		}
	}

void Mixer::spCallback(void* iRefcon, Uint8* oDest, int iLen)
	{ static_cast<Mixer*>(iRefcon)->pCallback(oDest, iLen); }

} // namespace ZSDL
} // namespace ZooLib
