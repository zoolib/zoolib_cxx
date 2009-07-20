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

#ifndef __ZFile__
#define __ZFile__ 1

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZTrail.h"

NAMESPACE_ZOOLIB_BEGIN

class ZFileIterRep;
class ZFileLoc;
class ZFileSpec;

class ZFileR;
class ZFileW;
class ZFileRW;

// =================================================================================================
#pragma mark -
#pragma mark * ZFile

namespace ZFile {

enum Error
	{
	errorNone,
	errorGeneric,
	errorDoesntExist,
	errorAlreadyExists,
	errorInvalidFileSpec,
	errorIllegalFileName,
	errorNoPermission,
	errorWrongTypeForOperation,
	errorReadPastEOF,
	errorInsufficientSpace
	};

enum Kind
	{
	kindNone,
	kindFile,
	kindLink,
	kindDir
	};

} // namespace ZFile

// =================================================================================================
#pragma mark -
#pragma mark * ZFileSpec

/// Represents a node in the file system.

class ZFileSpec
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZFileSpec, operator_bool_generator_type, operator_bool_type);
public:
	typedef ZFile::Error Error;

	ZFileSpec();
	ZFileSpec(const ZFileSpec& iSpec);
	ZFileSpec(ZFileLoc* iLoc);
	ZFileSpec(const ZRef<ZFileLoc>& iLoc);
	ZFileSpec(const ZRef<ZFileLoc>& iLoc, const std::string& iComp);

	template <class I>
	ZFileSpec(const ZRef<ZFileLoc>& iLoc, const I& iBegin, const I& iEnd)
	:	fLoc(iLoc),
		fComps(iBegin, iEnd)
		{}

	template <class I>
	ZFileSpec(const ZRef<ZFileLoc>& iLoc,
		const I& iBegin, const I& iEnd, const std::string& iAdditional)
	:	fLoc(iLoc),
		fComps(iBegin, iEnd)
		{ fComps.push_back(iAdditional); }

	ZFileSpec(const std::string& iPath);

	operator operator_bool_type() const;

	static ZFileSpec sCWD();
	static ZFileSpec sRoot();
	static ZFileSpec sApp();

	std::string Name(Error* oError = 0) const;
	ZFileSpec Parent(Error* oError = 0) const;
	ZFileSpec Child(const std::string& iName, Error* oError = 0) const;
	ZFileSpec Sibling(const std::string& iName, Error* oError = 0) const;
	ZFileSpec Trail(const ZTrail& iTrail, Error* oError = 0) const;
	ZFileSpec Ancestor(size_t iCount, Error* oError = 0) const;
	ZFileSpec Descendant(const std::string* iComps, size_t iCount, Error* oError = 0) const;

	std::string AsString() const;
	std::string AsString_Native() const;

	ZTrail TrailTo(const ZFileSpec& iDest, Error* oError = 0) const;
	ZTrail TrailFrom(const ZFileSpec& iSource, Error* oError = 0) const;

	ZFile::Kind Kind(Error* oError = 0) const;

	bool IsRoot(Error* oError = 0) const;
	bool IsFile(Error* oError = 0) const;
	bool IsDir(Error* oError = 0) const;
	bool IsSymLink(Error* oError = 0) const;
	bool Exists(Error* oError = 0) const;

	uint64 Size(Error* oError = 0) const;
	ZTime TimeCreated(Error* oError = 0) const;
	ZTime TimeModified(Error* oError = 0) const;

	bool SetCreatorAndType(uint32 iCreator, uint32 iType, Error* oError = 0) const;

	ZFileSpec CreateDir(Error* oError = 0) const;

	ZFileSpec MoveTo(const ZFileSpec& iDest, Error* oError = 0) const;
	bool Delete(Error* oError = 0) const;

	// Open/create with stream API (stateful).
	ZRef<ZStreamerR> OpenR(bool iPreventWriters = false, Error* oError = 0) const;
	ZRef<ZStreamerRPos> OpenRPos(bool iPreventWriters = false, Error* oError = 0) const;
	ZRef<ZStreamerW> OpenW(bool iPreventWriters = true, Error* oError = 0) const;
	ZRef<ZStreamerWPos> OpenWPos(bool iPreventWriters = true, Error* oError = 0) const;
	ZRef<ZStreamerRWPos> OpenRWPos(bool iPreventWriters = true, Error* oError = 0) const;

	ZRef<ZStreamerWPos> CreateWPos(
		bool iOpenExisting, bool iPreventWriters = true, Error* oError = 0) const;
	ZRef<ZStreamerRWPos> CreateRWPos(
		bool iOpenExisting, bool iPreventWriters = true, Error* oError = 0) const;

	// Open/create stateless.
	ZRef<ZFileR> OpenFileR(bool iPreventWriters = false, Error* oError = 0) const;
	ZRef<ZFileW> OpenFileW(bool iPreventWriters = true, Error* oError = 0) const;
	ZRef<ZFileRW> OpenFileRW(bool iPreventWriters = true, Error* oError = 0) const;

	ZRef<ZFileW> CreateFileW(
		bool iOpenExisting, bool iPreventWriters = true, Error* oError = 0) const;
	ZRef<ZFileRW> CreateFileRW(
		bool iOpenExisting, bool iPreventWriters = true, Error* oError = 0) const;

	// As ever, do not abuse...
	ZRef<ZFileLoc> GetFileLoc() const;

