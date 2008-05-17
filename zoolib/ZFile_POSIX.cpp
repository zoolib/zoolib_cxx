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

// Enable 64 bit variants of system calls on Linux. We don't go the whole hog and
// use _FILE_OFFSET_BITS to do renaming of entry points and data types because I'd
// rather be explicit about which functions we're calling.
#ifndef _LARGEFILE64_SOURCE
#	define _LARGEFILE64_SOURCE
#endif

#include "ZFile_POSIX.h"

#if ZCONFIG_API_Enabled(File_POSIX)

#if ZCONFIG_SPI_Enabled(BeOS)
#	define ZCONFIG_File_AtAPISupported 0
#else
#	define ZCONFIG_File_AtAPISupported 1
#endif

#include "ZDebug.h"
#include "ZMain.h" // For ZMain::sArgV

#include <vector>

#define kDebug_File_POSIX 2

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

#include "ZFactoryChain.h"

static bool sMake_FileLoc(ZRef<ZFileLoc>& oResult, ZFileLoc::ELoc iParam)
	{
	try
		{
		switch (iParam)
			{
			case ZFileLoc::eLoc_Root: oResult = ZFileLoc_POSIX::sGet_Root(); return true;
			case ZFileLoc::eLoc_CWD: oResult = ZFileLoc_POSIX::sGet_CWD(); return true;
			case ZFileLoc::eLoc_App: oResult = ZFileLoc_POSIX::sGet_App(); return true;
			}
		}
	catch (...)
		{}
	return false;
	}

static ZFactoryChain_Maker_T<ZRef<ZFileLoc>, ZFileLoc::ELoc>
	sMaker1(sMake_FileLoc);

// =================================================================================================
#pragma mark -
#pragma mark * Shared implementation details

static ZFile::Error sTranslateError(int iNativeError)
	{
	ZFile::Error theErr = ZFile::errorGeneric;
	switch (iNativeError)
		{
		case 0:
			theErr = ZFile::errorNone;
			break;
		case EEXIST:
			theErr = ZFile::errorAlreadyExists;
			break;
		case ENAMETOOLONG:
			theErr = ZFile::errorIllegalFileName;
			break;
		case EAGAIN:
		case EACCES:
			theErr = ZFile::errorNoPermission;
			break;
		case ENOENT:
			theErr = ZFile::errorDoesntExist;
			break;
		case ENOTDIR:
			theErr = ZFile::errorWrongTypeForOperation;
			break;
		case EISDIR:
			theErr = ZFile::errorWrongTypeForOperation;
			break;
		case EPERM:
			theErr = ZFile::errorNoPermission;
			break;
		}
	return theErr;
	}

static int sFCntl(int iFD, int iCmd, struct flock& ioFLock)
	{
	for (;;)
		{
		if (-1 != ::fcntl(iFD, iCmd, &ioFLock))
			return 0;

		if (errno != EINTR)
			return errno;
		}
	}

static int sLockOrClose(int iFD, bool iRead, bool iWrite, bool iPreventWriters, ZFile::Error* oErr)
	{
	ZAssertStop(kDebug_File_POSIX, iRead || iWrite);

	if (ZCONFIG_SPI_Enabled(BeOS))
		{
		// Doesn't look like BeOS supports advisory locks.
		return iFD;
		}

	// We always respect advisory locks, and we may apply a lock ourselves.
	struct flock theFLock;
	theFLock.l_whence = SEEK_SET;
	theFLock.l_start = 0;
	theFLock.l_len = 0;

	if (iWrite)
		theFLock.l_type = F_WRLCK;
	else
		theFLock.l_type = F_RDLCK;

	int err;
	if (iPreventWriters)
		{
		err = ::sFCntl(iFD, F_SETLK, theFLock);
		if (err == 0)
			return iFD;
		}
	else
		{
		err = ::sFCntl(iFD, F_GETLK, theFLock);

		if (err == ENOLCK || err == EACCES)
			{
			// Assume that we're hitting an NFS-based file. Allow the access to happen.
			return iFD;
			}
		else if (err == 0 && theFLock.l_type == F_UNLCK)
			{
			return iFD;
			}
		}

	::close(iFD);
	if (oErr)
		*oErr = ::sTranslateError(err);
	return -1;
	}

static int sOpen(const char* iPath, bool iRead, bool iWrite, bool iPreventWriters, ZFile::Error* oErr)
	{
	#if defined(linux)
		int theFlags = O_NOCTTY | O_LARGEFILE;
	#else
		int theFlags = O_NOCTTY;
	#endif

	if (iRead && iWrite)
		theFlags |= O_RDWR;
	else if (iRead)
		theFlags |= O_RDONLY;
	else
		theFlags |= O_WRONLY;

	int theFD = ::open(iPath, theFlags);

	if (theFD < 0)
		{
		if (oErr)
			*oErr = ::sTranslateError(errno);
		return -1;
		}

	return sLockOrClose(theFD, iRead, iWrite, iPreventWriters, oErr);
	}

static void sClose(int iFD)
	{
	if (iFD < 0)
		return;

	// We do not need to release the lock -- closing
	// the file will release any locks on the file.
	// AG2002-07-30. I'm no longer certain that comment is true, so
	// we'll release the lock for now -- locks are actually by inode and process,
	// not by file descriptor and process.

	struct flock file_lock;
	file_lock.l_type = F_UNLCK;
	file_lock.l_whence = SEEK_SET;
	file_lock.l_start = 0;
	file_lock.l_len = 0;
	::fcntl(iFD, F_SETLK, &file_lock);

	::close(iFD);
	}

