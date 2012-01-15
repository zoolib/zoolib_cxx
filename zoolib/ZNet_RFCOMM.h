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

#ifndef __ZNet_RFCOMM_h__
#define __ZNet_RFCOMM_h__ 1
#include "zconfig.h"

#include "zoolib/ZNet.h"

namespace ZooLib {

// Need to figure out what the accessors for all this stuff are.

// =================================================================================================
// MARK: - ZNet_RFCOMM

class ZNet_RFCOMM
	{
public:
	struct DeviceAddress_t
		{
		uint8 fAddress[6];
		};
	};

// =================================================================================================
// MARK: - ZNetName_RFCOMM

class ZNetName_RFCOMM
:	public ZNetName,
	public ZNet_RFCOMM
	{
protected:
	ZNetName_RFCOMM();
	};

// =================================================================================================
// MARK: - ZNetAddress_RFCOMM

class ZNetAddress_RFCOMM
:	public ZNetAddress,
	public ZNet_RFCOMM
	{
protected:
	ZNetAddress_RFCOMM();

	uint8 fAddress[6];
	};

// =================================================================================================
// MARK: - ZNetListener_RFCOMM

class ZNetListener_RFCOMM
:	public ZNetListener,
	public ZNet_RFCOMM
	{
protected:
	ZNetListener_RFCOMM();

public:
	struct MakeParam_t {};

	ZRef<ZNetListener_RFCOMM> sCreate();
	};

// =================================================================================================
// MARK: - ZNetEndpoint_RFCOMM

class ZNetEndpoint_RFCOMM
:	public ZNetEndpoint,
	public ZNet_RFCOMM
	{
protected:
	ZNetEndpoint_RFCOMM();

public:
	struct MakeParam_t {};

	ZRef<ZNetEndpoint_RFCOMM> sCreate(const DeviceAddress_t& iDA, int iChannelID);
	};

} // namespace ZooLib

#endif // __ZNet_RFCOMM_h__
