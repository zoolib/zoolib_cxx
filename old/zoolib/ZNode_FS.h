/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZNode_FS_h__
#define __ZNode_FS_h__ 1
#include "zconfig.h"

#include "zoolib/ZFile.h"
#include "zoolib/ZNode.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZNodeRep_FS

class ZNodeRep_FS : public ZNodeRep
	{
public:
	ZNodeRep_FS(const ZFileSpec& iFileSpec);

	virtual ZRef<ZNodeIterRep> CreateIterRep();

	virtual bool GetProp(const std::string& iPropName, ZTValue& oTV, ZNode::Error* oError);
	virtual bool CanHaveChildren(ZNode::Error* oError);

	virtual std::string GetName(ZNode::Error* oError);
	virtual ZTrail TrailTo(ZRef<ZNodeRep> iDest, ZNode::Error* oError);

	virtual ZRef<ZNodeRep> GetAncestor(size_t iCount, ZNode::Error* oError);
	virtual ZRef<ZNodeRep> GetDescendant(const std::string* iComps, size_t iCount, ZNode::Error* oError);

	virtual bool Exists(ZNode::Error* oError);

	virtual ZRef<ZNodeRep> CreateDir(ZNode::Error* oError);

	virtual ZRef<ZNodeRep> MoveTo(ZRef<ZNodeRep> iDest, ZNode::Error* oError);
	virtual bool Delete(ZNode::Error* oError);

	virtual ZRef<ZStreamerR> OpenR(bool iPreventWriters, ZNode::Error* oError);
	virtual ZRef<ZStreamerRPos> OpenRPos(bool iPreventWriters, ZNode::Error* oError);
	virtual ZRef<ZStreamerW> OpenW(bool iPreventWriters, ZNode::Error* oError);
	virtual ZRef<ZStreamerWPos> OpenWPos(bool iPreventWriters, ZNode::Error* oError);
	virtual ZRef<ZStreamerRWPos> OpenRWPos(bool iPreventWriters, ZNode::Error* oError);

	virtual ZRef<ZStreamerWPos> CreateWPos(bool iOpenExisting, bool iPreventWriters, ZNode::Error* oError);
	virtual ZRef<ZStreamerRWPos> CreateRWPos(bool iOpenExisting, bool iPreventWriters, ZNode::Error* oError);

	ZFileSpec GetFileSpec();

private:
	ZFileSpec fFileSpec;
	};

// =================================================================================================
// MARK: - ZNodeIterRep_FS

class ZNodeIterRep_FS : public ZNodeIterRep
	{
public:
	ZNodeIterRep_FS(const ZFileIter& iFileIter);
	virtual ~ZNodeIterRep_FS();

	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZNodeRep> Current();
	virtual std::string CurrentName();

	virtual ZRef<ZNodeIterRep> Clone();

private:
	ZFileIter fFileIter;
	};

} // namespace ZooLib

#endif // __ZNode_FS_h__
