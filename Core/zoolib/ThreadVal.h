// Copyright (c) 2011-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ThreadVal_h__
#define __ZooLib_ThreadVal_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h" // For SaveSetRestore
#include "zoolib/TagVal.h"

#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ThreadVal

template <class Type_p, class Tag_p = Type_p>
class ThreadVal
:	public TagVal<Type_p,Tag_p>
	{
	typedef TagVal<Type_p,Tag_p> inherited;
public:
	typedef Type_p Type_t;
	typedef Tag_p Tag_t;

	// -----

	ThreadVal() : fSSR(spMut(), this) {}

	ThreadVal(const ThreadVal& iOther) : inherited(iOther), fSSR(spMut(), this) {}

	~ThreadVal() {}

	ThreadVal& operator=(const ThreadVal& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

	// -----

	ThreadVal(const Type_p& iVal) : inherited(iVal), fSSR(spMut(), this) {}

	ThreadVal& operator=(const Type_p& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	// -----

	static const Type_p* sPGet()
		{
		if (ThreadVal* theTV = spMut())
			return &theTV->Get();
		return nullptr;
		}

	static ZQ<Type_p> sQGet()
		{
		if (ThreadVal* theTV = spMut())
			return theTV->Get();
		return null;
		}

	static const Type_p& sGet()
		{
		if (ThreadVal* theTV = spMut())
			return theTV->Get();
		return sDefault<Type_p>();
		}

	static Type_p* sPMut()
		{
		if (ThreadVal* theTV = spMut())
			return &theTV->Mut();
		return nullptr;
		}

	static Type_p& sMut()
		{
		ThreadVal* theTV = spMut();
		ZAssert(theTV);
		return theTV->Mut();
		}

private:
	static ThreadVal*& spMut()
		{
		static thread_local ThreadVal* spCurrent;
		return spCurrent;
		}

	SaveSetRestore<ThreadVal*> fSSR;
	};

// =================================================================================================
#pragma mark - sThreadVal

template <class Type_p, class Tag_p = Type_p>
ZQ<Type_p> sQThreadVal()
	{ return ThreadVal<Type_p,Tag_p>::sQGet(); }

//template <class Type_p>
//ZQ<Type_p> sQThreadVal()
//	{ return ThreadVal<Type_p,Type_p>::sQGet(); }

template <class Type_p, class Tag_p = Type_p>
Type_p* sPThreadVal()
	{ return ThreadVal<Type_p,Tag_p>::sPMut(); }

//template <class Type_p>
//Type_p* sPThreadVal()
//	{ return ThreadVal<Type_p,Type_p>::sPMut(); }

template <class Type_p, class Tag_p = Type_p>
Type_p& sThreadVal()
	{ return ThreadVal<Type_p,Tag_p>::sMut(); }

//template <class Type_p>
//Type_p& sThreadVal()
//	{ return ThreadVal<Type_p,Type_p>::sMut(); }

} // namespace ZooLib

#endif // __ZooLib_ThreadVal_h__
