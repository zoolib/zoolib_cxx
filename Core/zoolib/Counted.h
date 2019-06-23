/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZooLib_Counted_h__
#define __ZooLib_Counted_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZP.h"
#include "zoolib/ZThread.h" // For ZMtx

namespace ZooLib {

// =================================================================================================
#pragma mark - CountedBase

class CountedBase
	{
public:
	CountedBase();
	virtual ~CountedBase();

	virtual void Initialize();
	virtual void Finalize();

	bool FinishFinalize();

	void Retain();
	void Release();
	bool IsShared() const;
	bool IsReferenced() const;

	class WPProxy;
	ZP<WPProxy> GetWPProxy();

protected:
	int pCOMAddRef();
	int pCOMRelease();

private:
	ZAtomic_t fRefCount;
	ZP<WPProxy> fWPProxy;
	};

// =================================================================================================
#pragma mark - sRetain/sRelase for CountedBase derivatives (ie Counted)

inline void sRetain(CountedBase& iObject)
	{ iObject.Retain(); }

inline void sRelease(CountedBase& iObject)
	{ iObject.Release(); }

inline void sCheck(CountedBase* iP)
	{ ZAssertStop(1, iP); }

// =================================================================================================
#pragma mark - Counted

class Counted : public virtual CountedBase
	{};

// =================================================================================================
#pragma mark - CountedBase::WPProxy

class CountedBase::WPProxy
:	public CountedWithoutFinalize
	{
private:
	WPProxy(CountedBase* iCountedBase);
	virtual ~WPProxy();

	ZP<CountedBase> pGetCountedBase();
	void pClear();

	ZMtx fMtx;
	CountedBase* fCountedBase;

	friend class CountedBase;
	friend class WPBase;
	};

// =================================================================================================
#pragma mark - WPBase

class WPBase
	{
protected:
	typedef CountedBase::WPProxy WPProxy;

	WPBase();
	~WPBase();
	WPBase(const WPBase& iOther);
	WPBase& operator=(const WPBase& iOther);

	WPBase(const ZP<WPProxy>& iWPProxy);

	void pAssign(const ZP<WPProxy>& iWPProxy);
	void pClear();

	ZP<CountedBase> pGet() const;
	ZP<WPProxy> pGetWPProxy() const;

private:
	ZP<WPProxy> fWPProxy;
	};

// =================================================================================================
#pragma mark - WP

template <class T>
class WP
:	protected WPBase
	{
public:
	WP()
		{}

	~WP()
		{}

	WP(const WP& iOther)
	:	WPBase(iOther)
		{}

	WP& operator=(const WP& iOther)
		{
		WPBase::operator=(iOther);
		return *this;
		}

	WP(const null_t&)
		{}

	template <class O>
	WP(const WP<O>& iOther)
	:	WPBase(iOther)
		{ (void)static_cast<T*>(static_cast<O*>(0)); }

	template <class O>
	WP& operator=(const WP<O>& iOther)
		{
		WPBase::operator=(iOther);
		return *this;
		}

	WP(const ZP<WPProxy>& iWPProxy)
	:	WPBase(iWPProxy)
		{}

	WP& operator=(const ZP<WPProxy>& iWPProxy)
		{
		WPBase::pAssign(iWPProxy);
		return *this;
		}

	WP(WPProxy* iWPProxy)
	:	WPBase(iWPProxy)
		{}

	WP& operator=(WPProxy* iWPProxy)
		{
		WPBase::pAssign(iWPProxy);
		return *this;
		}

	template <class O,bool Sense>
	WP(const ZRef<O,Sense>& iRef)
	:	WPBase(iRef ? iRef->GetWPProxy() : null)
		{
		// Ensures that O* converts to T*
		(void)static_cast<T*>(static_cast<O*>(0));
		}

	template <class O,bool Sense>
	WP& operator=(const ZRef<O,Sense>& iRef)
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		WPBase::pAssign(iRef ? iRef->GetWPProxy() : null);
		return *this;
		}

	void Clear()
		{ WPBase::pClear(); }

	ZP<WPProxy> GetWPProxy() const
		{ return WPBase::pGetWPProxy(); }

	ZP<T> Get() const
		{ return WPBase::pGet().template DynamicCast<T>(); }

	template <class O, bool Sense>
	operator ZRef<O,Sense>() const
		{
		(void)static_cast<T*>(static_cast<O*>(0));
		return WPBase::pGet().template DynamicCast<O>();
		}

	bool operator==(const WP& iOther) const
		{ return this->GetWPProxy() == iOther.GetWPProxy(); }

	bool operator!=(const WP& iOther) const
		{ return this->GetWPProxy() != iOther.GetWPProxy(); }

	bool operator<(const WP& iOther) const
		{ return this->GetWPProxy() < iOther.GetWPProxy(); }
	};

template <class T> using ZWeakRef = WP<T>;

// =================================================================================================
#pragma mark - WP

template <class T>
WP<T> sWP(T* iP)
	{
	if (iP)
		return WP<T>(iP->GetWPProxy());
	return null;
	}

template <class T,bool Sense>
WP<T> sWP(ZRef<T,Sense> iP)
	{
	if (iP)
		return WP<T>(iP->GetWPProxy());
	return null;
	}

// =================================================================================================
#pragma mark - sWeakRef

template <class T>
WP<T> sWeakRef(T* iP)
	{
	if (iP)
		return WP<T>(iP->GetWPProxy());
	return null;
	}

template <class T,bool Sense>
WP<T> sWeakRef(ZRef<T,Sense> iP)
	{
	if (iP)
		return WP<T>(iP->GetWPProxy());
	return null;
	}

// =================================================================================================
#pragma mark - ZP_Counted

// Useful in situations where we want the default ctor of a ZP<X> to default-ctor an X.
// e.g. sStarter_EachOnNewThread and its use of sSingleton/ZP_Counted

template <class Counted_p>
class ZP_Counted
:	public ZP<Counted_p>
	{
	typedef ZP<Counted_p> inherited;
public:
	typedef Counted_p Counted_t;

	ZP_Counted()
	:	inherited(new Counted_p)
		{}

	ZP_Counted(const ZP_Counted& iOther)
	:	inherited(iOther)
		{}

	~ZP_Counted()
		{}

	ZP_Counted& operator=(const ZP_Counted& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

	ZP_Counted(const inherited& iOther)
	:	inherited(iOther)
		{}

	ZP_Counted& operator=(const inherited& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}
	};

} // namespace ZooLib

#endif // __ZooLib_Counted_h__
