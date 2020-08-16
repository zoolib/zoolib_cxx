// Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZStream_ZLib_h__
#define __ZStream_ZLib_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Stream_ZLib
	#define ZCONFIG_API_Avail__Stream_ZLib ZCONFIG_SPI_Enabled(zlib)
#endif

#ifndef ZCONFIG_API_Desired__Stream_ZLib
	#define ZCONFIG_API_Desired__Stream_ZLib 1
#endif

#include "zoolib/ZStreamer.h"

#if ZCONFIG_API_Enabled(Stream_ZLib)

#include <zlib.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStream_ZLib

namespace ZStream_ZLib {

enum EFormatR
	{
	eFormatR_Raw,
	eFormatR_ZLib,
	eFormatR_GZip,
	eFormatR_Auto
	};

enum EFormatW
	{
	eFormatW_Raw,
	eFormatW_ZLib,
	eFormatW_GZip
	};
}

// =================================================================================================
#pragma mark - ZStreamR_ZLibDecode

/// A read filter stream that zlib-decompresses (inflates) a source stream.

class ZStreamR_ZLibDecode : public ZStreamR
	{
public:
	ZStreamR_ZLibDecode(const ZStreamR& iStreamSource);

	ZStreamR_ZLibDecode(
		ZStream_ZLib::EFormatR iFormatR, size_t iBufferSize,
		const ZStreamR& iStreamSource);

	~ZStreamR_ZLibDecode();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

protected:
	void pInit(ZStream_ZLib::EFormatR iFormatR, size_t iBufferSize);

	const ZStreamR& fStreamSource;
	z_stream fState;
	Bytef* fBuffer;
	size_t fBufferSize;
	};

// =================================================================================================
#pragma mark - ZStreamerR_ZLibDecode

/// A read filter streamer that zlib-decompresses (inflates) a source stream.

class ZStreamerR_ZLibDecode : public ZStreamerR
	{
public:
	ZStreamerR_ZLibDecode(ZRef<ZStreamerR> iStreamer);

	ZStreamerR_ZLibDecode(
		ZStream_ZLib::EFormatR iFormatR, size_t iBufferSize,
		ZRef<ZStreamerR> iStreamer);

	virtual ~ZStreamerR_ZLibDecode();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZRef<ZStreamerR> fStreamer;
	ZStreamR_ZLibDecode fStream;
	};

// =================================================================================================
#pragma mark - ZStreamW_ZLibEncode

/// A write filter stream that zlib-compresses (deflates) to a destination stream.

class ZStreamW_ZLibEncode : public ZStreamW
	{
public:
	ZStreamW_ZLibEncode(const ZStreamW& iStreamSink);

	ZStreamW_ZLibEncode(
		ZStream_ZLib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize,
		const ZStreamW& iStreamSink);

	~ZStreamW_ZLibEncode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	void pInit(ZStream_ZLib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize);
	void pFlush();

	const ZStreamW& fStreamSink;
	z_stream fState;
	Bytef* fBuffer;
	size_t fBufferSize;
	};

// =================================================================================================
#pragma mark - ZStreamerW_ZLibEncode

/// A write filter streamer that zlib-compresses (deflates) to a destination stream.

class ZStreamerW_ZLibEncode : public ZStreamerW
	{
public:
	ZStreamerW_ZLibEncode(ZRef<ZStreamerW> iStreamer);

	ZStreamerW_ZLibEncode(
		ZStream_ZLib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize,
		ZRef<ZStreamerW> iStreamer);

	virtual ~ZStreamerW_ZLibEncode();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZRef<ZStreamerW> fStreamer;
	ZStreamW_ZLibEncode fStream;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Stream_ZLib)

#endif // __ZStream_ZLib_h__
