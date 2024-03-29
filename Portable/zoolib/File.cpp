// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/File.h"

#include "zoolib/Compat_algorithm.h" // For std::find
#include "zoolib/Util_STL_vector.h" // For sFirstOrNil

using std::find;
using std::min;
using std::string;
using std::vector;

namespace ZooLib {

/**
\defgroup File
General information about files and filesystems.

\bug Locking is only guaranteed to work between processes. If you open a file
with \a iPreventWriters == true and subsequently try to open the same
file with write access it may still succeed. This could be handled in
ZooLib by maintaining internal locks keyed off device/inode numbers, but
that's more than I want to do right now.
*/

// =================================================================================================
#pragma mark - FileSpec
/**
\class FileSpec
\ingroup File
\sa File
*/

/** \brief Constructs a file spec that is invalid, ie that represents
no current or potential node in the file system. */
FileSpec::FileSpec()
	{}

/// Copy constructor.
FileSpec::FileSpec(const FileSpec& iSpec)
:	fLoc(iSpec.fLoc),
	fComps(iSpec.fComps)
	{}

/// Construct a file spec representing the file location \a iLoc.
FileSpec::FileSpec(FileLoc* iLoc)
:	fLoc(iLoc)
	{}

/// Construct a file spec representing the file location \a iLoc.
FileSpec::FileSpec(const ZP<FileLoc>& iLoc)
:	fLoc(iLoc)
	{}

/// Construct a file spec representing the immediate child of \a iLoc named \a iComp.
FileSpec::FileSpec(const ZP<FileLoc>& iLoc, const string& iComp)
:	fLoc(iLoc),
	fComps(1, iComp)
	{}

/// Construct a spec representing the node described by the POSIX-style path.
/**
Note that the string \a iPath is not simply passed to the operating system as-is,
it is treated as a trail delta-ed off the root or the current working directory,
and thus you \e may end up referencing a node different from that you expect
if symlinks intrude on the path.
*/
FileSpec::FileSpec(const string& iPath)
	{
	if (iPath.empty())
		*this = sCWD();

	if (iPath[0] == '/')
		*this = sRoot().Follow(Trail(iPath.substr(1)));
	else
		*this = sCWD().Follow(Trail(iPath));
	}

/** \brief Returns true if the file spec is valid, that is if it represents
an extant or potentially extant node in the file system. */
FileSpec::operator bool() const
	{ return true && fLoc; }

/// Returns a file spec representing the current working directory.
/**
\note The current working directory is not well defined on all systems, check
the FileLoc implementation for the platform and API you're using to see
what it considers to be the current working directory.
*/
FileSpec FileSpec::sCWD()
	{
	ZP<FileLoc> theLoc =
		FunctionChain<ZP<FileLoc>, FileLoc::ELoc>::sInvoke(FileLoc::eLoc_CWD);

	ZEnsure(theLoc);

	return FileSpec(theLoc);
	}

/// Returns a file spec representing the root of the file system tree.
FileSpec FileSpec::sRoot()
	{
	ZP<FileLoc> theLoc =
		FunctionChain<ZP<FileLoc>, FileLoc::ELoc>::sInvoke(FileLoc::eLoc_Root);

	ZEnsure(theLoc);

	return FileSpec(theLoc);
	}

/// Returns a file spec representing the application executing the code.
/**
\note If the code is executing in a plugin it's possible that what's returned
by this method represents the plugin's file rather than the file
of the application that's hosting the plugin.
*/
FileSpec FileSpec::sApp()
	{
	ZP<FileLoc> theLoc =
		FunctionChain<ZP<FileLoc>, FileLoc::ELoc>::sInvoke(FileLoc::eLoc_App);
	return FileSpec(theLoc);
	}

/// Return the name of the node referenced by this file spec.
/**
If the file spec is invalid, or it somehow references a node whose name
was never known and now doesn't exist (MacOS only) then return the empty string.
*/
string FileSpec::Name() const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return fLoc->GetName();
		return fComps.back();
		}

	return string();
	}

/// Return a file spec representing the parent node of the node referenced by this file spec.
FileSpec FileSpec::Parent() const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return FileSpec(fLoc->GetParent());

		return FileSpec(fLoc, fComps.begin(), fComps.end() - 1);
		}

	return FileSpec();
	}

