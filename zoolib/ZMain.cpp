/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZMain.h"
#include "zoolib/ZDebug.h"

#include <stdexcept>

int ZooLib::ZMainNS::sArgC;
char** ZooLib::ZMainNS::sArgV;

/** spMain puts argc and argv into the globals ZMainNS::sArgC and
ZMain::sArgV, and then invokes the application-defined ZMain, wrapping
the call in in a try/catch block. Ideally your ZMain will not allow
exceptions to propogate, and if it does this code will report them
using ZDebugLogf. */

static int spMain(int argc, char **argv)
	{
	using namespace ZooLib;

	ZMainNS::sArgC = argc;
	ZMainNS::sArgV = argv;

	try
		{
		return ZMain(argc, argv);
		}
	catch (std::exception& theEx)
		{
		ZDebugLogf(2, ("Exception \"%s\" not caught by ZMain", theEx.what()));
		}
	catch (...)
		{
		ZDebugLogf(0, ("Exception not caught by ZMain"));
		}

	return 1;
	}

extern "C" int main(int argc, char **argv)
	{
	return spMain(argc, argv);
	}

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

#if !(ZCONFIG(Compiler, CodeWarrior) || ZCONFIG(Compiler, MSVC) || defined(__MINGW32__))
	#error (Probably) unsupported compiler
#endif

// MSL and the Visual C++ runtime put argc and argv into the globals __argc and __argv.
// (see setupargs() in the MSL mslcrt runtime library).

extern "C" int __argc;
extern "C" char** __argv;

extern "C" int APIENTRY WinMain
	(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd)
	{
	// There are several places where we need not only to be running on Win95 or later, but also
	// require that our executable is *marked* as needing to run on Win95 or later, so that the
	// host OS knows to send us messages that we're expecting to receive. This assertion will
	// trip if our executable is marked as needing an OS version below 4.0. This is not detectable
	// by ZooLib at compile or link time, so we have to check it at runtime. It's a considered a
	// fatal error and can be corrected in CodeWarrior by ensuring that the SubSystem ID field of
	// the x86 Linker panel contains 4.00 (or greater).
	DWORD theProcessVersion = ::GetProcessVersion(0);
	ZAssertStopf(0, theProcessVersion >= 0x00040000,
		("theProcessVersion == 0x08x", theProcessVersion));

	return spMain(__argc, __argv);
	}

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================

#if ZCONFIG_SPI_Enabled(POSIX)
	#include <cstdlib>
	#include <unistd.h>
	#include <fcntl.h>
#endif

namespace ZooLib {

void ZMainNS::sDaemonize(bool iForceFDClose)
	{
	#if ZCONFIG_SPI_Enabled(POSIX)
		switch (::fork())
			{
			case -1:
				{
				ZDebugStopf(0,("cannot fork"));
				break;
				}
			case 0:
				{
				// stdin
				if (iForceFDClose || ::isatty(0))
					{
					close(0);
					::open("/dev/null",O_RDONLY);
					}
				// stdout
				if (iForceFDClose || ::isatty(1))
					{
					close(1);
					::open("/dev/null",O_WRONLY);
					}
				// stderr
				if (iForceFDClose || ::isatty(2))
					{
					close(2);
					::open("/dev/null",O_WRONLY);
					}
				// Previously we'd used setpgrp. Changed per Chris Teplov's suggestion.
				::setsid();
				break;
				}
			default:
				{
				exit(0);
				}
			}
	#endif // ZCONFIG_SPI_Enabled(POSIX)
	}

} // namespace ZooLib
