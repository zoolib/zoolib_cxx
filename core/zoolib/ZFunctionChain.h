/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZFunctionChain_h__
#define __ZFunctionChain_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_MSVCStaticLib.h"

namespace ZooLib {

/**
\file
If you're using the MSVC or other linkers and putting a factory in a static library, be aware
that the statically-initialized factory may will not be considered active code unless some other
part of the containing translation unit is active. See ZCompat_MSVCStaticLib.h for
more discussion and suggested workarounds.
*/

// =================================================================================================
// MARK: - ZFunctionChain_T

template <class Result, class Param>
class ZFunctionChain_T
	{
public:
	typedef ZFunctionChain_T Base_t;
	typedef Result Result_t;
	typedef Param Param_t;

	static bool sInvoke(Result& oResult, Param iParam)
		{
		ZFunctionChain_T* head = spHead();

		// Try preferred first, then non-preferred
		for (int xx = 0; xx < 2; ++xx)
			{
			for (ZFunctionChain_T* iter = head; iter; iter = iter->fNext)
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
	ZFunctionChain_T()
	:	fIsPreferred(true)
		{
		ZFunctionChain_T*& theHead = spHead();
		fNext = theHead;
		theHead = this;
		}

	ZFunctionChain_T(bool iPreferred)
	:	fIsPreferred(iPreferred)
		{
		ZFunctionChain_T*& theHead = spHead();
		fNext = theHead;
		theHead = this;
		}

	virtual ~ZFunctionChain_T()
		{}

	virtual bool Invoke(Result& oResult, Param iParam) = 0;

private:
	static ZFunctionChain_T*& spHead()
		{
		static ZFunctionChain_T* spHead;
		return spHead;
		}

	ZFunctionChain_T* fNext;
	const bool fIsPreferred;
	};

} // namespace ZooLib

#endif // __ZFunctionChain_h__
