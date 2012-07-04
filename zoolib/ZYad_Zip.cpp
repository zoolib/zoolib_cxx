/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#include "zoolib/ZTrail.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZYad_Zip.h"

#include <map>

namespace ZooLib {
namespace ZYad_Zip {

using namespace std;
using namespace ZUtil_STL;

namespace { // anonymous

// =================================================================================================
// MARK: - Node

class Node;

typedef map<ZName, ZRef<Node> > MapNameNode;

struct Node
:	public ZCountedWithoutFinalize
	{
	MapNameNode fChildren;
	size_t fEntryNum;
	};

// =================================================================================================
// MARK: - ZipHolder

struct ZipHolder
:	public ZCounted
	{
	static
	void spStuff(size_t iEntryNum, const ZRef<Node>& ioNode, const ZTrail& iTrail, size_t iIndex)
		{
		if (iIndex >= iTrail.Count())
			{
			ioNode->fEntryNum = iEntryNum;
			return;
			}
			
		const ZName curName = iTrail.At(iIndex);
	
		ZRef<Node>& theNode = ioNode->fChildren[curName];
		if (not theNode)
			theNode = new Node;
		spStuff(iEntryNum, theNode, iTrail, iIndex + 1);
		}

	ZipHolder(zip* i_zip, bool iAdopt)
	:	f_zip(i_zip)
	,	fOwned(iAdopt)
	,	fNode(new Node)
		{
		if (not f_zip)
			throw std::runtime_error("Couldn't open zip");

		fNode = new Node;
		for (size_t xx = 0, count = ::zip_get_num_files(f_zip); xx < count; ++xx)
			{
			const string theName = ::zip_get_name(f_zip, xx, 0);
			ZTrail theTrail(theName);
			spStuff(xx, fNode, theTrail, 0);
			}
		}

	virtual ~ZipHolder()
		{
		if (fOwned)
			::zip_close(f_zip);
		}

	zip* f_zip;
	bool fOwned;

	ZRef<Node> fNode;
	};

// =================================================================================================
// MARK: - YadStreamerR

struct YadStreamerR
:	public ZYadStreamerR
,	private ZStreamR
	{
	ZRef<ZipHolder> fZipHolder;
	zip_file* f_zip_file;

	YadStreamerR(const ZRef<ZipHolder>& iZipHolder, size_t iIndex)
	:	fZipHolder(iZipHolder)
	,	f_zip_file(::zip_fopen_index(fZipHolder->f_zip, iIndex, 0))
		{}
	
	virtual ~YadStreamerR()
		{ ::zip_fclose(f_zip_file); }

// From ZStreamerR via ZYadStreamerR
	const ZStreamR& GetStreamR()
		{ return *this; }

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
		{
		const ssize_t countRead = ::zip_fread(f_zip_file, oDest, iCount);
		if (oCountRead)
			{
			if (countRead <= 0)
				*oCountRead = 0;
			else
				*oCountRead = countRead;
			}
		}	
	};

// =================================================================================================
// MARK: - YadMapAtRPos

struct YadMapAtRPos
:	public ZYadMapAtRPos
	{
	YadMapAtRPos
		(const ZRef<ZipHolder>& iZipHolder, const string8& iPrefix, const ZRef<Node>& iNode)
	:	fZipHolder(iZipHolder)
	,	fPrefix(iPrefix)
	,	fNode(iNode)
	,	fIter(fNode->fChildren.begin())
	,	fEnd(fNode->fChildren.end())
		{}

	YadMapAtRPos
		(const ZRef<ZipHolder>& iZipHolder, const string8& iPrefix, const ZRef<Node>& iNode,
		const MapNameNode::const_iterator& iIter)
	:	fZipHolder(iZipHolder)
	,	fPrefix(iPrefix)
	,	fNode(iNode)
	,	fIter(iIter)
	,	fEnd(fNode->fChildren.end())
		{}
	
	virtual ~YadMapAtRPos()
		{}

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(ZName& oName)
		{
		if (fIter == fEnd)
			return null;

		oName = fIter->first;
		const ZRef<Node> theNode = fIter->second;
		++fIter;
		return this->pYadR(oName, theNode);
		}

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone()
		{ return new YadMapAtRPos(fZipHolder, fPrefix, fNode, fIter); }

// From ZYadMapRPos via ZYadMapAtRPos
	virtual void SetPosition(const ZName& iName)
		{ fIter = fNode->fChildren.find(iName); }

// From ZYadMapAtR via ZYadMapAtRPos
	virtual ZRef<ZYadR> ReadAt(const ZName& iName)
		{ return this->pYadR(iName, sGet(sQGet(fNode->fChildren, iName))); }

// Internal
	ZRef<ZYadR> pYadR(const ZName& iName, const ZRef<Node>& iNode)
		{
		if (iNode)
			{
			if (iNode->fChildren.empty())
				return new YadStreamerR(fZipHolder, iNode->fEntryNum);
			else
				return new YadMapAtRPos(fZipHolder, fPrefix + "/" + string8(iName), iNode);
			}
		return null;
		}

	const ZRef<ZipHolder> fZipHolder;
	const string8 fPrefix;
	const ZRef<Node> fNode;
	MapNameNode::const_iterator fIter;
	const MapNameNode::const_iterator fEnd;
	};
    
} // anonymous namespace

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadMapAtRPos> sYadR(const char* iFilePath)
	{
	if (zip* the_zip = ::zip_open(iFilePath, 0, nullptr))
		return sYadR(the_zip, true);
	return null;
	}

ZRef<ZYadMapAtRPos> sYadR(zip* i_zip, bool iAdopt)
	{
	ZRef<ZipHolder> theHolder = new ZipHolder(i_zip, iAdopt);
	return new YadMapAtRPos(theHolder, string8(), theHolder->fNode);
	}

} // namespace ZYad_Zip
} // namespace ZooLib
