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

#ifndef __ZNodeRep_Wrapper_h__
#define __ZNodeRep_Wrapper_h__ 1
#include "zconfig.h"

#include "zoolib/ZNode.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZNodeRep_Wrapper

class ZNodeRep_Wrapper : public ZNodeRep
	{
public:
	ZNodeRep_Wrapper(ZRef<ZNodeRep> iParent, const std::string& iName,
		size_t iDepth, ZRef<ZNodeRep> iNodeRep);

// From ZNodeRep
	virtual ZRef<ZNodeIterRep> CreateIterRep();

	virtual bool GetProp(const std::string& iPropName, ZTValue& oTV, Error* oError);
	virtual bool CanHaveChildren(Error* oError);

	virtual std::string GetName(Error* oError);
	virtual ZTrail TrailTo(ZRef<ZNodeRep> iDest, Error* oError);

	virtual ZRef<ZNodeRep> GetAncestor(size_t iCount, Error* oError);
	virtual ZRef<ZNodeRep> GetDescendant(const std::string* iComps, size_t iCount, Error* oError);

	virtual bool Exists(Error* oError);

	virtual ZRef<ZNodeRep> CreateDir(Error* oError);

	virtual ZRef<ZNodeRep> MoveTo(ZRef<ZNodeRep> iDest, Error* oError);
	virtual bool Delete(Error* oError);

	virtual ZRef<ZStreamerR> OpenR(bool iPreventWriters, Error* oError);
	virtual ZRef<ZStreamerRPos> OpenRPos(bool iPreventWriters, Error* oError);
	virtual ZRef<ZStreamerW> OpenW(bool iPreventWriters, Error* oError);
	virtual ZRef<ZStreamerWPos> OpenWPos(bool iPreventWriters, Error* oError);
	virtual ZRef<ZStreamerRWPos> OpenRWPos(bool iPreventWriters, Error* oError);

	virtual ZRef<ZStreamerWPos> CreateWPos(bool iOpenExisting,
		bool iPreventWriters, Error* oError);

	virtual ZRef<ZStreamerRWPos> CreateRWPos(bool iOpenExisting,
		bool iPreventWriters, Error* oError);

// Our protocol
	ZRef<ZNodeRep> GetRealNodeRep();

private:
	ZRef<ZNodeRep> fParent;
	std::string fName;
	size_t fDepth;
	ZRef<ZNodeRep> fNodeRep;
	};

} // namespace ZooLib

#endif // __ZNodeRep_Wrapper_h__
