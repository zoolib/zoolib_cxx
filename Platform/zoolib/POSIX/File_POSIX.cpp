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
	#define _LARGEFILE64_SOURCE
#endif

#include "zoolib/POSIX/File_POSIX.h"

#if ZCONFIG_API_Enabled(File_POSIX)

#if ZCONFIG_SPI_Enabled(BeOS) || defined (__ANDROID__)
	#define ZCONFIG_File_AtAPISupported 0
#else
	#define ZCONFIG_File_AtAPISupported 1
#endif

#include "zoolib/ZDebug.h"

#include <cstring>
#include <vector>

#define kDebug_File_POSIX 2

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if ZCONFIG_SPI_Enabled(Linux)
	#include <stdio.h>
#elif ZCONFIG_SPI_Enabled(MacOSX) || ZCONFIG_SPI_Enabled(iPhone)
	#include <mach-o/dyld.h>
#else
	#include "zoolib/ZMain.h" // For ZMainNS::sArgV
#endif

#if (!__MWERKS__)
	using std::strchr;
#endif
using std::string;
using std::vector;

ZMACRO_MSVCStaticLib_cpp(File_POSIX)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Factory functions

namespace { // anonymous

class Make_FileLoc
:	public FunctionChain<ZRef<FileLoc>, FileLoc::ELoc>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		switch (iParam)
			{
			case FileLoc::eLoc_Root: oResult = FileLoc_POSIX::sGet_Root(); return true;
			case FileLoc::eLoc_CWD: oResult = FileLoc_POSIX::sGet_CWD(); return true;
			case FileLoc::eLoc_App: oResult = FileLoc_POSIX::sGet_App(); return true;
			}
		return false;
		}
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Shared implementation details

static File::Error spTranslateError(int iNativeError)
	{
	File::Error theErr = File::errorGeneric;
	switch (iNativeError)
		{
		case 0:
			// We shouldn't call this if there's no error
			ZUnimplemented();
			break;
		case EEXIST:
			theErr = File::errorAlreadyExists;
			break;
		case ENAMETOOLONG:
			theErr = File::errorIllegalFileName;
			break;
		case EAGAIN:
		case EACCES:
			theErr = File::errorNoPermission;
			break;
		case ENOENT:
			theErr = File::errorDoesntExist;
			break;
		case ENOTDIR:
			theErr = File::errorWrongTypeForOperation;
			break;
		case EISDIR:
			theErr = File::errorWrongTypeForOperation;
			break;
		case EPERM:
			theErr = File::errorNoPermission;
			break;
		}
	return theErr;
	}

static int spFCntl(int iFD, int iCmd, struct flock& ioFLock)
	{
	for (;;)
		{
		if (-1 != ::fcntl(iFD, iCmd, &ioFLock))
			return 0;

		if (errno != EINTR)
			return errno;
		}
	}

static int spLockOrClose(int iFD, bool iRead, bool iWrite, bool iPreventWriters, File::Error* oErr)
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
		err = spFCntl(iFD, F_SETLK, theFLock);
		if (err == 0)
			return iFD;
		}
	else
		{
		err = spFCntl(iFD, F_GETLK, theFLock);

		if (err == ENOLCK || err == EACCES || err == ENOTSUP || err == EINVAL)
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
		*oErr = spTranslateError(err);
	return -1;
	}

static int spOpen(const char* iPath,
	bool iRead, bool iWrite, bool iPreventWriters, File::Error* oErr)
	{
	#if defined(linux) || defined(__linux__)
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
			*oErr = spTranslateError(errno);
		return -1;
		}

	return spLockOrClose(theFD, iRead, iWrite, iPreventWriters, oErr);
	}

static void spClose(int iFD)
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

