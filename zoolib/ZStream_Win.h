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

#ifndef __ZStream_Win_h__
#define __ZStream_Win_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZStreamer.h"

#if ZCONFIG_SPI_Enabled(Win)

#include <string>
#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamRPos_Win_MultiResource

class ZStreamRPos_Win_MultiResource : public ZStreamRPos
	{
public:
	ZStreamRPos_Win_MultiResource
		(HMODULE iHMODULE, const std::string& iType, const std::string& iName);
	~ZStreamRPos_Win_MultiResource();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	HMODULE fHMODULE;
	std::vector<HRSRC> fVector_HRSRC;
	std::vector<size_t> fVector_Ends;
	size_t fIndex;
	size_t fBegin;
	size_t fEnd;
	HGLOBAL fHGLOBAL_Current;
	LPVOID fLPVOID_Current;

	size_t fSize;
	uint64 fPosition;
	};

// =================================================================================================
// MARK: - ZStreamerRPos_Win_MultiResource

class ZStreamerRPos_Win_MultiResource : public ZStreamerRPos
	{
public:
	ZStreamerRPos_Win_MultiResource
		(HMODULE iHMODULE, const std::string& iType, const std::string& iName);
	virtual ~ZStreamerRPos_Win_MultiResource();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

protected:
	ZStreamRPos_Win_MultiResource fStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

#endif // __ZStream_Win_h__
