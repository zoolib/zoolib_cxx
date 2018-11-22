/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/POSIX/SubProcess_POSIX.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/Log.h"
#include "zoolib/POSIX/Util_POSIXFD.h"

#include <signal.h>
#include <unistd.h> // for close

namespace ZooLib {

// =================================================================================================
#pragma mark - SubProcess_POSIX

SubProcess_POSIX::SubProcess_POSIX(const std::string& iProgram,
	const std::vector<std::string>& iArgs)
:	fProgram(iProgram)
,	fArgs(iArgs)
	{}

SubProcess_POSIX::~SubProcess_POSIX()
	{}

void SubProcess_POSIX::Initialize()
	{
	ZCounted::Initialize();

	if (ZLOGF(w, eDebug))
		{
		w << "Program: " << fProgram << ", args: ";
		for (size_t xx = 0; xx < fArgs.size(); ++xx)
			w << (xx ? ", " : "") << fArgs[xx];
		}

	f_pid_t = -1;
//	create a pipe, bind our channer to it, and fork.

//	Launch the program, remember the pid
	}

void SubProcess_POSIX::Stop()
	{
	::kill(f_pid_t, SIGKILL);

	}

void SubProcess_POSIX::WaitTillStopped()
	{
	for (;;)
		{
		int status;
		pid_t result = ::waitpid(f_pid_t, &status, 0);
		if (result >= 0 || errno != EINTR)
			break;
		}
	}

ZRef<ChannerRCon_Bin> SubProcess_POSIX::GetChannerRCon()
	{ return fChannerR; }

ZRef<ChannerWCon_Bin> SubProcess_POSIX::GetChannerWCon()
	{ return fChannerW; }

// ----------

ZRef<SubProcess_POSIX> sLaunchSubProcess(
	const std::string& iProgram,
	const std::vector<std::string>& iArgs)
	{
	return new SubProcess_POSIX(iProgram, iArgs);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
