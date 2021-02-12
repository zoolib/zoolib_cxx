// Copyright (c) 2019-21 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_File_Archive_h__
#define __ZooLib_File_Archive_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

#include <map>

namespace ZooLib {

class ArchiveNode;
class ArchiveNode_Directory;
class ArchiveNode_File;

// =================================================================================================
#pragma mark - Archive

class Archive
:	public Counted
	{
public:
	virtual ZP<ArchiveNode> GetRoot() = 0;

	virtual ZP<ChannerR_Bin> OpenR(const ZP<ArchiveNode_File>& iArchiveNode)
		 { return null; }

	virtual ZP<ChannerRPos_Bin> OpenRPos(const ZP<ArchiveNode_File>& iArchiveNode)
		 { return null; }
	};

// =================================================================================================
#pragma mark - ArchiveNode

class ArchiveNode
:	public CountedWithoutFinalize
	{
public:
	ArchiveNode(ArchiveNode_Directory* iParent, const string8& iName)
	:	fParent(iParent)
	,	fName(iName)
		{}

	ArchiveNode_Directory* fParent;
	const string8 fName;
	};

class ArchiveNode_Directory
:	public ArchiveNode
	{
public:
	ArchiveNode_Directory(ArchiveNode_Directory* iParent, const string8& iName)
	:	ArchiveNode(iParent, iName)
		{}

	typedef std::map<string8, ZP<ArchiveNode> > MapNameNode;
	MapNameNode fChildren;
	};

class ArchiveToken
:	public Counted
	{
	};

class ArchiveNode_File
:	public ArchiveNode
	{
public:
	ArchiveNode_File(ArchiveNode_Directory* iParent, const string8& iName,
		const ZP<ArchiveToken>& iToken)
	:	ArchiveNode(iParent, iName)
	,	fToken(iToken)
		{}

	ZP<ArchiveToken> GetToken()
		{ return fToken; }

	const ZP<ArchiveToken> fToken;
	};

// =================================================================================================
#pragma mark - FileLoc_Archive

class FileLoc_Archive : public FileLoc
	{
public:
	FileLoc_Archive(const ZP<Archive>& iArchive);

	FileLoc_Archive(const ZP<Archive>& iArchive, const ZP<ArchiveNode>& iNode);

	virtual ~FileLoc_Archive();

// From FileLoc
	virtual ZP<FileIterRep> CreateIterRep();

	virtual std::string GetName() const;

	virtual ZQ<Trail> TrailTo(ZP<FileLoc> oDest) const;

	virtual ZP<FileLoc> GetParent();

	virtual ZP<FileLoc> GetDescendant(const std::string* iComps, size_t iCount);

	virtual bool IsRoot();

	virtual ZP<FileLoc> Follow();

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount);

	virtual std::string AsString_Native(const std::string* iComps, size_t iCount);

	virtual File::Kind Kind();

	virtual uint64 Size();

	virtual double TimeCreated();

	virtual double TimeModified();

	virtual ZP<FileLoc> CreateDir();

	virtual ZP<FileLoc> MoveTo(ZP<FileLoc> oDest);

	virtual bool Delete();

	virtual ZP<ChannerR_Bin> OpenR(bool iPreventWriters);

	virtual ZP<ChannerRPos_Bin> OpenRPos(bool iPreventWriters);

	std::string pGetPath();

private:
	ZP<Archive> fArchive;
	ZP<ArchiveNode> fArchiveNode;
	};

// =================================================================================================
#pragma mark -

void sGrowArchiveDirectoryTree(const ZP<ArchiveToken>& iArchiveToken,
	const ZP<ArchiveNode_Directory>& ioParent,
	const Trail& iTrail, size_t iDepth);

FileSpec sFileSpec_Archive(const ZP<Archive>& iArchive);

} // namespace ZooLib

#endif // __ZooLib_File_Archive_h__
