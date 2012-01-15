/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZProgressWatcher_h__
#define __ZProgressWatcher_h__ 1
#include "zconfig.h"
#include "zoolib/ZTypes.h"
#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - ZProgressWatcher

class ZProgressWatcher
	{
protected:
	ZProgressWatcher() {}
	~ZProgressWatcher() {}

public:
	virtual void Push(const std::string& iDescription, int32 iMax) = 0;
	virtual void Process(int32 iDelta) = 0;
	virtual void Pop() = 0;
	};

// =================================================================================================
// MARK: - ZProgressPusher

class ZProgressPusher
	{
public:
	ZProgressPusher(ZProgressWatcher* iProgressWatcher, const std::string& iDescription, int32 iMax);
	ZProgressPusher(ZProgressWatcher* iProgressWatcher, int32 iMax);

	ZProgressPusher(ZProgressPusher& iOtherPusher, const std::string& iDescription, int32 iMax);
	ZProgressPusher(ZProgressPusher& iOtherPusher, int32 iMax);

	~ZProgressPusher();

	void Push(const std::string& iDescription, int32 iMax);
	void Push(int32 iMax);
	void Process(int32 iDelta);
	void Pop();

private:
	ZProgressWatcher* fWatcher;
	size_t fPushes;
	};

} // namespace ZooLib

#endif // __ZProgressWatcher_h__
