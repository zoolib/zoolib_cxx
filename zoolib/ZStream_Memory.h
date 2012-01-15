/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_Memory_h__
#define __ZStream_Memory_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_Memory

/// A read stream that gets its data from normal memory.

class ZStreamR_Memory : public ZStreamR
	{
public:
	ZStreamR_Memory(const void* iAddress);

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// Our protocol
	/// Returns the address from which the next read would be satisfied.
	const void* GetCurrentAddress();

protected:
	void pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	const char* fAddress;
	};

typedef ZStreamerR_T<ZStreamR_Memory> ZStreamerR_Memory;

// =================================================================================================
// MARK: - ZStreamRPos_Memory

/// A positionable read stream that gets its data from normal memory.

class ZStreamRPos_Memory : public ZStreamRPos
	{
public:
	ZStreamRPos_Memory(const void* iAddress, size_t iSize);
	ZStreamRPos_Memory(const std::pair<const void*, size_t>& iParam);

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// Our protocol
	/// Returns the address from which the next read would be satisfied.
	const void* GetCurrentAddress();

	void Reset(const void* iAddress, size_t iSize);

protected:
	void pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	const char* fAddress;
	uint64 fPosition;
	size_t fSize;
	};

// =================================================================================================
// MARK: - ZStreamerRPos_Memory

/// A positionable read streamer encapsulating a ZStreamRPos_Memory.

class ZStreamerRPos_Memory : public ZStreamerRPos
	{
public:
	ZStreamerRPos_Memory(const void* iAddress, size_t iSize);
	virtual ~ZStreamerRPos_Memory();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

// Our protocol
	const void* GetCurrentAddress();

protected:
	ZStreamRPos_Memory fStream;
	};

// Alternate template-based definition:
// typedef ZStreamerRPos_T<ZStreamRPos_Memory> ZStreamerRPos_Memory;

// =================================================================================================
// MARK: - ZStreamW_Memory

/// A write stream that writes to normal memory.

class ZStreamW_Memory : public ZStreamW
	{
public:
	ZStreamW_Memory(void* iAddress);

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

// Our protocol
	/// Returns the address to which the next write will be made.
	void* GetCurrentAddress();

protected:
	void pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	char* fAddress;
	};

typedef ZStreamerW_T<ZStreamW_Memory> ZStreamerW_Memory;

// =================================================================================================
// MARK: - ZStreamWPos_Memory

/// A positionable write stream that writes to normal memory.

class ZStreamWPos_Memory : public ZStreamWPos
	{
public:
	ZStreamWPos_Memory(void* iAddress, size_t iSize);
	ZStreamWPos_Memory(void* iAddress, size_t iSize, size_t iCapacity);

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

// Our protocol
	/// Returns the address to which the next write will be made.
	void* GetCurrentAddress();

protected:
	void pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	char* fAddress;
	uint64 fPosition;
	size_t fSize;
	size_t fCapacity;
	};

// =================================================================================================
// MARK: - ZStreamerWPos_Memory

/// A positionable write streamer encapsulating a ZStreamWPos_Memory.

class ZStreamerWPos_Memory : public ZStreamerWPos
	{
public:
	ZStreamerWPos_Memory(void* iAddress, size_t iSize);
	ZStreamerWPos_Memory(void* iAddress, size_t iSize, size_t iCapacity);
	virtual ~ZStreamerWPos_Memory();

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos();

// Our protocol
	void* GetCurrentAddress();

protected:
	ZStreamWPos_Memory fStream;
	};

// Alternate template-based definition:
// typedef ZStreamerW_T<ZStreamWPos_Memory> ZStreamerWPos_Memory;

} // namespace ZooLib

#endif // __ZStream_Memory_h__
