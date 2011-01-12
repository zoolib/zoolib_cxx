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

#ifndef __ZAny__
#define __ZAny__
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // For std::swap
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZQ.h"

#include <typeinfo>

// =================================================================================================
#pragma mark -
#pragma mark * ZAny

namespace ZooLib {

class ZAny
	{
public:
	ZMACRO_operator_bool(ZAny, operator_bool) const
		{ return operator_bool_gen::translate(fRep); }

	ZAny()
	:	fRep(0)
		{}

	ZAny(const ZAny& iOther)
	:	fRep(iOther.fRep ? iOther.fRep->Clone() : 0)
		{}
	
	~ZAny()
		{ delete fRep; }

	ZAny& operator=(ZAny iOther)
		{
		this->swap(iOther);
		return *this;
		}

	template <class S>
	explicit ZAny(const S& iVal)
	:	fRep(new Rep_T<S>(iVal))
		{}

	template <class S>
	ZAny& operator=(const S& iVal)
		{
		ZAny(iVal).swap(*this);
		return *this;
		}

	void swap(ZAny& ioOther)
		{ std::swap(fRep, ioOther.fRep); }

	const std::type_info& Type() const
		{
		if (fRep)
			return fRep->Type();
		return typeid(void);
		}

	const void* VoidStar() const
		{
		if (fRep)
			return fRep->VoidStar();
		return 0;
		}

	void* VoidStar()
		{
		if (fRep)
			return fRep->VoidStar();
		return 0;
		}

// ZVal protocol, for use by ZVal derivatives
	void Clear()
		{ ZAny().swap(*this); }

	template <class S>
	S* PGet()
		{
		if (!fRep || fRep->Type() != typeid(S))
			return 0;
		return &static_cast<Rep_T<S>*>(fRep)->fValue;
		}

	template <class S>
	const S* PGet() const
		{
		if (!fRep || fRep->Type() != typeid(S))
			return 0;
		return &static_cast<Rep_T<S>*>(fRep)->fValue;
		}

	template <class S>
	ZQ<S> QGet() const
		{
		if (const S* theVal = this->PGet<S>())
			return *theVal;
		return null;
		}

	template <class S>
	S DGet(const S& iDefault) const
		{
		if (const S* theVal = this->PGet<S>())
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get() const
		{
		if (const S* theVal = this->PGet<S>())
			return *theVal;
		return S();
		}

	template <class S>
	void Set(const S& iVal)
		{ ZAny(iVal).swap(*this); }

// Our protocol
	template <class S>
	bool Is() const
		{ return this->PGet<S>(); }

private:
	class RepBase
		{
	public:
		virtual ~RepBase() {}

		virtual const std::type_info& Type() const = 0;
		virtual RepBase* Clone() const = 0;
		virtual void* VoidStar() = 0;
		};

	template<typename S>
	class Rep_T : public RepBase
		{
	public:
		Rep_T(const S& iValue)
		:	fValue(iValue)
			{}

		virtual const std::type_info& Type() const
			{ return typeid(S); }

		virtual RepBase* Clone() const
			{ return new Rep_T(fValue); }

		virtual void* VoidStar()
			{ return &fValue; }

		S fValue;
		};

	RepBase* fRep;
	};

inline void swap(ZAny& a, ZAny& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZAny__