private:
	ZRef<ZFileLoc> pPhysicalLoc() const;

	ZRef<ZFileLoc> fLoc;
	std::vector<std::string> fComps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIter

/// An iterator that provides access to the children of a directory.

class ZFileIter
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZFileIter, operator_bool_generator_type, operator_bool_type);
public:
	ZFileIter();
	ZFileIter(const ZFileIter& iOther);
	ZFileIter(const ZFileSpec& iRoot);
	~ZFileIter();
	ZFileIter& operator=(const ZFileIter& iOther);

	operator operator_bool_type() const;
	ZFileIter& Advance();
	ZFileSpec Current() const;
	std::string CurrentName() const;

private:
	ZRef<ZFileIterRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileTreeIter

/// An iterator that provides access to every node descending from a ZFileSpec.

class ZFileTreeIter
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(
		ZFileTreeIter, operator_bool_generator_type, operator_bool_type);
public:
	ZFileTreeIter();
	ZFileTreeIter(const ZFileTreeIter& iOther);
	ZFileTreeIter(const ZFileSpec& iRoot);
	~ZFileTreeIter();
	ZFileTreeIter& operator=(const ZFileTreeIter& iOther);

	operator operator_bool_type() const;
	ZFileTreeIter& Advance();
	ZFileSpec Current() const;
	std::string CurrentName() const;

private:
	ZFileSpec fCurrent;
	std::vector<ZFileIter> fStack;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileLoc

