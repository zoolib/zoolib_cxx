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

#ifndef __ZUtil_AppleEvent__
#define __ZUtil_AppleEvent__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(AppleEvent)

#include "zoolib/ZHandle_T.h"
#include "zoolib/ZTypes.h"

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
#	include ZMACINCLUDE3(ApplicationServices,AE,AEDataModel.h)
#else
#	include ZMACINCLUDE3(CoreServices,AE,AEDataModel.h)
#endif

#include ZMACINCLUDE3(CoreServices,CarbonCore,Aliases.h)
#include ZMACINCLUDE3(CoreServices,CarbonCore,Files.h)
#include ZMACINCLUDE3(CoreServices,CarbonCore,MacTypes.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZAETypeMap_T

template <class T> struct ZAETypeMap_T {};

#define ZMACRO_AETypeMap(cpp_t, DescType) \
	template <> struct ZAETypeMap_T<cpp_t> { enum { sDescType = DescType }; }

ZMACRO_AETypeMap(int16, typeSInt16);
ZMACRO_AETypeMap(int32, typeSInt32);
ZMACRO_AETypeMap(uint32, typeUInt32);
ZMACRO_AETypeMap(int64, typeSInt64);
ZMACRO_AETypeMap(float, typeIEEE32BitFloatingPoint);
ZMACRO_AETypeMap(double, typeIEEE64BitFloatingPoint);
ZMACRO_AETypeMap(FSRef, typeFSRef);
ZMACRO_AETypeMap(FSSpec, typeFSS);
ZMACRO_AETypeMap(AliasHandle, typeAlias);
ZMACRO_AETypeMap(ZHandle_T<AliasHandle>, typeAlias);

// =================================================================================================
#pragma mark -
#pragma mark * ZAEValRef_T, generic

template <class T, int D = ZAETypeMap_T<T>::sDescType>
class ZAEValRef_T
	{
public:
	enum { sDescType = D };
	const T& fVal;

	ZAEValRef_T(const T& iVal)
	:	fVal(iVal)
		{}

	const void* Ptr() const
		{ return &fVal; }

	size_t Size() const
		{ return sizeof(fVal); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAEValRef_T, specialized to disable pointer use

template <class T, int D>
class ZAEValRef_T<T*, D>
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZAEValRef_T, specialized for handles

template <class T, int D>
class ZAEValRef_T<T**, D>
	{
public:
	enum { sDescType = D };

	Handle fHandle;

	ZAEValRef_T(T** iHandle)
	:	fHandle((Handle)iHandle)
		{ ::HLock(fHandle); }

	~ZAEValRef_T()
		{ ::HUnlock(fHandle); }

	const void* Ptr() const
		{ return fHandle[0]; }

	size_t Size() const
		{ return ::GetHandleSize(fHandle); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAEValRef_T, specialized for ZHandle_T

template <class T, int D>
class ZAEValRef_T<ZHandle_T<T>, D>
	{
public:
	enum { sDescType = D };

	const ZHandle_T<T>& fHandle;

	ZAEValRef_T(const ZHandle_T<T>& iHandle)
	:	fHandle(iHandle)
		{ fHandle.Lock(); }

	~ZAEValRef_T()
		{ fHandle.Unlock(); }

	const void* Ptr() const
		{ return fHandle.Get()[0]; }

	size_t Size() const
		{ return fHandle.Size(); }
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(AppleEvent)

#endif // __ZUtil_AppleEvent__