/**
Return the file spec representing the node with the name iName that is a
child of the node represented by this spec.
*/
FileSpec FileSpec::Child(const string& iName) const
	{
	if (iName.empty())
		return *this;
	else
		return FileSpec(fLoc, fComps.begin(), fComps.end(), iName);
	}

/**
Return the file spec representing the node with the name iName that is a
child of of the parent of the node represented by this spec.
*/
FileSpec FileSpec::Sibling(const string& iName) const
	{
	if (this->IsRoot())
		{
		// The root doesn't have a parent and thus can't have siblings.
		return FileSpec();
		}

	if (FileSpec parent = this->Parent())
		return parent.Child(iName);

	return FileSpec();
	}

/**
Return the file spec representing the node that would be reached by following
the trail \a iTrail starting at the the node represented by this file spec.
*/
FileSpec FileSpec::Follow(const Trail& iTrail) const
	{
	if (fLoc)
		{
		vector<string> originalComps = fComps;
		originalComps.insert(originalComps.end(),
			iTrail.GetComps().begin(), iTrail.GetComps().end());

		vector<string> realComps;
		size_t bounces = sNormalize_ReturnLeadingBounces(originalComps, realComps);

		if (ZP<FileLoc> ancestorLoc = fLoc->GetAncestor(bounces))
			{
			if (realComps.size())
				return FileSpec(ancestorLoc, realComps.begin(), realComps.end());
			return FileSpec(ancestorLoc);
			}
		}
	return FileSpec();
	}

FileSpec FileSpec::Ancestor(size_t iCount) const
	{
	if (fLoc)
		{
		if (iCount >= fComps.size())
			{
			ZP<FileLoc> newLoc = fLoc->GetAncestor(iCount - fComps.size());
			return FileSpec(newLoc);
			}
		return FileSpec(fLoc, fComps.begin(), fComps.end() - iCount);
		}

	return FileSpec();
	}

FileSpec FileSpec::Descendant(const string* iComps, size_t iCount) const
	{
	if (fLoc)
		{
		if (iCount == 0)
			return *this;

		vector<string> newComps = fComps;
		newComps.insert(newComps.end(), iComps, iComps + iCount);

		return FileSpec(fLoc, newComps.begin(), newComps.end());
		}

	return FileSpec();
	}

/// Returns true if the spec references the root of the file system.
bool FileSpec::IsRoot() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->IsRoot();

	return false;
	}

FileSpec FileSpec::TraverseLink() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return FileSpec(realLoc->TraverseLink());
	return FileSpec();
	}

/** \brief Return a POSIX syntax textual representation of the path
that would be passed to the file system to access this node. */
string FileSpec::AsString() const
	{
	if (fLoc)
		return fLoc->AsString_POSIX(Util_STL::sFirstOrNil(fComps), fComps.size());

	return string();
	}

/** \brief Return a native textual representation of the path that would
be passed to the file system to access this node. */
/**
The representation will be in the syntax used by the underlying API.
eg <tt>C:\path\to\node</tt> on Windows,
<tt>"Volume:path:to:node"</tt> on MacOS and <tt>"/path/to/node"</tt> on POSIX.
*/
string FileSpec::AsString_Native() const
	{
	if (fLoc)
		return fLoc->AsString_Native(Util_STL::sFirstOrNil(fComps), fComps.size());

	return string();
	}

/// Return a Trail representing the steps to get from this file spec to \a iDest.
ZQ<Trail> FileSpec::QTrailTo(const FileSpec& iDest) const
	{
	if (ZP<FileLoc> myRealLoc = this->pPhysicalLoc())
		{
		if (ZP<FileLoc> destRealLoc = iDest.pPhysicalLoc())
			return myRealLoc->QTrailTo(destRealLoc);
		}

	return null;
	}

/// Return a Trail representing the steps to get from \a iSource to this file spec.
ZQ<Trail> FileSpec::QTrailFrom(const FileSpec& iSource) const
	{
	if (ZP<FileLoc> myRealLoc = this->pPhysicalLoc())
		{
		if (ZP<FileLoc> sourceRealLoc = iSource.pPhysicalLoc())
			return sourceRealLoc->QTrailTo(myRealLoc);
		}

	return null;
	}