static int sCreate(const char* iPath, bool iOpenExisting, bool iAllowRead, bool iPreventWriters, ZFile::Error* oErr)
	{
	#if defined(linux)
		int flags = O_CREAT | O_NOCTTY | O_LARGEFILE;
	#else
		int flags = O_CREAT | O_NOCTTY;
	#endif

	if (iAllowRead)
		flags |= O_RDWR;
	else
		flags |= O_WRONLY;

	if (!iOpenExisting)
		flags |= O_EXCL;

	int theFD = ::open(iPath, flags, 0666);

	if (theFD < 0)
		{
		if (oErr)
			*oErr = ::sTranslateError(errno);
		return -1;
		}

	return sLockOrClose(theFD, iAllowRead, true, iPreventWriters, oErr);
	}

static ZFile::Error sRead(int iFD, void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	char* localDest = reinterpret_cast<char*>(iDest);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		ssize_t countRead = ::read(iFD, localDest, countRemaining);

		if (countRead == 0)
			return ZFile::errorReadPastEOF;

		if (countRead < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return sTranslateError(err);
			}
		if (oCountRead)
			*oCountRead += countRead;
		countRemaining -= countRead;
		localDest += countRead;
		}
	return ZFile::errorNone;
	}

static ZFile::Error sWrite(int iFD, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	const char* localSource = reinterpret_cast<const char*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		ssize_t countWritten = ::write(iFD, localSource, countRemaining);
		if (countWritten < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return sTranslateError(err);
			}
		if (oCountWritten)
			*oCountWritten += countWritten;
		countRemaining -= countWritten;
		localSource += countWritten;
		}
	return ZFile::errorNone;
	}

static ZFile::Error sReadAt(int iFD, uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{
#if ZCONFIG_File_AtAPISupported

	if (oCountRead)
		*oCountRead = 0;

	#if !defined(linux)
	if (sizeof(off_t) == 4 && iOffset > 0x7FFFFFFFL)
		return ZFile::errorGeneric;
	#endif

	char* localDest = reinterpret_cast<char*>(iDest);
	uint64 localOffset = iOffset;
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		#if defined(linux)
			ssize_t countRead = ::pread64(iFD, localDest, countRemaining, localOffset);
		#else
			ssize_t countRead = ::pread(iFD, localDest, countRemaining, localOffset);
		#endif

		if (countRead == 0)
			return ZFile::errorReadPastEOF;

		if (countRead < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return sTranslateError(err);
			}
		if (oCountRead)
			*oCountRead += countRead;
		countRemaining -= countRead;
		localDest += countRead;
		localOffset += countRead;
		}
	return ZFile::errorNone;

#else

	ZUnimplemented();
	return ZFile::errorNone;

#endif
	}

static ZFile::Error sWriteAt(int iFD, uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
#if ZCONFIG_File_AtAPISupported

	if (oCountWritten)
		*oCountWritten = 0;

	#if !defined(linux)
	if (sizeof(off_t) == 4 && iOffset > 0x7FFFFFFFL)
		return ZFile::errorGeneric;
	#endif

	const char* localSource = reinterpret_cast<const char*>(iSource);
	uint64 localOffset = iOffset;
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		#if defined(linux)
			ssize_t countWritten = ::pwrite64(iFD, localSource, countRemaining, localOffset);
		#else
			ssize_t countWritten = ::pwrite(iFD, localSource, countRemaining, localOffset);
		#endif

		if (countWritten < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return sTranslateError(err);
			}
		if (oCountWritten)
			*oCountWritten += countWritten;
		countRemaining -= countWritten;
		localSource += countWritten;
		localOffset += countWritten;
		}
	return ZFile::errorNone;

#else

	ZUnimplemented();
	return ZFile::errorNone;

#endif
	}

static ZFile::Error sGetPosition(int iFD, uint64& oPosition)
	{
	#if defined(linux)
		off64_t result = ::lseek64(iFD, 0, SEEK_CUR);
	#else
		off_t result = ::lseek(iFD, 0, SEEK_CUR);
	#endif

	if (result < 0)
		return sTranslateError(errno);
	oPosition = result;
	return ZFile::errorNone;
	}

static ZFile::Error sSetPosition(int iFD, uint64 iPosition)
	{
	#if defined(linux)
		if (::lseek64(iFD, iPosition, SEEK_SET) < 0)
			return sTranslateError(errno);
	#else
		if (::lseek(iFD, iPosition, SEEK_SET) < 0)
			return sTranslateError(errno);
	#endif

	return ZFile::errorNone;
	}

static ZFile::Error sGetSize(int iFD, uint64& oSize)
	{
	#if defined(linux)
		struct stat64 theStatStruct;
		if (::fstat64(iFD, &theStatStruct))
			return sTranslateError(errno);
	#else
		struct stat theStatStruct;
		if (::fstat(iFD, &theStatStruct))
			return sTranslateError(errno);
	#endif

	oSize = theStatStruct.st_size;
	return ZFile::errorNone;
	}

static ZFile::Error sSetSize(int iFD, uint64 iSize)
	{
	// NB ftruncate is not supported on all systems
	#if defined(linux)
		if (::ftruncate64(iFD, iSize) < 0)
			return sTranslateError(errno);
	#else
		if (::ftruncate(iFD, iSize) < 0)
			return sTranslateError(errno);
	#endif

	return ZFile::errorNone;
	}

