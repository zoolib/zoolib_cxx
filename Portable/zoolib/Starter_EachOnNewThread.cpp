/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#include "zoolib/Starter_EachOnNewThread.h"
#include "zoolib/StartOnNewThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_EachOnNewThread

class Starter_EachOnNewThread
:	public Starter
	{
public:
// From Starter
	virtual bool QStart(const ZRef<Startable>& iStartable)
		{
		if (iStartable)
			{
			try
				{
				sStartOnNewThread(iStartable);
				return true;
				}
			catch (...)
				{}
			}
		return false;
		}
	};

// =================================================================================================
#pragma mark - sStarter_EachOnNewThread

ZRef<Starter> sStarter_EachOnNewThread()
	{ return sSingleton<ZRef_Counted<Starter_EachOnNewThread> >(); }

} // namespace ZooLib
