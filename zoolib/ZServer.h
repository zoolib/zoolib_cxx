/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZServer__
#define __ZServer__ 1
#include "zconfig.h"

#include "zoolib/ZRefSafe.h"
#include "zoolib/ZSafeSet.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZTask.h"
#include "zoolib/ZWorker.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZServer

class ZServer : public ZTaskOwner
	{
public:
	class Responder;
	friend class Responder;

	ZServer();
	virtual ~ZServer();

// From ZRefCountedWithFinalize
	virtual void Finalize();

// From ZTaskOwner
	virtual void Task_Finished(ZRef<ZTask> iTask);

// Our protocol
	virtual ZRef<Responder> MakeResponder() = 0;

	void StartListener(ZRef<ZStreamerRWFactory> iFactory);

	void StopListener();
	void StopListenerWait();

	void KillResponders();
	void KillRespondersWait();
	
	ZRef<ZStreamerRWFactory> GetFactory();

	ZSafeSetIterConst<ZRef<Responder> > GetResponders();

private:
	class StreamerListener;
	friend class StreamerListener;

// Called by StreamerListener
	void pConnected(ZRef<ZStreamerRW> iStreamer);

	ZMtx fMtx;
	ZCnd fCnd;

	ZRef<StreamerListener> fStreamerListener;
	ZSafeSet<ZRef<Responder> > fResponders;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServer::Responder

class ZServer::Responder : public ZTask
	{
public:
	Responder(ZRef<ZServer> iServer);
	virtual ~Responder();

	virtual void Handle(ZRef<ZStreamerRW> iStreamerRW) = 0;

	ZRef<ZServer> GetServer();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServer_T

template <class R, class P>
class ZServer_T : public ZServer
	{
public:
	ZServer_T() {}
	ZServer_T(const P& iParam) : fParam(iParam) {}
	
// From ZServer
	virtual ZRef<Responder> MakeResponder()
		{ return new R(this, fParam); }

private:
	P fParam;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZServer__