static int spCreate(const char* iPath,
	bool iOpenExisting, bool iAllowRead, bool iPreventWriters, File::Error* oErr)
	{
	#if defined(linux) || defined(__linux__)
		int flags = O_CREAT | O_NOCTTY | O_LARGEFILE;
	#else
		int flags = O_CREAT | O_NOCTTY;
	#endif

	if (iAllowRead)
		flags |= O_RDWR;
	else
		flags |= O_WRONLY;

	if (not iOpenExisting)
		flags |= O_EXCL;

	int theFD = ::open(iPath, flags, 0666);

	if (theFD < 0)
		{
		if (oErr)
			*oErr = spTranslateError(errno);
		return -1;
		}

	return spLockOrClose(theFD, iAllowRead, true, iPreventWriters, oErr);
	}

static File::Error spRead(int iFD, void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	char* localDest = reinterpret_cast<char*>(oDest);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		ssize_t countRead = ::read(iFD, localDest, countRemaining);

		if (countRead == 0)
			return File::errorReadPastEOF;

		if (countRead < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return spTranslateError(err);
			}
		if (oCountRead)
			*oCountRead += countRead;
		countRemaining -= countRead;
		localDest += countRead;
		}
	return File::errorNone;
	}

static File::Error spWrite(int iFD, const void* iSource, size_t iCount, size_t* oCountWritten)
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
			return spTranslateError(err);
			}
		if (oCountWritten)
			*oCountWritten += countWritten;
		countRemaining -= countWritten;
		localSource += countWritten;
		}
	return File::errorNone;
	}

static File::Error spReadAt(
	int iFD, uint64 iOffset, void* oDest, size_t iCount, size_t* oCountRead)
	{
#if ZCONFIG_File_AtAPISupported

	if (oCountRead)
		*oCountRead = 0;

	#if !defined(linux) || defined(__linux__)
	if (sizeof(off_t) == 4 && iOffset > 0x7FFFFFFFL)
		return File::errorGeneric;
	#endif

	char* localDest = reinterpret_cast<char*>(oDest);
	uint64 localOffset = iOffset;
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
			ssize_t countRead = ::pread64(iFD, localDest, countRemaining, localOffset);
		#else
			ssize_t countRead = ::pread(iFD, localDest, countRemaining, localOffset);
		#endif

		if (countRead == 0)
			return File::errorReadPastEOF;

		if (countRead < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return spTranslateError(err);
			}
		if (oCountRead)
			*oCountRead += countRead;
		countRemaining -= countRead;
		localDest += countRead;
		localOffset += countRead;
		}
	return File::errorNone;

#else

	ZUnimplemented();
	return File::errorNone;

#endif
	}

static File::Error spWriteAt(int iFD,
	uint64 iOffset, const void* iSource, size_t iCount, size_t* oCountWritten)
	{
#if ZCONFIG_File_AtAPISupported

	if (oCountWritten)
		*oCountWritten = 0;

	#if !(defined(linux) || defined(__linux__))
	if (sizeof(off_t) == 4 && iOffset > 0x7FFFFFFFL)
		return File::errorGeneric;
	#endif

	const char* localSource = reinterpret_cast<const char*>(iSource);
	uint64 localOffset = iOffset;
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
			ssize_t countWritten = ::pwrite64(iFD, localSource, countRemaining, localOffset);
		#else
			ssize_t countWritten = ::pwrite(iFD, localSource, countRemaining, localOffset);
		#endif

		if (countWritten < 0)
			{
			int err = errno;
			if (err == EINTR)
				continue;
			return spTranslateError(err);
			}
		if (oCountWritten)
			*oCountWritten += countWritten;
		countRemaining -= countWritten;
		localSource += countWritten;
		localOffset += countWritten;
		}
	return File::errorNone;

#else

	ZUnimplemented();
	return File::errorNone;

#endif
	}

static File::Error spGetPosition(int iFD, uint64& oPosition)
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		off64_t result = ::lseek64(iFD, 0, SEEK_CUR);
	#else
		off_t result = ::lseek(iFD, 0, SEEK_CUR);
	#endif

	if (result < 0)
		{
		oPosition = 0;
		return spTranslateError(errno);
		}
	oPosition = result;
	return File::errorNone;
	}

static File::Error spSetPosition(int iFD, uint64 iPosition)
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		if (::lseek64(iFD, iPosition, SEEK_SET) < 0)
			return spTranslateError(errno);
	#else
		if (::lseek(iFD, iPosition, SEEK_SET) < 0)
			return spTranslateError(errno);
	#endif

	return File::errorNone;
	}

