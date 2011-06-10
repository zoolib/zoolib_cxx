/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZSOCKS.h"

namespace ZooLib {
namespace ZSOCKS {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZSOCKS::sConnect

bool sConnect(const ZStreamR& r, const ZStreamW& w, const std::string& iHost, uint16 iPort)
	{
	w.WriteUInt8(4); // Version, use 4 for now
	w.WriteUInt8(1); // Open TCP connection
	w.WriteUInt16(iPort);
	w.WriteUInt32(1); // 0.0.0.1, indicates that host name will follow

	// user ID, terminated by zero
	w.WriteUInt8(0);

	w.WriteString(iHost); // Host name
	w.WriteUInt8(0);

	w.Flush();

	r.ReadUInt8(); // null byte
	const uint8 status = r.ReadUInt8();
	/*const uint16 port = */r.ReadUInt16();
	/*const uint16 address = */r.ReadUInt32();
	
	if (status == 0x5a)
		return true;

	return false;
	}

} // namespace ZSOCKS
} // namespace ZooLib
