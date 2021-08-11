// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_File_h__
#define __ZooLib_File_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_NonCopyable.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/FunctionChain.h"
#include "zoolib/Time.h"
#include "zoolib/Trail.h"
#include "zoolib/Util_Relops.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

class FileIterRep;
class FileLoc;
class FileSpec;

// =================================================================================================
#pragma mark - EFileError (not used)

enum EFileError
	{
	None,
	Generic,
	DoesntExist,
	AlreadyExists,
	InvalidFileSpec,
	IllegalFileName,
	NoPermission,
	WrongTypeForOperation,
	ReadPastEOF,
	InsufficientSpace
	};

// =================================================================================================
#pragma mark - EFileKind

enum class EFileKind
	{
	None,
	File,
	Link,
	Dir
	};

// =================================================================================================
#pragma mark - FileSpec

/// Represents a node in the file system.

class FileSpec
	{
public:
	FileSpec();
	FileSpec(const FileSpec& iSpec);
	FileSpec(FileLoc* iLoc);
	FileSpec(const ZP<FileLoc>& iLoc);
	FileSpec(const ZP<FileLoc>& iLoc, const std::string& iComp);

	template <class I>
	FileSpec(const ZP<FileLoc>& iLoc, const I& iBegin, const I& iEnd)
	:	fLoc(iLoc)
	,	fComps(iBegin, iEnd)
		{}

	template <class I>
	FileSpec(const ZP<FileLoc>& iLoc,
		const I& iBegin, const I& iEnd, const std::string& iAdditional)
	:	fLoc(iLoc)
	,	fComps(iBegin, iEnd)
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
	bool IsRoot() const;

	FileSpec TraverseLink() const;

	std::string AsString() const;
	std::string AsString_Native() const;

	ZQ<Trail> QTrailTo(const FileSpec& iDest) const;
	ZQ<Trail> QTrailFrom(const FileSpec& iSource) const;

	EFileKind Kind() const;
	bool Exists() const;
	bool IsFile() const;
	bool IsDir() const;
	bool IsSymLink() const;

	ZQ<uint64> QSize() const;
	ZQ<double> QTimeCreated() const;
	ZQ<double> QTimeModified() const;

	FileSpec CreateDir() const;

	FileSpec MoveTo(const FileSpec& iDest) const;
	bool Delete() const;

	// Open/create with stream API (stateful).
	ZP<ChannerR_Bin> OpenR(bool iPreventWriters = false) const;
	ZP<ChannerRPos_Bin> OpenRPos(bool iPreventWriters = false) const;
	ZP<ChannerW_Bin> OpenW(bool iPreventWriters = true) const;
	ZP<ChannerWPos_Bin> OpenWPos(bool iPreventWriters = true) const;
	ZP<ChannerRWPos_Bin> OpenRWPos(bool iPreventWriters = true) const;

	ZP<ChannerWPos_Bin> CreateWPos(bool iOpenExisting, bool iPreventWriters = true) const;
	ZP<ChannerRWPos_Bin> CreateRWPos(bool iOpenExisting, bool iPreventWriters = true) const;

	// As ever, do not abuse...
	ZP<FileLoc> GetFileLoc() const;

private:
	// It's not generally possible to relate FileSpecs to one another. Disable it.
	void operator<(const FileSpec& iOther);

	ZP<FileLoc> pPhysicalLoc() const;

	ZP<FileLoc> fLoc;
	std::vector<std::string> fComps;
	};

// Disable other relops.
template <>
struct RelopsTraits_HasLT<FileSpec> : public RelopsTraits_Has {};

// =================================================================================================
#pragma mark - FileIter

/// An iterator that provides access to the children of a directory.

class FileIter
	{
public:
	FileIter();
	FileIter(const FileIter& iOther);
	~FileIter();
	FileIter& operator=(const FileIter& iOther);

	FileIter(const FileSpec& iRoot);
	FileIter(const ZP<FileIterRep>& iRep);

	ZMACRO_operator_bool(FileIter, operator_bool) const;
	FileIter& Advance();
	FileSpec Current() const;
	std::string CurrentName() const;

private:
	ZP<FileIterRep> fRep;
	};

