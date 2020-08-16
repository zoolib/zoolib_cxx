// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

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
:	public Counted
	{
public:
	SubProcess_POSIX(const std::string& iProgram,
		const std::vector<std::string>& iArgs);

	virtual ~SubProcess_POSIX();

// From Counted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void Stop();
	void WaitTillStopped();

	ZP<ChannerRAbort_Bin> GetChannerRAbort();
	ZP<ChannerWAbort_Bin> GetChannerWAbort();

protected:
	void pWaitTillStopped();

	const std::string fProgram;
	const std::vector<std::string> fArgs;
	pid_t fPid;
	ZP<ChannerRAbort_Bin> fChannerRAbort;
	ZP<ChannerWAbort_Bin> fChannerWAbort;
	};

ZP<SubProcess_POSIX> sLaunchSubProcess(
	const std::string& iProgram,
	const std::vector<std::string>& iArgs);

ZP<SubProcess_POSIX> sLaunchSubProcessAndLogIt(
	const std::string& iProgram,
	const std::vector<std::string>& iArgs);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_POSIX_SubProcess_POSIX_h__
