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

#ifndef __ZBlackBerryCOM_h__
#define __ZBlackBerryCOM_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZStdInt.h"
#include "zoolib/ZWinCOM.h"

// winnt.h defines 'STDMETHODIMP' to be 'HRESULT STDMETHODCALLTYPE'.

// =================================================================================================

namespace ZooLib {
namespace ZBlackBerryCOM {

// =================================================================================================
// MARK: - IChannelEvents

ZMACRO_WinCOM_Class(IChannelEvents, IUnknown,
	C7168312,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP CheckClientStatus(uint16 iRHS) = 0;
	virtual STDMETHODIMP OnChallenge(int32 iAttemptsRemaining, uint8 oPasswordHash[20]) = 0;
	virtual STDMETHODIMP OnNewData() = 0;
	virtual STDMETHODIMP OnClose() = 0;
	};

// =================================================================================================
// MARK: - IChannel

struct ChannelParams
	{
	uint16 fMaxReceiveUnit;
	uint16 fMaxTransmitUnit;
	uint16 fDeviceBuffers;
	uint16 fHostBuffers;
	uint16 fDroppedPacketCount;
	};

ZMACRO_WinCOM_Class(IChannel, IUnknown,
	C7168311,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP Params(ChannelParams* oParams) = 0;
	virtual STDMETHODIMP hostBuffers(uint16 iRHS) = 0;
	virtual STDMETHODIMP PacketsAvailable(int32* oPacketsAvailable) = 0;
	virtual STDMETHODIMP ReadPacket(void* oDest, int32 iCount, int32* oCount) = 0;
	virtual STDMETHODIMP WritePacket(const void* iSource, int32 iLength) = 0;
	};

// =================================================================================================
// MARK: - IDeviceProperty

ZMACRO_WinCOM_Class(IDeviceProperty, IUnknown,
	C7168310,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP Name(BSTR* oName) = 0;
	virtual STDMETHODIMP Value(VARIANT* oValue) = 0;
	};

// =================================================================================================
// MARK: - IDeviceProperties

ZMACRO_WinCOM_Class(IDeviceProperties, IUnknown,
	C716830F,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP Count(uint32* oCount) = 0;
	virtual STDMETHODIMP Item(VARIANT iVariant, IDeviceProperty** oDeviceProperty) = 0;
	};

// =================================================================================================
// MARK: - IDevice

ZMACRO_WinCOM_Class(IDevice, IUnknown,
	C716830E,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP Properties(IDeviceProperties** oDeviceProperties) = 0;
	virtual STDMETHODIMP OpenChannel(
		LPCWSTR iChannelName,
		IChannelEvents* iChannelEvents,
		IChannel** oChannel) = 0;

	virtual STDMETHODIMP Reset() = 0;
	virtual STDMETHODIMP Equals(IDevice* iDevice, int32* oResult) = 0;
	};

// =================================================================================================
// MARK: - IDevices

ZMACRO_WinCOM_Class(IDevices, IUnknown,
	C716830D,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP Count(uint32* oCount) = 0;
	virtual STDMETHODIMP Item(uint32 iIndex, IDevice** oDevice) = 0;
	};

// =================================================================================================
// MARK: - IDeviceManagerEvents

ZMACRO_WinCOM_Class(IDeviceManagerEvents, IUnknown,
	C716830C,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	virtual STDMETHODIMP DeviceConnect(IDevice* iDevice) = 0;
	virtual STDMETHODIMP DeviceDisconnect(IDevice* iDevice) = 0;
	};

// =================================================================================================
// MARK: - IDeviceManagerNotification

ZMACRO_WinCOM_Class(IDeviceManagerNotification, IDeviceManagerEvents,
	5F67EACC,D387,4B71,96,37,59,69,01,D0,0C,E0)

	virtual STDMETHODIMP SuspendRequest() = 0;
	virtual STDMETHODIMP SuspendNotification(int32 iSuspending) = 0;
	virtual STDMETHODIMP ResumeNotification() = 0;
	};

// =================================================================================================
// MARK: - IDeviceManager

ZMACRO_WinCOM_Class(IDeviceManager, IUnknown,
	C716830B,A0F4,46DF,B8,2A,54,8C,FB,08,75,5E)

	static const GUID sCLSID;

	virtual STDMETHODIMP Devices(IDevices** oDevices) = 0;

	virtual STDMETHODIMP Advise(
		IDeviceManagerEvents* iDeviceManagerEvents, uint32* oCookie) = 0;

	virtual STDMETHODIMP Unadvise(uint32 iCookie) = 0;
	};

} // namespace ZBlackBerryCOM
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)


#endif // __ZBlackBerryCOM_h__
