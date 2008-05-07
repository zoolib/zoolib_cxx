/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZHandle.h"
#include "ZMemory.h"
#include "ZDebug.h"
#include "ZThread.h"
#include "ZCompat_algorithm.h" // For min()

#if ZCONFIG(OS, Win32)
#	include "ZWinHeader.h"
#endif

using std::bad_alloc;
using std::max;
using std::min;

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle::Rep

ZHandle::Rep::Rep()
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		fMacHandle = nil;
	#else
		fData = nil;
		fSize = 0;
	#endif

	fLockCount.fValue = 0;
	}

ZHandle::Rep::Rep(size_t iSize)
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		// May need toolbox lock.
		fMacHandle = ::NewHandle(iSize);
		if (fMacHandle == nil)
			throw bad_alloc();
	#elif ZCONFIG(OS, Win32)
		if (iSize)
			{
			fData = reinterpret_cast<char*>(::GlobalAlloc(GMEM_FIXED, iSize));
			if (fData == nil)
				throw bad_alloc();
			}
		else
			{
			fData = nil;
			}
		fSize = iSize;
	#else
		if (iSize)
			{
			fData = reinterpret_cast<char*>(malloc(iSize));
			if (fData == nil)
				throw bad_alloc();
			}
		else
			{
			fData = nil;
			}
		fSize = iSize;
	#endif

	fLockCount.fValue = 0;
	}

ZHandle::Rep::~Rep()
	{
	ZAssertStop(2, fLockCount.fValue == 0);
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (fMacHandle)
			{
			// May need toolbox lock.
			::DisposeHandle(fMacHandle);
			}
	#elif ZCONFIG(OS, Win32)
		if (fData)
			::GlobalFree(reinterpret_cast<HGLOBAL>(fData));
	#else
		if (fData)
			free(fData);
	#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle

ZHandle::ZHandle()
:	fRep(new Rep)
	{}

ZHandle::ZHandle(size_t iInitialSize)
:	fRep(new Rep(iInitialSize))
	{}

ZHandle::ZHandle(const void* iSourceData, size_t iSize)
:	fRep(new Rep(iSize))
	{
	this->CopyFrom(iSourceData, iSize);
	}

ZHandle::ZHandle(const ZHandle& iOther)
: fRep(iOther.fRep)
	{}

ZHandle::~ZHandle()
	{}

ZHandle& ZHandle::operator=(const ZHandle& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

size_t ZHandle::GetSize() const
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (fRep->fMacHandle)
			{
			// May need toolbox lock.
			return ::GetHandleSize(fRep->fMacHandle);
			}
		return 0;
	#else
		return fRep->fSize;
	#endif
	}

void ZHandle::SetSize(size_t iSize)
	{
// We shouldn't be trying to resize when we're locked
	ZAssertStop(2, fRep->fLockCount.fValue == 0);
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (iSize == 0)
			{
			if (fRep->fMacHandle == nil)
				return;
			fRep = new Rep;
			}
		else
			{
			if (fRep->fMacHandle == nil)
				{
				fRep = new Rep(iSize);
				}
			else
				{
				// May need toolbox lock.
				size_t currentSize = ::GetHandleSize(fRep->fMacHandle);
				if (currentSize != iSize)
					{
					if (fRep->GetRefCount() == 1)
						{
						::SetHandleSize(fRep->fMacHandle, iSize);
						if (::MemError() != noErr)
							throw bad_alloc();
						}
					else
						{
						Rep* newRep = new Rep(iSize);
						ZBlockCopy(fRep->fMacHandle[0], newRep->fMacHandle[0],
							min(currentSize, iSize));
						fRep = newRep;
						}
					}
				}
			}
	#else // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (iSize == 0)
			{
			if (fRep->fData == nil)
				return;
			fRep = new Rep;
			}
		else
			{
			if (fRep->fData == nil)
				{
				fRep = new Rep(iSize);
				}
			else
				{
				if (iSize != fRep->fSize)
					{
					if (fRep->GetRefCount() == 1)
						{
						#if ZCONFIG(OS, Win32)
							char* newData = reinterpret_cast<char*>(
								::GlobalReAlloc(
									reinterpret_cast<HGLOBAL>(fRep->fData),
									iSize, GMEM_MOVEABLE));
						#else
							char* newData = reinterpret_cast<char*>(
								::realloc(fRep->fData, iSize));
						#endif
	
						if (newData == nil)
							throw bad_alloc();
						fRep->fData = newData;
						fRep->fSize = iSize;
						}
					else
						{
						Rep* newRep = new Rep(iSize);
						ZBlockCopy(fRep->fData, newRep->fData, min(fRep->fSize, newRep->fSize));
						fRep = newRep;
						}
					}
				}
			}
	#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	}

void ZHandle::CopyFrom(const void* iSource, size_t iSize, size_t iOffset)
	{
	ZAssertStop(2, iSize + iOffset <= this->GetSize());
	if (iSize == 0)
		return;
	this->Touch();

	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		// We don't need to lock the handle -- this call will complete
		// without any other calls to the memory manager possible
		ZBlockCopy(iSource, fRep->fMacHandle[0] + iOffset, iSize);
	#else
		ZBlockCopy(iSource, fRep->fData + iOffset, iSize);
	#endif
	}

void ZHandle::CopyTo(void* iDest, size_t iSize, size_t iOffset) const
	{
	ZAssertStop(2, iSize + iOffset <= this->GetSize());
	if (iSize == 0)
		return;

	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		ZBlockCopy(fRep->fMacHandle[0] + iOffset, iDest, iSize);
	#else
		ZBlockCopy(fRep->fData + iOffset, iDest, iSize);
	#endif
	}

void ZHandle::Touch()
	{
	if (fRep->GetRefCount() == 1)
		return;

	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		ZRef<Rep> newRep = new Rep;
		if (fRep->fMacHandle)
			{
			// May need toolbox lock.
			newRep->fMacHandle = fRep->fMacHandle;
			::HandToHand(&newRep->fMacHandle);
			if (newRep->fMacHandle == nil)
				throw bad_alloc();
			}
		fRep = newRep;
	#else
		Rep* newRep = new Rep(this->GetSize());
		ZBlockCopy(fRep->fData, newRep->fData, fRep->fSize);
		fRep = newRep;
	#endif
	}

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

Handle ZHandle::GetMacHandle()
	{ return fRep->fMacHandle; }

#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle::Ref

ZHandle::Ref::Ref(ZHandle& iHandle)
:	fHandle(iHandle)
	{
	if (ZThreadSafe_IncReturnOld(fHandle.fRep->fLockCount) == 0)
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
			if (fHandle.fRep->fMacHandle)
				{
				// May need toolbox lock.
				::HLockHi(fHandle.fRep->fMacHandle);
				}
		#endif
		}
	}

ZHandle::Ref::~Ref()
	{
	if (ZThreadSafe_DecReturnOld(fHandle.fRep->fLockCount) == 1)
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
			if (fHandle.fRep->fMacHandle)
				{
				// May need toolbox lock.
				::HUnlock(fHandle.fRep->fMacHandle);
				}
		#endif
		}
	}

