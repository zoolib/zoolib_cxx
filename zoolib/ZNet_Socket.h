/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZNet_Socket_h__
#define __ZNet_Socket_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Net_Socket
#	define ZCONFIG_API_Avail__Net_Socket ZCONFIG_SPI_Enabled(POSIX)
#endif

#ifndef ZCONFIG_API_Desired__Net_Socket
#	define ZCONFIG_API_Desired__Net_Socket 1
#endif

#include "zoolib/ZNet.h"

#if ZCONFIG_API_Enabled(Net_Socket)

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZNet_Socket

class ZNet_Socket
	{
public:
	static int sSend(int iSocket, const char* iSource, size_t iCount);
	static int sReceive(int iSocket, char* oDest, size_t iCount);
	static bool sWaitReadable(int iSocket, double iTimeout);
	static void sWaitWriteable(int iSocket);
	static ZNet::Error sTranslateError(int iNativeError);
	};

// =================================================================================================
// MARK: - ZNetListener_Socket

class ZNetListener_Socket
:	public virtual ZNetListener,
	protected ZNet_Socket
	{
protected:
	ZNetListener_Socket(int iSocketFD);

public:
	virtual ~ZNetListener_Socket();

// From ZNetListener
	virtual ZRef<ZNetEndpoint> Listen();
	virtual void CancelListen();

// Our protocol
	int GetSocketFD();

	virtual ZRef<ZNetEndpoint> Imp_MakeEndpoint(int iSocketFD) = 0;

	static void sEnableFastCancellation();

protected:
	ZMtx fMutexNR;
	ZCnd fCondition;
	int fSocketFD;
	ZThread::ID fThreadID_Listening;
	};

// =================================================================================================
// MARK: - ZNetEndpoint_Socket

class ZNetEndpoint_Socket
:	public virtual ZNetEndpoint,
	private ZStreamRCon,
	private ZStreamWCon,
	protected ZNet_Socket
	{
protected:
	ZNetEndpoint_Socket(int iSocketFD);

public:
	virtual ~ZNetEndpoint_Socket();

// From ZStreamerRCon via ZNetEndpoint
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZNetEndpoint
	virtual const ZStreamWCon& GetStreamWCon();

// Our protocol
	int GetSocketFD();

private:
// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	int fSocketFD;
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Socket)

#endif // __ZNet_Socket_h__
