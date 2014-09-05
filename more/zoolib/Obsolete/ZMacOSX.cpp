/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZMacOSX.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Carbon64)
	#include ZMACINCLUDE3(CoreServices,CarbonCore,Gestalt.h)
	#if !ZCONFIG_SPI_Enabled(MacOSX)
		static bool sIsMacOSX_Checked;
		static bool sIsMacOSX_Value;
	#endif
#endif

namespace ZooLib {

bool ZMacOSX::sIsMacOSX()
	{
	#if ZCONFIG_SPI_Enabled(MacOSX)

		return true;

	#elif ZCONFIG_SPI_Enabled(Carbon64)

		if (not sIsMacOSX_Checked)
			{
			UInt32 systemVersion;
			OSErr err = ::Gestalt(gestaltSystemVersion, (SInt32*)&systemVersion);
			if (err == noErr && systemVersion >= 0x01000)
				sIsMacOSX_Value = true;
			sIsMacOSX_Checked = true;
			}
		return sIsMacOSX_Value;

	#else

		return false;

	#endif
	}

} // namespace ZooLib