static File::Error spGetSize(int iFD, uint64& oSize)
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		struct stat64 theStatStruct;
		if (::fstat64(iFD, &theStatStruct))
			{
			oSize = 0;
			return spTranslateError(errno);
			}
	#else
		struct stat theStatStruct;
		if (::fstat(iFD, &theStatStruct))
			{
			oSize = 0;
			return spTranslateError(errno);
			}
	#endif

	oSize = theStatStruct.st_size;
	return File::errorNone;
	}

static File::Error spSetSize(int iFD, uint64 iSize)
	{
	// NB ftruncate is not supported on all systems
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		if (::ftruncate64(iFD, iSize) < 0)
			return spTranslateError(errno);
	#else
		if (::ftruncate(iFD, iSize) < 0)
			return spTranslateError(errno);
	#endif

	return File::errorNone;
	}

static File::Error spFlush(int iFD)
	{
	// AG 2001-08-02. Stumbled across the docs for fdatasync, which ensures the contents of a
	// file are flushed to disk, but does not necessarily ensure mod time is brought up to date.
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		::fdatasync(iFD);
	#else
		::fsync(iFD);
	#endif

	return File::errorNone;
	}

static File::Error spFlushVolume(int iFD)
	{
	::fsync(iFD);
	return File::errorNone;
	}

static void spSplit(
	char iSep, bool iIncludeEmpties, const char* iPath, const char* iEnd,
	vector<string>& oComps)
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

static void spSplit(char iSep, bool iIncludeEmpties, const char* iPath, vector<string>& oComps)
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

