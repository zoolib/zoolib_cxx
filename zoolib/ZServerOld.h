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

#ifndef __ZServerOld__
#define __ZServerOld__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZSafeSet.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZServerOld

class ZServerOld : public ZCounted
	{
public:
	class Responder;
	friend class Responder;

	ZServerOld();
	virtual ~ZServerOld();

// From ZCounted
	virtual void Finalize();

// Our protocol
	virtual ZRef<Responder> MakeResponder() = 0;

	ZRef<ZStreamerRWFactory> GetListener();

	void StartListener(ZRef<ZCaller> iCaller, ZRef<ZStreamerRWFactory> iFactory);

	void StopListener();
	void StopListenerWait();

	void KillResponders();
	void KillRespondersWait();

	ZRef<ZStreamerRWFactory> GetFactory();

	ZSafeSetIterConst<ZRef<Responder> > GetResponders();

private:
	bool pListener_Work(ZRef<ZWorker> iWorker);
	void pListener_Finished(ZRef<ZWorker> iWorker);

	void pResponderFinished(ZRef<Responder> iResponder);

	ZMtx fMtx;
	ZCnd fCnd;

	ZRef<ZWorker> fWorker;
	ZRef<ZStreamerRWFactory> fFactory;
	ZSafeSet<ZRef<Responder> > fResponders;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServerOld::Responder

class ZServerOld::Responder : public ZCounted
	{
public:
	Responder(ZRef<ZServerOld> iServer);
	virtual ~Responder();

	virtual void Respond(ZRef<ZStreamerRW> iStreamerRW) = 0;

	virtual void Kill();

	ZRef<ZServerOld> GetServer();

protected:
	void pFinished();

private:
	ZWeakRef<ZServerOld> fServer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServerOld_T

template <class R, class P = void>
class ZServerOld_T : public ZServerOld
	{
public:
	ZServerOld_T() {}
	ZServerOld_T(const P& iParam) : fParam(iParam) {}

// From ZServerOld
	virtual ZRef<Responder> MakeResponder()
		{ return new R(this, fParam); }

private:
	P fParam;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZServerOld_T (specialized for void Param)

template <class R>
class ZServerOld_T<R,void> : public ZServerOld
	{
public:
	ZServerOld_T() {}

// From ZServerOld
	virtual ZRef<Responder> MakeResponder()
		{ return new R(this); }
	};

} // namespace ZooLib

#endif // __ZServerOld__
