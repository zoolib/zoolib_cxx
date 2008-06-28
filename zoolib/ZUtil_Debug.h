/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_Debug__
#define __ZUtil_Debug__ 1
#include "zconfig.h"

#include "zoolib/ZStackCrawl.h"
#include "zoolib/ZStrimmer.h"
#include "zoolib/ZLog.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Debug

namespace ZUtil_Debug {

#if ZCONFIG_API_Enabled(StackCrawl)
	void sDumpStackCrawl(const ZStackCrawl& iCrawl, const ZStrimW& s);
#endif

class LogMeister : public ZLog::LogMeister
	{
public:
	LogMeister();

// From ZLog::LogMeister
	virtual bool Enabled(ZLog::EPriority iPriority, const std::string& iName);
	virtual void LogIt(ZLog::EPriority iPriority, const std::string& iName, const std::string& iMessage);

// Our protocol
	void SetStrimmer(ZRef<ZStrimmerW> iStrimmerW);

	void SetLogPriority(ZLog::EPriority iLogPriority);
	ZLog::EPriority GetLogPriority();

	static LogMeister* sLogMeister;

private:
	ZRef<ZStrimmerW> fStrimmerW;
	ZLog::EPriority fLogPriority;
	};

void sInstall();

void sSetStrimmer(ZRef<ZStrimmerW> iStrimmerW);

void sSetLogPriority(ZLog::EPriority iLogPriority);
ZLog::EPriority sGetLogPriority();

} // namespace ZUtil_Debug

#endif // __ZUtil_Debug__
