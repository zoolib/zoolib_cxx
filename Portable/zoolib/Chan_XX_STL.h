// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_STL_h__
#define __ZooLib_Chan_XX_STL_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

namespace ZooLib {

template <class CC>
class ContainerHolder
	{
	ContainerHolder& operator=(const ContainerHolder&) = delete;

public:
	using CC_wo_const = typename std::remove_const<CC>::type;

	ContainerHolder()
		{
		fIsPtr = false;
		new(&fContainer) CC_wo_const();
		}

	ContainerHolder(CC* iPtr)
		{
		fIsPtr = true;
		fPtr = iPtr;
		}

	ContainerHolder(CC& iContainer)
		{
		fIsPtr = false;
		new(&fContainer) CC_wo_const(iContainer);
		}

	~ContainerHolder()
		{
		if (not fIsPtr)
			fContainer.~CC_wo_const();
		}

	CC& GetContainer()
		{
		if (fIsPtr)
			return *fPtr;
		return fContainer;
		}

	union
		{
		CC* fPtr;
		CC_wo_const fContainer;
		};
	bool fIsPtr;
	};

// =================================================================================================
#pragma mark - ChanW_XX_STL

template <class CC>
class ChanW_XX_STL
:	public ChanW<typename CC::value_type>
,	public ContainerHolder<CC>
	{
	using inherited = ContainerHolder<CC>;

public:
	using inherited::ContainerHolder::ContainerHolder; // ctors

	using Container_t = CC;
	using EE = typename Container_t::value_type;

// From ChanW
	size_t Write(const EE* iSource, size_t iCount) override
		{
		Container_t& theContainer = inherited::GetContainer();
		theContainer.insert(theContainer.end(), iSource, iSource + iCount);
		return iCount;
		}
	};

// =================================================================================================
#pragma mark - ChanR_XX_STL

template <class CC>
class ChanR_XX_STL
:	public ChanR<typename CC::value_type>
,	public ContainerHolder<const CC>
	{
	using inherited = ContainerHolder<const CC>;

public:
	using inherited::ContainerHolder::ContainerHolder; // ctors

	using Container_t = const CC;
	using EE = typename Container_t::value_type;

// From ChanR
	size_t Read(EE* oDest, size_t iCount) override
		{
		return 0;
		}
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_STL_h__
