// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZDebug.h"
#include "zoolib/Python/Util_Python.h"

namespace ZooLib {
namespace Util_Python {

// =================================================================================================
#pragma mark -

ZQ<std::string> sQAsString(PyObject* iObject)
	{
	if (iObject and PyString_Check(iObject))
		return std::string(PyString_AS_STRING(iObject), PyString_GET_SIZE(iObject));
	return null;
	}

// =================================================================================================
#pragma mark -

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

GILStateEnsurer::GILStateEnsurer()
:	fState(PyGILState_Ensure())
	{}

GILStateEnsurer::~GILStateEnsurer()
	{ PyGILState_Release(fState); }

// =================================================================================================
#pragma mark -

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

} // namespace Util_Python
} // namespace ZooLib
