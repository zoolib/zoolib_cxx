// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/HTTP/Connect.h"

#include "zoolib/Chan_XX_Buffered.h"
#include "zoolib/Net_Internet.h"

//###include "zoolib/ZStreamerRWCon_SSL.h"

namespace ZooLib {
namespace HTTP {

// =================================================================================================
#pragma mark - HTTP::sQConnect

ZP<ChannerRWClose_Bin> sConnect(const std::string& iHost, uint16 iPort, bool iUseSSL)
	{
	if (ZP<ChannerRWClose_Bin> theSR = sCall(sZP(new NetName_Internet(iHost, iPort))))
		{
//##		theComboQ->SetW(new ChanW_XX_Buffered<byte>(theComboQ->GetW()))

		if (not iUseSSL)
			return theSR;

//##		if (ZP<ZStreamerRWCon> wrapped = sStreamerRWCon_SSL(theEP, theSW))
//##			return Connection_t(wrapped, wrapped, new ChannerClose_RWCon(wrapped));
		}
	return null;
	}

} // namespace HTTP
} // namespace ZooLib
