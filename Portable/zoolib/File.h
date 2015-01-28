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

#ifndef __ZooLib_File_h__
#define __ZooLib_File_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_NonCopyable.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/ChannerXX.h"
#include "zoolib/FunctionChain.h"
#include "zoolib/Trail.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"

namespace ZooLib {

class FileIterRep;
class FileLoc;
class FileSpec;

// =================================================================================================
#pragma mark -
#pragma mark File

namespace File {

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

} // namespace File

// =================================================================================================
#pragma mark -
#pragma mark FileSpec

/// Represents a node in the file system.

class FileSpec
	{
public:
	typedef File::Error Error;

	FileSpec();
	FileSpec(const FileSpec& iSpec);
	FileSpec(FileLoc* iLoc);
	FileSpec(const ZRef<FileLoc>& iLoc);
	FileSpec(const ZRef<FileLoc>& iLoc, const std::string& iComp);

	template <class I>
	FileSpec(const ZRef<FileLoc>& iLoc, const I& iBegin, const I& iEnd)
	:	fLoc(iLoc),
		fComps(iBegin, iEnd)
		{}

	template <class I>
	FileSpec(const ZRef<FileLoc>& iLoc,
		const I& iBegin, const I& iEnd, const std::string& iAdditional)
	:	fLoc(iLoc),
		fComps(iBegin, iEnd)
		{ fComps.push_back(iAdditional); }

	FileSpec(const std::string& iPath);

	ZMACRO_operator_bool(FileSpec, operator_bool) const;

	static FileSpec sCWD();
	static FileSpec sRoot();
	static FileSpec sApp();

	std::string Name() const;
	FileSpec Parent() const;
	FileSpec Child(const std::string& iName) const;
	FileSpec Sibling(const std::string& iName) const;
	FileSpec Follow(const Trail& iTrail) const;
	FileSpec Ancestor(size_t iCount) const;
	FileSpec Descendant(const std::string* iComps, size_t iCount) const;

	FileSpec Follow() const;

	std::string AsString() const;
	std::string AsString_Native() const;

	ZQ<Trail> TrailTo(const FileSpec& oDest) const;
	ZQ<Trail> TrailFrom(const FileSpec& iSource) const;

	File::Kind Kind() const;

	bool IsRoot() const;
	bool IsFile() const;
	bool IsDir() const;
	bool IsSymLink() const;
	bool Exists() const;

	uint64 Size() const;
	ZTime TimeCreated() const;
	ZTime TimeModified() const;

	bool SetCreatorAndType(uint32 iCreator, uint32 iType) const;

	FileSpec CreateDir() const;

	FileSpec MoveTo(const FileSpec& oDest) const;
	bool Delete() const;

	// Open/create with stream API (stateful).
	ZRef<ChannerR_Bin> OpenR(bool iPreventWriters = false) const;
	ZRef<ChannerRPos_Bin> OpenRPos(bool iPreventWriters = false) const;
	ZRef<ChannerW_Bin> OpenW(bool iPreventWriters = true) const;
	ZRef<ChannerWPos_Bin> OpenWPos(bool iPreventWriters = true) const;
	ZRef<ChannerRWPos_Bin> OpenRWPos(bool iPreventWriters = true) const;

	ZRef<ChannerWPos_Bin> CreateWPos(bool iOpenExisting, bool iPreventWriters = true) const;
	ZRef<ChannerRWPos_Bin> CreateRWPos(bool iOpenExisting, bool iPreventWriters = true) const;

	// As ever, do not abuse...
	ZRef<FileLoc> GetFileLoc() const;

private:
	ZRef<FileLoc> pPhysicalLoc() const;

	ZRef<FileLoc> fLoc;
	std::vector<std::string> fComps;
	};

// =================================================================================================
#pragma mark -
#pragma mark FileIter

/// An iterator that provides access to the children of a directory.

class FileIter
	{
public:
	FileIter();
	FileIter(const FileIter& iOther);
	FileIter(const FileSpec& iRoot);
	~FileIter();
	FileIter& operator=(const FileIter& iOther);

