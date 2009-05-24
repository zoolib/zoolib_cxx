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

#ifndef __ZWeakRef__
#define __ZWeakRef__ 1
#include "zconfig.h"

#include "zoolib/ZRef.h"

NAMESPACE_ZOOLIB_BEGIN

class ZWeakRefereeProxy;

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakReferee

class ZWeakReferee
	{
public:
	ZWeakReferee();
	~ZWeakReferee();

protected:
	void pDetachProxy();

private:
	ZWeakRefereeProxy* pGetWeakRefereeProxy();

	friend class ZWeakRefBase;
	ZWeakRefereeProxy* fWRP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRefBase

class ZWeakRefBase
	{
protected:
	ZWeakRefBase();
	ZWeakRefBase(const ZWeakRefBase& iOther);
	ZWeakRefBase(ZWeakReferee* iWeakReferee);
	~ZWeakRefBase();

	void AssignFrom(const ZWeakRefBase& iOther);
	void AssignFrom(ZWeakReferee* iWeakReferee);

	void Clear();

	ZWeakReferee* LockGet() const;
	void Unlock() const;

	ZRef<ZWeakRefereeProxy> fWRP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWeakRef

template <class T>
class ZWeakRef : protected ZWeakRefBase
	{
public:
	ZWeakRef()
		{}

	~ZWeakRef()
		{}

	ZWeakRef(const ZWeakRef& iOther)
	:	ZWeakRefBase(iOther)
		{}

	ZWeakRef& operator=(const ZWeakRef& iOther)
		{
		this->AssignFrom(iOther);
		return *this;
		}

	template <class O>
	ZWeakRef(const ZWeakRef<O>& iOther)
	:	ZWeakRefBase(iOther)
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		}

	template <class O>
	ZWeakRef& operator=(const ZWeakRef<O>& iOther)
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		this->AssignFrom(iOther);
		return *this;
		}

	template <class O>
	ZWeakRef(const ZRef<O>& iRef)
	:	ZWeakRefBase(iRef.GetObject())
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		}

	template <class O>
	ZWeakRef& operator=(const ZRef<O>& iRef)
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		ZWeakRefBase::AssignFrom(iRef.GetObject());
		return *this;
		}

	void Clear()
		{ ZWeakRefBase::Clear(); }

	operator ZRef<T>() const
		{
		ZRef<T> result;
		if (ZWeakReferee* theWeakReferee = ZWeakRefBase::LockGet())
			{
			result = static_cast<T*>(theWeakReferee);
			ZWeakRefBase::Unlock();
			}
		return result;
		}
	};

NAMESPACE_ZOOLIB_END

#endif // __ZWeakRef__