static void spGetCWD(vector<string8>& oComps)
	{
	for (size_t bufSize = 1024; bufSize < 16384; bufSize *= 2)
		{
		vector<char> buffer(bufSize);
		if (::getcwd(&buffer[0], bufSize))
			{
			spSplit('/', false, &buffer[0], oComps);
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark RealRep

namespace { // anonymous

class RealRep_POSIX : public FileIterRep_Std::RealRep
	{
public:
	RealRep_POSIX(ZRef<FileLoc_POSIX> iFileLoc);
	virtual ~RealRep_POSIX();

// From FileIterRep_Std::RealRep
	virtual bool HasValue(size_t iIndex);
	virtual FileSpec GetSpec(size_t iIndex);
	virtual string GetName(size_t iIndex);

private:
	ZMtx fMtx;
	ZRef<FileLoc_POSIX> fFileLoc;
	DIR* fDIR;
	vector<string> fNames;
	};

RealRep_POSIX::RealRep_POSIX(ZRef<FileLoc_POSIX> iFileLoc)
:	fFileLoc(iFileLoc)
	{
	if (fFileLoc)
		fDIR = ::opendir(fFileLoc->pGetPath().c_str());
	else
		fDIR = nullptr;
	}

RealRep_POSIX::~RealRep_POSIX()
	{
	if (fDIR)
		::closedir(fDIR);
	}

bool RealRep_POSIX::HasValue(size_t iIndex)
	{
	ZAcqMtx locker(fMtx);
	while (fDIR && iIndex >= fNames.size())
		{
		// We must use dirent64/readdir64 on linux if we want to be able to read directories on
		// NFS mounts. See the Linux-Kernel mailing list, 2001-05-08, message from Leon Bottou
		// (leonb@research.att.com) titled "Subtle NFS/VFS/GLIBC interaction bug"
		// <http://www.ussg.iu.edu/hypermail/linux/kernel/0103.1/0140.html>

		#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
			dirent64* theDirEnt = ::readdir64(fDIR);
		#else
			dirent* theDirEnt = ::readdir(fDIR);
		#endif

		if (theDirEnt == nullptr)
			{
			::closedir(fDIR);
			fDIR = nullptr;
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

FileSpec RealRep_POSIX::GetSpec(size_t iIndex)
	{
	ZAcqMtx locker(fMtx);
	ZAssertStop(kDebug_File_POSIX, iIndex < fNames.size());
	return FileSpec(fFileLoc, fNames[iIndex]);
	}

string RealRep_POSIX::GetName(size_t iIndex)
	{
	ZAcqMtx locker(fMtx);
	ZAssertStop(kDebug_File_POSIX, iIndex < fNames.size());
	return fNames[iIndex];
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark FileLoc_POSIX

ZRef<FileLoc_POSIX> FileLoc_POSIX::sGet_CWD()
	{ return new FileLoc_POSIX(false); }

ZRef<FileLoc_POSIX> FileLoc_POSIX::sGet_Root()
	{ return new FileLoc_POSIX(true); }

#if ZCONFIG_SPI_Enabled(Linux)

ZRef<FileLoc_POSIX> FileLoc_POSIX::sGet_App()
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
			spSplit('/', false, &buffer[0], &buffer[result], comps);
			return new FileLoc_POSIX(true, &comps, IKnowWhatIAmDoing);
			}
		}
	return null;
	}

#elif ZCONFIG_SPI_Enabled(MacOSX)

// From <http://www.oroboro.com/rafael/docserv.php/article/news/entry/52/num_entries/1>

ZRef<FileLoc_POSIX> FileLoc_POSIX::sGet_App()
	{
	// Prior to 10.4 _NSGetExecutablePath took a pointer to
	// unsigned long, later headers specify a uint32_t.
	#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
		unsigned long theSize = 0;
	#else
		uint32_t theSize = 0;
	#endif

	::_NSGetExecutablePath(nullptr, &theSize);

	vector<char> buffer(theSize);
	if (0 == ::_NSGetExecutablePath(&buffer[0], &theSize))
		{
		vector<string> comps;
		// Pass theSize - 1 to omit the terminating NUL.
		spSplit('/', false, &buffer[0], &buffer[0] + theSize - 1, comps);
		return new FileLoc_POSIX(true, &comps, IKnowWhatIAmDoing);
		}
	return null;
	}

#elif ZCONFIG_SPI_Enabled(iPhone)

ZRef<FileLoc_POSIX> FileLoc_POSIX::sGet_App()
	{ return null; }

#else

// This will require that we're built as part of a real application.

ZRef<FileLoc_POSIX> FileLoc_POSIX::sGet_App()
	{
	if (ZMainNS::sArgC > 0)
		{
		const string name = ZMainNS::sArgV[0];

		if (name.size())
			{
			if (name[0] == '/')
				{
				// It starts with a separator and so is an absolute path.
				vector<string> comps;
				spSplit('/', false, name, comps);
				return new FileLoc_POSIX(true, &comps, IKnowWhatIAmDoing);
				}
			else if (string::npos != name.find("/"))
				{
				// It contains at least one separator and so should be interpreted
				// as a path relative to the current working directory.
				vector<string> comps;
				spSplit('/', false, name, comps);
				return new FileLoc_POSIX(false, &comps, IKnowWhatIAmDoing);
				}
			else
				{
				// It's an unadorned name, so we probe for a file
				// in directories listed in $PATH.
				if (const char* pEnv = ::getenv("PATH"))
					{
					vector<string> paths;
					spSplit(':', true, pEnv, paths);

					for (vector<string>::const_iterator ii = paths.begin(); ii != paths.end(); ++ii)
						{
						string trial = *ii;
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
								spSplit('/', false, trial, comps);
								return new FileLoc_POSIX(trial[0] == '/', &comps, IKnowWhatIAmDoing);
								}
							}
						}
					}
				}
			}
		}

	return null;
	}

#endif

FileLoc_POSIX::FileLoc_POSIX(bool iIsAtRoot)
:	fIsAtRoot(iIsAtRoot)
	{}

FileLoc_POSIX::FileLoc_POSIX(bool iIsAtRoot, const vector<string>& iComps)
:	fIsAtRoot(iIsAtRoot)
	{
	for (vector<string>::const_iterator ii = iComps.begin(); ii != iComps.end(); ++ii)
		{
		if (size_t length = ii->size())
			{
			if (length != 1 || (*ii)[0] != '.')
				{
				// It's longer than one char, or that char is not a dot.
				fComps.push_back(*ii);
				}
			}
		}
	}

FileLoc_POSIX::FileLoc_POSIX(bool iIsAtRoot, const string* iComps, size_t iCount)
:	fIsAtRoot(iIsAtRoot),
	fComps(iComps, iComps + iCount)
	{}

FileLoc_POSIX::FileLoc_POSIX(bool iIsAtRoot, vector<string>* ioComps, const IKnowWhatIAmDoing_t&)
:	fIsAtRoot(iIsAtRoot)
	{
	ioComps->swap(fComps);
	}

FileLoc_POSIX::~FileLoc_POSIX()
	{}

ZRef<FileIterRep> FileLoc_POSIX::CreateIterRep()
	{ return new FileIterRep_Std(new RealRep_POSIX(this), 0); }

string FileLoc_POSIX::GetName() const
	{
	if (fComps.size())
		return fComps.back();
	return string();
	}

ZQ<Trail> FileLoc_POSIX::TrailTo(ZRef<FileLoc> oDest) const
	{
	if (FileLoc_POSIX* dest = oDest.DynamicCast<FileLoc_POSIX>())
		{
		if (fIsAtRoot == dest->fIsAtRoot)
			return sTrailFromTo(fComps, dest->fComps);

		vector<string8> theFullTrail;
		spGetCWD(theFullTrail);
		if (fIsAtRoot)
			{
			theFullTrail.insert(theFullTrail.end(), dest->fComps.begin(), dest->fComps.end());
			return sTrailFromTo(fComps, theFullTrail);
			}

		theFullTrail.insert(theFullTrail.end(), fComps.begin(), fComps.end());
		return sTrailFromTo(theFullTrail, dest->fComps);
		}

	return null;
	}

ZRef<FileLoc> FileLoc_POSIX::GetParent()
	{
	if (fComps.size())
		{
		// We have components, so just bounce up.
		return new FileLoc_POSIX(fIsAtRoot, &fComps[0], fComps.size() - 1);
		}

	if (fIsAtRoot)
		{
		// We're at the root, our parent is nil.
		return null;
		}
	else
		{
		// We're empty and not at explicitly at the root, ie we reference the CWD. So
		// get the CWD as a list of components.
		vector<string> realComps;
		spGetCWD(realComps);
		if (not realComps.empty())
			{
			// There's at least one component, so return the list minus the last component.
			realComps.pop_back();
			return new FileLoc_POSIX(true, &realComps, IKnowWhatIAmDoing);
			}
		else
			{
			// The CWD is the root, and so our parent is nil.
			return null;
			}
		}
	}

ZRef<FileLoc> FileLoc_POSIX::GetDescendant(
	const string* iComps, size_t iCount)
	{
	if (not iCount)
		return this;

	vector<string> newComps = fComps;
	newComps.insert(newComps.end(), iComps, iComps + iCount);
	return new FileLoc_POSIX(fIsAtRoot, &newComps, IKnowWhatIAmDoing);
	}

bool FileLoc_POSIX::IsRoot()
	{ return fIsAtRoot && fComps.empty(); }

ZRef<FileLoc> FileLoc_POSIX::Follow()
	{
	struct stat theStat;
	if (0 > ::lstat(this->pGetPath().c_str(), &theStat))
		return null;

	// Need to interpret the link content somewhat, to deal with rel-paths etc.
	if (S_ISLNK(theStat.st_mode))
		{
		char buf[PATH_MAX];
		ssize_t len = ::readlink(this->pGetPath().c_str(), &buf[0], countof(buf));
		if (len < 0)
			return null;

		vector<string> comps;
		spSplit('/', false, &buf[0], &buf[len], comps);
		if (buf[0] == '/')
			return new FileLoc_POSIX(true, &comps, IKnowWhatIAmDoing);

		comps.insert(comps.begin(), fComps.begin(), fComps.end() - 1);
		return new FileLoc_POSIX(fIsAtRoot, &comps, IKnowWhatIAmDoing);
		}
	return this;
	}

string FileLoc_POSIX::AsString_POSIX(const string* iComps, size_t iCount)
	{
	return FileLoc_POSIX::AsString_Native(iComps, iCount);
	}

string FileLoc_POSIX::AsString_Native(const string* iComps, size_t iCount)
	{
	string result;
	if (fComps.empty())
		{
		if (iCount)
			{
			if (not fIsAtRoot)
				result = ".";
			for (size_t xx = 0; xx < iCount; ++xx)
				{
				result += "/";
				result += iComps[xx];
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
		for (size_t xx = 0; xx < iCount; ++xx)
			{
			result += "/";
			result += iComps[xx];
			}
		}
	return result;
	}

File::Kind FileLoc_POSIX::Kind()
	{
	struct stat theStat;

	if (0 > ::lstat(this->pGetPath().c_str(), &theStat))
		{
		return File::kindNone;
		}

	if (S_ISREG(theStat.st_mode))
		return File::kindFile;

	if (S_ISDIR(theStat.st_mode))
		return File::kindDir;

	if (S_ISLNK(theStat.st_mode))
		return File::kindLink;

	return File::kindNone;
	}

uint64 FileLoc_POSIX::Size()
	{
	#if (defined(linux) || defined(__linux__)) && not defined (__ANDROID__)
		struct stat64 theStat;
		int result = ::stat64(this->pGetPath().c_str(), &theStat);
	#else
		struct stat theStat;
		int result = ::stat(this->pGetPath().c_str(), &theStat);
	#endif

	if (result < 0)
		return 0;

	return theStat.st_size;
	}

double FileLoc_POSIX::TimeCreated()
	{
	struct stat theStat;
	if (0 > ::stat(this->pGetPath().c_str(), &theStat))
		return 0;

	#if __MACH__ && !defined(_POSIX_SOURCE)
		return double(theStat.st_ctimespec.tv_sec + (theStat.st_ctimespec.tv_nsec / 1000000000.0));
	#elif defined(__USE_MISC) || defined(__sun__)
		return double(theStat.st_ctim.tv_sec + (theStat.st_ctim.tv_nsec / 1000000000.0));
	#else
		return double(theStat.st_ctime + (theStat.st_ctimensec / 1000000000.0));
	#endif
	}

double FileLoc_POSIX::TimeModified()
	{
	struct stat theStat;
	if (0 > ::stat(this->pGetPath().c_str(), &theStat))
		return 0;

	#if __MACH__ && !defined(_POSIX_SOURCE)
		return double(theStat.st_mtimespec.tv_sec + (theStat.st_mtimespec.tv_nsec / 1000000000.0));
	#elif defined(__USE_MISC) || defined(__sun__)
		return double(theStat.st_mtim.tv_sec + (theStat.st_mtim.tv_nsec / 1000000000.0));
	#else
		return double(theStat.st_mtime + (theStat.st_mtimensec / 1000000000.0));
	#endif
	}

ZRef<FileLoc> FileLoc_POSIX::CreateDir()
	{
	if (0 > ::mkdir(this->pGetPath().c_str(), 0777))
		return null;
	return this;
	}

ZRef<FileLoc> FileLoc_POSIX::MoveTo(ZRef<FileLoc> oDest)
	{
	FileLoc_POSIX* other = oDest.DynamicCast<FileLoc_POSIX>();
	if (not other)
		return null;

	if (0 > ::rename(this->pGetPath().c_str(), other->pGetPath().c_str()))
		return null;

	return other;
	}

bool FileLoc_POSIX::Delete()
	{
	string myPath = this->pGetPath();
	// Assume it's a file first
	if (0 > ::unlink(myPath.c_str()))
		{
		if (ENOENT == errno)
			return false;

		// Try it as if it's a directory
		if (0 > ::rmdir(myPath.c_str()))
			return false;
		}
	return true;
	}

ZRef<ChannerRPos_Bin> FileLoc_POSIX::OpenRPos(bool iPreventWriters)
	{
	File::Error err;
	const int theFD = spOpen(this->pGetPath().c_str(), true, false, iPreventWriters, &err);
	if (theFD < 0)
		return null;

	return sChanner_T<ChanRPos_File_POSIX>(ChanRPos_File_POSIX::Init_t(theFD, true));
	}

ZRef<ChannerWPos_Bin> FileLoc_POSIX::OpenWPos(bool iPreventWriters)
	{
	File::Error err;
	const int theFD = spOpen(this->pGetPath().c_str(), false, true, iPreventWriters, &err);
	if (theFD < 0)
		return null;

	return sChanner_T<ChanWPos_File_POSIX>(ChanWPos_File_POSIX::Init_t(theFD, true));
	}

ZRef<ChannerRWPos_Bin> FileLoc_POSIX::OpenRWPos(bool iPreventWriters)
	{
	File::Error err;
	const int theFD = spOpen(this->pGetPath().c_str(), true, true, iPreventWriters, &err);
	if (theFD < 0)
		return null;

	return sChanner_T<ChanRWPos_File_POSIX>(ChanRPos_File_POSIX::Init_t(theFD, true));
	}

ZRef<ChannerWPos_Bin> FileLoc_POSIX::CreateWPos(bool iOpenExisting, bool iPreventWriters)
	{
	File::Error err;
	const int theFD = spCreate(this->pGetPath().c_str(), iOpenExisting, false, iPreventWriters, &err);
	if (theFD < 0)
		return null;

	return sChanner_T<ChanWPos_File_POSIX>(FDHolder::Init_t(theFD, true));
	}

ZRef<ChannerRWPos_Bin> FileLoc_POSIX::CreateRWPos(bool iOpenExisting, bool iPreventWriters)
	{
	File::Error err;
	const int theFD = spCreate(this->pGetPath().c_str(), iOpenExisting, true, iPreventWriters, &err);
	if (theFD < 0)
		return null;

	return sChanner_T<ChanRWPos_File_POSIX>(FDHolder::Init_t(theFD, true));
	}

string FileLoc_POSIX::pGetPath()
	{
	if (fComps.empty())
		{
		if (fIsAtRoot)
			return "/";
		return ".";
		}

	string result;
	if (not fIsAtRoot)
		result = ".";
	for (size_t xx = 0; xx < fComps.size(); ++xx)
		{
		result += "/";
		result += fComps[xx];
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark FDHolder

FDHolder::FDHolder(const Init_t& iInit)
:	fFD(iInit.first)
,	fCloseWhenFinalized(iInit.second)
	{}

FDHolder::~FDHolder()
	{
	if (fCloseWhenFinalized)
		spClose(fFD);
	}

int FDHolder::GetFD() const
	{ return fFD; }

// =================================================================================================
#pragma mark -
#pragma mark ChanRPos_File_POSIX

ChanRPos_File_POSIX::ChanRPos_File_POSIX(const Init_t& iInit)
:	FDHolder(iInit)
	{}

ChanRPos_File_POSIX::~ChanRPos_File_POSIX()
	{}

size_t ChanRPos_File_POSIX::Read(byte* oDest, size_t iCount)
	{
	size_t countRead;
	spRead(fFD, oDest, iCount, &countRead);
	return countRead;
//	if (File::errorNone == spRead(fFD, oDest, iCount, &countRead))
//		return countRead;
//	return 0;
	}

size_t ChanRPos_File_POSIX::Readable()
	{
	uint64 theSize;
	if (File::errorNone == spGetSize(fFD, theSize))
		{
		uint64 thePos;
		if (File::errorNone == spGetPosition(fFD, thePos))
			return theSize - thePos;
		}
	return 0;
	}

uint64 ChanRPos_File_POSIX::Pos()
	{
	uint64 pos;
	if (File::errorNone == spGetPosition(fFD, pos))
		return pos;
	return 0;
	}

void ChanRPos_File_POSIX::PosSet(uint64 iPos)
	{ spSetPosition(fFD, iPos); }

uint64 ChanRPos_File_POSIX::Size()
	{
	uint64 theSize;
	if (File::errorNone == spGetSize(fFD, theSize))
		return theSize;
	return 0;
	}

size_t ChanRPos_File_POSIX::Unread(const byte* iSource, size_t iCount)
	{
	uint64 thePos;
	if (File::errorNone == spGetPosition(fFD, thePos))
		{
		const size_t countToUnread = std::min<uint64>(thePos, iCount);
		spSetPosition(fFD, thePos - countToUnread);
		return countToUnread;
		}
	return 0;
	}

size_t ChanRPos_File_POSIX::UnreadableLimit()
	{
	uint64 pos;
	if (File::errorNone == spGetPosition(fFD, pos))
		return pos;
	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanWPos_File_POSIX

ChanWPos_File_POSIX::ChanWPos_File_POSIX(const Init_t& iInit)
:	FDHolder(iInit)
	{}

ChanWPos_File_POSIX::~ChanWPos_File_POSIX()
	{}

size_t ChanWPos_File_POSIX::Write(const byte* iSource, size_t iCount)
	{
	size_t countWritten;
	spWrite(fFD, iSource, iCount, &countWritten);
	return countWritten;
//	if (File::errorNone == spWrite(fFD, iSource, iCount, &countWritten))
//		return countWritten;
//	return 0;
	}

uint64 ChanWPos_File_POSIX::Pos()
	{
	uint64 pos;
	if (File::errorNone == spGetPosition(fFD, pos))
		return pos;
	return 0;
	}

void ChanWPos_File_POSIX::PosSet(uint64 iPos)
	{ spSetPosition(fFD, iPos); }

uint64 ChanWPos_File_POSIX::Size()
	{
	uint64 theSize;
	if (File::errorNone == spGetSize(fFD, theSize))
		return theSize;
	return 0;
	}

void ChanWPos_File_POSIX::SizeSet(uint64 iSize)
	{ spSetSize(fFD, iSize); }

// =================================================================================================
#pragma mark -
#pragma mark ChanRWPos_File_POSIX

ChanRWPos_File_POSIX::ChanRWPos_File_POSIX(const Init_t& iInit)
:	FDHolder(iInit)
	{}

ChanRWPos_File_POSIX::~ChanRWPos_File_POSIX()
	{}

size_t ChanRWPos_File_POSIX::Read(byte* oDest, size_t iCount)
	{
	size_t countRead;
	if (File::errorNone == spRead(fFD, oDest, iCount, &countRead))
		return countRead;
	return 0;
	}

size_t ChanRWPos_File_POSIX::Readable()
	{
	uint64 theSize;
	if (File::errorNone == spGetSize(fFD, theSize))
		{
		uint64 thePos;
		if (File::errorNone == spGetPosition(fFD, thePos))
			return theSize - thePos;
		}
	return 0;
	}

size_t ChanRWPos_File_POSIX::Write(const byte* iSource, size_t iCount)
	{
	size_t countWritten;
	if (File::errorNone == spWrite(fFD, iSource, iCount, &countWritten))
		return countWritten;
	return 0;
	}

void ChanRWPos_File_POSIX::SizeSet(uint64 iSize)
	{ spSetSize(fFD, iSize); }


uint64 ChanRWPos_File_POSIX::Pos()
	{
	uint64 pos;
	if (File::errorNone == spGetPosition(fFD, pos))
		return pos;
	return 0;
	}

void ChanRWPos_File_POSIX::PosSet(uint64 iPos)
	{ spSetPosition(fFD, iPos); }

uint64 ChanRWPos_File_POSIX::Size()
	{
	uint64 theSize;
	if (File::errorNone == spGetSize(fFD, theSize))
		return theSize;
	return 0;
	}

size_t ChanRWPos_File_POSIX::Unread(const byte* iSource, size_t iCount)
	{
	uint64 thePos;
	if (File::errorNone == spGetPosition(fFD, thePos))
		{
		const size_t countToUnread = std::min<uint64>(thePos, iCount);
		spSetPosition(fFD, thePos - countToUnread);
		return countToUnread;
		}
	return 0;
	}

size_t ChanRWPos_File_POSIX::UnreadableLimit()
	{
	uint64 pos;
	if (File::errorNone == spGetPosition(fFD, pos))
		return pos;
	return 0;
	}
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(File_POSIX)
