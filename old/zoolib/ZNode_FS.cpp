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

#include "zoolib/ZNode_FS.h"
#include "zoolib/ZLog.h"

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - Static helpers

static ZNode::Error spTranslateError(ZFile::Error iErr)
	{
	switch (iErr)
		{
		case ZFile::errorNone: return ZNode::errorNone;
		case ZFile::errorDoesntExist: return ZNode::errorDoesntExist;
		case ZFile::errorAlreadyExists: return ZNode::errorAlreadyExists;
		case ZFile::errorInvalidFileSpec: return ZNode::errorInvalidNode;
		case ZFile::errorIllegalFileName: return ZNode::errorIllegalName;
		case ZFile::errorNoPermission: return ZNode::errorNoPermission;
		case ZFile::errorWrongTypeForOperation: return ZNode::errorWrongTypeForOperation;
		case ZFile::errorReadPastEOF: return ZNode::errorReadPastEOF;
		case ZFile::errorInsufficientSpace: return ZNode::errorInsufficientSpace;
		case ZFile::errorGeneric:
			break;
		}
	return ZNode::errorGeneric;
	}

static void spTranslateError(ZFile::Error iErr, ZNode::Error* oError)
	{
	if (oError)
		*oError = spTranslateError(iErr);
	}

// =================================================================================================
// MARK: - ZNodeRep_FS

ZNodeRep_FS::ZNodeRep_FS(const ZFileSpec& iFileSpec)
:	fFileSpec(iFileSpec)
	{}

ZRef<ZNodeIterRep> ZNodeRep_FS::CreateIterRep()
	{
	return new ZNodeIterRep_FS(ZFileIter(fFileSpec));
	}

bool ZNodeRep_FS::GetProp(const string& iPropName, ZTValue& oTV, ZNode::Error* oError)
	{
	if (iPropName == "ContentLength")
		{
		if (fFileSpec.IsFile())
			{
			ZFile::Error fileError;
			uint64 theSize = fFileSpec.Size(&fileError);
			spTranslateError(fileError, oError);
			if (fileError == ZFile::errorNone)
				{
				oTV.SetInt64(theSize);
				return true;
				}
			}
		}
	else if (iPropName == "TimeCreated")
		{
		ZFile::Error fileError;
		ZTime lastModified = fFileSpec.TimeCreated(&fileError);
		spTranslateError(fileError, oError);
		if (fileError == ZFile::errorNone)
			{
			oTV.SetTime(lastModified);
			return true;
			}
		}
	else if (iPropName == "TimeModified")
		{
		ZFile::Error fileError;
		ZTime lastModified = fFileSpec.TimeModified(&fileError);
		spTranslateError(fileError, oError);
		if (fileError == ZFile::errorNone)
			{
			oTV.SetTime(lastModified);
			return true;
			}
		}
	return ZNodeRep::GetProp(iPropName, oTV, oError);
	}

bool ZNodeRep_FS::CanHaveChildren(ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		bool result = fFileSpec.IsDir(&fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.IsDir();
		}
	}

std::string ZNodeRep_FS::GetName(ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		std::string result = fFileSpec.Name(&fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.Name();
		}
	}

ZTrail ZNodeRep_FS::TrailTo(ZRef<ZNodeRep> iDest, ZNode::Error* oError)
	{
	if (ZRef<ZNodeRep_FS> theDest = ZRefDynamicCast<ZNodeRep_FS>(iDest))
		{
		if (oError)
			{
			ZFile::Error fileError;
			ZTrail result = fFileSpec.TrailTo(theDest->GetFileSpec(), &fileError);
			spTranslateError(fileError, oError);
			return result;
			}
		else
			{
			return fFileSpec.TrailTo(theDest->GetFileSpec());
			}
		}

	if (oError)
		*oError = ZNode::errorWrongTypeForOperation;
	return ZTrail();
	}


ZRef<ZNodeRep> ZNodeRep_FS::GetAncestor(size_t iCount, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		if (ZFileSpec newSpec = fFileSpec.Ancestor(iCount, &fileError))
			{
			*oError = ZNode::errorNone;
			return new ZNodeRep_FS(newSpec);
			}
		spTranslateError(fileError, oError);
		}
	else
		{
		if (ZFileSpec newSpec = fFileSpec.Ancestor(iCount))
			return new ZNodeRep_FS(newSpec);
		}
	return ZRef<ZNodeRep>();
	}

ZRef<ZNodeRep> ZNodeRep_FS::GetDescendant(const std::string* iComps, size_t iCount, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		if (ZFileSpec newSpec = fFileSpec.Descendant(iComps, iCount, &fileError))
			{
			*oError = ZNode::errorNone;
			return new ZNodeRep_FS(newSpec);
			}
		spTranslateError(fileError, oError);
		}
	else
		{
		if (ZFileSpec newSpec = fFileSpec.Descendant(iComps, iCount))
			return new ZNodeRep_FS(newSpec);
		}
	return ZRef<ZNodeRep>();
	}

