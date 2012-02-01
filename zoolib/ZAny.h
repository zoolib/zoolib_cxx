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

#ifndef __ZAny_h__
#define __ZAny_h__
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZQ.h" 
#include "zoolib/ZRef.h"

#include <typeinfo> // for std::type_info

#if ZCONFIG(Compiler,GCC)
	#include <tr1/type_traits> // For std::tr1::is_pod
#endif

// =================================================================================================
// MARK: - ZAny

namespace ZooLib {

template <class S>
struct ZAnyTraits
	{
	enum { eAllowInPlace = 1 };
	};

class ZAny
	{
public:
	ZAny();
	ZAny(const ZAny& iOther);
	~ZAny();
	ZAny& operator=(const ZAny& iOther);

	ZAny(const null_t&);
	ZAny& operator=(const null_t&);

	template <class S>
	explicit ZAny(const S& iVal)
		{ pCtor_T<S>(iVal); }

	template <class S>
	ZAny& operator=(const S& iVal);

	const std::type_info& Type() const;

	void* VoidStar();
	const void* ConstVoidStar() const;

// ZVal protocol, generally for use by ZVal derivatives
	void swap(ZAny& ioOther);

	bool IsNull() const;

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
		pCtor_T<S>(iVal);
		}

// Our protocol
	template <class S>
	bool Is() const
		{ return this->PGet<S>(); }

	template <class S>
	S& Mutable()
		{
		if (S* theP = this->PGetMutable<S>())
			return *theP;
		pDtor();
		return pCtorRet_T<S>();
		}

	template <class S>
	S& DMutable(const S& iDefault)
		{
		if (S* theP = this->PGetMutable<S>())
			return *theP;
		pDtor();
		return pCtorRet_T<S>(iDefault);
		}

// Special purpose constructors, called by sAny and sAnyCounted
	template <class S, class P0>
	ZAny(const S* dummy, const P0& iP0)
		{ pCtor_T<S>(iP0); }

	template <class S, class P0, class P1>
	ZAny(const S* dummy, const P0& iP0, const P1& iP1)
		{ pCtor_T<S>(iP0, iP1); }

	template <class S, class P0>
	ZAny(const S* dummy, const P0& iP0, const IKnowWhatIAmDoing_t&)
		{ pCtor_Counted_T<S>(iP0); }

	template <class S, class P0, class P1>
	ZAny(const S* dummy, const P0& iP0, const P1& iP1, const IKnowWhatIAmDoing_t&)
		{ pCtor_Counted_T<S>(iP0, iP1); }

private:
// -----------------

	class InPlace
		{
	public:
		virtual ~InPlace() {}

		virtual void CtorInto(void* iOther) const = 0;

		virtual const std::type_info& Type() const = 0;

		virtual void* VoidStar() = 0;
		virtual const void* ConstVoidStar() const = 0;

		virtual void* VoidStarIf(const std::type_info& iTI) = 0;
		virtual const void* ConstVoidStarIf(const std::type_info& iTI) const = 0;
		};

// -----------------

	template<typename S>
	class InPlace_T : public InPlace
		{
	public:
		InPlace_T() {}

		template <class P0>
		InPlace_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		InPlace_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual void CtorInto(void* iOther) const { sCtor_T<InPlace_T>(iOther, fValue); }

		virtual const std::type_info& Type() const { return typeid(S); }

		virtual void* VoidStar() { return &fValue; }
		virtual const void* ConstVoidStar() const { return &fValue; }

		virtual void* VoidStarIf(const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}
		virtual const void* ConstVoidStarIf(const std::type_info& iTI) const
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		S fValue;
		};

// -----------------

	class Reffed : public ZCountedWithoutFinalize
		{
	public:
		virtual const std::type_info& Type() const = 0;
		virtual Reffed* Clone() const = 0;
		virtual void* VoidStar() = 0;
		virtual void* VoidStarIf(const std::type_info& iTI) = 0;
		};

// -----------------

	template<typename S>
	class Reffed_T : public Reffed
		{
	public:
		Reffed_T() {}

		template <class P0>
		Reffed_T(const P0& iP0) : fValue(iP0) {}

		template <class P0, class P1>
		Reffed_T(const P0& iP0, const P1& iP1) : fValue(iP0, iP1) {}

		virtual const std::type_info& Type() const { return typeid(S); }
		virtual Reffed* Clone() const { return new Reffed_T(fValue); }
		virtual void* VoidStar() { return &fValue; }
		virtual void* VoidStarIf(const std::type_info& iTI)
			{
			if (iTI == typeid(S))
				return &fValue;
			return 0;
			}

		S fValue;
		};

// -----------------

	InPlace& pAsInPlace();
	const InPlace& pAsInPlace() const;

	ZRef<Reffed>& pAsReffed();
	const ZRef<Reffed>& pAsReffed() const;

// -----------------

	void* pGetMutable(const std::type_info& iTypeInfo);
	const void* pGet(const std::type_info& iTypeInfo) const;

	void pCtor(const ZAny& iOther);
	void pCtor_NonPOD(const ZAny& iOther);

	void pDtor();
	void pDtor_NonPOD();

	static bool spIsPOD(const void* iPtr);

// -----------------

