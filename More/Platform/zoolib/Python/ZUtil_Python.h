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

#ifndef __ZUtil_Python_h__
#define __ZUtil_Python_h__
#include "zconfig.h"

#include "zoolib/ZCompat_Python.h"
#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {
namespace ZUtil_Python {

// =================================================================================================
#pragma mark -
#pragma mark 

ZQ<std::string> sQAsString(PyObject* iObject);

// =================================================================================================
#pragma mark -
#pragma mark 

class ThreadStateReleaser
	{
public:
	ThreadStateReleaser();
	~ThreadStateReleaser();

	void Acquire();
	void Release();

private:
	PyThreadState* fState;
	};

// =================================================================================================
#pragma mark -
#pragma mark 

class GILStateEnsurer
	{
public:
	GILStateEnsurer();
	~GILStateEnsurer();

private:
	const PyGILState_STATE fState;
	};

// =================================================================================================
#pragma mark -
#pragma mark 

PyObject* sInvokeSafely(PyCFunction iFunc, PyObject* self, PyObject* args);

template <PyCFunction Func>
struct MethodWrapper_T
	{
	static PyObject* sFunc(PyObject* self, PyObject* args)
		{ return sInvokeSafely(Func, self, args); }
	};

} // namespace ZUtil_Python
} // namespace ZooLib

#endif // __ZUtil_Python_h__
