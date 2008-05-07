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

#ifndef __ZStream__
#define __ZStream__ 1
#include "zconfig.h"

#include "ZTypes.h"

#include <stdexcept> // For std::range_error
#include <string>

class ZStreamR;
class ZStreamW;

/** Helper function used internally by ZStreamR and ZStreamW,
but made available in case it's needed by application code. */
void sCopyReadToWrite(const ZStreamR& iStreamR, const ZStreamW& iStreamW, uint64 iCount,
						uint64* oCountRead, uint64* oCountWritten);

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR

/// Base interface for read streams.

class ZStreamR
	{
protected:
/** \name Canonical Methods
The canonical methods are protected, thus you cannot create, destroy or assign through a
ZStreamR reference, you must work with some derived class.
*/	//@{
	ZStreamR() {}
	~ZStreamR() {}
	ZStreamR(const ZStreamR&) {}
	ZStreamR& operator=(const ZStreamR&) { return *this; }
	//@}

public:
	class ExEndOfStream;
	static void sThrowEndOfStream();

/** \name Read raw data
Read data from this stream into memory.
*/	//@{
	void Read(void* iDest, size_t iCount) const;
	void Read(void* iDest, size_t iCount, size_t* oCountRead) const;
	//@}


/** \name Copy To
Read data from this stream and write it to iStreamW.
*/	//@{
	void CopyAllTo(const ZStreamW& iStreamW) const;
	void CopyAllTo(const ZStreamW& iStreamW, uint64* oCountRead, uint64* oCountWritten) const;

	void CopyTo(const ZStreamW& iStreamW, uint64 iCount) const;
	void CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten) const;
	//@}


/** \name Skip
Skip over bytes from this stream without returning them to the caller.
*/	//@{
	void Skip(uint64 iCount) const;
	void Skip(uint64 iCount, uint64* oCountSkipped) const;
	void SkipAll() const;
	void SkipAll(uint64* oCountSkipped) const;
	//@}


/** \name Read formatted data
Read data from the stream in standard formats.
*/	//@{
	int8 ReadInt8() const;
	int16 ReadInt16() const;
	int32 ReadInt32() const;
	int64 ReadInt64() const;
	float ReadFloat() const;
	double ReadDouble() const;

	int16 ReadInt16LE() const;
	int32 ReadInt32LE() const;
	int64 ReadInt64LE() const;
	float ReadFloatLE() const;
	double ReadDoubleLE() const;

	bool ReadBool() const;

	uint8 ReadUInt8() const { return uint8(this->ReadInt8()); }
	uint16 ReadUInt16() const { return uint16(this->ReadInt16()); }
	uint32 ReadUInt32() const { return uint32(this->ReadInt32()); }
	uint64 ReadUInt64() const { return uint64(this->ReadInt64()); }

	uint16 ReadUInt16LE() const { return uint16(this->ReadInt16LE()); }
	uint32 ReadUInt32LE() const { return uint32(this->ReadInt32LE()); }
	uint64 ReadUInt64LE() const { return uint64(this->ReadInt64LE()); }
	//@}


/** \name Useful variants
Read single bytes without exceptions being thrown.
*/	//@{
	bool ReadByte(uint8& oResult) const;
	bool ReadChar(char& oResult) const;
	//@}


/** \name Read standard string
*/	//@{
	std::string ReadString(size_t iSize) const;
	bool ReadString(size_t iSize, std::string& oString) const;
	//@}


/** \name Read a 'count'
*/	//@{
	uint32 ReadCount() const;
	//@}


/** \name Informational
*/	//@{
	size_t CountReadable() const;
	//@}


/** \name Essential overrides
All ZStreamR functionality is implemented by calling this method.
*/	//@{
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead) = 0;
	//@}


/** \name Optionally overridable API
Default implementations that directly or indirectly call ZStreamR::Imp_Read are provided,
but particular subclasses may have more efficient implementations available.
*/	//@{
	virtual size_t Imp_CountReadable();

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);
	//@}

