/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZNet_Local_Socket_h__
#define __ZNet_Local_Socket_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZNet_Local.h"
#include "zoolib/ZNet_Socket.h"

#ifndef ZCONFIG_API_Avail__Net_Local_Socket
#	if ZCONFIG_API_Enabled(Net_Socket)
#		define ZCONFIG_API_Avail__Net_Local_Socket 1
#	endif
#endif

#ifndef ZCONFIG_API_Avail__Net_Local_Socket
#	define ZCONFIG_API_Avail__Net_Local_Socket 0
#endif

#ifndef ZCONFIG_API_Desired__Net_Local_Socket
#	define ZCONFIG_API_Desired__Net_Local_Socket 1
#endif

#if ZCONFIG_API_Enabled(Net_Local_Socket)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup_Local_Socket

class ZNetNameLookup_Local_Socket : public ZNetNameLookup
	{
public:
	ZNetNameLookup_Local_Socket(const std::string& iPath);
	virtual ~ZNetNameLookup_Local_Socket();

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
#pragma mark * ZNetListener_Local_Socket

class ZNetListener_Local_Socket
:	public ZNetListener_Local,
	public ZNetListener_Socket
	{
	ZNetListener_Local_Socket(int iFD, const IKnowWhatIAmDoing_t&);
public:
	static ZRef<ZNetListener_Local_Socket> sCreateWithFD(int iFD);

	ZNetListener_Local_Socket(const std::string& iPath);
	virtual ~ZNetListener_Local_Socket();

// From ZNetListener_Socket
	virtual ZRef<ZNetAddress> GetAddress();
	virtual ZRef<ZNetEndpoint> Imp_MakeEndpoint(int iSocketFD);

private:
	std::string fPath;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_Local_Socket

class ZNetEndpoint_Local_Socket
:	public ZNetEndpoint_Local,
	public ZNetEndpoint_Socket
	{
public:
	ZNetEndpoint_Local_Socket(int iSocketFD);
	ZNetEndpoint_Local_Socket(const std::string& iRemotePath);

	virtual ~ZNetEndpoint_Local_Socket();

// From ZNetEndpoint via ZNetEndpoint_Socket
	virtual ZRef<ZNetAddress> GetRemoteAddress();
	};

// =================================================================================================

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Net_Local_Socket)

#endif // __ZNet_Local_Socket_h__
