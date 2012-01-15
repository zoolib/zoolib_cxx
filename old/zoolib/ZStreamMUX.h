/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#ifndef __ZStreamMUX_h__
#define __ZStreamMUX_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTime.h"

#include <deque>
#include <map>
#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamMUX

class ZStreamMUX
:	public ZCounted
	{
public:
	class Options;

	ZStreamMUX();
	ZStreamMUX(const Options& iOptions);
	~ZStreamMUX();

// From ZCounted
	virtual void Finalize();

// Our protocol
	virtual void Finished();

	void Start(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);

	void Stop();

	ZRef<ZStreamerRWConFactory> Listen(const std::string& iName);
	ZRef<ZStreamerRWConFactory> Listen(const std::string& iName, size_t iReceiveBufferSize);

	ZRef<ZStreamerRWCon> Connect(const std::string& iName);
	ZRef<ZStreamerRWCon> Connect(const std::string& iName, size_t iReceiveBufferSize);

	static uint32 sGetConID(ZRef<ZStreamerRWCon> iCon);

	void SetPingInterval(double iInterval);

private:
	class Commer;
	friend class Commer;
	ZRef<Commer> fCommer;
	bool pRead(const ZStreamR& iStreamR);
	bool pWrite(const ZStreamW& iStreamW);

	class DLink_Endpoint_Pending;
	class Endpoint;
	friend class Endpoint;

	class Listener;
	friend class Listener;

	bool Endpoint_Finalize(Endpoint* iEP);

	void Endpoint_Read(Endpoint* iEP,
		void* oDest, size_t iCount, size_t* oCountRead);

	size_t Endpoint_CountReadable(Endpoint* iEP);

	bool Endpoint_WaitReadable(Endpoint* iEP, double iTimeout);

	void Endpoint_Write(Endpoint* iEP,
		const void* iSource, size_t iCount, size_t* oCountWritten);

	bool Endpoint_ReceiveDisconnect(Endpoint* iEP, double iTimeout);

	void Endpoint_SendDisconnect(Endpoint* iEP);

	void Endpoint_Abort(Endpoint* iEP);

	void Listener_Finalize(Listener* iListener);

	ZRef<ZStreamerRWCon> Listener_Listen(Listener* iListener);

	void Listener_Cancel(Listener* iListener);

	uint32 pGetUnusedID();

	Endpoint* pGetEndpointNilOkay(uint32 iGlobalID);
	Endpoint* pGetEndpoint(uint32 iGlobalID);

	bool pDetachIfUnused(Endpoint* iEP);

	void pAbort(Endpoint* iEP);

	void pReadOne(const ZStreamR& iStreamR);

	bool pSendMessage(const ZStreamW& iStreamW, uint8 iMessage, uint32 iEPID);
	bool pSendMessage(const ZStreamW& iStreamW, uint8 iMessage);
	bool pSendMessage_Param(const ZStreamW& iStreamW, uint8 iMessage, uint32 iEPID, uint32 iParam);
	bool pSendMessage_Open(const ZStreamW& iStreamW, Endpoint* iEP);
	bool pSendMessage_Data
		(const ZStreamW& iStreamW, uint32 iEPID, const void* iSource, size_t iCount);
	void pFlush(const ZStreamW& iStreamW);
	bool pWriteOne(const ZStreamW& iStreamW, Endpoint* iEP);

	ZMtxR fMutex;

	const size_t fMaxFragmentSize;
	const size_t fDefaultReceiveBufferSize;

	enum ELifecycle
		{
//		eLifecycle_PreRun,
		eLifecycle_Running,
		eLifecycle_StreamsDead,
		eLifecycle_StoppingRun,
		eLifecycle_ReadDead
//		eLifecycle_PostRun
		};

	ELifecycle fLifecycle;

	std::deque<uint32> fOpenNacksToSend;

	ZTime fTime_LastRead;
	double fPingInterval;
	bool fPingReceived;
	bool fPingSent;

	std::map<uint32, Endpoint*> fMap_IDToEndpoint;
	std::map<std::string, Listener*> fMap_NameToListener;
	};

// =================================================================================================
// MARK: - ZStreamMUX::Options

class ZStreamMUX::Options
	{
public:
	Options();
	Options(size_t iMaxFragmentSize, size_t iDefaultReceiveBufferSize);

	size_t fMaxFragmentSize;
	size_t fDefaultReceiveBufferSize;
	};

} // namespace ZooLib

#endif // __ZStreamMUX_h__
