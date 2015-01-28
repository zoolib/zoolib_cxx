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

#include "zoolib/ZDebug.h"
#include "zoolib/ZUtil_Python.h"

namespace ZooLib {
namespace ZUtil_Python {

// =================================================================================================
#pragma mark -
#pragma mark 

ZQ<std::string> sQAsString(PyObject* iObject)
	{
	if (iObject and PyString_Check(iObject))
		return std::string(PyString_AS_STRING(iObject), PyString_GET_SIZE(iObject));
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark 

ThreadStateReleaser::ThreadStateReleaser()
:	fState(PyEval_SaveThread())
	{}

ThreadStateReleaser::~ThreadStateReleaser()
	{
	if (fState)
		PyEval_RestoreThread(fState);
	}

void ThreadStateReleaser::Acquire()
	{
	ZAssert(fState);
	PyEval_RestoreThread(fState);
	fState = nullptr;
	}

void ThreadStateReleaser::Release()
	{
	ZAssert(not fState);
	fState = PyEval_SaveThread();
	}

// =================================================================================================
#pragma mark -
#pragma mark 

GILStateEnsurer::GILStateEnsurer()
:	fState(PyGILState_Ensure())
	{}

GILStateEnsurer::~GILStateEnsurer()
	{ PyGILState_Release(fState); }

// =================================================================================================
#pragma mark -
#pragma mark 

PyObject* sInvokeSafely(PyCFunction iFunc, PyObject* self, PyObject* args)
	{
	try
		{
		if (PyObject* result = iFunc(self, args))
			return result;
		return Py_INCREF(Py_None), Py_None;
		}
	catch (std::exception& ex)
		{
		PyErr_SetString(PyExc_RuntimeError, ex.what());
		}
	catch (...)
		{
		PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
		}
	return nullptr;
	}


} // namespace ZUtil_Python
} // namespace ZooLib