/// Returns the kind of file system entity represented by the node, if it exists.
/**
\param oError (optional output) The spec may reference an extant entity, but
it may be inaccessible. The precise nature of the fault will be placed in this parameter.
\return \li \c EFileKind::None The entity does not exist.
\return \li \c EFileKind::File The node exists and is a file.
\return \li \c EFileKind::Directory The node exists and is a directory.
*/
EFileKind FileSpec::Kind() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->Kind();

	return EFileKind::None;
	}

/// Returns true if the spec references an extant accessible entity.
bool FileSpec::Exists() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->Exists();

	return false;
	}

/// Returns true if the spec references an extant accessible file.
bool FileSpec::IsFile() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->IsFile();

	return false;
	}

/// Returns true if the spec references an extant accessible directory.
bool FileSpec::IsDir() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->IsDir();

	return false;
	}

/// Returns true if the spec references an extant accessible symlink.
bool FileSpec::IsSymLink() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->IsSymLink();

	return false;
	}

ZQ<uint64> FileSpec::QSize() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->QSize();

	return null;
	}

ZQ<double> FileSpec::QTimeCreated() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->QTimeCreated();

	return null;
	}

ZQ<double> FileSpec::QTimeModified() const
	{
	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->QTimeModified();

	return null;
	}

/// Create a directory at the location in the file system represented by the file spec.
FileSpec FileSpec::CreateDir() const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateDir();
		}
	return FileSpec();
	}

/** \brief Move the entity in the file system referenced by
the file spec to the location referenced by \a iDest. */
FileSpec FileSpec::MoveTo(const FileSpec& iDest) const
	{
	if (fLoc && iDest.fLoc)
		{
		if (ZP<FileLoc> myRealLoc = this->pPhysicalLoc())
			{
			if (ZP<FileLoc> destRealLoc = iDest.pPhysicalLoc())
				return myRealLoc->MoveTo(destRealLoc);
			}
		}

	return FileSpec();
	}

/// Delete the entity in the file system referenced by the file spec.
bool FileSpec::Delete() const
	{
	if (not fLoc)
		return false;

	if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->Delete();
	return false;
	}

/// Return a new ChannerR backed by the contents of the file referenced by the file spec.
ZP<ChannerR_Bin> FileSpec::OpenR(bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenR(iPreventWriters);
		}
	return null;
	}

/// Return a new ChannerRPos backed by the contents of the file referenced by the file spec.
ZP<ChannerRPos_Bin> FileSpec::OpenRPos(bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenRPos(iPreventWriters);
		}
	return null;
	}

/// Return a new ChannerW backed by the contents of the file referenced by the file spec.
ZP<ChannerW_Bin> FileSpec::OpenW(bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenW(iPreventWriters);
		}
	return null;
	}

/// Return a new ChannerWPos backed by the contents of the file referenced by the file spec.
ZP<ChannerWPos_Bin> FileSpec::OpenWPos(bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenWPos(iPreventWriters);
		}
	return null;
	}

/// Return a new ChannerRWPos backed by the contents of the file referenced by the file spec.
ZP<ChannerRWPos_Bin> FileSpec::OpenRWPos(bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenRWPos(iPreventWriters);
		}
	return null;
	}

/// Create a new file in the file system and return a ChannerWPos that provides access to it.
/**
\param iOpenExisting If the file already exists and \a iOpenExisting is true then it will
be opened (if it is accessible). If the file already exists and \a iOpenExisiting is false
then an invalid ChannerWPos is returned.
*/
ZP<ChannerWPos_Bin> FileSpec::CreateWPos(
	bool iOpenExisting, bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateWPos(iOpenExisting, iPreventWriters);
		}
	return null;
	}

ZP<ChannerRWPos_Bin> FileSpec::CreateRWPos(
	bool iOpenExisting, bool iPreventWriters) const
	{
	if (fLoc)
		{
		if (ZP<FileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateRWPos(iOpenExisting, iPreventWriters);
		}
	return null;
	}

ZP<FileLoc> FileSpec::GetFileLoc() const
	{ return this->pPhysicalLoc(); }

ZP<FileLoc> FileSpec::pPhysicalLoc() const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return fLoc;
		return fLoc->GetDescendant(&fComps[0], fComps.size());
		}
	return null;
	}

