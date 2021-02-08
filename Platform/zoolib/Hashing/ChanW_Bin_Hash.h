// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanW_Bin_Hash_h__
#define __ZooLib_ChanW_Bin_Hash_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin_Hash

template <class Hash_p>
class ChanW_Bin_Hash
:	public ChanW_Bin
	{
public:
	typedef uint8 Hash_t[Hash_p::kHashSize];

	ChanW_Bin_Hash()
		{
		Hash_p::sInit(fContext);
		}

	virtual size_t Write(const byte* iSource, size_t iCount)
		{
		Hash_p::sUpdate(fContext, iSource, iCount);
		return iCount;
		}

	void GetHash(Hash_t oHash) const
		{
		typename Hash_p::Context tempContext = fContext;
		Hash_p::sFinal(tempContext, oHash);
		}

protected:
	typename Hash_p::Context fContext;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_Hash_h__
