// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/PixvalIters.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixvalIterR

PixvalIterR::PixvalIterR(const PixvalDesc& iPixvalDesc, const void* iAddress, int iH)
:	fAccessor(iPixvalDesc)
,	fAddress(iAddress)
,	fH(iH)
	{}

Pixval PixvalIterR::ReadInc()
	{ return fAccessor.GetPixval(fAddress, fH++); }

Pixval PixvalIterR::Read()
	{ return fAccessor.GetPixval(fAddress, fH); }

void PixvalIterR::Inc()
	{ ++fH; }

void PixvalIterR::Reset(const void* iAddress, int iH)
	{
	fAddress = iAddress;
	fH = iH;
	}

// =================================================================================================
#pragma mark - PixvalIterRW

PixvalIterRW::PixvalIterRW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH)
:	fAccessor(iPixvalDesc)
,	fAddress(iAddress)
,	fH(iH)
	{}

Pixval PixvalIterRW::ReadInc()
	{ return fAccessor.GetPixval(fAddress, fH++); }

Pixval PixvalIterRW::Read()
	{ return fAccessor.GetPixval(fAddress, fH); }

void PixvalIterRW::WriteInc(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH++, iPixval); }

void PixvalIterRW::Write(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH, iPixval); }

void PixvalIterRW::Inc()
	{ ++fH; }

void PixvalIterRW::Reset(void* iAddress, int iH)
	{
	fAddress = iAddress;
	fH = iH;
	}

// =================================================================================================
#pragma mark - PixvalIterW

PixvalIterW::PixvalIterW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH)
:	fAccessor(iPixvalDesc)
,	fAddress(iAddress)
,	fH(iH)
	{}

void PixvalIterW::WriteInc(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH++, iPixval); }

void PixvalIterW::Write(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH, iPixval); }

void PixvalIterW::Inc()
	{ ++fH; }

void PixvalIterW::Reset(void* iAddress, int iH)
	{
	fAddress = iAddress;
	fH = iH;
	}

} // namespace Pixels
} // namespace ZooLib
