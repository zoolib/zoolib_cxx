/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZNet_Local_Win__
#define __ZNet_Local_Win__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZNet_Local.h"

#ifndef ZCONFIG_API_Avail__Net_Local_Win
#	define ZCONFIG_API_Avail__Net_Local_Win ZCONFIG_SPI_Enabled(Win)
#endif

#ifndef ZCONFIG_API_Avail__Net_Local_Win
#	define ZCONFIG_API_Avail__Net_Local_Win 0
#endif

#ifndef ZCONFIG_API_Desired__Net_Local_Win
#	define ZCONFIG_API_Desired__Net_Local_Win 1
#endif


#if ZCONFIG_API_Enabled(Net_Local_Win)

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_Reference(Net_Local_Win)

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZUnicodeString.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Local_Win

class ZNetNameLookup_Local_Win : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Local_Win(const std::string& iName);
	virtual ~ZNetNameLookup_Local_Win();

// From ZNetNameLookup
	virtual void Start();
	virtual bool Finished();
	virtual void Advance();

	virtual ZRef<ZNetAddress> CurrentAddress();
	virtual ZRef<ZNetName> CurrentName();

protected:
	std::string fPath;
	bool fFinished;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_Local_Win

class ZNetListener_Local_Win
:	public ZNetListener_Local
	{
public:
	ZNetListener_Local_Win(const std::string& iName);
	virtual ~ZNetListener_Local_Win();

// From ZNetListener via ZNetListener_Local
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

private:
	const string16 fPath;
	ZRef<HANDLE> fHANDLE;
//	ZRef<HANDLE> fEvent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_Local_Win

class ZNetEndpoint_Local_Win
:	public ZNetEndpoint_Local
,	private ZStreamRCon
,	private ZStreamWCon
	{
protected:
	ZNetEndpoint_Local_Win(const ZRef<HANDLE>& iHANDLE);

public:
	ZNetEndpoint_Local_Win(const std::string& iName);
	virtual ~ZNetEndpoint_Local_Win();

// From ZStreamerRCon via ZNetEndpoint_Local
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZNetEndpoint_Local
	virtual const ZStreamWCon& GetStreamWCon();

// From ZNetEndpoint via ZNetEndpoint_Local
	virtual ZRef<ZNetAddress> GetRemoteAddress();

// Our protocol
	ZRef<HANDLE> GetHANDLE();

private:
// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	ZRef<HANDLE> fHANDLE;

	friend class ZNetListener_Local_Win;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Local_Win)

#endif // __ZNet_Local_Win__
