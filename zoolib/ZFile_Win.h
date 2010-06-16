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

#ifndef __ZFile_Win__
#define __ZFile_Win__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__File_Win
#	define ZCONFIG_API_Avail__File_Win ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Desired__File_Win
#	define ZCONFIG_API_Desired__File_Win 1
#endif

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_Reference(File_Win)

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZFile.h"

#if ZCONFIG_API_Enabled(File_Win)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZFileLoc_Win

class ZFileLoc_Win : public ZFileLoc
	{
public:
	static ZRef<ZFileLoc> sGet_CWD();
	static ZRef<ZFileLoc> sGet_Root();
	static ZRef<ZFileLoc> sGet_App();
	static ZRef<ZFileLoc> sFromFullWinPath(const char* iPath);

private:
	ZFileLoc_Win();
	explicit ZFileLoc_Win(const std::string& iBase);
	explicit ZFileLoc_Win(const std::string& iBase, const std::vector<std::string>& iComps);
	explicit ZFileLoc_Win(const std::string& iBase, const std::string* iComps, size_t iCount);

public:
	virtual ~ZFileLoc_Win();

// From ZFileLoc
	virtual ZRef<ZFileIterRep> CreateIterRep();

	virtual std::string GetName(ZFile::Error* oErr) const;
	virtual ZTrail TrailTo(ZRef<ZFileLoc> oDest, ZFile::Error* oErr) const;

	virtual ZRef<ZFileLoc> GetParent(ZFile::Error* oErr);
	virtual ZRef<ZFileLoc> GetDescendant(
		const std::string* iComps, size_t iCount, ZFile::Error* oErr);

	virtual bool IsRoot();

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
	std::string fBase;
	std::vector<std::string> fComps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileLoc_WinNT

class ZFileLoc_WinNT : public ZFileLoc
	{
public:
	static ZRef<ZFileLoc> sGet_CWD();
	static ZRef<ZFileLoc> sGet_Root();
	static ZRef<ZFileLoc> sGet_App();
	static ZRef<ZFileLoc> sFromFullWinPath(const UTF16* iPath);

private:
	ZFileLoc_WinNT();
	explicit ZFileLoc_WinNT(const string16& iBase);
	explicit ZFileLoc_WinNT(const string16& iBase, const std::vector<string16>& iComps);
	explicit ZFileLoc_WinNT(const string16& iBase, const string16* iComps, size_t iCount);

public:
	virtual ~ZFileLoc_WinNT();

// From ZFileLoc
	virtual ZRef<ZFileIterRep> CreateIterRep();

	virtual std::string GetName(ZFile::Error* oErr) const;
	virtual ZTrail TrailTo(ZRef<ZFileLoc> oDest, ZFile::Error* oErr) const;

	virtual ZRef<ZFileLoc> GetParent(ZFile::Error* oErr);
	virtual ZRef<ZFileLoc> GetDescendant(
		const std::string* iComps, size_t iCount, ZFile::Error* oErr);

	virtual bool IsRoot();

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

	string16 pGetPath();

private:
	string16 fBase;
	std::vector<string16> fComps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR_Win

class ZFileR_Win : public ZFileR
	{
public:
	ZFileR_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZFileR_Win();

// From ZFileR
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);

	virtual ZFile::Error GetSize(uint64& oSize);

private:
	ZMtx fMutex;
	uint64 fPosition;
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR_WinNT

class ZFileR_WinNT : public ZFileR
	{
public:
	ZFileR_WinNT(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZFileR_WinNT();

// From ZFileR
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);

	virtual ZFile::Error GetSize(uint64& oSize);

private:
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW_Win

class ZFileW_Win : public ZFileW
	{
public:
	ZFileW_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZFileW_Win();

// From ZFileW
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	ZMtx fMutex;
	uint64 fPosition;
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW_WinNT

class ZFileW_WinNT : public ZFileW
	{
public:
	ZFileW_WinNT(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZFileW_WinNT();

// From ZFileW
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW_Win

class ZFileRW_Win : public ZFileRW
	{
public:
	ZFileRW_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZFileRW_Win();

// From ZFileRW
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	ZMtx fMutex;
	uint64 fPosition;
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW_WinNT

class ZFileRW_WinNT : public ZFileRW
	{
public:
	ZFileRW_WinNT(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZFileRW_WinNT();

// From ZFileRW
	virtual ZFile::Error ReadAt(uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead);
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_File_Win

class ZStreamRPos_File_Win : public ZStreamRPos
	{
public:
	ZStreamRPos_File_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	~ZStreamRPos_File_Win();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_File_Win

class ZStreamerRPos_File_Win : public ZStreamerRPos
	{
public:
	ZStreamerRPos_File_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZStreamerRPos_File_Win();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

private:
	ZStreamRPos_File_Win fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_File_Win

class ZStreamWPos_File_Win : public ZStreamWPos
	{
public:
	ZStreamWPos_File_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	~ZStreamWPos_File_Win();

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

	virtual void Imp_Truncate();

private:
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_File_Win

class ZStreamerWPos_File_Win : public ZStreamerWPos
	{
public:
	ZStreamerWPos_File_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZStreamerWPos_File_Win();

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos();

private:
	ZStreamWPos_File_Win fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_File_Win

class ZStreamRWPos_File_Win : public ZStreamRWPos
	{
public:
	ZStreamRWPos_File_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	~ZStreamRWPos_File_Win();

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

private:
	HANDLE fFileHANDLE;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_File_Win

class ZStreamerRWPos_File_Win : public ZStreamerRWPos
	{
public:
	ZStreamerRWPos_File_Win(HANDLE iFileHANDLE, bool iCloseWhenFinalized);
	virtual ~ZStreamerRWPos_File_Win();

// From ZStreamerRWPos
	virtual const ZStreamRWPos& GetStreamRWPos();

private:
	ZStreamRWPos_File_Win fStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(File_Win)

#endif // __ZFile_Win__