// =================================================================================================
#pragma mark - FileTreeIter

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
#pragma mark - FileLoc

class FileLoc : public Counted
	{
protected:
	FileLoc();

public:
	enum ELoc { eLoc_Root, eLoc_CWD, eLoc_App };

	virtual ~FileLoc();

	virtual ZP<FileIterRep> CreateIterRep();

	virtual std::string GetName() = 0;
	virtual ZQ<Trail> QTrailTo(ZP<FileLoc> iDest) = 0;

	virtual ZP<FileLoc> GetAncestor(size_t iCount);
	virtual ZP<FileLoc> GetParent() = 0;
	virtual ZP<FileLoc> GetDescendant(const std::string* iComps, size_t iCount) = 0;
	virtual bool IsRoot() = 0;

	virtual ZP<FileLoc> TraverseLink();

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount) = 0;
	virtual std::string AsString_Native(const std::string* iComps, size_t iCount) = 0;

	virtual EFileKind Kind();
	virtual bool Exists();
	virtual bool IsFile();
	virtual bool IsDir();
	virtual bool IsSymLink();

	virtual ZQ<uint64> QSize();
	virtual ZQ<double> QTimeCreated();
	virtual ZQ<double> QTimeModified();

	virtual ZP<FileLoc> CreateDir();

	virtual ZP<FileLoc> MoveTo(ZP<FileLoc> iDest);
	virtual bool Delete();

	virtual ZP<ChannerR_Bin> OpenR(bool iPreventWriters);
	virtual ZP<ChannerRPos_Bin> OpenRPos(bool iPreventWriters);
	virtual ZP<ChannerW_Bin> OpenW(bool iPreventWriters);
	virtual ZP<ChannerWPos_Bin> OpenWPos(bool iPreventWriters);
	virtual ZP<ChannerRWPos_Bin> OpenRWPos(bool iPreventWriters);

	virtual ZP<ChannerWPos_Bin> CreateWPos(bool iOpenExisting, bool iPreventWriters);
	virtual ZP<ChannerRWPos_Bin> CreateRWPos(bool iOpenExisting, bool iPreventWriters);
	};

// =================================================================================================
#pragma mark - FileLoc_Std

class FileLoc_Std : public FileLoc
	{
public:
	std::string GetName() override;
	ZQ<Trail> QTrailTo(ZP<FileLoc> iDest) override;

	ZP<FileLoc> GetParent() override;
	ZP<FileLoc> GetDescendant(const std::string* iComps, size_t iCount) override;
	bool IsRoot() override;

	std::string AsString_POSIX(const std::string* iComps, size_t iCount) override;
	std::string AsString_Native(const std::string* iComps, size_t iCount) override;
	};

// =================================================================================================
#pragma mark - FileIterRep

class FileIterRep : public Counted
	{
protected:
	FileIterRep();

public:
	virtual ~FileIterRep();

	virtual bool HasValue() = 0;
	virtual void Advance() = 0;
	virtual FileSpec Current() = 0;
	virtual std::string CurrentName() = 0;

	virtual ZP<FileIterRep> Clone() = 0;
	};

// =================================================================================================
#pragma mark - FileIterRep_Std

class FileIterRep_Std : public FileIterRep
	{
public:
	class RealRep;

	FileIterRep_Std(ZP<RealRep> iRealRep, size_t iIndex);
	virtual ~FileIterRep_Std();

// From FileIterRep
	virtual bool HasValue();
	virtual void Advance();
	virtual FileSpec Current();
	virtual std::string CurrentName();

	virtual ZP<FileIterRep> Clone();

private:
	ZP<RealRep> fRealRep;
	size_t fIndex;
	};

// =================================================================================================
#pragma mark - FileIterRep_Std::RealRep

class FileIterRep_Std::RealRep : public Counted
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
