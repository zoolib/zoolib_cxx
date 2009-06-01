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

#ifndef __ZRefWeak__
#define __ZRefWeak__ 1
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

	friend class ZRefWeakBase;
	ZWeakRefereeProxy* fWRP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRefWeakBase

class ZRefWeakBase
	{
protected:
	ZRefWeakBase();
	ZRefWeakBase(const ZRefWeakBase& iOther);
	ZRefWeakBase(ZWeakReferee* iWeakReferee);
	~ZRefWeakBase();

	void AssignFrom(const ZRefWeakBase& iOther);
	void AssignFrom(ZWeakReferee* iWeakReferee);

	void Clear();

	ZWeakReferee* LockUse() const;
	void Unlock() const;

	ZRef<ZWeakRefereeProxy> fWRP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRefWeak

template <class T>
class ZRefWeak : protected ZRefWeakBase
	{
public:
	ZRefWeak()
		{}

	~ZRefWeak()
		{}

	ZRefWeak(const ZRefWeak& iOther)
	:	ZRefWeakBase(iOther)
		{}

	ZRefWeak& operator=(const ZRefWeak& iOther)
		{
		this->AssignFrom(iOther);
		return *this;
		}

	template <class O>
	ZRefWeak(const ZRefWeak<O>& iOther)
	:	ZRefWeakBase(iOther)
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		}

	template <class O>
	ZRefWeak& operator=(const ZRefWeak<O>& iOther)
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		this->AssignFrom(iOther);
		return *this;
		}

	template <class O>
	ZRefWeak(const ZRef<O>& iRef)
	:	ZRefWeakBase(iRef.GetObject())
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		}

	template <class O>
	ZRefWeak& operator=(const ZRef<O>& iRef)
		{
		static_cast<T*>(static_cast<O*>(0)); // Ensure that T is a supertype of O
		ZRefWeakBase::AssignFrom(iRef.GetObject());
		return *this;
		}

	void Clear()
		{ ZRefWeakBase::Clear(); }

	ZRef<T> Use() const
		{
		ZRef<T> result;
		if (ZWeakReferee* theWeakReferee = ZRefWeakBase::LockUse())
			{
			result = static_cast<T*>(theWeakReferee);
			ZRefWeakBase::Unlock();
			}
		return result;
		}
	};

NAMESPACE_ZOOLIB_END

#endif // __ZWeakRef__
