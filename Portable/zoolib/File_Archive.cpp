// Copyright (c) 2019-21 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/File_Archive.h"

#include "zoolib/Util_STL_map.h"

namespace ZooLib {

using namespace Util_STL;
using std::map;
using std::string;
using std::vector;

class ArchiveNode_Directory;
class ArchiveNode_File;

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

class ArchiveNode_File
:	public ArchiveNode
	{
public:
	ArchiveNode_File(ArchiveNode_Directory* iParent, const string8& iName, size_t iIndex)
	:	ArchiveNode(iParent, iName)
	,	fIndex(iIndex)
		{}

	const size_t fIndex;
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
#pragma mark - FileIterRep_Archive

class FileIterRep_Archive
:	public FileIterRep
	{
public:
	typedef ArchiveNode_Directory::MapNameNode MapNameNode;

	const ZP<Archive> fArchive;
	MapNameNode::const_iterator fIter;
	const MapNameNode::const_iterator fEnd;

	FileIterRep_Archive(const ZP<Archive>& iArchive,
		MapNameNode::const_iterator iIter,
		MapNameNode::const_iterator iEnd)
	:	fArchive(iArchive)
	,	fIter(iIter)
	,	fEnd(iEnd)
		{}

	virtual ~FileIterRep_Archive()
		{}

	virtual bool HasValue()
		{ return fIter != fEnd; }

	virtual void Advance()
		{
		if (fIter != fEnd)
			++fIter;
		}

	virtual FileSpec Current()
		{
		if (fIter != fEnd)
			return new FileLoc_Archive(fArchive, fIter->second);

		return FileSpec();
		}

	virtual std::string CurrentName() const
		{ return fIter->first; }

	virtual ZP<FileIterRep> Clone()
		{ return new FileIterRep_Archive(fArchive, fIter, fEnd); }
	};

// =================================================================================================
#pragma mark - FileLoc_Archive

FileLoc_Archive::FileLoc_Archive(const ZP<Archive>& iArchive, const ZP<ArchiveNode>& iArchiveNode)
:	fArchive(iArchive)
,	fArchiveNode(iArchiveNode)
	{}

FileLoc_Archive::~FileLoc_Archive()
	{}

ZP<FileIterRep> FileLoc_Archive::CreateIterRep()
	{
	if (ZP<ArchiveNode_Directory> theDir = fArchiveNode.DynamicCast<ArchiveNode_Directory>())
		{
		return new FileIterRep_Archive(fArchive,
			theDir->fChildren.begin(), theDir->fChildren.end());
		}
	return null;
	}

std::string FileLoc_Archive::GetName() const
	{ return fArchiveNode->fName; }

ZQ<Trail> FileLoc_Archive::TrailTo(ZP<FileLoc> oDest) const
	{ return null; }

ZP<FileLoc> FileLoc_Archive::GetParent()
	{
	if (fArchiveNode && fArchiveNode->fParent)
		return new FileLoc_Archive(fArchive, fArchiveNode->fParent);
	return null;
	}

ZP<FileLoc> FileLoc_Archive::GetDescendant(const std::string* iComps, size_t iCount)
	{
	ZP<ArchiveNode> theNode = fArchiveNode;
	for (;;)
		{
		if (not iCount--)
			{
			if (theNode)
				return new FileLoc_Archive(fArchive, theNode);
			return null;
			}

		if (ZP<ArchiveNode_Directory> theDir = theNode.DynamicCast<ArchiveNode_Directory>())
			theNode = sGet(theDir->fChildren, *iComps++);
		else
			return null;
		}
	}

bool FileLoc_Archive::IsRoot()
	{ return not fArchiveNode->fParent; }

ZP<FileLoc> FileLoc_Archive::Follow()
	{ return this; }

std::string FileLoc_Archive::AsString_POSIX(const std::string* iComps, size_t iCount)
	{
	string result = this->pGetPath();
	for (size_t xx = 0; xx < iCount; ++xx)
		{
		result += "/";
		result += iComps[xx];
		}
	return result;
	}

std::string FileLoc_Archive::AsString_Native(const std::string* iComps, size_t iCount)
	{ return this->AsString_POSIX(iComps, iCount); }

File::Kind FileLoc_Archive::Kind()
	{
	if (fArchiveNode.DynamicCast<ArchiveNode_Directory>())
		return File::kindDir;

	return File::kindFile;
	}

uint64 FileLoc_Archive::Size()
	{
	ZUnimplemented();
	}

double FileLoc_Archive::TimeCreated()
	{ return 0; }

double FileLoc_Archive::TimeModified()
	{ return 0; }

ZP<FileLoc> FileLoc_Archive::CreateDir()
	{ return null; }

ZP<FileLoc> FileLoc_Archive::MoveTo(ZP<FileLoc> oDest)
	{ return null; }

bool FileLoc_Archive::Delete()
	{ return false; }

ZP<ChannerR_Bin> FileLoc_Archive::OpenR(bool iPreventWriters)
	{
	if (ZP<ArchiveNode_File> theNode = fArchiveNode.DynamicCast<ArchiveNode_File>())
		return fArchive->OpenR(theNode->fIndex);
	return null;
	}

ZP<ChannerRPos_Bin> FileLoc_Archive::OpenRPos(bool iPreventWriters)
	{
	if (ZP<ArchiveNode_File> theNode = fArchiveNode.DynamicCast<ArchiveNode_File>())
		return fArchive->OpenRPos(theNode->fIndex);
	return null;
	}

std::string FileLoc_Archive::pGetPath()
	{
	string result;
	for (ZP<ArchiveNode> theNode = fArchiveNode; theNode; theNode = theNode->fParent)
		{
		if (theNode->fParent || theNode == fArchiveNode)
			result = "/" + theNode->fName + result;
		}
	return result;
	}

// =================================================================================================
#pragma mark -

static
void spGrowArchiveDirectoryTree(size_t iIndex,
	const ZP<ArchiveNode_Directory>& ioParent,
	const Trail& iTrail, size_t iDepth)
	{
	const string8 theName = iTrail.At(iDepth);

	if (iDepth == iTrail.Count() - 1)
		{
		// We've hit the file
		ioParent->fChildren[theName] =
			new ArchiveNode_File(ioParent.Get(), theName, iIndex);
		}
	else
		{
		ZP<ArchiveNode_Directory> theNode_Directory =
			ioParent->fChildren[theName].StaticCast<ArchiveNode_Directory>();

		if (not theNode_Directory)
			{
			theNode_Directory = new ArchiveNode_Directory(ioParent.Get(), theName);
			ioParent->fChildren[theName] = theNode_Directory;
			}

		spGrowArchiveDirectoryTree(iIndex, theNode_Directory, iTrail, iDepth + 1);
		}
	}


FileSpec sFileSpec_Archive(const ZP<Archive>& iArchive)
	{
	if (iArchive)
		{
		ZP<ArchiveNode_Directory> theRoot = new ArchiveNode_Directory(nullptr, string8());
		for (size_t xx = 0, count = iArchive->Count(); xx < count; ++xx)
			{
			if (iArchive->IsFile(xx))
				{
				const string8 theName = iArchive->Name(xx);
				spGrowArchiveDirectoryTree(xx, theRoot, Trail(theName), 0);
				}
			}
		
		return new FileLoc_Archive(iArchive, theRoot);
		}
	return FileSpec();
	}

} // namespace ZooLib
