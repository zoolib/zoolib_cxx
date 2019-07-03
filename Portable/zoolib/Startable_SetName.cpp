/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#include "zoolib/Startable_SetName.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"

namespace ZooLib {

class Startable_SetName
:	public Startable
	{
public:
	Startable_SetName(const std::string& iName, const ZP<Startable>& iStartable)
	:	fName(iName)
	,	fStartable(iStartable)
		{}

	virtual bool QCall()
		{
		ZThread::sSetName(fName.c_str());
		fStartable->QCall();
		return true;
		}

	const std::string fName;
	const ZP<Startable> fStartable;
	};

ZP<Startable> sStartable_SetName(const std::string& iName, const ZP<Startable>& iStartable)
	{
	if (iStartable)
		return new Startable_SetName(iName, iStartable);
	return null;
	}

} // namespace ZooLib
