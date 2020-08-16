// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Python_Util_Python_h__
#define __ZooLib_Python_Util_Python_h__
#include "zconfig.h"

#include "zoolib/Python/Compat_Python.h"
#include "zoolib/ZQ.h"

#include <string>

namespace ZooLib {
namespace Util_Python {

// =================================================================================================
#pragma mark -

ZQ<std::string> sQAsString(PyObject* iObject);

// =================================================================================================
#pragma mark -

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

PyObject* sInvokeSafely(PyCFunction iFunc, PyObject* self, PyObject* args);

template <PyCFunction Func>
struct MethodWrapper_T
	{
	static PyObject* sFunc(PyObject* self, PyObject* args)
		{ return sInvokeSafely(Func, self, args); }
	};

} // namespace Util_Python
} // namespace ZooLib

#endif // __ZooLib_Python_Util_Python_h__