static ZFile::Error sFlush(int iFD)
	{
	// AG 2001-08-02. Stumbled across the docs for fdatasync, which ensures the contents of a
	// file are flushed to disk, but does not necessarily ensure mod time is brought up to date.
	#if defined(linux)
		::fdatasync(iFD);
	#else
		::fsync(iFD);
	#endif

	return ZFile::errorNone;
	}

static ZFile::Error sFlushVolume(int iFD)
	{
	::fsync(iFD);
	return ZFile::errorNone;
	}

static void sSplit(char iSep, bool iIncludeEmpties, const char* iPath, vector<string>& oComps)
	{
	for (;;)
		{
		if (const char* nextSep = strchr(iPath, iSep))
			{
			size_t length = nextSep - iPath;
			if (iIncludeEmpties || length)
				oComps.push_back(string(iPath, length));
			iPath = nextSep + 1;
			}
		else
			{
			size_t length = ::strlen(iPath);
			if (iIncludeEmpties || length)
				oComps.push_back(string(iPath, length));
			break;
			}
		}
	}

static void sSplit(char iSep, bool iIncludeEmpties, const char* iPath, const char* iEnd, vector<string>& oComps)
	{
	for (;;)
		{
		if (const char* nextSep = strchr(iPath, iSep))
			{
			size_t length = nextSep - iPath;
			if (iIncludeEmpties || length)
				oComps.push_back(string(iPath, length));
			iPath = nextSep + 1;
			}
		else
			{
			size_t length = iEnd - iPath;
			if (iIncludeEmpties || length)
				oComps.push_back(string(iPath, length));
			break;
			}
		}
	}

static void sSplit(char iSep, bool iIncludeEmpties, const string& iPath, vector<string>& oComps)
	{
	if (size_t length = iPath.length())
		::sSplit(iSep, iIncludeEmpties, iPath.data(), iPath.data() + length, oComps);
	}

