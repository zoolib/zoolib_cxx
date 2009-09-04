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

#include "zoolib/ZCompat_algorithm.h" // For std::find
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZFile.h"
#include "zoolib/ZTrail.h"

using std::find;
using std::min;
using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

/**
\defgroup group_File Files
General information about files and filesystems.



\bug Locking is only guaranteed to work between processes. If you open a file
with \a iPreventWriters == true and subsequently try to open the same
file with write access it may still succeed. This could be handled in
ZooLib by maintaining internal locks keyed off device/inode numbers, but
that's more than I want to do right now.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZFileSpec
/**
\class ZFileSpec
\ingroup group_File
\sa group_File
*/

/** \brief Constructs a file spec that is invalid, ie that represents
no current or potential node in the file system. */
ZFileSpec::ZFileSpec()
	{}

/// Copy constructor.
ZFileSpec::ZFileSpec(const ZFileSpec& iSpec)
:	fLoc(iSpec.fLoc),
	fComps(iSpec.fComps)
	{}

/// Construct a file spec representing the file location \a iLoc.
ZFileSpec::ZFileSpec(ZFileLoc* iLoc)
:	fLoc(iLoc)
	{}

/// Construct a file spec representing the file location \a iLoc.
ZFileSpec::ZFileSpec(const ZRef<ZFileLoc>& iLoc)
:	fLoc(iLoc)
	{}

/// Construct a file spec representing the immediate child of \a iLoc named \a iComp.
ZFileSpec::ZFileSpec(const ZRef<ZFileLoc>& iLoc, const string& iComp)
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
ZFileSpec::ZFileSpec(const string& iPath)
	{
	if (iPath.empty())
		*this = sCWD();

	if (iPath[0] == '/')
		*this = sRoot().Trail(ZTrail(iPath.substr(1)));
	else
		*this = sCWD().Trail(ZTrail(iPath));
	}

/** \brief Returns true if the file spec is valid, that is if it represents
an extant or potentially extant node in the file system. */
ZFileSpec::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fLoc && true); }

/// Returns a file spec representing the current working directory.
/**
\note The current working directory is not well defined on all systems, check
the ZFileLoc implementation for the platform and API you're using to see
what it considers to be the current working directory.
*/
ZFileSpec ZFileSpec::sCWD()
	{
	ZRef<ZFileLoc> theLoc =
		ZFunctionChain_T<ZRef<ZFileLoc>, ZFileLoc::ELoc>::sInvoke(ZFileLoc::eLoc_CWD);
	return ZFileSpec(theLoc);
	}

/// Returns a file spec representing the root of the file system tree.
ZFileSpec ZFileSpec::sRoot()
	{
	ZRef<ZFileLoc> theLoc =
		ZFunctionChain_T<ZRef<ZFileLoc>, ZFileLoc::ELoc>::sInvoke(ZFileLoc::eLoc_Root);
	return ZFileSpec(theLoc);
	}

/// Returns a file spec representing the application executing the code.
/**
\note If the code is executing in a plugin it's possible that what's returned
by this method represents the plugin's file rather than the file
of the application that's hosting the plugin.
*/
ZFileSpec ZFileSpec::sApp()
	{
	ZRef<ZFileLoc> theLoc =
		ZFunctionChain_T<ZRef<ZFileLoc>, ZFileLoc::ELoc>::sInvoke(ZFileLoc::eLoc_App);
	return ZFileSpec(theLoc);
	}

/// Return the name of the node referenced by this file spec.
/**
If the file spec is invalid, or it somehow references a node whose name
was never known and now doesn't exist (MacOS only) then return the empty string.
*/
string ZFileSpec::Name(ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return fLoc->GetName(oError);
		if (oError)
			*oError = ZFile::errorNone;
		return fComps.back();
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return string();
	}

/// Return a file spec representing the parent node of the node referenced by this file spec.
ZFileSpec ZFileSpec::Parent(ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return ZFileSpec(fLoc->GetParent(oError));

		if (oError)
			*oError = ZFile::errorNone;
		return ZFileSpec(fLoc, fComps.begin(), fComps.end() - 1);
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;
	return ZFileSpec();
	}

