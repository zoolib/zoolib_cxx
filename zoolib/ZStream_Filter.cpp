/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStream_Filter.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Filter

ZStreamR_Filter::ZStreamR_Filter(const ZStreamR& iStreamReal)
:	fStreamReal(iStreamReal)
	{}

ZStreamR_Filter::~ZStreamR_Filter()
	{}

void ZStreamR_Filter::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ const_cast<ZStreamR&>(fStreamReal).Imp_Read(iDest, iCount, oCountRead); }

size_t ZStreamR_Filter::Imp_CountReadable()
	{ return const_cast<ZStreamR&>(fStreamReal).Imp_CountReadable(); }

void ZStreamR_Filter::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{ const_cast<ZStreamR&>(fStreamReal).Imp_Skip(iCount, oCountSkipped); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU_Filter

ZStreamU_Filter::ZStreamU_Filter(const ZStreamU& iStreamReal)
:	fStreamReal(iStreamReal)
	{}

ZStreamU_Filter::~ZStreamU_Filter()
	{}

void ZStreamU_Filter::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ const_cast<ZStreamU&>(fStreamReal).Imp_Read(iDest, iCount, oCountRead); }

size_t ZStreamU_Filter::Imp_CountReadable()
	{ return const_cast<ZStreamU&>(fStreamReal).Imp_CountReadable(); }

void ZStreamU_Filter::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{ const_cast<ZStreamU&>(fStreamReal).Imp_Skip(iCount, oCountSkipped); }

void ZStreamU_Filter::Imp_Unread()
	{ const_cast<ZStreamU&>(fStreamReal).Imp_Unread(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Filter

ZStreamRPos_Filter::ZStreamRPos_Filter(const ZStreamRPos& iStreamReal)
:	fStreamReal(iStreamReal)
	{}

ZStreamRPos_Filter::~ZStreamRPos_Filter()
	{}

void ZStreamRPos_Filter::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ const_cast<ZStreamRPos&>(fStreamReal).Imp_Read(iDest, iCount, oCountRead); }

size_t ZStreamRPos_Filter::Imp_CountReadable()
	{ return const_cast<ZStreamRPos&>(fStreamReal).Imp_CountReadable(); }

void ZStreamRPos_Filter::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{ const_cast<ZStreamRPos&>(fStreamReal).Imp_Skip(iCount, oCountSkipped); }

uint64 ZStreamRPos_Filter::Imp_GetPosition()
	{ return const_cast<ZStreamRPos&>(fStreamReal).Imp_GetPosition(); }

void ZStreamRPos_Filter::Imp_SetPosition(uint64 iPosition)
	{ return const_cast<ZStreamRPos&>(fStreamReal).Imp_SetPosition(iPosition); }

uint64 ZStreamRPos_Filter::Imp_GetSize()
	{ return const_cast<ZStreamRPos&>(fStreamReal).Imp_GetSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Filter

ZStreamW_Filter::ZStreamW_Filter(const ZStreamW& iStreamReal)
:	fStreamReal(iStreamReal)
	{}

ZStreamW_Filter::~ZStreamW_Filter()
	{}

void ZStreamW_Filter::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ const_cast<ZStreamW&>(fStreamReal).Imp_Write(iSource, iCount, oCountWritten); }

void ZStreamW_Filter::Imp_Flush()
	{ const_cast<ZStreamW&>(fStreamReal).Imp_Flush(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_Filter

ZStreamWPos_Filter::ZStreamWPos_Filter(const ZStreamWPos& iStreamReal)
:	fStreamReal(iStreamReal)
	{}

ZStreamWPos_Filter::~ZStreamWPos_Filter()
	{}

void ZStreamWPos_Filter::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ const_cast<ZStreamWPos&>(fStreamReal).Imp_Write(iSource, iCount, oCountWritten); }

void ZStreamWPos_Filter::Imp_Flush()
	{ const_cast<ZStreamWPos&>(fStreamReal).Imp_Flush(); }

uint64 ZStreamWPos_Filter::Imp_GetPosition()
	{ return const_cast<ZStreamWPos&>(fStreamReal).Imp_GetPosition(); }
	
void ZStreamWPos_Filter::Imp_SetPosition(uint64 iPosition)
	{ return const_cast<ZStreamWPos&>(fStreamReal).Imp_SetPosition(iPosition); }

uint64 ZStreamWPos_Filter::Imp_GetSize()
	{ return const_cast<ZStreamWPos&>(fStreamReal).Imp_GetSize(); }

void ZStreamWPos_Filter::Imp_SetSize(uint64 iSize)
	{ return const_cast<ZStreamWPos&>(fStreamReal).Imp_SetSize(iSize); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_Filter

ZStreamRWPos_Filter::ZStreamRWPos_Filter(const ZStreamRWPos& iStreamReal)
:	fStreamReal(iStreamReal)
	{}

ZStreamRWPos_Filter::~ZStreamRWPos_Filter()
	{}

void ZStreamRWPos_Filter::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ const_cast<ZStreamRWPos&>(fStreamReal).Imp_Read(iDest, iCount, oCountRead); }

size_t ZStreamRWPos_Filter::Imp_CountReadable()
	{ return const_cast<ZStreamRWPos&>(fStreamReal).Imp_CountReadable(); }

void ZStreamRWPos_Filter::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{ const_cast<ZStreamRWPos&>(fStreamReal).Imp_Skip(iCount, oCountSkipped); }

void ZStreamRWPos_Filter::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ const_cast<ZStreamRWPos&>(fStreamReal).Imp_Write(iSource, iCount, oCountWritten); }

void ZStreamRWPos_Filter::Imp_Flush()
	{ const_cast<ZStreamRWPos&>(fStreamReal).Imp_Flush(); }

uint64 ZStreamRWPos_Filter::Imp_GetPosition()
	{ return const_cast<ZStreamRWPos&>(fStreamReal).Imp_GetPosition(); }
	
void ZStreamRWPos_Filter::Imp_SetPosition(uint64 iPosition)
	{ return const_cast<ZStreamRWPos&>(fStreamReal).Imp_SetPosition(iPosition); }

uint64 ZStreamRWPos_Filter::Imp_GetSize()
	{ return const_cast<ZStreamRWPos&>(fStreamReal).Imp_GetSize(); }

void ZStreamRWPos_Filter::Imp_SetSize(uint64 iSize)
	{ return const_cast<ZStreamRWPos&>(fStreamReal).Imp_SetSize(iSize); }
