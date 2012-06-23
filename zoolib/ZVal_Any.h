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

#ifndef __ZVal_Any_h__
#define __ZVal_Any_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_unordered_map.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZCompare_String.h" // For FastComparator_String
#include "zoolib/ZName.h"
#include "zoolib/ZSafeStack.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZVal_T.h"

#include <map>
#include <vector>

namespace ZooLib {

class ZSeq_Any;
class ZMap_Any;

typedef ZVal_T<ZMap_Any,ZSeq_Any> ZVal_Any;

// =================================================================================================
// MARK: - ZSeq_Any

class ZSeq_Any
	{
	class Rep;

public:
	typedef std::vector<ZVal_Any> Vector_t;
	typedef ZVal_Any Val;

	ZSeq_Any();
	ZSeq_Any(const ZSeq_Any& iOther);
	~ZSeq_Any();
	ZSeq_Any& operator=(const ZSeq_Any& iOther);

	ZSeq_Any(const Vector_t& iOther);
	ZSeq_Any& operator=(const Vector_t& iOther);

	template <class Iterator>
	ZSeq_Any(Iterator begin, Iterator end);

	int Compare(const ZSeq_Any& iOther) const;

// ZSeq protocol
	size_t Count() const;

	void Clear();

	ZVal_Any* PGetMutable(size_t iIndex);
	const ZVal_Any* PGet(size_t iIndex) const;
	const ZQ<ZVal_Any> QGet(size_t iIndex) const;
	const ZVal_Any DGet(const ZVal_Any& iDefault, size_t iIndex) const;
	const ZVal_Any& Get(size_t iIndex) const;

