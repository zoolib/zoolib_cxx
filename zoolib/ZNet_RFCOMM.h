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

#ifndef __ZNet_RFCOMM__
#define __ZNet_RFCOMM__ 1

#include "zconfig.h"

#include "ZNet.h"

// Need to figure out what the accessors for all this stuff are.

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName_RFCOMM

class ZNetName_RFCOMM : public ZNetName
	{
protected:
	ZNetName_RFCOMM();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_RFCOMM

class ZNetAddress_RFCOMM : public ZNetAddress
	{
protected:
	ZNetAddress_RFCOMM();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_RFCOMM

class ZNetListener_RFCOMM : public ZNetListener
	{
protected:
	ZNetListener_RFCOMM();

public:
	struct MakeParam_t {};
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_RFCOMM

class ZNetEndpoint_RFCOMM : public ZNetEndpoint
	{
protected:
	ZNetEndpoint_RFCOMM();

public:
	struct MakeParam_t {};
	};

#endif // __ZNet_RFCOMM__
