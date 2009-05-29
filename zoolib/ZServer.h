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

#ifndef __NewServer__
#define __NewServer__ 1
#include "zconfig.h"

#include "zoolib/ZSafeSet.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZTask.h"
#include "zoolib/ZWaiter.h"

#include <vector>

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

	void KillResponders();
	
	ZRef<ZStreamerRWFactory> GetFactory();

	void GetResponders(std::vector<ZRef<Responder> >& oResponders);

private:
	class StreamerListener;
	friend class StreamerListener;

// Called by StreamerListener
	void pConnected(ZRef<ZStreamerRW> iStreamer);

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
#pragma mark * ZServer_Simple

class ZServer_Simple : public ZServer
	{
public:
	ZServer_Simple();

// From ZServer
	virtual ZRef<Responder> MakeResponder();

// Our protocol
	virtual void Handle(ZRef<ZStreamerRW> iStreamerRW);

private:
	class Responder;
	};

NAMESPACE_ZOOLIB_END

#endif // __NewServer__
