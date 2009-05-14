/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#ifndef __ZPtrUseCounted__
#define __ZPtrUseCounted__ 1

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZPtrUseCounted_T

template <class T>
class ZPtrUseCounted_T
	{
	mutable ZMutexNR fMutexNR;
	mutable ZCondition fCondition;
	T* fPtr;
	mutable size_t fUseCount;

public:
	ZPtrUseCounted_T(T* iPtr)
	:	fPtr(iPtr),
		fUseCount(0)
		{}

	~ZPtrUseCounted_T()
		{
		fMutexNR.Acquire();
		while (fUseCount)
			fCondition.Wait(fMutexNR);
		fMutexNR.Release();
		}

	T* Use() const
		{
		fMutexNR.Acquire();
		++fUseCount;
		fMutexNR.Release();
		return fPtr;
		}

	void Unuse() const
		{
		fMutexNR.Acquire();
		if (0 == --fUseCount)
			fCondition.Broadcast();
		fMutexNR.Release();
		}

	void Clear()
		{
		fMutexNR.Acquire();
		while (fUseCount)
			fCondition.Wait(fMutexNR);
		fPtr = nullptr;
		fMutexNR.Release();
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUsedPtr_T

template <class T>
class ZUsedPtr_T
	{
	const ZPtrUseCounted_T<T>& fPUC;
	T* fPtr;
public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZUsedPtr_T<T>,
    	operator_bool_generator_type, operator_bool_type);

	ZUsedPtr_T(const ZPtrUseCounted_T<T>& iPUC)
	:	fPUC(iPUC),
		fPtr(iPUC.Use())
		{}

	~ZUsedPtr_T()
		{
		fPUC.Unuse();
		}

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fPtr); }

	T* operator->() const
		{ return fPtr; }	
	};

NAMESPACE_ZOOLIB_END

#endif // __ZPtrUseCounted__
