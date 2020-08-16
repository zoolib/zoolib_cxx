// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Python/ZRef_PyObject.h"

namespace ZooLib {

template <> void sRetain_T(PyObject*& ioObject)
	{
	if (ioObject)
		{
		PyGILState_STATE gstate = PyGILState_Ensure();
		Py_INCREF(ioObject);
		PyGILState_Release(gstate);
		}
	}

template <> void sRelease_T(PyObject* iObject)
	{
	if (iObject)
		{
		PyGILState_STATE gstate = PyGILState_Ensure();
		Py_DECREF(iObject);
		PyGILState_Release(gstate);
		}
	}

} // namespace ZooLib
