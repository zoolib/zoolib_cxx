/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZAsset_Std__
#define __ZAsset_Std__ 1
#include "zconfig.h"

#include "ZAsset.h"
#include "ZFile.h"

class ZAssetIterRep_Std_Directory;
class ZAssetRep_Std_Directory;
class ZAssetTree_Std;

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std

class ZAssetRep_Std : public ZAssetRep
	{
public:
	ZAssetRep_Std(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength);
	virtual ~ZAssetRep_Std();

// From ZRefCountedWithFinalization via ZAssetRep
	virtual void Finalize();

// From ZAssetRep
	virtual std::string GetName();
	virtual std::string GetPath();
	virtual ZRef<ZAssetRep> GetParentAssetRep();

// Our protocol
	virtual void DoInitialization() = 0;
	virtual void DoFinalization() = 0;

protected:
	struct CompareAgainstRep;
	struct CompareAgainstPath;

	ZRef<ZAssetTree_Std> fAssetTree;
	const char* fName;
	size_t fNameLength;

private:
	ZAssetRep_Std_Directory* fParent;

	friend class ZAssetIterRep_Std_Directory;
	friend class ZAssetRep_Std_Directory;
	friend class ZAssetTree_Std;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std_Directory

class ZAssetRep_Std_Directory : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Directory(ZAssetTree_Std* inAssetTree, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream, const char* inNameTable, const size_t* inNameOffsets);
	virtual ~ZAssetRep_Std_Directory();

// From ZAssetRep via ZAssetRep_Std
	virtual ZRef<ZAssetRep> ResolvePath(const char* inPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	size_t fChildrenCount;
	union
		{
		ZAssetRep_Std* fChild;
		ZAssetRep_Std** fChildren;
		};

	friend class ZAssetIterRep_Std_Directory;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std_Union

class ZAssetRep_Std_Union : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Union(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream, const char* inNameTable, const size_t* inNameOffsets);
	virtual ~ZAssetRep_Std_Union();

// From ZAssetRep via ZAssetRep_Std
	virtual ZRef<ZAssetRep> ResolvePath(const char* inPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	void pResolvePaths();

	std::vector<const char*> fPaths;

	ZMutex fMutex;
	bool fResolved;
	std::vector<ZRef<ZAssetRep> > fResolvedReps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetIterRep_Std_Directory

class ZAssetIterRep_Std_Directory : public ZAssetIterRep
	{
public:
	ZAssetIterRep_Std_Directory(ZRef<ZAssetRep_Std_Directory> iAssetRep, size_t iIndex);

	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZAssetRep> Current();
	virtual std::string CurrentName();

	virtual ZRef<ZAssetIterRep> Clone();

private:
	ZRef<ZAssetRep_Std_Directory> fAssetRep;
	size_t fCurrent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std

class ZAssetTree_Std : public ZAssetTree
	{
protected:
	ZAssetTree_Std();
	virtual ~ZAssetTree_Std();

// Called by sub classes and ZAssetRep_Std_Directory during load up
	ZAssetRep_Std* LoadAssetRep(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream, const char* inNameTable, const size_t* inNameOffsets);

// Implemented by subclasses
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream) = 0;

// Called by reps
	ZRef<ZAssetRep_Std> UseRep(ZAssetRep_Std* iRep);
	void RepBeingFinalized(ZAssetRep_Std* iRep);

	ZMutex fMutex;

	friend class ZAssetIterRep_Std_Directory;
	friend class ZAssetRep_Std;
	friend class ZAssetRep_Std_Directory;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Stream

class ZAssetRep_Std_Data_Stream;

class ZAssetTree_Std_Stream : public ZAssetTree_Std
	{
protected:
	ZAssetTree_Std_Stream();

public:
	ZAssetTree_Std_Stream(const ZStreamRPos& inStream, size_t inOffset, size_t inSize);
	virtual ~ZAssetTree_Std_Stream();

protected:
// From ZAssetTree via ZAssetTree_Std
	virtual ZRef<ZAssetRep> GetRoot();

// From ZAssetTree_Std
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream);

// Our protocol
	void LoadUp(const ZStreamRPos* inStream, size_t inOffset, size_t inSize);
	void ShutDown();

// Called by ZAssetRep_Std_Data_Stream
	ZRef<ZStreamerRPos> OpenRPos(size_t inOffset, size_t inSize);

// Called by ZAssetRep_Std_Data_Stream and StreamerRPos
	void ReadData(size_t inOffset, size_t inSize, void* outData);

	const ZStreamRPos* fStream;
	char* fNameTable;
	size_t fDataOffset;
	ZAssetRep_Std* fRoot;

	class StreamerRPos;
	friend class StreamerRPos;
	friend class ZAssetRep_Std_Data_Stream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Streamer

class ZAssetTree_Std_Streamer : public ZAssetTree_Std_Stream
	{
public:
	ZAssetTree_Std_Streamer(ZRef<ZStreamerRPos> inStreamer);
	ZAssetTree_Std_Streamer(ZRef<ZStreamerRPos> inStreamer, size_t inOffset, size_t inSize);
	virtual ~ZAssetTree_Std_Streamer();

protected:
	ZRef<ZStreamerRPos> fStreamer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_File

class ZAssetRep_Std_Data_File;

class ZAssetTree_Std_File : public ZAssetTree_Std
	{
public:
	ZAssetTree_Std_File(ZRef<ZFileR> inFile, uint64 inOffset, size_t inSize);
	virtual ~ZAssetTree_Std_File();

protected:
// From ZAssetTree via ZAssetTree_Std
	virtual ZRef<ZAssetRep> GetRoot();

// From ZAssetTree_Std
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream);

// Called by ZAssetRep_Std_Data_File
	ZRef<ZStreamerRPos> OpenRPos(uint64 inOffset, size_t inSize);

// Called by ZAssetRep_Std_Data_File and StreamerStream
	void ReadData(uint64 inOffset, size_t inSize, void* outData);

	ZRef<ZFileR> fFile;
	char* fNameTable;
	uint64 fDataOffset;
	ZAssetRep_Std* fRoot;

	class StreamerRPos;
	friend class StreamerRPos;
	friend class ZAssetRep_Std_Data_File;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Memory

class ZAssetRep_Std_Data_Memory;

class ZAssetTree_Std_Memory : public ZAssetTree_Std
	{
protected:
	ZAssetTree_Std_Memory();

	ZAssetTree_Std_Memory(const void* inAddress, size_t inSize);
	virtual ~ZAssetTree_Std_Memory();

public:
// From ZAssetTree via ZAssetTree_Std
	virtual ZRef<ZAssetRep> GetRoot();

protected:
// From ZAssetTree_Std
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream);

// Our protocol
	void LoadUp(const void* inAddress, size_t inSize);
	void ShutDown();

	const char* fAddress;
	ZAssetRep_Std* fRoot;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Memory_StaticData

class ZAssetTree_Std_Memory_StaticData : public ZAssetTree_Std_Memory
	{
public:
	ZAssetTree_Std_Memory_StaticData(const void* inAddress, size_t inSize);
	virtual ~ZAssetTree_Std_Memory_StaticData();
	};

#endif // __ZAsset_Std__
