/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZMACRO_FileFunctionLine_h__
#define __ZMACRO_FileFunctionLine_h__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark -
#pragma mark ZMACRO_HERE

namespace ZooLib {

struct ZFileFunctionLine
	{
	ZFileFunctionLine()
	:	fFileName(0)
	,	fFunctionName_Short(0)
	,	fFunctionName_Long(0)
	,	fLine(0)
		{}

	ZFileFunctionLine(const char* iFileName,
		const char* iFunctionName_Short,
		const char* iFunctionName_Long,
		int iLine)
	:	fFileName(iFileName)
	,	fFunctionName_Short(iFunctionName_Short)
	,	fFunctionName_Long(iFunctionName_Long)
	,	fLine(iLine)
		{}

	ZFileFunctionLine(const char* iFileName,
		const char* iFunctionName,
		int iLine)
	:	fFileName(iFileName)
	,	fFunctionName_Short(iFunctionName)
	,	fFunctionName_Long(iFunctionName)
	,	fLine(iLine)
		{}

	const char* fFileName;
	const char* fFunctionName_Short;
	const char* fFunctionName_Long;
	int fLine;
	};

} // namespace ZooLib

#ifndef ZMACRO_FunctionName_Short
	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901
		#define ZMACRO_FunctionName_Short __func__
	#else
		#define ZMACRO_FunctionName_Short __FUNCTION__
	#endif
#endif

#ifndef ZMACRO_FunctionName_Long
	#if ZCONFIG(Compiler, GCC)
		#define ZMACRO_FunctionName_Long __PRETTY_FUNCTION__
	#elif ZCONFIG(Compiler, MSVC)
		#define ZMACRO_FunctionName_Long __FUNCSIG__
	#else
		#define ZMACRO_FunctionName_Long ZMACRO_FunctionName_Short
	#endif
#endif

#ifndef ZMACRO_FileFunctionLine
	#define ZMACRO_FileFunctionLine \
		ZooLib::ZFileFunctionLine \
			(__FILE__, ZMACRO_FunctionName_Short, ZMACRO_FunctionName_Long, __LINE__)
#endif

#endif // __ZMACRO_FileFunctionLine_h__

