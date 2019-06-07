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

#ifndef __ZooLib_POSIX_SubProcess_POSIX_h__
#define __ZooLib_POSIX_SubProcess_POSIX_h__ 1
#include "zconfig.h"

#include <sys/wait.h> // For pid_t
#include <string>
#include <vector>

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Channer_Bin.h"

#if ZCONFIG_SPI_Enabled(POSIX)

namespace ZooLib {

// =================================================================================================
#pragma mark - SubProcess_POSIX

class SubProcess_POSIX
:	public ZCounted
	{
public:
	SubProcess_POSIX(const std::string& iProgram,
		const std::vector<std::string>& iArgs);

	virtual ~SubProcess_POSIX();

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void Stop();
	void WaitTillStopped();

	ZRef<ChannerRAbort_Bin> GetChannerRAbort();
	ZRef<ChannerWAbort_Bin> GetChannerWAbort();

protected:
	void pWaitTillStopped();

	const std::string fProgram;
	const std::vector<std::string> fArgs;
	pid_t fPid;
	ZRef<ChannerRAbort_Bin> fChannerRAbort;
	ZRef<ChannerWAbort_Bin> fChannerWAbort;
	};

ZRef<SubProcess_POSIX> sLaunchSubProcess(
	const std::string& iProgram,
	const std::vector<std::string>& iArgs);

ZRef<SubProcess_POSIX> sLaunchSubProcessAndLogIt(
	const std::string& iProgram,
	const std::vector<std::string>& iArgs);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_POSIX_SubProcess_POSIX_h__
