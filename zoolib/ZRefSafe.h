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

#ifndef __ZRefSafe__
#define __ZRefSafe__ 1
#include "zconfig.h"

#include "zoolib/ZRef.h"
#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZRefSafe declaration

template <class T>
class ZRefSafe
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZRefSafe,
		operator_bool_generator_type, operator_bool_type);

	ZRefSafe(const ZRefSafe&); // Not implemented
	const ZRefSafe& operator=(const ZRefSafe&); // Not implemented

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
	ZRefSafe() : fP(nullptr) {}
	ZRefSafe(T* iP)
	:	fP(fP)
		{ spRetain(fP); }

	~ZRefSafe()
		{ spRelease(fP); }

	ZRefSafe& operator=(T* iP)
		{
		ZGuardMtx guard(fMtx);
		std::swap(iP, fP);
		spRetain(fP);
		spRelease(iP);	
		return *this;		
		}

	template <class O>
	ZRefSafe(const ZRef<O>& iOther)
	:	fP(iOther.Get())
		{ spRetain(fP); }

	template <class O>
	ZRefSafe& operator=(const ZRef<O>& iOther)
		{
		ZGuardMtx guard(fMtx);
		T* theP = iOther.Get();
		std::swap(theP, fP);
		spRetain(fP);
		spRelease(theP);
		return *this;
		}

	template <class O>
	operator ZRef<O>()
		{
		ZGuardMtx guard(fMtx);
		return ZRef<O>(fP);
		}

	template <class O>
	bool operator==(const ZRef<O>& iOther) const
		{
		ZGuardMtx guard(fMtx);
		return fP == iOther.Get();
		}

	template <class O>
	bool operator!=(const ZRef<O>& iOther) const
		{
		ZGuardMtx guard(fMtx);
		return fP != iOther.Get();
		}

	void Clear()
		{
		ZGuardMtx guard(fMtx);
		T* theP = nullptr;
		std::swap(theP, fP);
		spRelease(theP);
		}

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fP); }

private:
	mutable ZMtx fMtx;
	T* fP;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZWeakRef__