static void sGetCWD(vector<string>& oComps)
	{
	for (size_t bufSize = 1024; bufSize < 16384; bufSize *= 2)
		{
		vector<char> buffer(bufSize);
		if (::getcwd(&buffer[0], bufSize))
			{
			::sSplit('/', false, &buffer[0], oComps);
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * RealRep

namespace ZANONYMOUS {

class RealRep_POSIX : public ZFileIterRep_Std::RealRep
	{
public:
	RealRep_POSIX(ZRef<ZFileLoc_POSIX> iFileLoc);
	virtual ~RealRep_POSIX();

// From ZFileIterRep_Std::RealRep
	virtual bool HasValue(size_t iIndex);
	virtual ZFileSpec GetSpec(size_t iIndex);
	virtual string GetName(size_t iIndex);

private:
	ZMutex fMutex;
	ZRef<ZFileLoc_POSIX> fFileLoc;
	DIR* fDIR;
	vector<string> fNames;
	};

RealRep_POSIX::RealRep_POSIX(ZRef<ZFileLoc_POSIX> iFileLoc)
:	fFileLoc(iFileLoc)
	{
	if (fFileLoc)
		fDIR = ::opendir(fFileLoc->pGetPath().c_str());
	else
		fDIR = nil;
	}

RealRep_POSIX::~RealRep_POSIX()
	{
	if (fDIR)
		::closedir(fDIR);
	}

bool RealRep_POSIX::HasValue(size_t iIndex)
	{
	ZMutexLocker locker(fMutex);
	while (fDIR && iIndex >= fNames.size())
		{
		// We must use dirent64/readdir64 on linux if we want to be able to read directories on
		// NFS mounts. See the Linux-Kernel mailing list, 2001-05-08, message from Leon Bottou
		// (leonb@research.att.com) titled "Subtle NFS/VFS/GLIBC interaction bug"
		// <http://www.ussg.iu.edu/hypermail/linux/kernel/0103.1/0140.html>

		#if defined(linux)
			dirent64* theDirEnt = ::readdir64(fDIR);
		#else
			dirent* theDirEnt = ::readdir(fDIR);
		#endif

		if (theDirEnt == nil)
			{
			::closedir(fDIR);
			fDIR = nil;
			}
		else
			{
			if (theDirEnt->d_name[0] == '.')
				{
				if (theDirEnt->d_name[1] == '\0')
					{
					// Got a "."
					continue;
					}

				if (theDirEnt->d_name[1] == '.')
					{
					if (theDirEnt->d_name[2] == '\0')
						{
						// Got a ".."
						continue;
						}
					}
				}
			fNames.push_back(theDirEnt->d_name);
			}
		}

	return iIndex < fNames.size();
	}

ZFileSpec RealRep_POSIX::GetSpec(size_t iIndex)
	{
	ZMutexLocker locker(fMutex);
	ZAssertStop(kDebug_File_POSIX, iIndex < fNames.size());
	return ZFileSpec(fFileLoc, fNames[iIndex]);
	}

string RealRep_POSIX::GetName(size_t iIndex)
	{
	ZMutexLocker locker(fMutex);
	ZAssertStop(kDebug_File_POSIX, iIndex < fNames.size());
	return fNames[iIndex];
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZFileLoc_POSIX

ZRef<ZFileLoc_POSIX> ZFileLoc_POSIX::sGet_CWD()
	{
	return new ZFileLoc_POSIX(false);
	}

ZRef<ZFileLoc_POSIX> ZFileLoc_POSIX::sGet_Root()
	{
	return new ZFileLoc_POSIX(true);
	}

ZRef<ZFileLoc_POSIX> ZFileLoc_POSIX::sGet_App()
	{
	for (size_t bufSize = 1024; bufSize < 16384; bufSize *= 2)
		{
		vector<char> buffer(bufSize);
		int result = ::readlink("/proc/self/exe", &buffer[0], bufSize);
		if (result < 0)
			break;
		if (result < bufSize)
			{
			vector<string> comps;
			::sSplit('/', false, &buffer[0], &buffer[result], comps);
			return new ZFileLoc_POSIX(true, comps, true);
			}
		}

	// We've failed to extract the path to the executable from
	// /proc/self/exe. Try using argv[0], the current working
	// directory and entries in getenv("PATH").

	string name = ZMainNS::sArgV[0];

	if (name.size())
		{
		if (name[0] == '/')
			{
			// It starts with a separator and so is an absolute path.
			vector<string> comps;
			::sSplit('/', false, name, comps);
			return new ZFileLoc_POSIX(true, comps);
			}
		else if (string::npos != name.find("/"))
			{
			// It contains at least one separator and so should be interpreted
			// as a path relative to the current working directory.
			vector<string> comps;
			::sSplit('/', false, name, comps);
			return new ZFileLoc_POSIX(false, comps);
			}
		else
			{
			// It's an unadorned name, so we probe for a file
			// in directories listed in $PATH.
			if (const char* pEnv = ::getenv("PATH"))
				{
				vector<string> paths;
				::sSplit(':', true, pEnv, paths);

				for (vector<string>::const_iterator i = paths.begin(); i != paths.end(); ++i)
					{
					string trial = *i;
					if (trial.size())
						trial += '/' + name;
					else
						trial = name;

					struct stat theStat;
					if (0 <= ::stat(trial.c_str(), &theStat))
						{
						// Should we also check that it's executable? We'll need our
						// UID and GID to tell if the U and G X bits are applicable.
						if (S_ISREG(theStat.st_mode))
							{
							vector<string> comps;
							::sSplit('/', false, trial, comps);
							return new ZFileLoc_POSIX(trial[0] == '/', comps);
							}
						}
					}
				}
			}
		}

	return ZRef<ZFileLoc_POSIX>();
	}

ZFileLoc_POSIX::ZFileLoc_POSIX(bool iIsAtRoot)
:	fIsAtRoot(iIsAtRoot)
	{}

ZFileLoc_POSIX::ZFileLoc_POSIX(bool iIsAtRoot, const vector<string>& iComps)
:	fIsAtRoot(iIsAtRoot)
	{
	for (vector<string>::const_iterator i = iComps.begin(); i != iComps.end(); ++i)
		{
		if (size_t length = (*i).size())
			{
			if (length != 1 || (*i)[0] != '.')
				{
				// It's longer than one char, or that char is not a dot.
				fComps.push_back(*i);
				}
			}
		}
	}

ZFileLoc_POSIX::ZFileLoc_POSIX(bool iIsAtRoot, const string* iComps, size_t iCount)
:	fIsAtRoot(iIsAtRoot),
	fComps(iComps, iComps + iCount)
	{}

ZFileLoc_POSIX::ZFileLoc_POSIX(bool iIsAtRoot, vector<string>& ioComps, bool iKnowWhatImDoing)
:	fIsAtRoot(iIsAtRoot)
	{
	ZAssert(iKnowWhatImDoing);
	fComps.swap(ioComps);
	}

ZFileLoc_POSIX::~ZFileLoc_POSIX()
	{}

ZRef<ZFileIterRep> ZFileLoc_POSIX::CreateIterRep()
	{ return new ZFileIterRep_Std(new RealRep_POSIX(this), 0); }

string ZFileLoc_POSIX::GetName(ZFile::Error* oErr) const
	{
	if (oErr)
		*oErr = ZFile::errorNone;

	if (fComps.size())
		return fComps.back();
	return string();
	}

ZTrail ZFileLoc_POSIX::TrailTo(ZRef<ZFileLoc> iDest, ZFile::Error* oErr) const
	{
	if (oErr)
		*oErr = ZFile::errorNone;

	if (ZFileLoc_POSIX* dest = ZRefDynamicCast<ZFileLoc_POSIX>(iDest))
		{
		if (fIsAtRoot == dest->fIsAtRoot)
			return ZTrail::sTrailFromTo(fComps, dest->fComps);

		vector<string> theFullTrail;
		::sGetCWD(theFullTrail);
		if (fIsAtRoot)
			{
			theFullTrail.insert(theFullTrail.end(), dest->fComps.begin(), dest->fComps.end());
			return ZTrail::sTrailFromTo(fComps, theFullTrail);
			}

		theFullTrail.insert(theFullTrail.end(), fComps.begin(), fComps.end());
		return ZTrail::sTrailFromTo(theFullTrail, dest->fComps);
		}

	if (oErr)
		*oErr = ZFile::errorGeneric;
	return ZTrail(false);
	}

ZRef<ZFileLoc> ZFileLoc_POSIX::GetParent(ZFile::Error* oErr)
	{
	if (oErr)
		*oErr = ZFile::errorNone;

	if (fComps.size())
		{
		// We have components, so just bounce up.
		return new ZFileLoc_POSIX(fIsAtRoot, &fComps[0], fComps.size() - 1);
		}

	if (fIsAtRoot)
		{
		// We're at the root, our parent is nil.
		return ZRef<ZFileLoc>();
		}
	else
		{
		// We're empty and not at explicitly at the root, ie we reference the CWD. So
		// get the CWD as a list of components.
		vector<string> realComps;
		::sGetCWD(realComps);
		if (!realComps.empty())
			{
			// There's at least one component, so return the list minus the last component.
			realComps.pop_back();
			return new ZFileLoc_POSIX(true, realComps, true);
			}
		else
			{
			// The CWD is the root, and so our parent is nil.
			return ZRef<ZFileLoc>();
			}
		}
	}

ZRef<ZFileLoc> ZFileLoc_POSIX::GetDescendant(const string* iComps, size_t iCount, ZFile::Error* oErr)
	{
	if (oErr)
		*oErr = ZFile::errorNone;

	if (!iCount)
		return this;

	vector<string> newComps = fComps;
	newComps.insert(newComps.end(), iComps, iComps + iCount);
	return new ZFileLoc_POSIX(fIsAtRoot, newComps, true);
	}

bool ZFileLoc_POSIX::IsRoot()
	{ return fIsAtRoot && fComps.empty(); }

string ZFileLoc_POSIX::AsString_POSIX(const string* iComps, size_t iCount)
	{
	return ZFileLoc_POSIX::AsString_Native(iComps, iCount);
	}

string ZFileLoc_POSIX::AsString_Native(const string* iComps, size_t iCount)
	{
	string result;
	if (fComps.empty())
		{
		if (iCount)
			{
			if (!fIsAtRoot)
				result = ".";
			for (size_t x = 0; x < iCount; ++x)
				{
				result += "/";
				result += iComps[x];
				}
			}
		else
			{
			if (fIsAtRoot)
				result = "/";
			else
				result = ".";
			}
		}
	else
		{
		result = this->pGetPath();
		for (size_t x = 0; x < iCount; ++x)
			{
			result += "/";
			result += iComps[x];
			}
		}
	return result;
	}

ZFile::Kind ZFileLoc_POSIX::Kind(ZFile::Error* oErr)
	{
	struct stat theStat;

	// For now let's use stat() rather than lstat() so we can deal with
	// symlinked files and directories -ct 2002-04-19
#if 0
	if (0 > ::lstat(this->pGetPath().c_str(), &theStat))
#else
	if (0 > ::stat(this->pGetPath().c_str(), &theStat))
#endif
		{
		if (oErr)
			*oErr = sTranslateError(errno);
		return ZFile::kindNone;
		}

	if (oErr)
		*oErr = ZFile::errorNone;

	if (S_ISREG(theStat.st_mode))
		return ZFile::kindFile;

	if (S_ISDIR(theStat.st_mode))
		return ZFile::kindDir;

	#if 0
		// no need to check if entity is a symlink if we're stat-ing rather than lstat-ing -ct 2002-04-19
		if (S_ISLNK(theStat.st_mode))
			return ZFile::kindLink;
	#endif

	return ZFile::kindNone;
	}

uint64 ZFileLoc_POSIX::Size(ZFile::Error* oErr)
	{
	#if defined(linux)
		struct stat64 theStat;
		int result = ::stat64(this->pGetPath().c_str(), &theStat);
	#else
		struct stat theStat;
		int result = ::stat(this->pGetPath().c_str(), &theStat);
	#endif

	if (result < 0)
		{
		if (oErr)
			*oErr = sTranslateError(errno);
		return 0;
		}

	if (oErr)
		*oErr = ZFile::errorNone;

	return theStat.st_size;
	}

ZTime ZFileLoc_POSIX::TimeCreated(ZFile::Error* oErr)
	{
	struct stat theStat;
	if (0 > ::stat(this->pGetPath().c_str(), &theStat))
		{
		if (oErr)
			*oErr = sTranslateError(errno);
		return ZTime();
		}

	if (oErr)
		*oErr = ZFile::errorNone;

	#if __MACH__ && !defined(_POSIX_SOURCE)
		return ZTime(theStat.st_ctimespec.tv_sec + (theStat.st_ctimespec.tv_nsec / 1000000000.0));
	#elif defined(__USE_MISC) || defined(__sun__)
		return ZTime(theStat.st_ctim.tv_sec + (theStat.st_ctim.tv_nsec / 1000000000.0));
	#else
		return ZTime(theStat.st_ctime + (theStat.st_ctimensec / 1000000000.0));
	#endif
	}

ZTime ZFileLoc_POSIX::TimeModified(ZFile::Error* oErr)
	{
	struct stat theStat;
	if (0 > ::stat(this->pGetPath().c_str(), &theStat))
		{
		if (oErr)
			*oErr = sTranslateError(errno);
		return ZTime();
		}

	if (oErr)
		*oErr = ZFile::errorNone;

	#if __MACH__ && !defined(_POSIX_SOURCE)
		return ZTime(theStat.st_mtimespec.tv_sec + (theStat.st_mtimespec.tv_nsec / 1000000000.0));
	#elif defined(__USE_MISC) || defined(__sun__)
		return ZTime(theStat.st_mtim.tv_sec + (theStat.st_mtim.tv_nsec / 1000000000.0));
	#else
		return ZTime(theStat.st_mtime + (theStat.st_mtimensec / 1000000000.0));
	#endif
	}

ZRef<ZFileLoc> ZFileLoc_POSIX::CreateDir(ZFile::Error* oErr)
	{
	if (0 > ::mkdir(this->pGetPath().c_str(), 0777))
		{
		if (oErr)
			*oErr = ::sTranslateError(errno);
		return ZRef<ZFileLoc>();
		}

	if (oErr)
		*oErr = ZFile::errorNone;

	return this;
	}

ZRef<ZFileLoc> ZFileLoc_POSIX::MoveTo(ZRef<ZFileLoc> iDest, ZFile::Error* oErr)
	{
	ZFileLoc_POSIX* other = ZRefDynamicCast<ZFileLoc_POSIX>(iDest);
	if (!other)
		{
		if (oErr)
			*oErr = ZFile::errorGeneric;
		return ZRef<ZFileLoc>();
		}

	if (0 > ::rename(this->pGetPath().c_str(), other->pGetPath().c_str()))
		{
		if (oErr)
			*oErr = ::sTranslateError(errno);
		return ZRef<ZFileLoc>();
		}

	return other;
	}

bool ZFileLoc_POSIX::Delete(ZFile::Error* oErr)
	{
	string myPath = this->pGetPath();
	// Assume it's a file first
	if (0 > ::unlink(myPath.c_str()))
		{
		if (ENOENT == errno)
			{
			if (oErr)
				*oErr = ::sTranslateError(ENOENT);
			return false;
			}

		// Try it as if it's a directory
		if (0 > ::rmdir(myPath.c_str()))
			{
			if (oErr)
				*oErr = ::sTranslateError(errno);
			return false;
			}
		}

	if (oErr)
		*oErr = ZFile::errorNone;
	return true;
	}

ZRef<ZStreamerRPos> ZFileLoc_POSIX::OpenRPos(bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sOpen(this->pGetPath().c_str(), true, false, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZStreamerRPos>();

	return new ZStreamerRWPos_File_POSIX(theFD, true);
	}

ZRef<ZStreamerWPos> ZFileLoc_POSIX::OpenWPos(bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sOpen(this->pGetPath().c_str(), false, true, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZStreamerWPos>();

	return new ZStreamerWPos_File_POSIX(theFD, true);
	}

ZRef<ZStreamerRWPos> ZFileLoc_POSIX::OpenRWPos(bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sOpen(this->pGetPath().c_str(), true, true, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZStreamerRWPos>();

	return new ZStreamerRWPos_File_POSIX(theFD, true);
	}

ZRef<ZStreamerWPos> ZFileLoc_POSIX::CreateWPos(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sCreate(this->pGetPath().c_str(), iOpenExisting, false, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZStreamerWPos>();

	return new ZStreamerWPos_File_POSIX(theFD, true);
	}

ZRef<ZStreamerRWPos> ZFileLoc_POSIX::CreateRWPos(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sCreate(this->pGetPath().c_str(), iOpenExisting, true, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZStreamerRWPos>();

	return new ZStreamerRWPos_File_POSIX(theFD, true);
	}

ZRef<ZFileR> ZFileLoc_POSIX::OpenFileR(bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sOpen(this->pGetPath().c_str(), true, false, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZFileR>();

	#if ZCONFIG_File_AtAPISupported
		return new ZFileR_POSIX(theFD, true);
	#else
		return new ZFileR_POSIXMutex(theFD, true);
	#endif
	}

ZRef<ZFileW> ZFileLoc_POSIX::OpenFileW(bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sOpen(this->pGetPath().c_str(), false, true, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZFileW>();

	#if ZCONFIG_File_AtAPISupported
		return new ZFileW_POSIX(theFD, true);
	#else
		return new ZFileW_POSIXMutex(theFD, true);
	#endif
	}

ZRef<ZFileRW> ZFileLoc_POSIX::OpenFileRW(bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sOpen(this->pGetPath().c_str(), true, true, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZFileRW>();

	#if ZCONFIG_File_AtAPISupported
		return new ZFileRW_POSIX(theFD, true);
	#else
		return new ZFileRW_POSIXMutex(theFD, true);
	#endif
	}

ZRef<ZFileW> ZFileLoc_POSIX::CreateFileW(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sCreate(this->pGetPath().c_str(), iOpenExisting, false, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZFileW>();

	#if ZCONFIG_File_AtAPISupported
		return new ZFileW_POSIX(theFD, true);
	#else
		return new ZFileW_POSIXMutex(theFD, true);
	#endif
	}

ZRef<ZFileRW> ZFileLoc_POSIX::CreateFileRW(bool iOpenExisting, bool iPreventWriters, ZFile::Error* oErr)
	{
	int theFD = ::sCreate(this->pGetPath().c_str(), iOpenExisting, true, iPreventWriters, oErr);
	if (theFD < 0)
		return ZRef<ZFileRW>();

	#if ZCONFIG_File_AtAPISupported
		return new ZFileRW_POSIX(theFD, true);
	#else
		return new ZFileRW_POSIXMutex(theFD, true);
	#endif
	}

string ZFileLoc_POSIX::pGetPath()
	{
	if (fComps.empty())
		{
		if (fIsAtRoot)
			return "/";
		return ".";
		}

	string result;
	if (!fIsAtRoot)
		result = ".";
	for (size_t x = 0; x < fComps.size(); ++x)
		{
		result += "/";
		result += fComps[x];
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR_POSIX

ZFileR_POSIX::ZFileR_POSIX(int iFD, bool iCloseWhenFinalized)
:	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZFileR_POSIX::~ZFileR_POSIX()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

ZFile::Error ZFileR_POSIX::ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{ return ::sReadAt(fFD, iOffset, iDest, iCount, oCountRead); }

ZFile::Error ZFileR_POSIX::GetSize(uint64& oSize)
	{ return ::sGetSize(fFD, oSize); }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW_POSIX

ZFileW_POSIX::ZFileW_POSIX(int iFD, bool iCloseWhenFinalized)
:	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZFileW_POSIX::~ZFileW_POSIX()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

ZFile::Error ZFileW_POSIX::WriteAt(uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{ return ::sWriteAt(fFD, iOffset, iSource, iCount, oCountWritten); }

ZFile::Error ZFileW_POSIX::GetSize(uint64& oSize)
	{ return ::sGetSize(fFD, oSize); }

ZFile::Error ZFileW_POSIX::SetSize(uint64 iSize)
	{ return ::sSetSize(fFD, iSize); }

ZFile::Error ZFileW_POSIX::Flush()
	{ return ::sFlush(fFD); }

ZFile::Error ZFileW_POSIX::FlushVolume()
	{ return ::sFlushVolume(fFD); }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW_POSIX

ZFileRW_POSIX::ZFileRW_POSIX(int iFD, bool iCloseWhenFinalized)
:	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZFileRW_POSIX::~ZFileRW_POSIX()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

ZFile::Error ZFileRW_POSIX::ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{ return ::sReadAt(fFD, iOffset, iDest, iCount, oCountRead); }

ZFile::Error ZFileRW_POSIX::WriteAt(uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{ return ::sWriteAt(fFD, iOffset, iSource, iCount, oCountWritten); }

ZFile::Error ZFileRW_POSIX::GetSize(uint64& oSize)
	{ return ::sGetSize(fFD, oSize); }

ZFile::Error ZFileRW_POSIX::SetSize(uint64 iSize)
	{ return ::sSetSize(fFD, iSize); }

ZFile::Error ZFileRW_POSIX::Flush()
	{ return ::sFlush(fFD); }

ZFile::Error ZFileRW_POSIX::FlushVolume()
	{ return ::sFlushVolume(fFD); }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileR_POSIXMutex

ZFileR_POSIXMutex::ZFileR_POSIXMutex(int iFD, bool iCloseWhenFinalized)
:	fPosition(uint64(-1)),
	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZFileR_POSIXMutex::~ZFileR_POSIXMutex()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

ZFile::Error ZFileR_POSIXMutex::ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{
	ZMutexLocker locker(fMutex);
	if (fPosition != iOffset)
		{
		ZFile::Error err = ::sSetPosition(fFD, iOffset);
		if (err != ZFile::errorNone)
			{
			fPosition = uint64(-1);
			return err;
			}
		fPosition = iOffset;
		}
	return ::sRead(fFD, iDest, iCount, oCountRead);
	}

ZFile::Error ZFileR_POSIXMutex::GetSize(uint64& oSize)
	{ return ::sGetSize(fFD, oSize); }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileW_POSIXMutex

ZFileW_POSIXMutex::ZFileW_POSIXMutex(int iFD, bool iCloseWhenFinalized)
:	fPosition(uint64(-1)),
	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZFileW_POSIXMutex::~ZFileW_POSIXMutex()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

ZFile::Error ZFileW_POSIXMutex::WriteAt(uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZMutexLocker locker(fMutex);
	if (fPosition != iOffset)
		{
		ZFile::Error err = ::sSetPosition(fFD, iOffset);
		if (err != ZFile::errorNone)
			{
			fPosition = uint64(-1);
			return err;
			}
		fPosition = iOffset;
		}
	return ::sWrite(fFD, iSource, iCount, oCountWritten);
	}

ZFile::Error ZFileW_POSIXMutex::GetSize(uint64& oSize)
	{ return ::sGetSize(fFD, oSize); }

ZFile::Error ZFileW_POSIXMutex::SetSize(uint64 iSize)
	{ return ::sSetSize(fFD, iSize); }

ZFile::Error ZFileW_POSIXMutex::Flush()
	{ return ::sFlush(fFD); }

ZFile::Error ZFileW_POSIXMutex::FlushVolume()
	{ return ::sFlushVolume(fFD); }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileRW_POSIXMutex

ZFileRW_POSIXMutex::ZFileRW_POSIXMutex(int iFD, bool iCloseWhenFinalized)
:	fPosition(uint64(-1)),
	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZFileRW_POSIXMutex::~ZFileRW_POSIXMutex()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

ZFile::Error ZFileRW_POSIXMutex::ReadAt(uint64 iOffset, void* iDest, size_t iCount, size_t* oCountRead)
	{
	ZMutexLocker locker(fMutex);
	if (fPosition != iOffset)
		{
		ZFile::Error err = ::sSetPosition(fFD, iOffset);
		if (err != ZFile::errorNone)
			{
			fPosition = uint64(-1);
			return err;
			}
		fPosition = iOffset;
		}
	return ::sRead(fFD, iDest, iCount, oCountRead);
	}

ZFile::Error ZFileRW_POSIXMutex::WriteAt(uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZMutexLocker locker(fMutex);
	if (fPosition != iOffset)
		{
		ZFile::Error err = ::sSetPosition(fFD, iOffset);
		if (err != ZFile::errorNone)
			{
			fPosition = uint64(-1);
			return err;
			}
		fPosition = iOffset;
		}
	return ::sWrite(fFD, iSource, iCount, oCountWritten);
	}

ZFile::Error ZFileRW_POSIXMutex::GetSize(uint64& oSize)
	{ return ::sGetSize(fFD, oSize); }

ZFile::Error ZFileRW_POSIXMutex::SetSize(uint64 iSize)
	{ return ::sSetSize(fFD, iSize); }

ZFile::Error ZFileRW_POSIXMutex::Flush()
	{ return ::sFlush(fFD); }

ZFile::Error ZFileRW_POSIXMutex::FlushVolume()
	{ return ::sFlushVolume(fFD); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_File_POSIX

ZStreamRPos_File_POSIX::ZStreamRPos_File_POSIX(int iFD, bool iCloseWhenFinalized)
:	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZStreamRPos_File_POSIX::~ZStreamRPos_File_POSIX()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

void ZStreamRPos_File_POSIX::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ ::sRead(fFD, iDest, iCount, oCountRead); }

uint64 ZStreamRPos_File_POSIX::Imp_GetPosition()
	{
	uint64 pos;
	if (ZFile::errorNone == ::sGetPosition(fFD, pos))
		return pos;
	return 0;
	}

void ZStreamRPos_File_POSIX::Imp_SetPosition(uint64 iPosition)
	{ ::sSetPosition(fFD, iPosition); }

uint64 ZStreamRPos_File_POSIX::Imp_GetSize()
	{
	uint64 theSize;
	if (ZFile::errorNone == ::sGetSize(fFD, theSize))
		return theSize;
	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_File_POSIX

ZStreamerRPos_File_POSIX::ZStreamerRPos_File_POSIX(int iFD, bool iCloseWhenFinalized)
:	fStream(iFD, iCloseWhenFinalized)
	{}

ZStreamerRPos_File_POSIX::~ZStreamerRPos_File_POSIX()
	{}

const ZStreamRPos& ZStreamerRPos_File_POSIX::GetStreamRPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_File_POSIX

ZStreamWPos_File_POSIX::ZStreamWPos_File_POSIX(int iFD, bool iCloseWhenFinalized)
:	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZStreamWPos_File_POSIX::~ZStreamWPos_File_POSIX()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

void ZStreamWPos_File_POSIX::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ ::sWrite(fFD, iSource, iCount, oCountWritten); }

void ZStreamWPos_File_POSIX::Imp_Flush()
	{ ::sFlush(fFD); }

uint64 ZStreamWPos_File_POSIX::Imp_GetPosition()
	{
	uint64 pos;
	if (ZFile::errorNone == ::sGetPosition(fFD, pos))
		return pos;
	return 0;
	}

void ZStreamWPos_File_POSIX::Imp_SetPosition(uint64 iPosition)
	{ ::sSetPosition(fFD, iPosition); }

uint64 ZStreamWPos_File_POSIX::Imp_GetSize()
	{
	uint64 theSize;
	if (ZFile::errorNone == ::sGetSize(fFD, theSize))
		return theSize;
	return 0;
	}

void ZStreamWPos_File_POSIX::Imp_SetSize(uint64 iSize)
	{
	if (ZFile::errorNone != ::sSetSize(fFD, iSize))
		sThrowBadSize();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_File_POSIX

ZStreamerWPos_File_POSIX::ZStreamerWPos_File_POSIX(int iFD, bool iCloseWhenFinalized)
:	fStream(iFD, iCloseWhenFinalized)
	{}

ZStreamerWPos_File_POSIX::~ZStreamerWPos_File_POSIX()
	{}

const ZStreamWPos& ZStreamerWPos_File_POSIX::GetStreamWPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_File_POSIX

ZStreamRWPos_File_POSIX::ZStreamRWPos_File_POSIX(int iFD, bool iCloseWhenFinalized)
:	fFD(iFD),
	fCloseWhenFinalized(iCloseWhenFinalized)
	{}

ZStreamRWPos_File_POSIX::~ZStreamRWPos_File_POSIX()
	{
	if (fCloseWhenFinalized)
		::sClose(fFD);
	}

void ZStreamRWPos_File_POSIX::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{ ::sRead(fFD, iDest, iCount, oCountRead); }

void ZStreamRWPos_File_POSIX::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{ ::sWrite(fFD, iSource, iCount, oCountWritten); }

void ZStreamRWPos_File_POSIX::Imp_Flush()
	{ ::sFlush(fFD); }

uint64 ZStreamRWPos_File_POSIX::Imp_GetPosition()
	{
	uint64 pos;
	if (ZFile::errorNone == ::sGetPosition(fFD, pos))
		return pos;
	return 0;
	}

void ZStreamRWPos_File_POSIX::Imp_SetPosition(uint64 iPosition)
	{ ::sSetPosition(fFD, iPosition); }

uint64 ZStreamRWPos_File_POSIX::Imp_GetSize()
	{
	uint64 theSize;
	if (ZFile::errorNone == ::sGetSize(fFD, theSize))
		return theSize;
	return 0;
	}

void ZStreamRWPos_File_POSIX::Imp_SetSize(uint64 iSize)
	{
	if (ZFile::errorNone != ::sSetSize(fFD, iSize))
		sThrowBadSize();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_File_POSIX

ZStreamerRWPos_File_POSIX::ZStreamerRWPos_File_POSIX(int iFD, bool iCloseWhenFinalized)
:	fStream(iFD, iCloseWhenFinalized)
	{}

ZStreamerRWPos_File_POSIX::~ZStreamerRWPos_File_POSIX()
	{}

const ZStreamRWPos& ZStreamerRWPos_File_POSIX::GetStreamRWPos()
	{ return fStream; }

#endif // ZCONFIG_API_Enabled(File_POSIX)
