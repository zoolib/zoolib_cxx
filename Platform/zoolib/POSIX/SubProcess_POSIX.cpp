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
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/POSIX/Chan_Bin_POSIXFD.h"
#include "zoolib/POSIX/Util_POSIXFD.h"

#include <signal.h>
#include <string.h> // For strerror
#include <unistd.h> // For close

namespace ZooLib {

using std::vector;

// =================================================================================================
#pragma mark - SubProcess_POSIX

SubProcess_POSIX::SubProcess_POSIX(const std::string& iProgram,
	const std::vector<std::string>& iArgs)
:	fProgram(iProgram)
,	fArgs(iArgs)
,	fPid(-1)
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

	vector<const char*> theArgs;
	theArgs.push_back(fProgram.c_str());
	foreachi (iter, fArgs)
		theArgs.push_back(iter->c_str());
	theArgs.push_back(nullptr);

	int childStdout[2];
	::pipe(childStdout);

	int childStdin[2];
	::pipe(childStdin);

	const pid_t pid1 = ::fork();
	if (pid1 < 0)
		{
		if (ZLOGF(w, eErr))
			w << "Couldn't fork to run " << fProgram;
		throw std::runtime_error("Couldn't fork");
		}
	else if (pid1 != 0)
		{
		// We're still in the parent. Remember the pid and the pipe.
		fPid = pid1;

		fChannerRCon = sChanner_T<ChanRCon_Bin_POSIXFD>(new FDHolder_CloseOnDestroy(childStdout[0]));
		::close(childStdout[1]);

		fChannerWCon = sChanner_T<ChanWCon_Bin_POSIXFD>(new FDHolder_CloseOnDestroy(childStdin[1]));
		::close(childStdin[0]);
		}
	else if (pid1 == 0)
		{
		// We're in the child. dup2 the appropriate ends of the pipes over fds 0 and 1.
		::dup2(childStdin[0], 0);
		::close(childStdin[0]);
		::close(childStdin[1]);

		::dup2(childStdout[1], 1);
		::close(childStdout[0]);
		::close(childStdout[1]);

		// Leave stderr alone, and close everything else.
		for (int xx = 3; xx < 1024; ++xx)
			::close(xx);

		execv(fProgram.c_str(), (char* const*) &theArgs[0]);

		// We only get here if the execv fails.
		const int the_errno = errno;

		if (ZLOGF(w, eErr))
			{
			w << "Couldn't exec to run " << fProgram
				<< ", errno: " << strerror(the_errno)
				<< "(" << the_errno << ")";
			}

		_exit(EXIT_FAILURE);
		}
	}

void SubProcess_POSIX::Stop()
	{
	::kill(fPid, SIGKILL);
	}

void SubProcess_POSIX::WaitTillStopped()
	{
	for (;;)
		{
		if (0 <= waitpid(fPid, 0, 0) || errno != EINTR)
			break;
		}
	}

ZRef<ChannerRCon_Bin> SubProcess_POSIX::GetChannerRCon()
	{ return fChannerRCon; }

ZRef<ChannerWCon_Bin> SubProcess_POSIX::GetChannerWCon()
	{ return fChannerWCon; }

// ----------

ZRef<SubProcess_POSIX> sLaunchSubProcess(
	const std::string& iProgram,
	const std::vector<std::string>& iArgs)
	{
	try
		{
		return new SubProcess_POSIX(iProgram, iArgs);
		}
	catch (...)
		{
		return null;
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