class ZFileLoc : public ZRefCountedWithFinalize
	{
protected:
	ZFileLoc();

public:
	enum ELoc { eLoc_Root, eLoc_CWD, eLoc_App };

	virtual ~ZFileLoc();

	virtual ZRef<ZFileIterRep> CreateIterRep();

	virtual std::string GetName(ZFile::Error* oError) const = 0;
	virtual ZTrail TrailTo(ZRef<ZFileLoc> iDest, ZFile::Error* oError) const = 0;

	virtual ZRef<ZFileLoc> GetAncestor(size_t iCount, ZFile::Error* oError);
	virtual ZRef<ZFileLoc> GetParent(ZFile::Error* oError) = 0;
	virtual ZRef<ZFileLoc> GetDescendant(
		const std::string* iComps, size_t iCount, ZFile::Error* oError) = 0;
	virtual bool IsRoot() = 0;

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount) = 0;
	virtual std::string AsString_Native(const std::string* iComps, size_t iCount) = 0;

	virtual ZFile::Kind Kind(ZFile::Error* oError) = 0;
	virtual uint64 Size(ZFile::Error* oError) = 0;
	virtual ZTime TimeCreated(ZFile::Error* oError) = 0;
	virtual ZTime TimeModified(ZFile::Error* oError) = 0;

	virtual bool SetCreatorAndType(uint32 iCreator, uint32 iType, ZFile::Error* oError);

	virtual ZRef<ZFileLoc> CreateDir(ZFile::Error* oError) = 0;

	virtual ZRef<ZFileLoc> MoveTo(ZRef<ZFileLoc> iDest, ZFile::Error* oError) = 0;
	virtual bool Delete(ZFile::Error* oError) = 0;

	virtual ZRef<ZStreamerRPos> OpenRPos(bool iPreventWriters, ZFile::Error* oError);
	virtual ZRef<ZStreamerWPos> OpenWPos(bool iPreventWriters, ZFile::Error* oError);
	virtual ZRef<ZStreamerRWPos> OpenRWPos(bool iPreventWriters, ZFile::Error* oError);

	virtual ZRef<ZStreamerWPos> CreateWPos(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError);
	virtual ZRef<ZStreamerRWPos> CreateRWPos(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError);

	virtual ZRef<ZFileR> OpenFileR(bool iPreventWriters, ZFile::Error* oError);
	virtual ZRef<ZFileW> OpenFileW(bool iPreventWriters, ZFile::Error* oError);
	virtual ZRef<ZFileRW> OpenFileRW(bool iPreventWriters, ZFile::Error* oError);

	virtual ZRef<ZFileW> CreateFileW(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError);
	virtual ZRef<ZFileRW> CreateFileRW(
		bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIterRep

class ZFileIterRep : public ZRefCountedWithFinalize
	{
protected:
	ZFileIterRep();

public:
	virtual ~ZFileIterRep();

	virtual bool HasValue() = 0;
	virtual void Advance() = 0;
	virtual ZFileSpec Current() = 0;
	virtual std::string CurrentName() const = 0;

	virtual ZRef<ZFileIterRep> Clone() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIterRep_Std

class ZFileIterRep_Std : public ZFileIterRep
	{
public:
	class RealRep;

	ZFileIterRep_Std(ZRef<RealRep> iRealRep, size_t iIndex);
	virtual ~ZFileIterRep_Std();

// From ZFileIterRep
	virtual bool HasValue();
	virtual void Advance();
	virtual ZFileSpec Current();
	virtual std::string CurrentName() const;

	virtual ZRef<ZFileIterRep> Clone();

private:
	ZRef<RealRep> fRealRep;
	size_t fIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIterRep_Std::RealRep

class ZFileIterRep_Std::RealRep : public ZRefCountedWithFinalize
	{
protected:
	RealRep() {};

public:
	virtual bool HasValue(size_t iIndex) = 0;
	virtual ZFileSpec GetSpec(size_t iIndex) = 0;
	virtual std::string GetName(size_t iIndex) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR

/// Base interface for stateless read access.

class ZFileR
:	public ZRefCountedWithFinalize,
	NonCopyable
	{
protected:
	ZFileR() {}

public:
	virtual ZFile::Error ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead) = 0;

	virtual ZFile::Error GetSize(uint64& oSize) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW

/// Base interface for stateless write access.

class ZFileW
:	public ZRefCountedWithFinalize,
	NonCopyable
	{
protected:
	ZFileW() {}

public:
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten) = 0;

	virtual ZFile::Error GetSize(uint64& oSize) = 0;
	virtual ZFile::Error SetSize(uint64 iSize) = 0;

	virtual ZFile::Error Flush() = 0;
	virtual ZFile::Error FlushVolume() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW

/// Base interface for stateless read/write access.

class ZFileRW : public ZFileR, public ZFileW
	{
protected:
	ZFileRW() {}

public:
// From ZFileR/ZFileW. Re-declared here for disambiguation.
	virtual ZFile::Error ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead) = 0;
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten) = 0;

	virtual ZFile::Error GetSize(uint64& oSize) = 0;
	virtual ZFile::Error SetSize(uint64 iSize) = 0;

	virtual ZFile::Error Flush() = 0;
	virtual ZFile::Error FlushVolume() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_FileR

/// Puts a stream interface on top of a ZFileR.

class ZStreamRPos_FileR : public ZStreamRPos
	{
public:
	ZStreamRPos_FileR(ZRef<ZFileR> iFile);
	~ZStreamRPos_FileR();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	ZRef<ZFileR> fFile;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_FileR

class ZStreamerRPos_FileR : public ZStreamerRPos
	{
public:
	ZStreamerRPos_FileR(ZRef<ZFileR> iFile);
	virtual ~ZStreamerRPos_FileR();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

private:
	ZStreamRPos_FileR fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_FileW

/// Puts a stream interface on top of a ZFileW.

class ZStreamWPos_FileW : public ZStreamWPos
	{
public:
	ZStreamWPos_FileW(ZRef<ZFileW> iFile);
	~ZStreamWPos_FileW();

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

private:
	ZRef<ZFileW> fFile;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_FileW

class ZStreamerWPos_FileW : public ZStreamerWPos
	{
public:
	ZStreamerWPos_FileW(ZRef<ZFileW> iFile);
	virtual ~ZStreamerWPos_FileW();

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos();

private:
	ZStreamWPos_FileW fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_FileRW

/// Puts a stream interface on top of a ZFileRW.

class ZStreamRWPos_FileRW : public ZStreamRWPos
	{
public:
	ZStreamRWPos_FileRW(ZRef<ZFileRW> iFile);
	~ZStreamRWPos_FileRW();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

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
	ZRef<ZFileRW> fFile;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_FileRW

class ZStreamerRWPos_FileRW : public ZStreamerRWPos
	{
public:
	ZStreamerRWPos_FileRW(ZRef<ZFileRW> iFile);
	virtual ~ZStreamerRWPos_FileRW();

// From ZStreamerRWPos
	virtual const ZStreamRWPos& GetStreamRWPos();

private:
	ZStreamRWPos_FileRW fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR_StreamerRPos

/// Puts a ZFileR interface on top of a streamer.

class ZFileR_StreamerRPos : public ZFileR
	{
public:
	ZFileR_StreamerRPos(ZRef<ZStreamerRPos> iStreamer);

// From ZFileR
	virtual ZFile::Error ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead);

	virtual ZFile::Error GetSize(uint64& oSize);

private:
	ZMtx fMutex;
	ZRef<ZStreamerRPos> fStreamer;
	const ZStreamRPos& fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW_StreamerWPos

/// Puts a ZFileW interface on top of a streamer.

class ZFileW_StreamerWPos : public ZFileW
	{
public:
	ZFileW_StreamerWPos(ZRef<ZStreamerWPos> iStreamer);

// From ZFileW
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	ZMtx fMutex;
	ZRef<ZStreamerWPos> fStreamer;
	const ZStreamWPos& fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW_StreamerRWPos

/// Puts a ZFileRW interface on top of a streamer.

class ZFileRW_StreamerRWPos : public ZFileRW
	{
public:
	ZFileRW_StreamerRWPos(ZRef<ZStreamerRWPos> iStreamer);

// From ZFileR/ZFileW via ZFileRW.
	virtual ZFile::Error ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead);
	virtual ZFile::Error WriteAt(
		uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual ZFile::Error GetSize(uint64& oSize);
	virtual ZFile::Error SetSize(uint64 iSize);

	virtual ZFile::Error Flush();
	virtual ZFile::Error FlushVolume();

private:
	ZMtx fMutex;
	ZRef<ZStreamerRWPos> fStreamer;
	const ZStreamRWPos& fStream;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZFile__