/**
Return the file spec representing the node with the name iName that is a
child of the node represented by this spec.
*/
ZFileSpec ZFileSpec::Child(const string& iName, ZFile::Error* oError) const
	{
	if (oError)
		*oError = ZFile::errorNone;

	if (iName.empty())
		return *this;
	else
		return ZFileSpec(fLoc, fComps.begin(), fComps.end(), iName);
	}

/**
Return the file spec representing the node with the name iName that is a
child of of the parent of the node represented by this spec.
*/
ZFileSpec ZFileSpec::Sibling(const string& iName, ZFile::Error* oError) const
	{
	ZFile::Error theError;
	if (this->IsRoot(&theError))
		{
		// The root doesn't have a parent and thus can't have siblings.
		if (oError)
			*oError = ZFile::errorGeneric;
		return ZFileSpec();
		}

	if (theError != ZFile::errorNone)
		{
		// If there was an error determining if we're at the root then we also have to bail.
		if (oError)
			*oError = theError;
		return ZFileSpec();
		}

	if (ZFileSpec parent = this->Parent(oError))
		return parent.Child(iName, oError);

	return ZFileSpec();
	}

/**
Return the file spec representing the node that would be reached by traversing
the trail \a iTrail starting at the the node represented by this file spec.
*/
ZFileSpec ZFileSpec::Trail(const ZTrail& iTrail, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		vector<string> originalComps = fComps;
		originalComps.insert(originalComps.end(),
			iTrail.GetComps().begin(), iTrail.GetComps().end());

		vector<string> realComps;
		size_t bounces = ZTrail::sNormalize_ReturnLeadingBounces(originalComps, realComps);

		if (ZRef<ZFileLoc> ancestorLoc = fLoc->GetAncestor(bounces, oError))
			{
			if (realComps.size())
				return ZFileSpec(ancestorLoc, realComps.begin(), realComps.end());
			return ZFileSpec(ancestorLoc);
			}
		}
	if (oError)
		*oError = ZFile::errorInvalidFileSpec;
	return ZFileSpec();
	}

ZFileSpec ZFileSpec::Ancestor(size_t iCount, Error* oError) const
	{
	if (fLoc)
		{
		if (iCount >= fComps.size())
			{
			ZRef<ZFileLoc> newLoc = fLoc->GetAncestor(iCount - fComps.size(), oError);
			return ZFileSpec(newLoc);
			}
		if (oError)
			*oError = ZFile::errorNone;
		return ZFileSpec(fLoc, fComps.begin(), fComps.end() - iCount);
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;
	return ZFileSpec();
	}

ZFileSpec ZFileSpec::Descendant(const std::string* iComps, size_t iCount, Error* oError) const
	{		
	if (fLoc)
		{
		if (oError)
			*oError = ZFile::errorNone;
		if (iCount == 0)
			return *this;

		vector<string> newComps = fComps;
		newComps.insert(newComps.end(), iComps, iComps + iCount);

		return ZFileSpec(fLoc, newComps.begin(), newComps.end());
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;
	return ZFileSpec();
	}

/** \brief Return a POSIX syntax textual representation of the path
that would be passed to the file system to access this node. */
string ZFileSpec::AsString() const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return fLoc->AsString_POSIX(nullptr, 0);
		return fLoc->AsString_POSIX(&fComps[0], fComps.size());
		}
	return string();
	}

/** \brief  Return a native textual representation of the path that would
be passed to the file system to access this node. */
/**
The representation will be in the syntax used by the underlying API.
eg <tt>C:\path\to\node</tt> on Windows,
<tt>"Volume:path:to:node"</tt> on MacOS and <tt>"/path/to/node"</tt> on POSIX.
*/
string ZFileSpec::AsString_Native() const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return fLoc->AsString_Native(nullptr, 0);
		return fLoc->AsString_Native(&fComps[0], fComps.size());
		}
	return string();
	}

/// Return a ZTrail representing the steps to get from this file spec to \a iDest.
ZTrail ZFileSpec::TrailTo(const ZFileSpec& iDest, ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> myRealLoc = this->pPhysicalLoc())
		{
		if (ZRef<ZFileLoc> destRealLoc = iDest.pPhysicalLoc())
			return myRealLoc->TrailTo(destRealLoc, oError);
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return ZTrail(false);
	}