protected:
	static uint64 sDiffPosR(uint64 iSize, uint64 iPos)
		{ return iSize > iPos ? iSize - iPos : 0; }

	static size_t sClampedR(uint64 iCount)
		{
		if (sizeof(size_t) < sizeof(uint64))
			return size_t(std::min(iCount, uint64(size_t(-1))));
		else
			return iCount;
		}
	};

/** Read data into memory, returning the count actually read. */
inline void ZStreamR::Read(void* iDest, size_t iCount, size_t* oCountRead) const
	{ const_cast<ZStreamR*>(this)->Imp_Read(iDest, iCount, oCountRead); }

/** Attempt to skip bytes, returning the count actually skipped. */
inline void ZStreamR::Skip(uint64 iCount, uint64* oCountSkipped) const
	{ const_cast<ZStreamR*>(this)->Imp_Skip(iCount, oCountSkipped); }

/** Skip bytes until the end of the stream is reached. */
inline void ZStreamR::SkipAll() const
	{ this->SkipAll(nil); }

/** Return the number of bytes guaranteed to be readable without blocking. */
inline size_t ZStreamR::CountReadable() const
	{ return const_cast<ZStreamR*>(this)->Imp_CountReadable(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR::ExEndOfStream

class ZStreamR::ExEndOfStream : public std::range_error
	{
public:
	ExEndOfStream();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRCon

/// Base interface for connected read streams.

class ZStreamRCon : public ZStreamR
	{
public:
/** \name Connection API
*/	//@{
	bool WaitReadable(int iMilliseconds) const;
	bool ReceiveDisconnect(int iMilliseconds) const;
	void Abort() const;
	//@}


/** \name Essential overrides
*/	//@{
	virtual bool Imp_WaitReadable(int iMilliseconds) = 0;
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds) = 0;

	virtual void Imp_Abort() = 0;
	//@}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU

/// Base interface for read streams that can unread a single byte.

class ZStreamU : public ZStreamR
	{
public:
	void Unread() const;

	virtual void Imp_Unread() = 0;
	};

/** \brief Update the stream so that the prior byte read will be returned
as the first byte of any subsequent call to \c ReadXX or \c SkipXX. */
inline void ZStreamU::Unread() const
	{ const_cast<ZStreamU*>(this)->Imp_Unread(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos

/// Base interface for positionable read streams.

class ZStreamRPos : public ZStreamU
	{
public:
/** \name Implementation of optional ZStreamR method
*/	//@{
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);
	//@}


/** \name Implementation of required ZStreamU method
*/	//@{
	virtual void Imp_Unread();
	//@}


/** \name Position and size API
*/	//@{
	uint64 GetPosition() const;
	void SetPosition(uint64 iPosition) const;

	uint64 GetSize() const;
	//@}


/** \name Essential overrides
*/	//@{
	virtual uint64 Imp_GetPosition() = 0;
	virtual void Imp_SetPosition(uint64 iPosition) = 0;

	virtual uint64 Imp_GetSize() = 0;
	//@}
	};

/** Return the stream's current position. */
inline uint64 ZStreamRPos::GetPosition() const
	{ return const_cast<ZStreamRPos*>(this)->Imp_GetPosition(); }

/** Set the stream's position. */
inline void ZStreamRPos::SetPosition(uint64 iPosition) const
	{ const_cast<ZStreamRPos*>(this)->Imp_SetPosition(iPosition); }

/** Return the stream's size. */
inline uint64 ZStreamRPos::GetSize() const
	{ return const_cast<ZStreamRPos*>(this)->Imp_GetSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW

/// Base interface for write streams.

class ZStreamW
	{
protected:
/** \name Canonical Methods
The canonical methods are protected, thus you cannot create, destroy or assign through a
ZStreamW reference, you must work with some derived class.
*/	//@{
	ZStreamW() {}
	~ZStreamW() {}
	ZStreamW(const ZStreamW&) {}
	ZStreamW& operator=(const ZStreamW&) { return *this; }
	//@}

public:
	class ExEndOfStream;
	static void sThrowEndOfStream();

/** \name Write raw data
Write data from memory into this stream.
*/	//@{
	void Write(const void* iSource, size_t iCount) const;
	void Write(const void* iSource, size_t iCount, size_t* oCountWritten) const;
	//@}


/** \name Copy from
Read data from iStreamR and write it to this stream.
*/	//@{
	void CopyAllFrom(const ZStreamR& iStreamR) const;
	void CopyAllFrom(const ZStreamR& iStreamR, uint64* oCountRead, uint64* oCountWritten) const;

	void CopyFrom(const ZStreamR& iStreamR, uint64 iCount) const;
	void CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten) const;
	//@}


/** \name Write formatted data
Write data to the stream in standard formats.
*/	//@{
	void WriteInt8(int8 iVal) const;
	void WriteInt16(int16 iVal) const;
	void WriteInt32(int32 iVal) const;
	void WriteInt64(int64 iVal) const;
	void WriteFloat(float iVal) const;
	void WriteDouble(double iVal) const;

	void WriteInt16LE(int16 iVal) const;
	void WriteInt32LE(int32 iVal) const;
	void WriteInt64LE(int64 iVal) const;
	void WriteFloatLE(float iVal) const;
	void WriteDoubleLE(double iVal) const;

	void WriteBool(bool iVal) const;

	void WriteUInt8(uint8 iVal) const { this->WriteInt8(int8(iVal)); }
	void WriteUInt16(uint16 iVal) const { this->WriteInt16(int16(iVal)); }
	void WriteUInt32(uint32 iVal) const { this->WriteInt32(int32(iVal)); }
	void WriteUInt64(uint64 iVal) const { this->WriteInt64(int64(iVal)); }

	void WriteUInt16LE(uint16 iVal) const { this->WriteInt16LE(int16(iVal)); }
	void WriteUInt32LE(uint32 iVal) const { this->WriteInt32LE(int32(iVal)); }
	void WriteUInt64LE(uint64 iVal) const { this->WriteInt64LE(int64(iVal)); }

	void WriteString(const char* iString) const;
	void WriteString(const std::string& iString) const;

	size_t Writef(const char* iString, ...) const;
	//@}


/** \name Useful variants
Write single bytes without exceptions being thrown.
*/	//@{
	bool WriteByte(uint8 iVal) const;
	//@}


/** \name Write a 'count'
*/	//@{
	void WriteCount(uint32 iCount) const;
	//@}


/** \name Flushing potentially buffered data
*/	//@{
	void Flush() const;
	//@}


/** \name Essential overrides
All ZStreamW functionality is implemented by calling this method.
*/	//@{
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten) = 0;
	//@}


/** \name Optionally overridable API
Default implementations that directly or indirectly call ZStreamW::Imp_Write are provided,
but particular subclasses may have more efficient implementations available.
*/	//@{
	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Flush();
	//@}

protected:
	static uint64 sDiffPosW(uint64 iSize, uint64 iPos)
		{ return iSize > iPos ? iSize - iPos : 0; }
	static size_t sClampedW(uint64 iCount)
		{
		if (sizeof(size_t) < sizeof(uint64))
			return size_t(std::min(iCount, uint64(size_t(-1))));
		else
			return iCount;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW::ExEndOfStream

class ZStreamW::ExEndOfStream : public std::range_error
	{
public:
	ExEndOfStream();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWCon

/// Base interface for connected write streams.

class ZStreamWCon : public ZStreamW
	{
public:
/** \name Connection API
*/	//@{
	void SendDisconnect() const;
	void Abort() const;
	//@}


/** \name Essential overrides
*/	//@{
	virtual void Imp_SendDisconnect() = 0;
	virtual void Imp_Abort() = 0;
	//@}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos

/// Base interface for positionable write streams.

class ZStreamWPos : public ZStreamW
	{
public:
	class ExBadSize;
	static void sThrowBadSize();

/** \name Position and size API
*/	//@{
	uint64 GetPosition() const;
	void SetPosition(uint64 iPosition) const;

	uint64 GetSize() const;
	void SetSize(uint64 iSize) const;

	void Truncate() const;
	//@}


/** \name Essential overrides
	API that must be overridden.
*/	//@{
	virtual uint64 Imp_GetPosition() = 0;
	virtual void Imp_SetPosition(uint64 iPosition) = 0;

	virtual uint64 Imp_GetSize() = 0;
	virtual void Imp_SetSize(uint64 iSize) = 0;
	//@}


/** \name Optionally overridable API
*/	//@{
	virtual void Imp_Truncate();
	//@}
	};

/** Return the stream's position. */
inline uint64 ZStreamWPos::GetPosition() const
	{ return const_cast<ZStreamWPos*>(this)->Imp_GetPosition(); }

/** Set the stream's position, without making any change in its size,
even if \a iPosition is greater than the stream's size. */
inline void ZStreamWPos::SetPosition(uint64 iPosition) const
	{ const_cast<ZStreamWPos*>(this)->Imp_SetPosition(iPosition); }

/** Return the stream's size. */
inline uint64 ZStreamWPos::GetSize() const
	{ return const_cast<ZStreamWPos*>(this)->Imp_GetSize(); }

/** Set the stream's size, without making any change in its position
even if that position is greater than \a iSize. */
inline void ZStreamWPos::SetSize(uint64 iSize) const
	{ return const_cast<ZStreamWPos*>(this)->Imp_SetSize(iSize); }

/** Set the stream's size to be the stream's position. */
inline void ZStreamWPos::Truncate() const
	{ return const_cast<ZStreamWPos*>(this)->Imp_Truncate(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos::ExBadSize

class ZStreamWPos::ExBadSize : public std::range_error
	{
public:
	ExBadSize();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos

/// Base interface for positionable read/write streams.

class ZStreamRWPos : public ZStreamRPos, public ZStreamWPos
	{
protected:
/** \name Canonical Methods
*/	//@{
	ZStreamRWPos() {}
	~ZStreamRWPos() {}
	ZStreamRWPos(const ZStreamRWPos&) {}
	ZStreamRWPos& operator=(const ZStreamRWPos&) { return *this; }
	//@}

public:
/** \name From ZStreamRPos/ZStreamWPos
	Re-declared here for disambiguation.
*/	//@{
	uint64 GetPosition() const;
	void SetPosition(uint64 iPosition) const;

	uint64 GetSize() const;

	virtual uint64 Imp_GetPosition() = 0;
	virtual void Imp_SetPosition(uint64 iPosition) = 0;

	virtual uint64 Imp_GetSize() = 0;
	//@}


/** \name From ZStreamWPos
	Re-declared here for disambiguation.
*/	//@{
	virtual void Imp_SetSize(uint64 iSize) = 0;
	//@}
	};

inline uint64 ZStreamRWPos::GetPosition() const
	{ return const_cast<ZStreamRWPos*>(this)->Imp_GetPosition(); }

inline void ZStreamRWPos::SetPosition(uint64 iPosition) const
	{ const_cast<ZStreamRWPos*>(this)->Imp_SetPosition(iPosition); }

inline uint64 ZStreamRWPos::GetSize() const
	{ return const_cast<ZStreamRWPos*>(this)->Imp_GetSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU_Unreader

/// Implements ZStreamU::Unread by buffering a single byte from a source ZStreamR.

class ZStreamU_Unreader : public ZStreamU
	{
public:
	ZStreamU_Unreader(const ZStreamR& iStreamSource);

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// From ZStreamU
	virtual void Imp_Unread();

private:
	const ZStreamR& fStreamSource;
	enum { eStateFresh, eStateNormal, eStateUnread, eStateHitEnd } fState;
	char fChar;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Null

/// A read stream with no content.

class ZStreamR_Null : public ZStreamR
	{
public:
	ZStreamR_Null();
	~ZStreamR_Null();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Null

/// A positionable read stream of zero size.

class ZStreamRPos_Null : public ZStreamRPos
	{
public:
	ZStreamRPos_Null();
	~ZStreamRPos_Null();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Null

/// A write stream that discards all data written to it.

class ZStreamW_Null : public ZStreamW
	{
public:
	ZStreamW_Null();
	~ZStreamW_Null();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

// Our protocol
	void Internal_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_Null

/// A positionable write stream that discards all data written to it.

class ZStreamWPos_Null : public ZStreamWPos
	{
public:
	ZStreamWPos_Null();
	~ZStreamWPos_Null();

// From ZStreamW
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
	void Internal_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

private:
	uint64 fPosition;
	uint64 fSize;
	};

#endif // __ZStream__