void* ZHandle::Ref::GetData()
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (fHandle.fRep->fMacHandle)
			return fHandle.fRep->fMacHandle[0];
		return nil;
	#else
		return fHandle.fRep->fData;
	#endif
	}

size_t ZHandle::Ref::GetSize()
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (fHandle.fRep->fMacHandle)
			{
			// May need toolbox lock.
			return ::GetHandleSize(fHandle.fRep->fMacHandle);
			}
		return 0;
	#else
		return fHandle.fRep->fSize;
	#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle::ConstRef

ZHandle::ConstRef::ConstRef(const ZHandle& iHandle)
:	fHandle(iHandle)
	{
	if (ZThreadSafe_IncReturnOld(fHandle.fRep->fLockCount) == 0)
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
			if (fHandle.fRep->fMacHandle)
				{
				// May need toolbox lock.
				::HLockHi(fHandle.fRep->fMacHandle);
				}
		#endif
		}
	}

ZHandle::ConstRef::~ConstRef()
	{
	if (ZThreadSafe_DecReturnOld(fHandle.fRep->fLockCount) == 1)
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
			if (fHandle.fRep->fMacHandle)
				{
				::HUnlock(fHandle.fRep->fMacHandle);
				}
		#endif
		}
	}

const void* ZHandle::ConstRef::GetData()
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (fHandle.fRep->fMacHandle)
			return fHandle.fRep->fMacHandle[0];
		return nil;
	#else
		return fHandle.fRep->fData;
	#endif
	}

size_t ZHandle::ConstRef::GetSize()
	{
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		if (fHandle.fRep->fMacHandle)
			{
			// May need toolbox lock.
			return ::GetHandleSize(fHandle.fRep->fMacHandle);
			}
		return 0;
	#else
		return fHandle.fRep->fSize;
	#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_Handle

ZStreamRWPos_Handle::ZStreamRWPos_Handle(ZHandle& iHandle, size_t iGrowIncrement)
:	fHandle(iHandle)
	{
	fGrowIncrement = iGrowIncrement;
	fPosition = 0;
	fSizeLogical = fHandle.GetSize();
	}

ZStreamRWPos_Handle::ZStreamRWPos_Handle(ZHandle& iHandle)
:	fHandle(iHandle)
	{
	fGrowIncrement = 64;
	fPosition = 0;
	fSizeLogical = fHandle.GetSize();
	}

ZStreamRWPos_Handle::~ZStreamRWPos_Handle()
	{
	// Finally, make sure the handle is the real size, not the potentially
	// overallocated size we've been using
	fHandle.SetSize(fSizeLogical);
	}

void ZStreamRWPos_Handle::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countToCopy = min(uint64(iCount), sDiffPosR(fSizeLogical, fPosition));
	fHandle.CopyTo(iDest, countToCopy, fPosition);
	fPosition += countToCopy;
	if (oCountRead)
		*oCountRead = countToCopy;
	}

void ZStreamRWPos_Handle::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 realSkip = min(iCount, sDiffPosR(fSizeLogical, fPosition));
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

void ZStreamRWPos_Handle::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	uint64 neededSpace = fPosition + iCount;
	if (fHandle.GetSize() < neededSpace)
		{
		uint64 realSize = max(uint64(fHandle.GetSize()) + fGrowIncrement, neededSpace);
		if (realSize == size_t(realSize))
			fHandle.SetSize(realSize);
		}

	uint64 countToCopy = min(uint64(iCount), sDiffPosW(fHandle.GetSize(), fPosition));
	
	fHandle.CopyFrom(iSource, countToCopy, fPosition);

	fPosition += countToCopy;

	if (fSizeLogical < fPosition)
		fSizeLogical = fPosition;

	if (oCountWritten)
		*oCountWritten = countToCopy;
	}

void ZStreamRWPos_Handle::Imp_Flush()
	{ fHandle.SetSize(fSizeLogical); }

uint64 ZStreamRWPos_Handle::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_Handle::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_Handle::Imp_GetSize()
	{ return fSizeLogical; }

void ZStreamRWPos_Handle::Imp_SetSize(uint64 iSize)
	{
	size_t realSize = iSize;
	if (realSize != iSize)
		sThrowBadSize();

	fHandle.SetSize(realSize);
	fSizeLogical = realSize;
	}