/// Return a ZTrail representing the steps to get from \a iSource to this file spec.
ZTrail ZFileSpec::TrailFrom(const ZFileSpec& iSource, ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> myRealLoc = this->pPhysicalLoc())
		{
		if (ZRef<ZFileLoc> sourceRealLoc = iSource.pPhysicalLoc())
			return sourceRealLoc->TrailTo(myRealLoc, oError);
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return ZTrail(false);
	}

/// Returns the kind of file system entity represented by the node, if it exists.
/**
\param oError (optional output) The spec may reference an extant entity, but
it may be inaccessible. The precise nature of the fault will be placed in this parameter.
\return \li \c ZFile::kindNone The entity does not exist.
\return \li \c ZFile::kindFile The node exists and is a file.
\return \li \c ZFile::kindDirectory The node exists and is a directory.
*/
ZFile::Kind ZFileSpec::Kind(ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->Kind(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return ZFile::kindNone;
	}

/// Returns true if the spec references the root of the file system.
bool ZFileSpec::IsRoot(ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		{
		if (oError)
			*oError = ZFile::errorNone;
		return realLoc->IsRoot();
		}

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return false;
	}

/// Returns true if the spec references an extant accessible file.
bool ZFileSpec::IsFile(ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return ZFile::kindFile == realLoc->Kind(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return false;
	}

/// Returns true if the spec references an extant accessible directory.
bool ZFileSpec::IsDir(ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return ZFile::kindDir == realLoc->Kind(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return false;
	}

/// Returns true if the spec references an extant accessible symlink.
bool ZFileSpec::IsSymLink(ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return ZFile::kindLink == realLoc->Kind(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return false;
	}

/// Returns true if the spec references an extant accessible entity.
bool ZFileSpec::Exists(ZFile::Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return ZFile::kindNone != realLoc->Kind(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return false;
	}

uint64 ZFileSpec::Size(Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->Size(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return 0;
	}

ZTime ZFileSpec::TimeCreated(Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->TimeCreated(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return ZTime();
	}

ZTime ZFileSpec::TimeModified(Error* oError) const
	{
	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->TimeModified(oError);

	if (oError)
		*oError = ZFile::errorInvalidFileSpec;

	return ZTime();
	}

/// On MacOS sets the creator and type appropriately. On other platforms, does nothing.
bool ZFileSpec::SetCreatorAndType(uint32 iCreator, uint32 iType, ZFile::Error* oError) const
	{
	if (!fLoc)
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		return false;
		}

	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->SetCreatorAndType(iCreator, iType, oError);
	if (oError)
		*oError = ZFile::errorDoesntExist;
	return false;
	}

/// Create a directory at the location in the file system represented by the file spec.
ZFileSpec ZFileSpec::CreateDir(ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateDir(oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZFileSpec();
	}

/** \brief  Move the entity in the file system referenced by
the file spec to the location referenced by \a iDest. */
ZFileSpec ZFileSpec::MoveTo(const ZFileSpec& iDest, ZFile::Error* oError) const
	{
	if (fLoc && iDest.fLoc)
		{
		if (ZRef<ZFileLoc> myRealLoc = this->pPhysicalLoc())
			{
			if (ZRef<ZFileLoc> destRealLoc = iDest.pPhysicalLoc())
				return myRealLoc->MoveTo(destRealLoc, oError);
			}
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}

	return ZFileSpec();
	}

/// Delete the entity in the file system referenced by the file spec.
bool ZFileSpec::Delete(ZFile::Error* oError) const
	{
	if (!fLoc)
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		return false;
		}

	if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
		return realLoc->Delete(oError);
	if (oError)
		*oError = ZFile::errorDoesntExist;
	return false;
	}

/// Return a new ZStreamerR backed by the contents of the file referenced by the file spec.
ZRef<ZStreamerR> ZFileSpec::OpenR(bool iPreventWriters, ZFile::Error* oError) const
	{
	return this->OpenRPos(iPreventWriters, oError);
	}

/// Return a new ZStreamerRPos backed by the contents of the file referenced by the file spec.
ZRef<ZStreamerRPos> ZFileSpec::OpenRPos(bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenRPos(iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZStreamerRPos>();
	}

/// Return a new ZStreamerW backed by the contents of the file referenced by the file spec.
ZRef<ZStreamerW> ZFileSpec::OpenW(bool iPreventWriters, ZFile::Error* oError) const
	{
	return this->OpenWPos(iPreventWriters, oError);
	}

/// Return a new ZStreamerWPos backed by the contents of the file referenced by the file spec.
ZRef<ZStreamerWPos> ZFileSpec::OpenWPos(bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenWPos(iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZStreamerWPos>();
	}

/// Return a new ZStreamerRWPos backed by the contents of the file referenced by the file spec.
ZRef<ZStreamerRWPos> ZFileSpec::OpenRWPos(bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenRWPos(iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZStreamerRWPos>();
	}

/// Create a new file in the file system and return a ZStreamerWPos that provides access to it.
/**
\param iOpenExisting If the file already exists and \a iOpenExisting is true then it will
be opened (if it is accessible). If the file already exists and \a iOpenExisiting is false
then an invalid ZStreamerWPos is returned.
*/
ZRef<ZStreamerWPos> ZFileSpec::CreateWPos(
	bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateWPos(iOpenExisting, iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZFileSpec::CreateRWPos(
	bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateRWPos(iOpenExisting, iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZFileR> ZFileSpec::OpenFileR(bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenFileR(iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZFileR>();
	}

ZRef<ZFileW> ZFileSpec::OpenFileW(bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenFileW(iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZFileW>();
	}

ZRef<ZFileRW> ZFileSpec::OpenFileRW(bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->OpenFileRW(iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZFileRW>();
	}

ZRef<ZFileW> ZFileSpec::CreateFileW(
	bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateFileW(iOpenExisting, iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZFileW>();
	}

ZRef<ZFileRW> ZFileSpec::CreateFileRW(
	bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError) const
	{
	if (fLoc)
		{
		if (ZRef<ZFileLoc> realLoc = this->pPhysicalLoc())
			return realLoc->CreateFileRW(iOpenExisting, iPreventWriters, oError);
		if (oError)
			*oError = ZFile::errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = ZFile::errorInvalidFileSpec;
		}
	return ZRef<ZFileRW>();
	}

ZRef<ZFileLoc> ZFileSpec::GetFileLoc() const
	{ return this->pPhysicalLoc(); }

ZRef<ZFileLoc> ZFileSpec::pPhysicalLoc() const
	{
	if (fLoc)
		{
		if (fComps.empty())
			return fLoc;
		return fLoc->GetDescendant(&fComps[0], fComps.size(), nullptr);
		}
	return ZRef<ZFileLoc>();
	}

#if ZCONFIG_File_EnableDeprecated
ZFileSpec ZFileSpec::sPOSIXPath(const string& iPath)
	{ return ZFileSpec(iPath); }
#endif // ZCONFIG_File_EnableDeprecated

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIter

/** \class ZFileIter
\ingroup group_File
\sa group_File
*/

ZFileIter::ZFileIter()
	{}

/// Copy constructor.
ZFileIter::ZFileIter(const ZFileIter& iOther)
:	fRep(iOther.fRep)
	{}

/// Construct a file iterator referencing the children of the specification \a iRoot.
/**
If iRoot does not reference an accessible directory then the ZFileIter will be invalid.
*/
ZFileIter::ZFileIter(const ZFileSpec& iRoot)
	{
	if (ZRef<ZFileLoc> theLoc = iRoot.GetFileLoc())
		fRep = theLoc->CreateIterRep();
	}

ZFileIter::~ZFileIter()
	{}

ZFileIter& ZFileIter::operator=(const ZFileIter& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

/**
Returns true if the iterator references a file spec. It will
return false if the iterator was initialized with a file spec
that is not an extant directory, or if the iterator has
run off the end of the directory.
*/
ZFileIter::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fRep->HasValue()); }

/**
Updates the iterator to point to the next child of the directory.
If this causes it to advance past the last child of the directory
then operator_boot_type() will return false.
If it has reached the end of the directory then nothing happens.
*/
ZFileIter& ZFileIter::Advance()
	{
	if (fRep)
		{
		if (fRep->GetRefCount() > 1)
			fRep = fRep->Clone();

		if (fRep)
			fRep->Advance();
		}
	return *this;
	}

/**
If the iterator has not reached the end of the directory this
returns a ZFileSpec for that child. Otherwise it returns
an invalid ZFileSpec.
*/
ZFileSpec ZFileIter::Current() const
	{
	if (fRep)
		return fRep->Current();
	return ZFileSpec();
	}

/**
If the iterator has not reached the end of the directory this
returns the name of that child. Otherwise it returns
an empty string.
*/
string ZFileIter::CurrentName() const
	{
	if (fRep)
		return fRep->CurrentName();
	return string();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileTreeIter

ZFileTreeIter::ZFileTreeIter()
	{}

ZFileTreeIter::ZFileTreeIter(const ZFileTreeIter& iOther)
:	fCurrent(iOther.fCurrent),
	fStack(iOther.fStack)
	{}

ZFileTreeIter::ZFileTreeIter(const ZFileSpec& iCurrent)
:	fCurrent(iCurrent)
	{}

ZFileTreeIter::~ZFileTreeIter()
	{}

ZFileTreeIter& ZFileTreeIter::operator=(const ZFileTreeIter& iOther)
	{
	fCurrent = iOther.fCurrent;
	fStack = iOther.fStack;
	return *this;
	}

ZFileTreeIter::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fCurrent && true); }

ZFileTreeIter& ZFileTreeIter::Advance()
	{
	if (ZFileIter iter = fCurrent)
		{
		// fCurrent is an accessible directory, so descend into it.
		fStack.push_back(iter);
		fCurrent = fStack.back().Current();
		}
	else
		{
		// fCurrent is a file, or a directory that can't be accessed.
		fCurrent = ZFileSpec();
		while (!fStack.empty() && !fCurrent)
			{
			if (fStack.back().Advance())
				fCurrent = fStack.back().Current();
			else
				fStack.pop_back();
			}	
		}
	return *this;
	}

ZFileSpec ZFileTreeIter::Current() const
	{ return fCurrent; }

std::string ZFileTreeIter::CurrentName() const
	{ return fCurrent.Name(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileLoc

/** \class ZFileLoc
\ingroup group_File
ZFileLoc encapsulates the notion of a node in a file system. Generally
ZFileLoc can only refer to a node that exists, or existed at the time the ZFileLoc was
instantiated.

You'll rarely use ZFileLoc directly.

On POSIX it's just a wrapper round a regular path. On Win32 it also wraps a
path, but that might be a UNC path which is hard to fit into the strictly hierarchical
namespace we try to present. The ZFileLoc abstraction is most important on MacOS
using the FSSpec-style APIs. There it wraps a volume reference number and a directory ID,
and thus is able to maintain the distinction between two volumes with the same name and
can follow a directory as it is moved by the user.
*/

ZFileLoc::ZFileLoc()
	{}

ZFileLoc::~ZFileLoc()
	{}

ZRef<ZFileIterRep> ZFileLoc::CreateIterRep()
	{ return ZRef<ZFileIterRep>(); }

ZRef<ZFileLoc> ZFileLoc::GetAncestor(size_t iCount, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorNone;
	ZRef<ZFileLoc> theLoc = this;
	while (theLoc && iCount--)
		theLoc = theLoc->GetParent(oError);
	return theLoc;
	}

bool ZFileLoc::SetCreatorAndType(uint32 iCreator, uint32 iType, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return false;
	}

ZRef<ZStreamerRPos> ZFileLoc::OpenRPos(bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZStreamerRPos>();
	}

ZRef<ZStreamerWPos> ZFileLoc::OpenWPos(bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZFileLoc::OpenRWPos(bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZStreamerWPos> ZFileLoc::CreateWPos(
	bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZFileLoc::CreateRWPos(
	bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZFileR> ZFileLoc::OpenFileR(bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZFileR>();
	}

ZRef<ZFileW> ZFileLoc::OpenFileW(bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZFileW>();
	}

ZRef<ZFileRW> ZFileLoc::OpenFileRW(bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZFileRW>();
	}

ZRef<ZFileW> ZFileLoc::CreateFileW(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZFileW>();
	}

ZRef<ZFileRW> ZFileLoc::CreateFileRW(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oError)
	{
	if (oError)
		*oError = ZFile::errorGeneric;
	return ZRef<ZFileRW>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIterRep

/** \class ZFileIterRep
*/

ZFileIterRep::ZFileIterRep()
	{}

ZFileIterRep::~ZFileIterRep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileIterRep_Std

/** \class ZFileIterRep_Std
*/

ZFileIterRep_Std::ZFileIterRep_Std(ZRef<RealRep> iRealRep, size_t iIndex)
:	fRealRep(iRealRep),
	fIndex(iIndex)
	{}

ZFileIterRep_Std::~ZFileIterRep_Std()
	{}

bool ZFileIterRep_Std::HasValue()
	{ return fRealRep->HasValue(fIndex); }

void ZFileIterRep_Std::Advance()
	{ ++fIndex; }

ZFileSpec ZFileIterRep_Std::Current()
	{ return fRealRep->GetSpec(fIndex); }

string ZFileIterRep_Std::CurrentName() const
	{ return fRealRep->GetName(fIndex); }

ZRef<ZFileIterRep> ZFileIterRep_Std::Clone()
	{ return new ZFileIterRep_Std(fRealRep, fIndex); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_FileR

ZStreamRPos_FileR::ZStreamRPos_FileR(ZRef<ZFileR> iFile)
:	fFile(iFile),
	fPosition(0)
	{
	ZAssertStop(1, fFile);
	}

ZStreamRPos_FileR::~ZStreamRPos_FileR()
	{}

void ZStreamRPos_FileR::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	if (ZFile::errorNone == fFile->ReadAt(fPosition, iDest, iCount, &countRead))
		{
		fPosition += countRead;
		if (oCountRead)
			*oCountRead = countRead;
		}
	else if (oCountRead)
		{
		*oCountRead = countRead;
		}
	}

uint64 ZStreamRPos_FileR::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_FileR::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_FileR::Imp_GetSize()
	{
	uint64 curSize;
	if (ZFile::errorNone == fFile->GetSize(curSize))
		return curSize;
	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_FileR

ZStreamerRPos_FileR::ZStreamerRPos_FileR(ZRef<ZFileR> iFile)
:	fStream(iFile)
	{}

ZStreamerRPos_FileR::~ZStreamerRPos_FileR()
	{}

const ZStreamRPos& ZStreamerRPos_FileR::GetStreamRPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_FileW

ZStreamWPos_FileW::ZStreamWPos_FileW(ZRef<ZFileW> iFile)
:	fFile(iFile),
	fPosition(0)
	{
	ZAssertStop(1, fFile);
	}

ZStreamWPos_FileW::~ZStreamWPos_FileW()
	{}

void ZStreamWPos_FileW::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	if (ZFile::errorNone == fFile->WriteAt(fPosition, iSource, iCount, &countWritten))
		{
		fPosition += countWritten;
		if (oCountWritten)
			*oCountWritten = countWritten;
		}
	else if (oCountWritten)
		{
		*oCountWritten = 0;
		}
	}

void ZStreamWPos_FileW::Imp_Flush()
	{ fFile->Flush(); }

uint64 ZStreamWPos_FileW::Imp_GetPosition()
	{ return fPosition; }

void ZStreamWPos_FileW::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamWPos_FileW::Imp_GetSize()
	{
	uint64 curSize;
	if (ZFile::errorNone == fFile->GetSize(curSize))
		return curSize;
	return 0;
	}

void ZStreamWPos_FileW::Imp_SetSize(uint64 iSize)
	{
	if (ZFile::errorNone != fFile->SetSize(iSize))
		sThrowBadSize();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_FileW

ZStreamerWPos_FileW::ZStreamerWPos_FileW(ZRef<ZFileW> iFile)
:	fStream(iFile)
	{}

ZStreamerWPos_FileW::~ZStreamerWPos_FileW()
	{}

const ZStreamWPos& ZStreamerWPos_FileW::GetStreamWPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_FileRW

ZStreamRWPos_FileRW::ZStreamRWPos_FileRW(ZRef<ZFileRW> iFile)
:	fFile(iFile),
	fPosition(0)
	{
	ZAssertStop(1, fFile);
	}

ZStreamRWPos_FileRW::~ZStreamRWPos_FileRW()
	{}

void ZStreamRWPos_FileRW::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	if (ZFile::errorNone == fFile->ReadAt(fPosition, iDest, iCount, &countRead))
		{
		fPosition += countRead;
		if (oCountRead)
			*oCountRead = countRead;
		}
	else if (oCountRead)
		{
		*oCountRead = countRead;
		}
	}

void ZStreamRWPos_FileRW::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	if (ZFile::errorNone == fFile->WriteAt(fPosition, iSource, iCount, &countWritten))
		{
		fPosition += countWritten;
		if (oCountWritten)
			*oCountWritten = countWritten;
		}
	else if (oCountWritten)
		{
		*oCountWritten = 0;
		}
	}

void ZStreamRWPos_FileRW::Imp_Flush()
	{ fFile->Flush(); }

uint64 ZStreamRWPos_FileRW::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_FileRW::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_FileRW::Imp_GetSize()
	{
	uint64 curSize;
	if (ZFile::errorNone == fFile->GetSize(curSize))
		return curSize;
	return 0;
	}

void ZStreamRWPos_FileRW::Imp_SetSize(uint64 iSize)
	{
	if (ZFile::errorNone != fFile->SetSize(iSize))
		sThrowBadSize();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_FileRW

ZStreamerRWPos_FileRW::ZStreamerRWPos_FileRW(ZRef<ZFileRW> iFile)
:	fStream(iFile)
	{}

ZStreamerRWPos_FileRW::~ZStreamerRWPos_FileRW()
	{}

const ZStreamRWPos& ZStreamerRWPos_FileRW::GetStreamRWPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR_StreamerRPos

ZFileR_StreamerRPos::ZFileR_StreamerRPos(ZRef<ZStreamerRPos> iStreamer)
:	fStreamer(iStreamer),
	fStream(iStreamer->GetStreamRPos())
	{}

ZFile::Error ZFileR_StreamerRPos::ReadAt(
	uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{
	fMutex.Acquire();
	fStream.SetPosition(iOffset);
	fStream.Read(iDest, iCount, oCountRead);
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileR_StreamerRPos::GetSize(uint64& oSize)
	{
	fMutex.Acquire();
	oSize = fStream.GetSize();
	fMutex.Release();
	return ZFile::errorNone;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW_StreamerWPos

ZFileW_StreamerWPos::ZFileW_StreamerWPos(ZRef<ZStreamerWPos> iStreamer)
:	fStreamer(iStreamer),
	fStream(iStreamer->GetStreamWPos())
	{}

ZFile::Error ZFileW_StreamerWPos::WriteAt(
	uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	fMutex.Acquire();
	fStream.SetPosition(iOffset);
	fStream.Write(iSource, iCount, oCountWritten);
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileW_StreamerWPos::GetSize(uint64& oSize)
	{
	fMutex.Acquire();
	oSize = fStream.GetSize();
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileW_StreamerWPos::SetSize(uint64 iSize)
	{
	fMutex.Acquire();
	fStream.SetSize(iSize);
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileW_StreamerWPos::Flush()
	{
	fMutex.Acquire();
	fStream.Flush();
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileW_StreamerWPos::FlushVolume()
	{
	fMutex.Acquire();
	fStream.Flush();
	fMutex.Release();
	return ZFile::errorNone;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW_StreamerRWPos

ZFileRW_StreamerRWPos::ZFileRW_StreamerRWPos(ZRef<ZStreamerRWPos> iStreamer)
:	fStreamer(iStreamer),
	fStream(iStreamer->GetStreamRWPos())
	{}

ZFile::Error ZFileRW_StreamerRWPos::ReadAt(
	uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{
	fMutex.Acquire();
	fStream.SetPosition(iOffset);
	fStream.Read(iDest, iCount, oCountRead);
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileRW_StreamerRWPos::WriteAt(
	uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	fMutex.Acquire();
	fStream.SetPosition(iOffset);
	fStream.Write(iSource, iCount, oCountWritten);
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileRW_StreamerRWPos::GetSize(uint64& oSize)
	{
	fMutex.Acquire();
	oSize = fStream.GetSize();
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileRW_StreamerRWPos::SetSize(uint64 iSize)
	{
	fMutex.Acquire();
	fStream.SetSize(iSize);
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileRW_StreamerRWPos::Flush()
	{
	fMutex.Acquire();
	fStream.Flush();
	fMutex.Release();
	return ZFile::errorNone;
	}

ZFile::Error ZFileRW_StreamerRWPos::FlushVolume()
	{
	fMutex.Acquire();
	fStream.Flush();
	fMutex.Release();
	return ZFile::errorNone;
	}

NAMESPACE_ZOOLIB_END
