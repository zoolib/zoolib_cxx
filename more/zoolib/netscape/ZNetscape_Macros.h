/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZNetscape_Macros__
#define __ZNetscape_Macros__ 1
#include "zconfig.h"

#include "zoolib/ZLog.h"

#define ZNETSCAPE_BEFORE \
	try \
		{

#define ZNETSCAPE_AFTER_NPERROR_PRE \
	catch (NPError& err) \
		{ \
		return err; \
		}

#define ZNETSCAPE_AFTER_EXCEPTION \
	catch (std::exception& ex) \
		{ \
		if (ZLOG(s, eDebug, "ZNetscape::HostMeister")) \
			s << __FUNCTION__ << ", Caught exception: " << ex.what(); \
		} \
	catch (...) \
		{ \
		if (ZLOG(s, eDebug, "ZNetscape::HostMeister")) \
			s << __FUNCTION__ << ", Caught exception not derived from std::exception"; \
		}

#define ZNETSCAPE_AFTER_VOID \
		} \
	ZNETSCAPE_AFTER_EXCEPTION

#define ZNETSCAPE_AFTER_NPERROR \
		} \
	ZNETSCAPE_AFTER_NPERROR_PRE \
	ZNETSCAPE_AFTER_EXCEPTION \
	return NPERR_GENERIC_ERROR;

#define ZNETSCAPE_AFTER_RETURN(iVal) \
		} \
	ZNETSCAPE_AFTER_EXCEPTION \
	return iVal;

#define ZNETSCAPE_AFTER_RETURN_NIL \
	ZNETSCAPE_AFTER_RETURN(nullptr)

#define ZNETSCAPE_AFTER_RETURN_FALSE \
	ZNETSCAPE_AFTER_RETURN(false)

#endif // __ZNetscape_Macros__
