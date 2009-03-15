/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZYad_StdMore__
#define __ZYad_StdMore__ 1
#include "zconfig.h"

#include "zoolib/ZYad_Std.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN
// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Std

class ZYadPrimR_Std
:	public ZYadR_Std,
	public ZYadR_TValue
	{
public:
	ZYadPrimR_Std(const ZTValue& iTV);

// From ZYadR_Std
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_Std

class ZYadStreamRPos_Std
:	public ZYadR_Std,
	public ZYadStreamRPos_MemoryBlock
	{
public:
	ZYadStreamRPos_Std(const ZMemoryBlock& iMB);

// From ZYadR_Std
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimU_Std

class ZYadStrimU_Std
:	public ZYadR_Std,
	public ZYadStrimU_String
	{
public:
	ZYadStrimU_Std(const std::string& iString);

// From ZYadR_Std
	virtual void Finish();
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYad_StdMore__