bool ZNodeRep_FS::Exists(ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		bool result = fFileSpec.Exists(&fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.Exists();
		}
	}

ZRef<ZNodeRep> ZNodeRep_FS::CreateDir(ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		if (ZFileSpec newSpec = fFileSpec.CreateDir(&fileError))
			{
			*oError = ZNode::errorNone;
			return new ZNodeRep_FS(newSpec);
			}
		spTranslateError(fileError, oError);
		}
	else
		{
		if (ZFileSpec newSpec = fFileSpec.CreateDir())
			return new ZNodeRep_FS(newSpec);
		}
	return ZRef<ZNodeRep>();
	}

ZRef<ZNodeRep> ZNodeRep_FS::MoveTo(ZRef<ZNodeRep> iDest, ZNode::Error* oError)
	{
	if (ZRef<ZNodeRep_FS> theDest = ZRefDynamicCast<ZNodeRep_FS>(iDest))
		{
		if (ZLOG(s, eInfo, "ZNodeRep_FS::MoveTo"))
			s << "Move " << fFileSpec.AsString() << " to " << theDest->GetFileSpec().AsString();
		if (oError)
			{
			ZFile::Error fileError;
			if (ZFileSpec newSpec = fFileSpec.MoveTo(theDest->GetFileSpec(), &fileError))
				{
				*oError = ZNode::errorNone;
				return new ZNodeRep_FS(newSpec);
				}
			spTranslateError(fileError, oError);
			}
		else
			{
			if (ZFileSpec newSpec = fFileSpec.MoveTo(theDest->GetFileSpec()))
				return new ZNodeRep_FS(newSpec);
			}
		return ZRef<ZNodeRep>();
		}

	if (oError)
		*oError = ZNode::errorWrongTypeForOperation;
	return ZRef<ZNodeRep>();
	}

bool ZNodeRep_FS::Delete(ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		bool result = fFileSpec.Delete(&fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.Delete();
		}
	}

ZRef<ZStreamerR> ZNodeRep_FS::OpenR(bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerR> result = fFileSpec.OpenR(iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.OpenR(iPreventWriters);
		}
	}

ZRef<ZStreamerRPos> ZNodeRep_FS::OpenRPos(bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerRPos> result = fFileSpec.OpenRPos(iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.OpenRPos(iPreventWriters);
		}
	}

ZRef<ZStreamerW> ZNodeRep_FS::OpenW(bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerW> result = fFileSpec.OpenW(iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.OpenW(iPreventWriters);
		}
	}

ZRef<ZStreamerWPos> ZNodeRep_FS::OpenWPos(bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerWPos> result = fFileSpec.OpenWPos(iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.OpenWPos(iPreventWriters);
		}
	}

ZRef<ZStreamerRWPos> ZNodeRep_FS::OpenRWPos(bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerRWPos> result = fFileSpec.OpenRWPos(iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.OpenRWPos(iPreventWriters);
		}
	}

ZRef<ZStreamerWPos> ZNodeRep_FS::CreateWPos(bool iOpenExisting, bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerWPos> result = fFileSpec.CreateWPos(iOpenExisting, iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.CreateWPos(iOpenExisting, iPreventWriters);
		}
	}

ZRef<ZStreamerRWPos> ZNodeRep_FS::CreateRWPos(bool iOpenExisting, bool iPreventWriters, ZNode::Error* oError)
	{
	if (oError)
		{
		ZFile::Error fileError;
		ZRef<ZStreamerRWPos> result = fFileSpec.CreateRWPos(iOpenExisting, iPreventWriters, &fileError);
		spTranslateError(fileError, oError);
		return result;
		}
	else
		{
		return fFileSpec.CreateRWPos(iOpenExisting, iPreventWriters);
		}
	}

ZFileSpec ZNodeRep_FS::GetFileSpec()
	{ return fFileSpec; }

// =================================================================================================
// MARK: - ZNodeIterRep_FS

ZNodeIterRep_FS::ZNodeIterRep_FS(const ZFileIter& iFileIter)
:	fFileIter(iFileIter)
	{}

ZNodeIterRep_FS::~ZNodeIterRep_FS()
	{}

bool ZNodeIterRep_FS::HasValue()
	{ return fFileIter; }

void ZNodeIterRep_FS::Advance()
	{ fFileIter.Advance(); }

ZRef<ZNodeRep> ZNodeIterRep_FS::Current()
	{ return new ZNodeRep_FS(fFileIter.Current()); }

std::string ZNodeIterRep_FS::CurrentName()
	{ return fFileIter.CurrentName(); }

ZRef<ZNodeIterRep> ZNodeIterRep_FS::Clone()
	{ return new ZNodeIterRep_FS(fFileIter); }

} // namespace ZooLib
