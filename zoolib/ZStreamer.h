/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStreamer__
#define __ZStreamer__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZStream.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR

/** A refcounted entity that provides access to a ZStreamR.
\ingroup Streamer
*/

class ZStreamerR : public ZRefCountedWithFinalize
	{
public:
	virtual const ZStreamR& GetStreamR() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRCon

/** A refcounted entity that provides access to a ZStreamRCon.
\ingroup Streamer
*/

class ZStreamerRCon : public virtual ZStreamerR
	{
public:
// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

// Our protocol
	virtual const ZStreamRCon& GetStreamRCon() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerU

/** A refcounted entity that provides access to a ZStreamU.
\ingroup Streamer
*/

class ZStreamerU : public virtual ZStreamerR
	{
public:
// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

// Our protocol
	virtual const ZStreamU& GetStreamU() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos

/** A refcounted entity that provides access to a ZStreamRPos.
\ingroup Streamer
*/

class ZStreamerRPos : public ZStreamerU
	{
public:
// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

// From ZStreamerU
	virtual const ZStreamU& GetStreamU();

// Our protocol
	virtual const ZStreamRPos& GetStreamRPos() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW

/** A refcounted entity that provides access to a ZStreamW.
\ingroup Streamer
*/

class ZStreamerW : public ZRefCountedWithFinalize
	{
public:
	virtual const ZStreamW& GetStreamW() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWCon

/** A refcounted entity that provides access to a ZStreamWCon.
\ingroup Streamer
*/

class ZStreamerWCon : public virtual ZStreamerW
	{
public:
// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

// Our protocol
	virtual const ZStreamWCon& GetStreamWCon() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos

/** A refcounted entity that provides access to a ZStreamWPos.
\ingroup Streamer
*/

class ZStreamerWPos : public virtual ZStreamerW
	{
public:
// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

// Our protocol
	virtual const ZStreamWPos& GetStreamWPos() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos

/** A refcounted entity that provides access to a ZStreamRWPos.
\ingroup Streamer
*/

class ZStreamerRWPos
:	public virtual ZStreamerRPos,
	public virtual ZStreamerWPos
	{
public:
// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

// From ZStreamerU
	virtual const ZStreamU& GetStreamU();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos();

// Our protocol
	virtual const ZStreamRWPos& GetStreamRWPos() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRW

/** A refcounted entity that implements both ZStreamerR and ZStreamerW interfaces, and
thus provides access to a ZStreamR and a ZStreamW.
\ingroup Streamer
*/

class ZStreamerRW
:	public virtual ZStreamerR,
	public virtual ZStreamerW
	{
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWCon

/** A refcounted entity that implements both ZStreamerRCon and ZStreamerWCon interfaces, and
thus provides access to a ZStreamRCon and a ZStreamWCon.
\ingroup Streamer
*/

class ZStreamerRWCon
:	public ZStreamerRW,
	public ZStreamerRCon,
	public ZStreamerWCon
	{
public:
	void Disconnect();
	void Abort();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_T

template <class Stream_t>
class ZStreamerR_T : public ZStreamerR
	{
public:
	ZStreamerR_T() {}

	virtual ~ZStreamerR_T() {}

	template <class P>
	ZStreamerR_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_FT

template <class Stream_t>
class ZStreamerR_FT : public ZStreamerR
	{
protected:
	ZStreamerR_FT() {}

public:
	virtual ~ZStreamerR_FT() {}

	template <class P>
	ZStreamerR_FT(const P& iParam, ZRef<ZStreamerR> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamR())
		{}

	ZStreamerR_FT(ZRef<ZStreamerR> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamR())
		{}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerR> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRCon_T

template <class Stream_t>
class ZStreamerRCon_T : public ZStreamerRCon
	{
public:
	ZStreamerRCon_T() {}

	virtual ~ZStreamerRCon_T() {}

	template <class P>
	ZStreamerRCon_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerRCon
	virtual const ZStreamRCon& GetStreamRCon() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRCon_FT

template <class Stream_t>
class ZStreamerRCon_FT : public ZStreamerRCon
	{
protected:
	ZStreamerRCon_FT() {}

public:
	virtual ~ZStreamerRCon_FT() {}

	template <class P>
	ZStreamerRCon_FT(const P& iParam, ZRef<ZStreamerRCon> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamRCon())
		{}

	ZStreamerRCon_FT(ZRef<ZStreamerRCon> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamRCon())
		{}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerRCon
	virtual const ZStreamRCon& GetStreamRCon() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerRCon> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerU_T

template <class Stream_t>
class ZStreamerU_T : public ZStreamerU
	{
public:
	ZStreamerU_T() {}

	virtual ~ZStreamerU_T() {}

	template <class P>
	ZStreamerU_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerU
	virtual const ZStreamU& GetStreamU() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerU_FT

template <class Stream_t>
class ZStreamerU_FT : public ZStreamerU
	{
protected:
	ZStreamerU_FT() {}

public:
	virtual ~ZStreamerU_FT() {}

	template <class P>
	ZStreamerU_FT(const P& iParam, ZRef<ZStreamerR> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamR())
		{}

	ZStreamerU_FT(ZRef<ZStreamerR> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamR())
		{}

	template <class P>
	ZStreamerU_FT(const P& iParam, ZRef<ZStreamerU> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamU())
		{}

	ZStreamerU_FT(ZRef<ZStreamerU> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamU())
		{}

// From ZStreamerR via ZStreamerU
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerU
	virtual const ZStreamU& GetStreamU() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerR> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_T

template <class Stream_t>
class ZStreamerRPos_T : public ZStreamerRPos
	{
public:
	ZStreamerRPos_T() {}

	virtual ~ZStreamerRPos_T() {}

	template <class P>
	ZStreamerRPos_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerU
	virtual const ZStreamU& GetStreamU() { return fStream; }

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_FT

template <class Stream_t>
class ZStreamerRPos_FT : public ZStreamerU
	{
protected:
	ZStreamerRPos_FT() {}

public:
	virtual ~ZStreamerRPos_FT() {}

	template <class P>
	ZStreamerRPos_FT(const P& iParam, ZRef<ZStreamerRPos> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamRPos())
		{}

	ZStreamerRPos_FT(ZRef<ZStreamerRPos> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamRPos())
		{}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerU
	virtual const ZStreamU& GetStreamU() { return fStream; }

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerRPos> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_T

template <class Stream_t>
class ZStreamerW_T : public ZStreamerW
	{
public:
	ZStreamerW_T() {}

	virtual ~ZStreamerW_T() {}

	template <class P>
	ZStreamerW_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerW
	virtual const ZStreamW& GetStreamW() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_FT

template <class Stream_t>
class ZStreamerW_FT : public ZStreamerW
	{
protected:
	ZStreamerW_FT() {}

public:
	virtual ~ZStreamerW_FT() {}

	template <class P>
	ZStreamerW_FT(const P& iParam, ZRef<ZStreamerW> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamW())
		{}

	ZStreamerW_FT(ZRef<ZStreamerW> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamW())
		{}

// From ZStreamerW
	virtual const ZStreamW& GetStreamW() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerW> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWCon_T

template <class Stream_t>
class ZStreamerWCon_T : public ZStreamerWCon
	{
public:
	ZStreamerWCon_T() {}

	virtual ~ZStreamerWCon_T() {}

	template <class P>
	ZStreamerWCon_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerW via ZStreamerWCon
	virtual const ZStreamW& GetStreamW() { return fStream; }

// From ZStreamerWCon
	virtual const ZStreamWCon& GetStreamWCon() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWCon_FT

template <class Stream_t>
class ZStreamerWCon_FT : public ZStreamerWCon
	{
protected:
	ZStreamerWCon_FT() {}

public:
	virtual ~ZStreamerWCon_FT() {}

	template <class P>
	ZStreamerWCon_FT(const P& iParam, ZRef<ZStreamerWCon> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamWCon())
		{}

	ZStreamerWCon_FT(ZRef<ZStreamerWCon> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamWCon())
		{}

// From ZStreamerW via ZStreamerWCon
	virtual const ZStreamW& GetStreamW() { return fStream; }

// From ZStreamerWCon
	virtual const ZStreamWCon& GetStreamWCon() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerWCon> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_T

template <class Stream_t>
class ZStreamerWPos_T : public ZStreamerWPos
	{
public:
	ZStreamerWPos_T() {}

	virtual ~ZStreamerWPos_T() {}

	template <class P>
	ZStreamerWPos_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerW via ZStreamerWPos
	virtual const ZStreamW& GetStreamW() { return fStream; }

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_FT

template <class Stream_t>
class ZStreamerWPos_FT : public ZStreamerWPos
	{
protected:
	ZStreamerWPos_FT() {}

public:
	virtual ~ZStreamerWPos_FT() {}

	template <class P>
	ZStreamerWPos_FT(const P& iParam, ZRef<ZStreamerWPos> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamWPos())
		{}

	ZStreamerWPos_FT(ZRef<ZStreamerWPos> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamWPos())
		{}

// From ZStreamerW via ZStreamerWPos
	virtual const ZStreamW& GetStreamW() { return fStream; }

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerWPos> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_T

template <class Stream_t>
class ZStreamerRWPos_T : public ZStreamerRWPos
	{
public:
	ZStreamerRWPos_T() {}

	virtual ~ZStreamerRWPos_T() {}

	template <class P>
	ZStreamerRWPos_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerR via ZStreamerRWPos
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerU via ZStreamerRWPos
	virtual const ZStreamU& GetStreamU() { return fStream; }

// From ZStreamerRPos via ZStreamerRWPos
	virtual const ZStreamRPos& GetStreamRPos() { return fStream; }

// From ZStreamerW via ZStreamerRWPos
	virtual const ZStreamW& GetStreamW() { return fStream; }

// From ZStreamerWPos via ZStreamerRWPos
	virtual const ZStreamWPos& GetStreamWPos() { return fStream; }

// From ZStreamerRWPos
	virtual const ZStreamRWPos& GetStreamRWPos() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_FT

template <class Stream_t>
class ZStreamerRWPos_FT : public ZStreamerRWPos
	{
protected:
	ZStreamerRWPos_FT() {}

public:
	virtual ~ZStreamerRWPos_FT() {}

	template <class P>
	ZStreamerRWPos_FT(const P& iParam, ZRef<ZStreamerRWPos> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iParam, iStreamer->GetStreamRWPos())
		{}

	ZStreamerRWPos_FT(ZRef<ZStreamerRWPos> iStreamer)
	:	fStreamerReal(iStreamer),
		fStream(iStreamer->GetStreamRWPos())
		{}

// From ZStreamerR via ZStreamerRWPos
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerU via ZStreamerRWPos
	virtual const ZStreamU& GetStreamU() { return fStream; }

// From ZStreamerRPos via ZStreamerRWPos
	virtual const ZStreamRPos& GetStreamRPos() { return fStream; }

// From ZStreamerW via ZStreamerRWPos
	virtual const ZStreamW& GetStreamW() { return fStream; }

// From ZStreamerWPos via ZStreamerRWPos
	virtual const ZStreamWPos& GetStreamWPos() { return fStream; }

// From ZStreamerRWPos
	virtual const ZStreamRWPos& GetStreamRWPos() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	ZRef<ZStreamerRWPos> fStreamerReal;
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRW_T

template <class Stream_t>
class ZStreamerRW_T : public ZStreamerRW
	{
public:
	ZStreamerRW_T() {}

	virtual ~ZStreamerRW_T() {}

	template <class P>
	ZStreamerRW_T(const P& iParam) : fStream(iParam) {}

// From ZStreamerR via ZStreamerRW
	virtual const ZStreamR& GetStreamR() { return fStream; }

// From ZStreamerW via ZStreamerRW
	virtual const ZStreamW& GetStreamW() { return fStream; }

// Our protocol
	Stream_t& GetStream() { return fStream; }

protected:
	Stream_t fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Streamer

class ZStreamR_Streamer : public ZStreamR
	{
public:
	ZStreamR_Streamer(ZRef<ZStreamerR> iStreamer);
	~ZStreamR_Streamer();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	ZRef<ZStreamerR> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU_Streamer

class ZStreamU_Streamer : public ZStreamU
	{
public:
	ZStreamU_Streamer(ZRef<ZStreamerU> iStreamer);
	~ZStreamU_Streamer();

// From ZStreamR via ZStreamU
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamU
	virtual void Imp_Unread();

private:
	ZRef<ZStreamerU> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Streamer

class ZStreamRPos_Streamer : public ZStreamRPos
	{
public:
	ZStreamRPos_Streamer(ZRef<ZStreamerRPos> iStreamer);
	~ZStreamRPos_Streamer();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

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
	ZRef<ZStreamerRPos> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Streamer

class ZStreamW_Streamer : public ZStreamW
	{
public:
	ZStreamW_Streamer(ZRef<ZStreamerW> iStreamer);
	~ZStreamW_Streamer();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Flush();

private:
	ZRef<ZStreamerW> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_Streamer

class ZStreamWPos_Streamer : public ZStreamWPos
	{
public:
	ZStreamWPos_Streamer(ZRef<ZStreamerWPos> iStreamer);
	~ZStreamWPos_Streamer();

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Flush();

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

private:
	ZRef<ZStreamerWPos> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_Streamer

class ZStreamRWPos_Streamer : public ZStreamRWPos
	{
public:
	ZStreamRWPos_Streamer(ZRef<ZStreamerRWPos> iStreamer);
	~ZStreamRWPos_Streamer();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 iSize);

// Our protocol
	ZRef<ZStreamerRWPos> GetStreamer()
		{ return fStreamer; }

	void SetStreamer(ZRef<ZStreamerRWPos> iStreamer)
		{ fStreamer = iStreamer; }

private:
	ZRef<ZStreamerRWPos> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRW_Wrapper

/// A RW streamer that wraps a ZStreamerR and a ZStreamerW into a single entity.

class ZStreamerRW_Wrapper : public ZStreamerRW
	{
public:
	ZStreamerRW_Wrapper(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~ZStreamerRW_Wrapper();

// From ZStreamerR via ZStreamerRW
	virtual const ZStreamR& GetStreamR();

// From ZStreamerW via ZStreamerRW
	virtual const ZStreamW& GetStreamW();

// Our protocol
	ZRef<ZStreamerR> GetStreamerR();
	ZRef<ZStreamerW> GetStreamerW();

protected:
	ZRef<ZStreamerR> fStreamerR;
	const ZStreamR& fStreamR;
	ZRef<ZStreamerW> fStreamerW;
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_Stream

/** \brief An R streamer that wraps a ZStreamR.

Note that the ZStreamR's lifetime must extend beyond that of the ZStreamerR.
*/

class ZStreamerR_Stream : public ZStreamerR
	{
public:
	ZStreamerR_Stream(const ZStreamR& iStreamR)
	:	fStreamR(iStreamR)
		{}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR() { return fStreamR; }

protected:
	const ZStreamR& fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_Stream

/** \brief A W streamer that wraps a ZStreamW.

Note that the ZStreamW's lifetime must extend beyond that of the ZStreamerW.
*/

class ZStreamerW_Stream : public ZStreamerW
	{
public:
	ZStreamerW_Stream(const ZStreamW& iStreamW)
	:	fStreamW(iStreamW)
		{}

// From ZStreamerR
	virtual const ZStreamW& GetStreamW() { return fStreamW; }

protected:
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRFactory

class ZStreamerRFactory : public ZRefCountedWithFinalize
	{
protected:
	ZStreamerRFactory();
	virtual ~ZStreamerRFactory();

public:
	virtual ZRef<ZStreamerR> MakeStreamerR() = 0;
	virtual void Cancel();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRConFactory

class ZStreamerRConFactory : public virtual ZStreamerRFactory
	{
protected:
	ZStreamerRConFactory();
	virtual ~ZStreamerRConFactory();

public:
// From ZStreamerRFactory
	virtual ZRef<ZStreamerR> MakeStreamerR();

// Our protocol
	virtual ZRef<ZStreamerRCon> MakeStreamerRCon() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWFactory

class ZStreamerWFactory : public ZRefCountedWithFinalize
	{
protected:
	ZStreamerWFactory();
	virtual ~ZStreamerWFactory();

public:
	virtual ZRef<ZStreamerW> MakeStreamerW() = 0;
	virtual void Cancel();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWConFactory

class ZStreamerWConFactory : public virtual ZStreamerWFactory
	{
protected:
	ZStreamerWConFactory();
	virtual ~ZStreamerWConFactory();

public:
// From ZStreamerWFactory
	virtual ZRef<ZStreamerW> MakeStreamerW();

// Our protocol
	virtual ZRef<ZStreamerWCon> MakeStreamerWCon() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWFactory

class ZStreamerRWFactory
:	public virtual ZStreamerRFactory,
	public virtual ZStreamerWFactory
	{
protected:
	ZStreamerRWFactory();
	virtual ~ZStreamerRWFactory();

public:
//	From ZStreamerRFactory
	virtual ZRef<ZStreamerR> MakeStreamerR();

//	From ZStreamerWFactory
	virtual ZRef<ZStreamerW> MakeStreamerW();

// From ZStreamerRFactory and ZStreamerWFactory
	virtual void Cancel();

// Our protocol
	virtual ZRef<ZStreamerRW> MakeStreamerRW() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWConFactory

class ZStreamerRWConFactory
:	public ZStreamerRConFactory,
	public ZStreamerWConFactory,
	public ZStreamerRWFactory
	{
protected:
	ZStreamerRWConFactory();
	virtual ~ZStreamerRWConFactory();

public:
// From ZStreamerRFactory via ZStreamerRConFactory
// and ZStreamerRWFactory (disambiguate)
	virtual ZRef<ZStreamerR> MakeStreamerR();

// From ZStreamerRConFactory
	virtual ZRef<ZStreamerRCon> MakeStreamerRCon();

// From ZStreamerWFactory via ZStreamerWConFactory
// and ZStreamerRWFactory (disambiguate)
	virtual ZRef<ZStreamerW> MakeStreamerW();

// From ZStreamerWConFactory
	virtual ZRef<ZStreamerWCon> MakeStreamerWCon();

// From ZStreamerRFactory via ZStreamerRConFactory,
// ZStreamerWFactory via ZStreamerWConFactory,
// and ZStreamerRWFactory (disambiguate)
	virtual void Cancel();

// From ZStreamerRWFactory
	virtual ZRef<ZStreamerRW> MakeStreamerRW();

// Our protocol
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon() = 0;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStreamer__
