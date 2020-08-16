// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_FunctionChain_h__
#define __ZooLib_FunctionChain_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_MSVCStaticLib.h"

namespace ZooLib {

/**
\file
If you're using the MSVC or other linkers and putting a factory in a static library, be aware
that the statically-initialized factory may not be considered active code unless some other
part of the containing translation unit is active. See Compat_MSVCStaticLib.h for
more discussion and suggested workarounds.
*/

// =================================================================================================
#pragma mark - FunctionChain

template <class Result, class Param>
class FunctionChain
	{
public:
	typedef FunctionChain Base_t;
	typedef Result Result_t;
	typedef Param Param_t;

	static bool sInvoke(Result& oResult, Param iParam)
		{
		FunctionChain* head = spHead();

		// Try preferred first, then non-preferred
		for (int xx = 0; xx < 2; ++xx)
			{
			for (FunctionChain* iter = head; iter; iter = iter->fNext)
				{
				try
					{
					if (((xx == 0) == iter->fIsPreferred) && iter->Invoke(oResult, iParam))
						return true;
					}
				catch (...)
					{}
				}
			}

		return false;
		}

	static Result sInvoke(Param iParam)
		{
		Result result;
		if (sInvoke(result, iParam))
			return result;
		return Result();
		}

protected:
	FunctionChain()
	:	fIsPreferred(true)
		{
		FunctionChain*& theHead = spHead();
		fNext = theHead;
		theHead = this;
		}

	FunctionChain(bool iPreferred)
	:	fIsPreferred(iPreferred)
		{
		FunctionChain*& theHead = spHead();
		fNext = theHead;
		theHead = this;
		}

	virtual ~FunctionChain()
		{}

	virtual bool Invoke(Result& oResult, Param iParam) = 0;

private:
	static FunctionChain*& spHead()
		{
		static FunctionChain* spHead;
		return spHead;
		}

	FunctionChain* fNext;
	const bool fIsPreferred;
	};

} // namespace ZooLib

#endif // __ZooLib_FunctionChain_h__
