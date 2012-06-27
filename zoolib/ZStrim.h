/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStrim_h__
#define __ZStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ZUnicodeString.h"

#include <stdarg.h> // For va_list
#include <stdexcept> // For range_error

namespace ZooLib {

class ZStrimW;

// =================================================================================================
// MARK: - ZStrim

namespace ZStrim {

class ExEndOfStrim : public std::range_error
	{
protected:
	ExEndOfStrim(const char* iWhat);
	};

} // namespace ZStrim

// =================================================================================================
// MARK: - ZStrimR

class ZStrimR
	{
protected:
/** \name Canonical Methods
The canonical methods are protected, thus you cannot create, destroy or assign through a
ZStrimR reference, you must work with some derived class.
*/	//@{
	ZStrimR() {}
	virtual ~ZStrimR() {}
	ZStrimR(const ZStrimR&) {}
	ZStrimR& operator=(const ZStrimR&) { return *this; }
	//@}


public:
	class ExEndOfStrim;
	static void sThrowEndOfStrim();

/** \name Read a single code point
*/	//@{
	bool ReadCP(UTF32& oCP) const;
	UTF32 ReadCP() const;
	//@}


/** \name Standard library strings
*/	//@{
	string32 ReadUTF32(size_t iCountCP) const;
	string16 ReadUTF16(size_t iCountCP) const;
	string8 ReadUTF8(size_t iCountCP) const;
	//@}


/** \name String buffers
*/	//@{
	void Read(UTF32* oDest, size_t iCount, size_t* oCount) const;
	void Read(UTF16* oDest, size_t iCountCU, size_t* oCountCU) const;
	void Read(UTF8* oDest, size_t iCountCU, size_t* oCountCU) const;
	//@}


/** \name String buffers, limiting and reporting both CU and CP
*/	//@{
	void Read(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;

	void Read(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;
	//@}

	string8 ReadAll8() const;
	string16 ReadAll16() const;
	string32 ReadAll32() const;

/** \name Copy To
Read code points from this strim and write them to iStrimW.
*/	//@{
	const ZStrimR& CopyAllTo(const ZStrimW& iStrimW) const;

	const ZStrimR& CopyAllTo(const ZStrimW& iStrimW,
		uint64* oCountCPRead, uint64* oCountCPWritten) const;

	const ZStrimR& CopyTo(const ZStrimW& iStrimW, uint64 iCountCP) const;

	const ZStrimR& CopyTo(const ZStrimW& iStrimW, uint64 iCountCP,
		uint64* oCountCPRead, uint64* oCountCPWritten) const;
	//@}


/** \name Skip
Skip over code points from this strim without returning them to the caller.
*/	//@{
	void Skip(uint64 iCountCP) const;
	void Skip(uint64 iCountCP, uint64* oCountCPSkipped) const;
	void SkipAll() const;
	void SkipAll(uint64* oCountCPSkipped) const;
	//@}


/** \name Essential overrides
These methods must be overridden by subclasses.
*/	//@{
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount) = 0;
	//@}


/** \name Optional overrides
Default implementations that directly or indirectly call ZStrimR::Imp_ReadUTF32 are provided,
but particular subclasses may have more efficient implementations available.
*/	//@{
	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_CopyToDispatch(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyTo(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual bool Imp_ReadCP(UTF32& oCP);

	virtual void Imp_Skip(uint64 iCountCP, uint64* oCountCPSkipped);
	//@}
	};

// =================================================================================================
// MARK: - ZStrimR::ExEndOfStrim

class ZStrimR::ExEndOfStrim : public ZStrim::ExEndOfStrim
	{
public:
	ExEndOfStrim();
	};

// =================================================================================================
// MARK: - ZStrimU

class ZStrimU : public ZStrimR
	{
public:
	void Unread(UTF32 iCP) const;
	size_t UnreadableLimit() const;

	virtual void Imp_Unread(UTF32 iCP) = 0;
	virtual size_t Imp_UnreadableLimit() = 0;
	};

// =================================================================================================
// MARK: - ZStrimW

class ZStrimW
	{
protected:
/** \name Canonical Methods
The canonical methods are protected, thus you cannot create, destroy or assign through a
ZStrimW reference, you must work with some derived class.
*/	//@{
	ZStrimW() {}
	virtual ~ZStrimW() {}
	ZStrimW(const ZStrimW&) {}
	ZStrimW& operator=(const ZStrimW&) { return *this; }
	//@}


public:
	class ExEndOfStrim;
	static void sThrowEndOfStrim();

/** \name Single code point
*/	//@{
	const ZStrimW& WriteCP(UTF32 iCP) const;
	//@}


/** \name Zero-terminated strings
*/	//@{
	const ZStrimW& Write(const UTF32* iString) const;
	const ZStrimW& Write(const UTF16* iString) const;
	const ZStrimW& Write(const UTF8* iString) const;
	//@}


/** \name Standard library strings
*/	//@{
	const ZStrimW& Write(const string32& iString) const;
	const ZStrimW& Write(const string16& iString) const;
	const ZStrimW& Write(const string8& iString) const;
	//@}


/** \name String buffers
*/	//@{
	const ZStrimW& Write(const UTF32* iSource, size_t iCountCU) const;
	const ZStrimW& Write(const UTF16* iSource, size_t iCountCU) const;
	const ZStrimW& Write(const UTF8* iSource, size_t iCountCU) const;
	//@}


/** \name String buffers, report CUs written
*/	//@{
	const ZStrimW& Write(const UTF32* iSource, size_t iCountCU, size_t* oCountCU) const;
	const ZStrimW& Write(const UTF16* iSource, size_t iCountCU, size_t* oCountCU) const;
	const ZStrimW& Write(const UTF8* iSource, size_t iCountCU, size_t* oCountCU) const;
	//@}


/** \name String buffers, limiting and reporting both CU and CP
*/	//@{
	const ZStrimW& Write(const UTF32* iSource,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;

	const ZStrimW& Write(const UTF16* iSource,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;

	const ZStrimW& Write(const UTF8* iSource,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP) const;
	//@}


/** \name Formatted strings
*/	//@{
	const ZStrimW& Writef(const UTF8* iString, ...) const
		ZMACRO_Attribute_Format_Printf(2,3);
	const ZStrimW& Writef(size_t* oCountCU, size_t* oWritten, const UTF8* iString, ...) const
		ZMACRO_Attribute_Format_Printf(4,5);
	const ZStrimW& Writev(const UTF8* iString, va_list iArgs) const;
	const ZStrimW& Writev(size_t* oCountCU, size_t* oWritten,
		const UTF8* iString, va_list iArgs) const;
	//@}


/** \name Copy from
Read code points from iStrimR and write it to this strim.
*/	//@{
	const ZStrimW& CopyAllFrom(const ZStrimR& iStrimR) const;

	const ZStrimW& CopyAllFrom(const ZStrimR& iStrimR,
		uint64* oCountCPRead, uint64* oCountCPWritten) const;

	const ZStrimW& CopyFrom(const ZStrimR& iStrimR, uint64 iCountCP) const;

	const ZStrimW& CopyFrom(const ZStrimR& iStrimR, uint64 iCountCP,
		uint64* oCountCPRead, uint64* oCountCPWritten) const;
	//@}


/** \name Flushing potentially buffered data
*/	//@{
	const ZStrimW& Flush() const;
	//@}


/** \name Essential overrides
These methods must be overridden by subclasses.
*/	//@{
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU) = 0;
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU) = 0;
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU) = 0;
	//@}


/** \name Optional overrides
*/	//@{
	virtual void Imp_CopyFromDispatch(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyFrom(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_Flush();
	//@}

protected:
	// Internal implementation
	void pWrite(const UTF32* iString) const;
	void pWrite(const UTF16* iString) const;
	void pWrite(const UTF8* iString) const;

	void pWrite(const string32& iString) const;
	void pWrite(const string16& iString) const;
	void pWrite(const string8& iString) const;

	void pWrite(const UTF32* iSource, size_t iCountCU) const;
	void pWrite(const UTF16* iSource, size_t iCountCU) const;
	void pWrite(const UTF8* iSource, size_t iCountCU) const;

	void pWrite(const UTF32* iSource, size_t iCountCU, size_t* oCountCU) const;
	void pWrite(const UTF16* iSource, size_t iCountCU, size_t* oCountCU) const;
	void pWrite(const UTF8* iSource, size_t iCountCU, size_t* oCountCU) const;

	void pWritev(size_t* oCountCU, size_t* oWritten, const UTF8* iString, va_list iArgs) const;
	};

// =================================================================================================
// MARK: - ZStrimW::ExEndOfStrim

class ZStrimW::ExEndOfStrim : public ZStrim::ExEndOfStrim
	{
public:
	ExEndOfStrim();
	};

// =================================================================================================
// MARK: - ZStrimW_NativeUTF32

/**
A derivation of ZStrimW that provides implementations of ZStrimW::Imp_WriteUTF16 and
ZStrimW::Imp_WriteUTF8 that call through to ZStrimW::Imp_WriteUTF32, which must be implemented
by subclasses. We redeclare ZStrimW::Imp_WriteUTF32 as pure virtual for emphasis.
*/

class ZStrimW_NativeUTF32 : public ZStrimW
	{
public:
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU) = 0;
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);
	};

// =================================================================================================
// MARK: - ZStrimW_NativeUTF16

/**
A derivation of ZStrimW that provides implementations of ZStrimW::Imp_WriteUTF32 and
ZStrimW::Imp_WriteUTF8 that call through to ZStrimW::Imp_WriteUTF16, which must be implemented
by subclasses. We redeclare ZStrimW::Imp_WriteUTF16 as pure virtual for emphasis.
*/

class ZStrimW_NativeUTF16 : public ZStrimW
	{
public:
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU) = 0;
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);
	};

// =================================================================================================
// MARK: - ZStrimW_NativeUTF8

/**
A derivation of ZStrimW that provides implementations of ZStrimW::Imp_WriteUTF32 and
ZStrimW::Imp_WriteUTF16 that call through to ZStrimW::Imp_WriteUTF8, which must be implemented
by subclasses. We redeclare ZStrimW::Imp_WriteUTF8 as pure virtual for emphasis.
*/

class ZStrimW_NativeUTF8 : public ZStrimW
	{
public:
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU) = 0;
	};

