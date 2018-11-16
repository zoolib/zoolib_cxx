/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZThread_pthread_more_h__
#define __ZThread_pthread_more_h__
#include "zconfig.h"

#include "zoolib/ZThread_pthread.h"

#if ZCONFIG_API_Enabled(Thread_pthread)

#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {
namespace ZThread_pthread {

// =================================================================================================
#pragma mark - sQGetName

ZQ<std::string> sQGetName();

// =================================================================================================
#pragma mark - SaveSetRestoreName

class SaveSetRestoreName
	{
public:
	SaveSetRestoreName(const std::string& iName);
	SaveSetRestoreName(const char* iName);
	~SaveSetRestoreName();

private:
	const ZQ<std::string> fQPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestorePriority

class SaveSetRestorePriority
	{
public:
	SaveSetRestorePriority(int iPriority);
	~SaveSetRestorePriority();

private:
	struct sched_param fSchedPrior;
	int fPolicyPrior;
	};

// =================================================================================================
#pragma mark - SaveSetRestorePriority_Max

class SaveSetRestorePriority_Max
:	public SaveSetRestorePriority
	{
public:
	SaveSetRestorePriority_Max();
	};

} // namespace ZThread_pthread
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Thread_pthread)

#endif // __ZThread_pthread_more_h__
