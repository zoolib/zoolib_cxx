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

#ifndef __ZooLib_POSIX_File_POSIX_h__
#define __ZooLib_POSIX_File_POSIX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanPos.h"
#include "zoolib/ChanSize.h"
#include "zoolib/ChanSizeSet.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/File.h"

#ifndef ZCONFIG_API_Avail__File_POSIX
	#define ZCONFIG_API_Avail__File_POSIX ZCONFIG_SPI_Enabled(POSIX)
#endif

#ifndef ZCONFIG_API_Desired__File_POSIX
	#define ZCONFIG_API_Desired__File_POSIX 1
#endif

#if ZCONFIG_API_Enabled(File_POSIX)

ZMACRO_MSVCStaticLib_Reference(File_POSIX)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark FileLoc_POSIX

class FileLoc_POSIX : public FileLoc
	{
public:
	static ZRef<FileLoc_POSIX> sGet_CWD();
	static ZRef<FileLoc_POSIX> sGet_Root();
	static ZRef<FileLoc_POSIX> sGet_App();

	FileLoc_POSIX(bool iIsAtRoot);
	FileLoc_POSIX(bool iIsAtRoot, const std::vector<std::string>& iComps);
	FileLoc_POSIX(bool iIsAtRoot, const std::string* iComps, size_t iCount);
	FileLoc_POSIX(bool iIsAtRoot, std::vector<std::string>* ioComps, const IKnowWhatIAmDoing_t&);
	virtual ~FileLoc_POSIX();

// From FileLoc
	virtual ZRef<FileIterRep> CreateIterRep();

	virtual std::string GetName() const;
	virtual ZQ<Trail> TrailTo(ZRef<FileLoc> oDest) const;

	virtual ZRef<FileLoc> GetParent();
	virtual ZRef<FileLoc> GetDescendant(
		const std::string* iComps, size_t iCount);

	virtual bool IsRoot();

	virtual ZRef<FileLoc> Follow();

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount);
	virtual std::string AsString_Native(const std::string* iComps, size_t iCount);

	virtual File::Kind Kind();
	virtual uint64 Size();
	virtual double TimeCreated();
	virtual double TimeModified();

	virtual ZRef<FileLoc> CreateDir();

	virtual ZRef<FileLoc> MoveTo(ZRef<FileLoc> oDest);
	virtual bool Delete();

	virtual ZRef<ChannerRPos_Bin> OpenRPos(bool iPreventWriters);
	virtual ZRef<ChannerWPos_Bin> OpenWPos(bool iPreventWriters);
	virtual ZRef<ChannerRWPos_Bin> OpenRWPos(bool iPreventWriters);

	virtual ZRef<ChannerWPos_Bin> CreateWPos(bool iOpenExisting, bool iPreventWriters);
	virtual ZRef<ChannerRWPos_Bin> CreateRWPos(bool iOpenExisting, bool iPreventWriters);

	std::string pGetPath();

private:
	bool fIsAtRoot;
	std::vector<std::string> fComps;
	};

// =================================================================================================
#pragma mark -
#pragma mark Chan_File_POSIX

class Chan_File_POSIX
	{
public:
	typedef std::pair<int,bool> Init_t;
	Chan_File_POSIX(const Init_t& iInit);
	~Chan_File_POSIX();

// Our protocol
	int GetFD() const;

protected:
	int fFD;
	bool fCloseWhenFinalized;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanRPos_File_POSIX

class ChanRPos_File_POSIX
:	public Chan_File_POSIX
,	public ChanR_Bin
,	public ChanU<byte>
,	public ChanPos
,	public ChanSize
	{
public:
	ChanRPos_File_POSIX(const Init_t& iInit);
	~ChanRPos_File_POSIX();

// From ChanR
	virtual size_t QRead(byte* oDest, size_t iCount);

	virtual size_t Readable();

// From ChanPos
	virtual uint64 Pos();

	virtual void SetPos(uint64 iPos);

// From ChanSize
	virtual uint64 Size();

// From ChanU
	virtual size_t Unread(const byte* iSource, size_t iCount);
	virtual size_t UnreadableLimit();
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanWPos_File_POSIX

class ChanWPos_File_POSIX
:	public Chan_File_POSIX
,	public ChanW_Bin
,	public ChanPos
,	public ChanSize
,	public ChanSizeSet
	{
public:
	ChanWPos_File_POSIX(const Init_t& iInit);
	~ChanWPos_File_POSIX();

// From ChanW_Bin
	virtual size_t QWrite(const byte* iSource, size_t iCount);

// From ChanPos
	virtual uint64 Pos();

	virtual void SetPos(uint64 iPos);

// From ChanSize
	virtual uint64 Size();

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize);
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanRWPos_File_POSIX

class ChanRWPos_File_POSIX
:	public ChanRPos_File_POSIX
,	public ChanW_Bin
,	public ChanSizeSet
	{
public:
	ChanRWPos_File_POSIX(const Init_t& iInit);
	~ChanRWPos_File_POSIX();

// From ChanW_Bin
	virtual size_t QWrite(const byte* iSource, size_t iCount);

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize);
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(File_POSIX)

#endif // __ZooLib_POSIX_File_POSIX_h__