// =================================================================================================
#pragma mark - FileIter

/** \class FileIter
\ingroup File
\sa File
*/

FileIter::FileIter()
	{}

/// Copy constructor.
FileIter::FileIter(const FileIter& iOther)
:	fRep(iOther.fRep)
	{}

FileIter::~FileIter()
	{}

FileIter& FileIter::operator=(const FileIter& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

/// Construct a file iterator referencing the children of the specification \a iRoot.
/**
If iRoot does not reference an accessible directory then the FileIter will be invalid.
*/
FileIter::FileIter(const FileSpec& iRoot)
	{
	if (ZP<FileLoc> theLoc = iRoot.GetFileLoc())
		fRep = theLoc->CreateIterRep();
	}

FileIter::FileIter(const ZP<FileIterRep>& iRep)
:	fRep(iRep)
	{}

/**
Returns true if the iterator references a file spec. It will
return false if the iterator was initialized with a file spec
that is not an extant directory, or if the iterator has
run off the end of the directory.
*/
FileIter::operator bool() const
	{ return fRep && fRep->HasValue(); }

/**
Updates the iterator to point to the next child of the directory.
If this causes it to advance past the last child of the directory
then operator_boot_type() will return false.
If it has reached the end of the directory then nothing happens.
*/
FileIter& FileIter::Advance()
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Clone();

		if (fRep)
			fRep->Advance();
		}
	return *this;
	}

/**
If the iterator has not reached the end of the directory this
returns a FileSpec for that child. Otherwise it returns
an invalid FileSpec.
*/
FileSpec FileIter::Current() const
	{
	if (fRep)
		return fRep->Current();
	return FileSpec();
	}

/**
If the iterator has not reached the end of the directory this
returns the name of that child. Otherwise it returns
an empty string.
*/
string FileIter::CurrentName() const
	{
	if (fRep)
		return fRep->CurrentName();
	return string();
	}

// =================================================================================================
#pragma mark - FileTreeIter

FileTreeIter::FileTreeIter()
	{}

FileTreeIter::FileTreeIter(const FileTreeIter& iOther)
:	fCurrent(iOther.fCurrent),
	fStack(iOther.fStack)
	{}

FileTreeIter::FileTreeIter(const FileSpec& iCurrent)
:	fCurrent(iCurrent)
	{}

FileTreeIter::~FileTreeIter()
	{}

FileTreeIter& FileTreeIter::operator=(const FileTreeIter& iOther)
	{
	fCurrent = iOther.fCurrent;
	fStack = iOther.fStack;
	return *this;
	}

FileTreeIter::operator bool() const
	{ return true && fCurrent; }

FileTreeIter& FileTreeIter::Advance()
	{
	if (FileIter iter = fCurrent)
		{
		// fCurrent is an accessible directory, so descend into it.
		fStack.push_back(iter);
		fCurrent = fStack.back().Current();
		}
	else
		{
		// fCurrent is a file, or a directory that can't be accessed.
		fCurrent = FileSpec();
		while (not fStack.empty() && !fCurrent)
			{
			if (fStack.back().Advance())
				fCurrent = fStack.back().Current();
			else
				fStack.pop_back();
			}
		}
	return *this;
	}

FileSpec FileTreeIter::Current() const
	{ return fCurrent; }

string FileTreeIter::CurrentName() const
	{ return fCurrent.Name(); }

// =================================================================================================
#pragma mark - FileLoc

/** \class FileLoc
\ingroup File
FileLoc encapsulates the notion of a node in a file system. Generally
FileLoc can only refer to a node that exists, or existed at the time the FileLoc was
instantiated.

You'll rarely use FileLoc directly.

On POSIX it's just a wrapper round a regular path. On Win32 it also wraps a
path, but that might be a UNC path which is hard to fit into the strictly hierarchical
namespace we try to present. The FileLoc abstraction is most important on MacOS
using the FSSpec-style APIs. There it wraps a volume reference number and a directory ID,
and thus is able to maintain the distinction between two volumes with the same name and
can follow a directory as it is moved by the user.
*/

