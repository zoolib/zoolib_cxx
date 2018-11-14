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

#include "zoolib/POSIX/File_POSIX.h"

#if ZCONFIG_API_Enabled(File_POSIX)

#include "zoolib/POSIX/Chan_Bin_POSIXFD.h"

#include <cstring>
#include <vector>

#define kDebug_File_POSIX 2

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
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
#pragma mark FDHolder_UnlockOnDestroy

class FDHolder_UnlockOnDestroy
:	public FDHolder
	{
public:
	FDHolder_UnlockOnDestroy(int iFD)
	:	fFD(iFD)
		{}

	virtual ~FDHolder_UnlockOnDestroy()
		{
		struct flock file_lock;
		file_lock.l_type = F_UNLCK;
		file_lock.l_whence = SEEK_SET;
		file_lock.l_start = 0;
		file_lock.l_len = 0;
		::fcntl(fFD, F_SETLK, &file_lock);

		::close(fFD);
		}

// Our protocol
	virtual int GetFD()
		{ return fFD; }

protected:
	int fFD;
	};

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

ZRef<FDHolder> spLockOrClose(int iFD, bool iRead, bool iWrite, bool iPreventWriters)
	{
	ZAssertStop(kDebug_File_POSIX, iRead || iWrite);

	if (ZCONFIG_SPI_Enabled(BeOS))
		{
		// Doesn't look like BeOS supports advisory locks.
		return new FDHolder_CloseOnDestroy(iFD);
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

	if (iPreventWriters)
		{
		if (0 == spFCntl(iFD, F_SETLK, theFLock))
			return new FDHolder_UnlockOnDestroy(iFD);
		}
	else
		{
		int err = spFCntl(iFD, F_GETLK, theFLock);

		if (err == ENOLCK || err == EACCES || err == ENOTSUP || err == EINVAL)
			{
			// Assume that we're hitting an NFS-based file. Allow the access to happen.
			return new FDHolder_CloseOnDestroy(iFD);
			}
		else if (err == 0 && theFLock.l_type == F_UNLCK)
			{
			return new FDHolder_CloseOnDestroy(iFD);
			}
		}
	::close(iFD);
	return null;
	}

static ZRef<FDHolder> spOpen(const std::string& iPath, bool iRead, bool iWrite, bool iPreventWriters)
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

	int theFD = ::open(iPath.c_str(), theFlags);

	if (theFD < 0)
		return null;

	return spLockOrClose(theFD, iRead, iWrite, iPreventWriters);
	}

static ZRef<FDHolder> spCreate(const std::string& iPath, bool iOpenExisting, bool iAllowRead, bool iPreventWriters)
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

	int theFD = ::open(iPath.c_str(), flags, 0666);

	if (theFD < 0)
		return null;

	return spLockOrClose(theFD, iAllowRead, true, iPreventWriters);
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
	if (ZRef<FDHolder> theFDHolder = spOpen(this->pGetPath().c_str(), true, false, iPreventWriters))
		return sChanner_T<ChanRPos_Bin_POSIXFD>(theFDHolder);
	return null;
	}

ZRef<ChannerWPos_Bin> FileLoc_POSIX::OpenWPos(bool iPreventWriters)
	{
	if (ZRef<FDHolder> theFDHolder = spOpen(this->pGetPath().c_str(), false, true, iPreventWriters))
		return sChanner_T<ChanWPos_Bin_POSIXFD>(theFDHolder);
	return null;
	}

ZRef<ChannerRWPos_Bin> FileLoc_POSIX::OpenRWPos(bool iPreventWriters)
	{
	if (ZRef<FDHolder> theFDHolder = spOpen(this->pGetPath().c_str(), true, true, iPreventWriters))
		return sChanner_T<ChanRWPos_Bin_POSIXFD>(theFDHolder);
	return null;
	}

ZRef<ChannerWPos_Bin> FileLoc_POSIX::CreateWPos(bool iOpenExisting, bool iPreventWriters)
	{
	if (ZRef<FDHolder> theFDHolder = spCreate(this->pGetPath().c_str(), iOpenExisting, false, iPreventWriters))
		return sChanner_T<ChanWPos_Bin_POSIXFD>(theFDHolder);
	return null;
	}

ZRef<ChannerRWPos_Bin> FileLoc_POSIX::CreateRWPos(bool iOpenExisting, bool iPreventWriters)
	{
	if (ZRef<FDHolder> theFDHolder = spCreate(this->pGetPath().c_str(), iOpenExisting, true, iPreventWriters))
		return sChanner_T<ChanRWPos_Bin_POSIXFD>(theFDHolder);
	return null;
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

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(File_POSIX)
