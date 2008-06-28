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

#include "zoolib/ZNet_RFCOMM.h"
#include "zoolib/ZFactoryChain.h"

// =================================================================================================
#pragma mark -
#pragma mark * Factories

//ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetNameLookup>, ZNetName_Internet::LookupParam_t);
ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetListener_RFCOMM>, ZNetListener_RFCOMM::MakeParam_t);
ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetEndpoint_RFCOMM>, ZNetEndpoint_RFCOMM::MakeParam_t);

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName_RFCOMM

ZNetName_RFCOMM::ZNetName_RFCOMM()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_RFCOMM

ZNetAddress_RFCOMM::ZNetAddress_RFCOMM()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_RFCOMM

ZNetListener_RFCOMM::ZNetListener_RFCOMM()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_RFCOMM

ZNetEndpoint_RFCOMM::ZNetEndpoint_RFCOMM()
	{}
