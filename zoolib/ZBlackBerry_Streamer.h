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

#ifndef __ZBlackBerry_Streamer__
#define __ZBlackBerry_Streamer__ 1
#include "zconfig.h"

#include "zoolib/ZBlackBerry.h"
#include "zoolib/ZCommer.h"
#include "zoolib/ZMemoryBlock.h"

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Device_Streamer

class Channel_Streamer;

class Device_Streamer : public Device, public ZCommer
	{
	friend class Channel_Streamer;

public:
	Device_Streamer();
	virtual ~Device_Streamer();

// From ZBlackBerry::Device
	virtual void Stop();
	virtual ZRef<Channel> Open(
		const std::string& iName, const PasswordHash* iPasswordHash, Error* oError);
	virtual ZMemoryBlock GetAttribute(uint16 iObject, uint16 iAttribute);
	virtual uint32 GetPIN();

// From ZStreamReader via ZCommer	
	virtual bool Read(const ZStreamR& iStreamR);

// From ZStreamWriter via ZCommer	
	virtual bool Write(const ZStreamW& iStreamW);

// From ZCommer
	virtual void Detached();

private:
	bool Channel_Finalize(Channel_Streamer* iChannel);

	void Channel_Read(Channel_Streamer* iChannel, void* iDest, size_t iCount, size_t* oCountRead);

	size_t Channel_CountReadable(Channel_Streamer* iChannel);

	void Channel_Write(
		Channel_Streamer* iChannel, const void* iSource, size_t iCount, size_t* oCountWritten);

	void Channel_SendDisconnect(Channel_Streamer* iChannel);

	bool Channel_ReceiveDisconnect(Channel_Streamer* iChannel, int iMilliseconds);

	void Channel_Abort(Channel_Streamer* iChannel);

	ZRef<Channel_Streamer> pFindChannel(uint16 iChannelID);
	ZRef<Channel_Streamer> pFindChannel(const std::string& iName);
	bool pDetachIfUnused(Channel_Streamer* iChannel);

	void pReadOne(uint16 iChannelID, uint16 iPayloadSize, const ZStreamR& iStreamR);

	bool pWriteOne(const ZStreamW& iStreamW, Channel_Streamer* iChannel);

	class StreamW_Chunked;
	bool pSend(StreamW_Chunked& iSC, uint16 iChannelID, const ZStreamW& iStreamW);
	void pFlush(const ZStreamW& iStreamW);

	ZMutex fMutex;
	ZCondition fCondition;

	struct GetAttribute_t
		{
		uint16 fObject;
		uint16 fAttribute;
		bool fFinished;
		ZMemoryBlock fResult;
		};
	bool fGetAttributeSent;
	GetAttribute_t* fGetAttribute;

	enum ELifecycle
		{
		eLifecycle_Running,
		eLifecycle_StreamsDead,
		eLifecycle_StoppingRun,
		eLifecycle_StoppingRead
		};

	ELifecycle fLifecycle;

	std::vector<Channel_Streamer*> fChannels;
	};

} // namespace ZBlackBerry

#endif // __ZBlackBerry_Streamer__
