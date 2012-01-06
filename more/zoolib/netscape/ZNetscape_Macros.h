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

#ifndef __ZNetscape_Macros_h__
#define __ZNetscape_Macros_h__ 1
#include "zconfig.h"

#include "zoolib/ZLog.h"

#define ZMACRO_Netscape_Before \
	try \
		{

#define ZMACRO_Netscape_After_NPErrorPre \
	catch (NPError& err) \
		{ \
		return err; \
		}

#define ZMACRO_Netscape_After_Exception \
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

#define ZMACRO_Netscape_After_Void \
		} \
	ZMACRO_Netscape_After_Exception

#define ZMACRO_Netscape_After_NPError \
		} \
	ZMACRO_Netscape_After_NPErrorPre \
	ZMACRO_Netscape_After_Exception \
	return NPERR_GENERIC_ERROR;

#define ZMACRO_Netscape_After_Return(iVal) \
		} \
	ZMACRO_Netscape_After_Exception \
	return iVal;

#define ZMACRO_Netscape_After_Return_Nil \
	ZMACRO_Netscape_After_Return(nullptr)

#define ZMACRO_Netscape_After_Return_False \
	ZMACRO_Netscape_After_Return(false)

#endif // __ZNetscape_Macros_h__