	template <class S>
	S* PGetMutable(size_t iIndex)
		{
		if (ZVal_Any* theVal = this->PGetMutable(iIndex))
			return theVal->PGetMutable<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(size_t iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const S& DGet(const S& iDefault, size_t iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return iDefault;
		}

	template <class S>
	const S& Get(size_t iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return sDefault<S>();
		}

	ZSeq_Any& Set(size_t iIndex, const ZVal_Any& iVal);

	ZSeq_Any& Erase(size_t iIndex);

	ZSeq_Any& Insert(size_t iIndex, const ZVal_Any& iVal);

	ZSeq_Any& Append(const ZVal_Any& iVal);

	bool operator==(const ZSeq_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZSeq_Any& r) const
		{ return this->Compare(r) < 0; }

	ZVal_Any& operator[](size_t iIndex);
	const ZVal_Any& operator[](size_t iIndex) const;

// Our protocol
	ZVal_Any& Mutable(size_t iIndex);

	template <class S>
	S& Mutable(size_t iIndex)
		{ return this->Mutable(iIndex).Mutable<S>(); }

	template <class S>
	S& DMutable(const S& iDefault, size_t iIndex)
		{ return this->Mutable(iIndex).DMutable(iDefault); }
	
private:
	void pTouch();

	ZRef<Rep> fRep;
	};

// =================================================================================================
// MARK: - ZSeq_Any::Rep

class ZSeq_Any::Rep
:	public ZCountedWithoutFinalize
	{
private:
	Rep();
	virtual ~Rep();

	Rep(const Vector_t& iVector);

	template <class Iterator>
	Rep(Iterator begin, Iterator end)
	:	fVector(begin, end)
		{}

	Vector_t fVector;
	friend class ZSeq_Any;
	};

// =================================================================================================
// MARK: - ZSeq_Any, inline templated constructor

template <class Iterator>
ZSeq_Any::ZSeq_Any(Iterator begin, Iterator end)
:	fRep(new Rep(begin, end))
	{}

// =================================================================================================
// MARK: - ZNameVal

typedef std::pair<ZName, ZVal_Any> ZNameVal;

// =================================================================================================
// MARK: - ZMap_Any

class ZMap_Any
	{
public:
	class Rep;
	typedef ZName Name_t;

#if ZCONFIG_SPI_Enabled(unordered_map)
	typedef unordered_map<Name_t, ZVal_Any> Map_t;
#else
	typedef std::map<Name_t, ZVal_Any> Map_t;
#endif

	typedef Map_t::iterator Index_t;
	typedef ZVal_Any Val_t;

	ZMap_Any();
	ZMap_Any(const ZMap_Any& iOther);
	~ZMap_Any();
	ZMap_Any& operator=(const ZMap_Any& iOther);

	ZMap_Any(const Map_t& iOther);
	ZMap_Any& operator=(Map_t& iOther);

	ZMap_Any(const ZNameVal& iNV);
	ZMap_Any(const char* iName, const char* iVal);
	ZMap_Any(const Name_t& iName, const ZVal_Any& iVal);

	int Compare(const ZMap_Any& iOther) const;

// ZMap protocol
	bool IsEmpty() const;

	void Clear();

	ZVal_Any* PGetMutable(const Name_t& iName);
	ZVal_Any* PGetMutable(const Index_t& iIndex);

	const ZVal_Any* PGet(const Name_t& iName) const;
	const ZVal_Any* PGet(const Index_t& iIndex) const;

	const ZQ<ZVal_Any> QGet(const Name_t& iName) const;
	const ZQ<ZVal_Any> QGet(const Index_t& iIndex) const;

	const ZVal_Any& DGet(const ZVal_Any& iDefault, const Name_t& iName) const;
	const ZVal_Any& DGet(const ZVal_Any& iDefault, const Index_t& iIndex) const;

	const ZVal_Any& Get(const Name_t& iName) const;
	const ZVal_Any& Get(const Index_t& iIndex) const;

	template <class S>
	S* PGetMutable(const Name_t& iName)
		{
		if (ZVal_Any* theVal = this->PGetMutable(iName))
			return theVal->PGetMutable<S>();
		return nullptr;
		}

	template <class S>
	S* PGetMutable(const Index_t& iIndex)
		{
		if (ZVal_Any* theVal = this->PGetMutable(iIndex))
			return theVal->PGetMutable<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const Name_t& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const S* PGet(const Index_t& iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	const ZQ<S> QGet(const Name_t& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const ZQ<S> QGet(const Index_t& iIndex) const
		{
		if (const ZVal_Any* theVal = this->PGet(iIndex))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	const S& DGet(const S& iDefault, const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return iDefault;
		}

	template <class S>
	const S& DGet(const S& iDefault, const Index_t& iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return iDefault;
		}

	template <class S>
	const S& Get(const Name_t& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return sDefault<S>();
		}

	template <class S>
	const S& Get(const Index_t& iIndex) const
		{
		if (const S* theVal = this->PGet<S>(iIndex))
			return *theVal;
		return sDefault<S>();
		}

	ZMap_Any& Set(const Name_t& iName, const ZVal_Any& iVal);
	ZMap_Any& Set(const Index_t& iIndex, const ZVal_Any& iVal);

	template <class S>
	ZMap_Any& Set(const Name_t& iName, const S& iVal)
		{ return this->Set(iName, ZVal_Any(iVal)); }

	template <class S>
	ZMap_Any& Set(const Index_t& iIndex, const S& iVal)
		{ return this->Set(iIndex, ZVal_Any(iVal)); }

	ZMap_Any& Erase(const Name_t& iName);
	ZMap_Any& Erase(const Index_t& iIndex);

// Our protocol
	ZVal_Any& Mutable(const Name_t& iName);

	template <class S>
	S& Mutable(const Name_t& iName)
		{ return this->Mutable(iName).Mutable<S>(); }

	template <class S>
	S& DMutable(const S& iDefault, const Name_t& iName)
		{
		if (S* theVal = this->PGetMutable<S>(iName))
			return *theVal;
		this->Set(iName, iDefault);
		return *this->PGetMutable<S>(iName);
		}
	
	Index_t Begin() const;
	Index_t End() const;

	const Name_t& NameOf(const Index_t& iIndex) const;
	Index_t IndexOf(const Name_t& iName) const;

	Index_t IndexOf(const ZMap_Any& iOther, const Index_t& iOtherIndex) const;

	ZMap_Any& Set(const ZNameVal& iNV);

	bool operator==(const ZMap_Any& r) const
		{ return this->Compare(r) == 0; }

	bool operator<(const ZMap_Any& r) const
		{ return this->Compare(r) < 0; }

	ZVal_Any& operator[](const Name_t& iName);
	const ZVal_Any& operator[](const Name_t& iName) const;

	ZVal_Any& operator[](const Index_t& iIndex);
	const ZVal_Any& operator[](const Index_t& iIndex) const;

private:
	void pTouch();
	Map_t::iterator pTouch(const Index_t& iIndex);

	ZRef<Rep> fRep;
	};

ZMap_Any operator*(const ZNameVal& iNV0, const ZNameVal& iNV1);

ZMap_Any& operator*=(ZMap_Any& ioMap, const ZNameVal& iNV);
ZMap_Any operator*(const ZMap_Any& iMap, const ZNameVal& iNV);

ZMap_Any operator*(const ZNameVal& iNV, const ZMap_Any& iMap);

ZMap_Any& operator*=(ZMap_Any& ioMap0, const ZMap_Any& iMap1);
ZMap_Any operator*(const ZMap_Any& iMap0, const ZMap_Any& iMap1);

// =================================================================================================
// MARK: - ZMap_Any::Rep

class SafeStackLink_Map_Any_Rep
:	public ZSafeStackLink<ZMap_Any::Rep,SafeStackLink_Map_Any_Rep>
	{};

class ZMap_Any::Rep
:	public ZCounted
,	public SafeStackLink_Map_Any_Rep
	{
public:
	virtual ~Rep();

private:
	Rep();

	Rep(const Map_t& iMap);

// From ZCounted
	virtual void Finalize();

// Our protocol
	static ZRef<Rep> sMake();
	static ZRef<Rep> sMake(const Map_t& iMap);

	Map_t fMap;
	friend class ZMap_Any;
	};

// =================================================================================================
// MARK: - 

ZMap_Any sAugmented(const ZMap_Any& iUnder, const ZMap_Any& iOver);

} // namespace ZooLib

#endif // __ZVal_Any_h__