	template <class S, class P0, class P1>
	void pCtor_T(const P0& iP0, const P1& iP1)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			sCtor_T<InPlace_T<S> >(&fDistinguisher, iP0, iP1);
			}
		else
			{
			fDistinguisher = 0;
			sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0, iP1));
			}
		}

	template <class S, class P0, class P1>
	void pCtor_Counted_T(const P0& iP0, const P1& iP1)
		{
		fDistinguisher = 0;
		sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0, iP1));
		}

	template <class S, class P0>
	void pCtor_T(const P0& iP0)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG(Compiler,GCC)
			else if (std::tr1::is_pod<S>::value)
				{
				fDistinguisher = (void*)(((intptr_t)&typeid(S)) | 1);
				sCtor_T<S>(&fPayload, iP0);
				}
			#endif
			else
				{
				sCtor_T<InPlace_T<S> >(&fDistinguisher, iP0);
				}
			}
		else
			{
			fDistinguisher = 0;
			sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0));
			}
		}

	template <class S, class P0>
	void pCtor_Counted_T(const P0& iP0)
		{
		fDistinguisher = 0;
		sCtor_T<ZRef<Reffed> >(&fPayload, new Reffed_T<S>(iP0));
		}

// -----------------

	template <class S, class P0>
	S& pCtorRet_T(const P0& iP0)
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG(Compiler,GCC)
			else if (std::tr1::is_pod<S>::value)
				{
				fDistinguisher = (void*)(((intptr_t)&typeid(S)) | 1);
				return *sCtor_T<S>(&fPayload, iP0);
				}
			#endif
			else
				{
				return sCtor_T<InPlace_T<S> >(&fDistinguisher, iP0)->fValue;
				}
			}
		else
			{
			fDistinguisher = 0;
			Reffed_T<S>* theReffed = new Reffed_T<S>(iP0);
			sCtor_T<ZRef<Reffed> >(&fPayload, theReffed);
			return theReffed->fValue;
			}
		}

	template <class S>
	S& pCtorRet_T()
		{
		if (ZAnyTraits<S>::eAllowInPlace && sizeof(S) <= sizeof(fPayload))
			{
			if (false)
				{}
			#if ZCONFIG(Compiler,GCC)
			else if (std::tr1::is_pod<S>::value)
				{
				fDistinguisher = (void*)(((intptr_t)&typeid(S)) | 1);
				return *sCtor_T<S>(&fPayload);
				}
			#endif
			else
				{
				return sCtor_T<InPlace_T<S> >(&fDistinguisher)->fValue;
				}
			}
		else
			{
			fDistinguisher = 0;
			Reffed_T<S>* theReffed = new Reffed_T<S>;
			sCtor_T<ZRef<Reffed> >(&fPayload, theReffed);
			return theReffed->fValue;
			}
		}

// -----------------
	// There are three situations, distinguished by the value in fDistinguisher.
	// 1. It's null. fPayload.fAsPtr points to an instance of a Reffed subclass. If
	//    fPayload.fAsPtr is also null then the ZAny is itself a null object.
	// 2. LSB is set. With an unset LSB it points to a typeid, and fPayload holds a POD value.
	// 3. LSB is unset. It's the vptr of an InPlace, the fields of the object itself
	//    spilling over into fPayload.
	
	void* fDistinguisher;

	union
		{
		// This union provides space for a refcounted pointer to a Reffed, space
		// for the most common in-place values, and makes some values legible in a debugger.
		// It has it's own name so that sizeof has something on which to operate.
		void* fAsPtr;

		bool fAsBool;
		char fAsChar;
		short fAsShort;
		int fAsInt;
		long fAsLong;
		int64 fAsLongLong;
		float fAsFloat;
		double fAsDouble;
		} fPayload;
	};

// =================================================================================================
// MARK: - ZAny, internal implementation inlines

inline bool ZAny::spIsPOD(const void* iPtr)
	{ return ((intptr_t)iPtr) & 1; }

inline void ZAny::pCtor(const ZAny& iOther)
	{
	if (spIsPOD(iOther.fDistinguisher))
		{
		fDistinguisher = iOther.fDistinguisher;
		fPayload = iOther.fPayload;
		}
	else
		{
		pCtor_NonPOD(iOther);
		}
	}

inline void ZAny::pDtor()
	{
	if (not spIsPOD(fDistinguisher))
		pDtor_NonPOD();
	}

// =================================================================================================
// MARK: - ZAny, public inlines

inline ZAny::ZAny()
	{
	fDistinguisher = 0;
	fPayload.fAsPtr = 0;
	}

inline ZAny::ZAny(const ZAny& iOther)
	{ pCtor(iOther); }

inline ZAny::~ZAny()
	{ pDtor(); }

inline ZAny& ZAny::operator=(const ZAny& iOther)
	{
	if (this != &iOther)
		{
		pDtor();
		pCtor(iOther);
		}
	return *this;
	}

inline ZAny::ZAny(const null_t&)
	{}

inline ZAny& ZAny::operator=(const null_t&)
	{
	this->Clear();
	return *this;
	}

template <class S>
inline ZAny& ZAny::operator=(const S& iVal)
	{
	pDtor();
	pCtor_T<S>(iVal);
	return *this;
	}

// =================================================================================================
// MARK: - ZAny, swap and pseudo-constructors

inline void swap(ZAny& a, ZAny& b)
	{ a.swap(b); }

template <class S, class P0>
ZAny sAny(const P0& iP0)
	{ return ZAny(static_cast<S*>(0), iP0); }

template <class S, class P0, class P1>
ZAny sAny(const P0& iP0, const P1& iP1)
	{ return ZAny(static_cast<S*>(0), iP0, iP1); }

template <class S, class P0>
ZAny sAnyCounted(const P0& iP0)
	{ return ZAny(static_cast<S*>(0), iP0, IKnowWhatIAmDoing); }

template <class S, class P0, class P1>
ZAny sAnyCounted(const P0& iP0, const P1& iP1)
	{ return ZAny(static_cast<S*>(0), iP0, iP1, IKnowWhatIAmDoing); }

} // namespace ZooLib

#endif // __ZAny_h__