	ZMACRO_operator_bool(FileIter, operator_bool) const;
	FileIter& Advance();
	FileSpec Current() const;
	std::string CurrentName() const;

private:
	ZRef<FileIterRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark FileTreeIter

/// An iterator that provides access to every node descending from a FileSpec.

class FileTreeIter
	{
public:
	FileTreeIter();
	FileTreeIter(const FileTreeIter& iOther);
	FileTreeIter(const FileSpec& iRoot);
	~FileTreeIter();
	FileTreeIter& operator=(const FileTreeIter& iOther);

	ZMACRO_operator_bool(FileTreeIter, operator_bool) const;
	FileTreeIter& Advance();
	FileSpec Current() const;
	std::string CurrentName() const;

private:
	FileSpec fCurrent;
	std::vector<FileIter> fStack;
	};

// =================================================================================================
#pragma mark -
#pragma mark FileLoc

class FileLoc : public ZCounted
	{
protected:
	FileLoc();

public:
	enum ELoc { eLoc_Root, eLoc_CWD, eLoc_App };

	virtual ~FileLoc();

	virtual ZRef<FileIterRep> CreateIterRep();

	virtual std::string GetName() const = 0;
	virtual ZQ<Trail> TrailTo(ZRef<FileLoc> oDest) const = 0;

	virtual ZRef<FileLoc> GetAncestor(size_t iCount);
	virtual ZRef<FileLoc> GetParent() = 0;
	virtual ZRef<FileLoc> GetDescendant(
		const std::string* iComps, size_t iCount) = 0;
	virtual bool IsRoot() = 0;

	virtual ZRef<FileLoc> Follow();

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount) = 0;
	virtual std::string AsString_Native(const std::string* iComps, size_t iCount) = 0;

	virtual File::Kind Kind() = 0;
	virtual uint64 Size() = 0;
	virtual ZTime TimeCreated() = 0;
	virtual ZTime TimeModified() = 0;

	virtual bool SetCreatorAndType(uint32 iCreator, uint32 iType);

	virtual ZRef<FileLoc> CreateDir() = 0;

	virtual ZRef<FileLoc> MoveTo(ZRef<FileLoc> oDest) = 0;
	virtual bool Delete() = 0;

	virtual ZRef<ChannerRPos_Bin> OpenRPos(bool iPreventWriters);
	virtual ZRef<ChannerWPos_Bin> OpenWPos(bool iPreventWriters);
	virtual ZRef<ChannerRWPos_Bin> OpenRWPos(bool iPreventWriters);

	virtual ZRef<ChannerWPos_Bin> CreateWPos(bool iOpenExisting, bool iPreventWriters);
	virtual ZRef<ChannerRWPos_Bin> CreateRWPos(bool iOpenExisting, bool iPreventWriters);
	};

// =================================================================================================
#pragma mark -
#pragma mark FileIterRep

class FileIterRep : public ZCounted
	{
protected:
	FileIterRep();

public:
	virtual ~FileIterRep();

	virtual bool HasValue() = 0;
	virtual void Advance() = 0;
	virtual FileSpec Current() = 0;
	virtual std::string CurrentName() const = 0;

	virtual ZRef<FileIterRep> Clone() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark FileIterRep_Std

class FileIterRep_Std : public FileIterRep
	{
public:
	class RealRep;

	FileIterRep_Std(ZRef<RealRep> iRealRep, size_t iIndex);
	virtual ~FileIterRep_Std();

// From FileIterRep
	virtual bool HasValue();
	virtual void Advance();
	virtual FileSpec Current();
	virtual std::string CurrentName() const;

	virtual ZRef<FileIterRep> Clone();

private:
	ZRef<RealRep> fRealRep;
	size_t fIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark FileIterRep_Std::RealRep

class FileIterRep_Std::RealRep : public ZCounted
	{
protected:
	RealRep() {};

public:
	virtual bool HasValue(size_t iIndex) = 0;
	virtual FileSpec GetSpec(size_t iIndex) = 0;
	virtual std::string GetName(size_t iIndex) = 0;
	};

} // namespace ZooLib

#endif // __ZooLib_File_h__