// =================================================================================================
// MARK: - ZStrimW_T

template <class Base_t, class Self_t>
class ZStrimW_T : public Base_t
	{
public:
//	using Base_t::Write;

	const Self_t& Write(const UTF32* iString) const
		{
		this->pWrite(iString);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Write(const UTF16* iString) const
		{
		this->pWrite(iString);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Write(const UTF8* iString) const
		{
		this->pWrite(iString);
		return static_cast<const Self_t&>(*this);
		}


	const Self_t& Write(const string32& iString) const
		{
		this->pWrite(iString);
		return static_cast<const Self_t&>(*this);
		}
	const Self_t& Write(const string16& iString) const
		{
		this->pWrite(iString);
		return static_cast<const Self_t&>(*this);
		}
	const Self_t& Write(const string8& iString) const
		{
		this->pWrite(iString);
		return static_cast<const Self_t&>(*this);
		}


	const Self_t& Write(const UTF32* iSource, size_t iCountCU) const
		{
		this->pWrite(iSource, iCountCU);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Write(const UTF16* iSource, size_t iCountCU) const
		{
		this->pWrite(iSource, iCountCU);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Write(const UTF8* iSource, size_t iCountCU) const
		{
		this->pWrite(iSource, iCountCU);
		return static_cast<const Self_t&>(*this);
		}


	const Self_t& Write(const UTF32* iSource, size_t iCountCU, size_t* oCountCU) const
		{
		this->pWrite(iSource, iCountCU, oCountCU);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Write(const UTF16* iSource, size_t iCountCU, size_t* oCountCU) const
		{
		this->pWrite(iSource, iCountCU, oCountCU);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Write(const UTF8* iSource, size_t iCountCU, size_t* oCountCU) const
		{
		this->pWrite(iSource, iCountCU, oCountCU);
		return static_cast<const Self_t&>(*this);
		}


	const Self_t& Writef(const UTF8* iString, ...) const
		ZMACRO_Attribute_Format_Printf(2,3)
		{
		va_list args;
		va_start(args, iString);
		size_t countCU, countWritten;
		this->pWritev(&countCU, &countWritten, iString, args);
		if (countWritten != countCU)
			Base_t::sThrowEndOfStrim();
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Writef(size_t* oCountCU, size_t* oWritten, const UTF8* iString, ...) const
		ZMACRO_Attribute_Format_Printf(4,5)
		{
		va_list args;
		va_start(args, iString);
		this->pWritev(oCountCU, oWritten, iString, args);
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Writev(const UTF8* iString, va_list iArgs) const
		{
		size_t countCU, countWritten;
		this->pWritev(&countCU, &countWritten, iString, iArgs);
		if (countWritten != countCU)
			Base_t::sThrowEndOfStrim();
		return static_cast<const Self_t&>(*this);
		}

	const Self_t& Writev(size_t* oCountCU, size_t* oWritten,
		const UTF8* iString, va_list iArgs) const
		{
		this->pWritev(oCountCU, oWritten, iString, iArgs);
		return static_cast<const Self_t&>(*this);
		}
	};

// =================================================================================================
// MARK: - ZStrimR_Null

/// A derivation of ZStrimR that is empty

class ZStrimR_Null : public ZStrimR
	{
public:
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_CopyToDispatch(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyTo(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_Skip(uint64 iCountCP, uint64* oCountCPSkipped);
	};

// =================================================================================================
// MARK: - ZStrimW_Null

/// A derivation of ZStrimW that discards without error any code points written to it.

class ZStrimW_Null : public ZStrimW
	{
public:
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);

	virtual void Imp_CopyFromDispatch(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyFrom(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);
	};

typedef ZStrimW_Null ZStrimW_Sink; // Old name.

// =================================================================================================
// MARK: - ZStrimU_String

/// Provides a ZStrimU interface to a standard library string containing UTF-32 code units.

class ZStrimU_String32 : public ZStrimU
	{
public:
	ZStrimU_String32(const string32& iString);
	~ZStrimU_String32();

// From ZStrimR via ZStrimU
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual bool Imp_ReadCP(UTF32& oCP);

// From ZStrimU
	virtual void Imp_Unread(UTF32 iCP);
	virtual size_t Imp_UnreadableLimit();

private:
	string32 fString;
	size_t fPosition;
	};

// =================================================================================================
// MARK: - ZStrimU_String16

/// Provides a ZStrimU interface to a standard library string containing UTF-16 code units.

class ZStrimU_String16 : public ZStrimU
	{
public:
	ZStrimU_String16(const string16& iString);
	~ZStrimU_String16();

// From ZStrimR via ZStrimU
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

// From ZStrimU
	virtual void Imp_Unread(UTF32 iCP);
	virtual size_t Imp_UnreadableLimit();

private:
	string16 fString;
	size_t fPosition;
	};

// =================================================================================================
// MARK: - ZStrimU_String8Ref

class ZStrimU_String8Ref : public ZStrimU
	{
public:
	ZStrimU_String8Ref(const string8& iStringRef);
	~ZStrimU_String8Ref();

// From ZStrimR via ZStrimU
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

// From ZStrimU
	virtual void Imp_Unread(UTF32 iCP);
	virtual size_t Imp_UnreadableLimit();

// Our protocol
	const string8& GetString8() const;

private:
	const string8& fString;
	size_t fPosition;
	};

// =================================================================================================
// MARK: - ZStrimU_String8

/// Provides a ZStrimU interface to a standard library string containing UTF-8 code units.

struct ZStrimU_String8Helper
	{
	ZStrimU_String8Helper(const string8& iString);
	const string8 fStringStorage;
	};

class ZStrimU_String8
:	public ZStrimU_String8Helper
,	public ZStrimU_String8Ref
	{
public:
	ZStrimU_String8(const string8& iString);
	};

typedef ZStrimU_String8 ZStrimU_String;

// =================================================================================================
// MARK: - ZStrimW_String

template <class T>
class ZStrimW_String;

template <>
class ZStrimW_String<string32>
:	public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_String(string32& ioString);
	~ZStrimW_String();

	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);

private:
	string32& fString;
	};

template <>
class ZStrimW_String<string16>
:	public ZStrimW_NativeUTF16
	{
public:
	ZStrimW_String(string16& ioString);
	~ZStrimW_String();

	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);

private:
	string16& fString;
	};

template <>
class ZStrimW_String<string8>
:	public ZStrimW_NativeUTF8
	{
public:
	ZStrimW_String(string8& ioString);
	~ZStrimW_String();

	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);

private:
	string8& fString;
	};

typedef ZStrimW_String<string32> ZStrimW_String32;
typedef ZStrimW_String<string16> ZStrimW_String16;
typedef ZStrimW_String<string8> ZStrimW_String8;

// =================================================================================================
// MARK: - ZStrimW_Indirect

class ZStrimW_Indirect : public ZStrimW
	{
public:
	ZStrimW_Indirect();
	ZStrimW_Indirect(const ZStrimW& iStrimW);
	~ZStrimW_Indirect();

// From ZStrimW
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);

	virtual void Imp_CopyFromDispatch(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyFrom(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_Flush();

// Our protocol
	void SetSink(const ZStrimW& iStrimW);
	void SetSink(const ZStrimW* iStrimW);

private:
	const ZStrimW* fSink;
	};

} // namespace ZooLib

#endif // __ZStrim_h__
