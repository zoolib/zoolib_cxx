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

#ifndef __ZSafeRef__
#define __ZSafeRef__ 1
#include "zconfig.h"

#include "zoolib/ZRef.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeRef declaration

template <class T>
class ZSafeRef
	{
	ZSafeRef(const ZSafeRef&); // Not implemented
	const ZSafeRef& operator=(const ZSafeRef&); // Not implemented

	static void spRetain(T* iP)
		{
		if (iP)
			sRetain(*iP);
		}

	static void spRelease(T* iP)
		{
		if (iP)
			sRelease(*iP);
		}

public:
	ZSafeRef()
	:	fP(nullptr)
		{}

	ZSafeRef(T* iP)
	:	fP(fP)
		{ spRetain(fP); }

	~ZSafeRef()
		{ spRelease(fP); }

	ZSafeRef& operator=(T* iP)
		{
		ZAcqMtx acq(fMtx);
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;		
		}

	template <class O>
	ZSafeRef(const ZRef<O>& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	template <class O>
	ZSafeRef& operator=(const ZRef<O>& iOther)
		{
		ZAcqMtx acq(fMtx);
		T* theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	template <class O>
	operator ZRef<O>()
		{
		ZAcqMtx acq(fMtx);
		return ZRef<O>(fP);
		}

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{
		ZAcqMtx acq(fMtx);
		return fP == iOther.Get();
		}

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{
		ZAcqMtx acq(fMtx);
		return fP != iOther.Get();
		}

	void Clear()
		{
		ZAcqMtx acq(fMtx);
		T* theP = nullptr;
		std::swap(theP, fP);
		spRelease(theP);
		}

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZSafeRef,
		operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fP); }

private:
	mutable ZMtx fMtx;
	T* fP;
	};

} // namespace ZooLib

#endif // __ZWeakRef__