FileLoc::FileLoc()
	{}

FileLoc::~FileLoc()
	{}

ZP<FileIterRep> FileLoc::CreateIterRep()
	{ return null; }

ZP<FileLoc> FileLoc::GetAncestor(size_t iCount)
	{
	ZP<FileLoc> theLoc = this;
	while (theLoc && iCount--)
		theLoc = theLoc->GetParent();
	return theLoc;
	}

ZP<FileLoc> FileLoc::TraverseLink()
	{ return this; }

EFileKind FileLoc::Kind()
	{ return EFileKind::None; }

bool FileLoc::Exists()
	{ return EFileKind::None != this->Kind(); }

bool FileLoc::IsFile()
	{ return EFileKind::File == this->Kind(); }

bool FileLoc::IsDir()
	{ return EFileKind::Dir == this->Kind(); }

bool FileLoc::IsSymLink()
	{ return EFileKind::Link == this->Kind(); }

ZQ<uint64> FileLoc::QSize()
	{ return null; }

ZQ<double> FileLoc::QTimeCreated()
	{ return null; }

ZQ<double> FileLoc::QTimeModified()
	{ return null; }

ZP<FileLoc> FileLoc::CreateDir()
	{ return null; }

ZP<FileLoc> FileLoc::MoveTo(ZP<FileLoc> iDest)
	{ return null; }

bool FileLoc::Delete()
	{ return false; }

ZP<ChannerR_Bin> FileLoc::OpenR(bool iPreventWriters)
	{ return this->OpenRPos(iPreventWriters); }

ZP<ChannerRPos_Bin> FileLoc::OpenRPos(bool iPreventWriters)
	{ return null; }

ZP<ChannerW_Bin> FileLoc::OpenW(bool iPreventWriters)
	{ return this->OpenWPos(iPreventWriters); }

ZP<ChannerWPos_Bin> FileLoc::OpenWPos(bool iPreventWriters)
	{ return null; }

ZP<ChannerRWPos_Bin> FileLoc::OpenRWPos(bool iPreventWriters)
	{ return null; }

ZP<ChannerWPos_Bin> FileLoc::CreateWPos(bool iOpenExisting, bool iPreventWriters)
	{ return null; }

ZP<ChannerRWPos_Bin> FileLoc::CreateRWPos(bool iOpenExisting, bool iPreventWriters)
	{ return null; }

// =================================================================================================
#pragma mark - FileLoc_Std

string FileLoc_Std::GetName()
	{ return string(); }

ZQ<Trail> FileLoc_Std::QTrailTo(ZP<FileLoc> iDest)
	{ return null; }

ZP<FileLoc> FileLoc_Std::GetParent()
	{ return null; }

ZP<FileLoc> FileLoc_Std::GetDescendant(const string* iComps, size_t iCount)
	{ return null; }

bool FileLoc_Std::IsRoot()
	{ return false; }

string FileLoc_Std::AsString_POSIX(const string* iComps, size_t iCount)
	{ return string(); }

string FileLoc_Std::AsString_Native(const string* iComps, size_t iCount)
	{ return this->AsString_POSIX(iComps, iCount); }

// =================================================================================================
#pragma mark - FileIterRep

/** \class FileIterRep
*/

FileIterRep::FileIterRep()
	{}

FileIterRep::~FileIterRep()
	{}

// =================================================================================================
#pragma mark - FileIterRep_Std

/** \class FileIterRep_Std
*/

FileIterRep_Std::FileIterRep_Std(ZP<RealRep> iRealRep, size_t iIndex)
:	fRealRep(iRealRep),
	fIndex(iIndex)
	{}

FileIterRep_Std::~FileIterRep_Std()
	{}

bool FileIterRep_Std::HasValue()
	{ return fRealRep->HasValue(fIndex); }

void FileIterRep_Std::Advance()
	{ ++fIndex; }

FileSpec FileIterRep_Std::Current()
	{ return fRealRep->GetSpec(fIndex); }

string FileIterRep_Std::CurrentName()
	{ return fRealRep->GetName(fIndex); }

ZP<FileIterRep> FileIterRep_Std::Clone()
	{ return new FileIterRep_Std(fRealRep, fIndex); }

} // namespace ZooLib
