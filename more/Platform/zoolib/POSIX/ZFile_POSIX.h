/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZFile_POSIX_h__
#define __ZFile_POSIX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__File_POSIX
	#define ZCONFIG_API_Avail__File_POSIX ZCONFIG_SPI_Enabled(POSIX)
#endif

#ifndef ZCONFIG_API_Desired__File_POSIX
	#define ZCONFIG_API_Desired__File_POSIX 1
#endif

#include "zoolib/ZFile.h"

#if ZCONFIG_API_Enabled(File_POSIX)

ZMACRO_MSVCStaticLib_Reference(File_Win)

namespace ZooLib {

// =================================================================================================
// MARK: - ZFileLoc_POSIX

class ZFileLoc_POSIX : public ZFileLoc
	{
public:
	static ZRef<ZFileLoc_POSIX> sGet_CWD();
	static ZRef<ZFileLoc_POSIX> sGet_Root();
	static ZRef<ZFileLoc_POSIX> sGet_App();

	ZFileLoc_POSIX(bool iIsAtRoot);
	ZFileLoc_POSIX(bool iIsAtRoot, const std::vector<std::string>& iComps);
	ZFileLoc_POSIX(bool iIsAtRoot, const std::string* iComps, size_t iCount);
	ZFileLoc_POSIX(bool iIsAtRoot, std::vector<std::string>* ioComps, const IKnowWhatIAmDoing_t&);
	virtual ~ZFileLoc_POSIX();

// From ZFileLoc
	virtual ZRef<ZFileIterRep> CreateIterRep();

	virtual std::string GetName(ZFile::Error* oErr) const;
	virtual ZQ<ZTrail> TrailTo(ZRef<ZFileLoc> oDest, ZFile::Error* oErr) const;

	virtual ZRef<ZFileLoc> GetParent(ZFile::Error* oErr);
	virtual ZRef<ZFileLoc> GetDescendant(
		const std::string* iComps, size_t iCount, ZFile::Error* oErr);

	virtual bool IsRoot();

	virtual ZRef<ZFileLoc> Follow();

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount);
	virtual std::string AsString_Native(const std::string* iComps, size_t iCount);

	virtual ZFile::Kind Kind(ZFile::Error* oError);
	virtual uint64 Size(ZFile::Error* oErr);
	virtual ZTime TimeCreated(ZFile::Error* oErr);
	virtual ZTime TimeModified(ZFile::Error* oErr);

	virtual ZRef<ZFileLoc> CreateDir(ZFile::Error* oError);

	virtual ZRef<ZFileLoc> MoveTo(ZRef<ZFileLoc> oDest, ZFile::Error* oErr);
	virtual bool Delete(ZFile::Error* oErr);

	virtual ZRef<ZStreamerRPos> OpenRPos(bool iPreventWriters, ZFile::Error* oErr);
	virtual ZRef<ZStreamerWPos> OpenWPos(bool iPreventWriters, ZFile::Error* oErr);
	virtual ZRef<ZStreamerRWPos> OpenRWPos(bool iPreventWriters, ZFile::Error* oErr);

	virtual ZRef<ZStreamerWPos> CreateWPos(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr);
	virtual ZRef<ZStreamerRWPos> CreateRWPos(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr);

	virtual ZRef<ZFileR> OpenFileR(bool iPreventWriters, ZFile::Error* oErr);
	virtual ZRef<ZFileW> OpenFileW(bool iPreventWriters, ZFile::Error* oErr);
	virtual ZRef<ZFileRW> OpenFileRW(bool iPreventWriters, ZFile::Error* oErr);

	virtual ZRef<ZFileW> CreateFileW(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr);
	virtual ZRef<ZFileRW> CreateFileRW(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr);

	std::string pGetPath();

private:
	bool fIsAtRoot;
	std::vector<std::string> fComps;
	};

// =================================================================================================
// MARK: - ZFileR_POSIX

class ZFileR_POSIX : public ZFileR
	{
public:
	ZFileR_POSIX(int iFD, bool iCloseWhenFinalized);
	virtual ~ZFileR_POSIX();

// From ZFileR
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);

	virtual ZFile::Error GetSize(uint64& oSize);

private:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZFileW_POSIX

class ZFileW_POSIX : public ZFileW
	{
public:
	ZFileW_POSIX(int iFD, bool iCloseWhenFinalized);
	virtual ~ZFileW_POSIX();

// From ZFileW
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZFileRW_POSIX

class ZFileRW_POSIX : public ZFileRW
	{
public:
	ZFileRW_POSIX(int iFD, bool iCloseWhenFinalized);
	virtual ~ZFileRW_POSIX();

// From ZFileRW
	virtual ZFile::Error ReadAt(
		uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZFileR_POSIXMutex

class ZFileR_POSIXMutex : public ZFileR
	{
public:
	ZFileR_POSIXMutex(int iFD, bool iCloseWhenFinalized);
	virtual ~ZFileR_POSIXMutex();

// From ZFileR
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);

	virtual ZFile::Error GetSize(uint64& oSize);

private:
	ZMtx fMtx;
	uint64 fPosition;
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZFileW_POSIXMutex

class ZFileW_POSIXMutex : public ZFileW
	{
public:
	ZFileW_POSIXMutex(int iFD, bool iCloseWhenFinalized);
	virtual ~ZFileW_POSIXMutex();

// From ZFileW
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	ZMtx fMtx;
	uint64 fPosition;
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZFileRW_POSIXMutex

class ZFileRW_POSIXMutex : public ZFileRW
	{
public:
	ZFileRW_POSIXMutex(int iFD, bool iCloseWhenFinalized);
	virtual ~ZFileRW_POSIXMutex();

// From ZFileRW
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	ZMtx fMtx;
	uint64 fPosition;
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZStreamRPos_File_POSIX

class ZStreamRPos_File_POSIX : public ZStreamRPos
	{
public:
	ZStreamRPos_File_POSIX(int iFD, bool iCloseWhenFinalized);
	~ZStreamRPos_File_POSIX();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// Our protocol
	int GetFD() const { return fFD; }

private:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZStreamerRPos_File_POSIX

class ZStreamerRPos_File_POSIX : public ZStreamerRPos
	{
public:
	ZStreamerRPos_File_POSIX(int iFD, bool iCloseWhenFinalized);
	virtual ~ZStreamerRPos_File_POSIX();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

private:
	ZStreamRPos_File_POSIX fStream;
	};

// =================================================================================================
// MARK: - ZStreamWPos_File_POSIX

class ZStreamWPos_File_POSIX : public ZStreamWPos
	{
public:
	ZStreamWPos_File_POSIX(int iFD, bool iCloseWhenFinalized);
	~ZStreamWPos_File_POSIX();

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

// Our protocol
	int GetFD() const { return fFD; }

private:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZStreamerWPos_File_POSIX

class ZStreamerWPos_File_POSIX : public ZStreamerWPos
	{
public:
	ZStreamerWPos_File_POSIX(int iFD, bool iCloseWhenFinalized);
	virtual ~ZStreamerWPos_File_POSIX();

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos();

private:
	ZStreamWPos_File_POSIX fStream;
	};

// =================================================================================================
// MARK: - ZStreamRWPos_File_POSIX

class ZStreamRWPos_File_POSIX : public ZStreamRWPos
	{
public:
	ZStreamRWPos_File_POSIX(int iFD, bool iCloseWhenFinalized);
	~ZStreamRWPos_File_POSIX();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 iSize);

// Our protocol
	int GetFD() const { return fFD; }

private:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
// MARK: - ZStreamerRWPos_File_POSIX

class ZStreamerRWPos_File_POSIX : public ZStreamerRWPos
	{
public:
	ZStreamerRWPos_File_POSIX(int iFD, bool iCloseWhenFinalized);
	virtual ~ZStreamerRWPos_File_POSIX();

// From ZStreamerRWPos
	virtual const ZStreamRWPos& GetStreamRWPos();

private:
	ZStreamRWPos_File_POSIX fStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(File_POSIX)

#endif // __ZFile_POSIX_h__
