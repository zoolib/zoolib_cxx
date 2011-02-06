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

#include "zoolib/ZQ.h" // ZQ.h transitively provides most of our dependencies
#include "zoolib/ZCountedWithoutFinalize.h"

#include <typeinfo>

// =================================================================================================
#pragma mark -
#pragma mark * ZAny

namespace ZooLib {

class ZAny
	{
public:
	ZAny();
	ZAny(const ZAny& iOther);
	~ZAny();
	ZAny& operator=(const ZAny& iOther);

	template <class S>
	explicit ZAny(const S& iVal)
		{ pCtorFrom_T<S>(iVal); }

	template <class S>
	ZAny& operator=(const S& iVal)
		{
		pDtor();
		pCtorFrom_T<S>(iVal);
		return *this;
		}

	ZMACRO_operator_bool(ZAny, operator_bool) const;

	const std::type_info& Type() const;

	void* VoidStar();
	const void* ConstVoidStar() const;

// ZVal protocol, for use by ZVal derivatives
	void swap(ZAny& ioOther);

	void Clear();

	template <class S>
	S* PGetMutable()
		{ return static_cast<S*>(pGetMutable(typeid(S))); }

	template <class S>
	const S* PGet() const
		{ return static_cast<const S*>(pGet(typeid(S))); }

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
		{
		pDtor();
		pCtorFrom_T<S>(iVal);
		}

// Our protocol
	template <class S>
	bool Is() const
		{ return this->PGet<S>(); }

private:
//---
	class Holder_InPlace
		{
	public:
		virtual void CtorInto(void* iOther) const = 0;
		virtual const std::type_info& Type() const = 0;
		virtual void* VoidStar() = 0;
		virtual const void* ConstVoidStar() const = 0;
		};

	template<typename S>
	class Holder_InPlace_T : public Holder_InPlace
		{
	public:
		Holder_InPlace_T(const S& iValue) : fValue(iValue) {}

		virtual void CtorInto(void* iOther) const { sCtor_T<Holder_InPlace_T>(iOther, fValue); }
		virtual const std::type_info& Type() const { return typeid(S); }
		virtual void* VoidStar() { return &fValue; }
		virtual const void* ConstVoidStar() const { return &fValue; }

		S fValue;
		};
//---
	class Holder_Counted : public ZCountedWithoutFinalize
		{
	public:
		virtual const std::type_info& Type() const = 0;
		virtual Holder_Counted* Clone() const = 0;
		virtual void* VoidStar() = 0;
		};

	template<typename S>
	class Holder_Counted_T : public Holder_Counted
		{
	public:
		Holder_Counted_T(const S& iValue) : fValue(iValue) {}

		virtual const std::type_info& Type() const { return typeid(S); }
		virtual Holder_Counted* Clone() const { return new Holder_Counted_T(fValue); }
		virtual void* VoidStar() { return &fValue; }

		S fValue;
		};
//---
	void* pGetMutable(const std::type_info& iTypeInfo);
	const void* pGet(const std::type_info& iTypeInfo) const;

	template <class S>
	void pCtorFrom_T(const S& iVal)
		{
		if (sizeof(S) <= sizeof(fPayload))
			{
			sCtor_T<Holder_InPlace_T<S> >(fBytes_InPlace, iVal);
			}
		else
			{
			fPtr_InPlace = 0;
			sCtor_T<ZRef<Holder_Counted> >(fBytes_Counted, new Holder_Counted_T<S>(iVal));
			}
		}

	void pCtorFrom(const ZAny& iOther);
	void pDtor();

	bool pIsInPlace() const;

	// Pseudo field accesors, hence the 'f' prefix
	Holder_InPlace& fHolder_InPlace();
	const Holder_InPlace& fHolder_InPlace() const;

	ZRef<Holder_Counted>& fHolder_Counted();
	const ZRef<Holder_Counted>& fHolder_Counted() const;
//---
	union
		{
		char fBytes_InPlace[1];
		void* fPtr_InPlace;
		};

	union
		{
		char fBytes_Counted[1];
		void* fPtr_Counted;
		union
			{
			// Reserves space for in-place values, and makes
			// some types interpretible when debugging.
			char fAsChar;
			short fAsShort;
			int fAsInt;
			long fAsLong;
			double fAsDouble;
			} fPayload;
		};
	};

inline void swap(ZAny& a, ZAny& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZAny__
