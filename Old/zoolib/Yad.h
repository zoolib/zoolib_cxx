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

#ifndef __ZooLib_Yad_h__
#define __ZooLib_Yad_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Name.h"
#include "zoolib/ParseException.h"
#include "zoolib/SafePtrStack.h"
#include "zoolib/YadOptions.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - YadR

typedef ZCounted YadR;
typedef ZRef<YadR> RefYad;

// =================================================================================================
#pragma mark - YadAtomR

class YadAtomR
:	public virtual YadR
	{
public:
// Our protocol
	virtual Any AsAny() = 0;
	};

// =================================================================================================
#pragma mark - YadStreamerR

typedef ChannerR_Bin YadStreamerR;

// =================================================================================================
#pragma mark - YadStrimmerR

typedef ChannerR_UTF YadStrimmerR;

// =================================================================================================
#pragma mark - YadSeqR

using ChanR_RefYad = DeriveFrom<ChanAspect_Read<RefYad>>;

using YadSeqR = Channer<ChanR_RefYad>;

inline RefYad sReadInc(const ChanR_RefYad& iChan)
	{
	RefYad result;
	if (1 == sRead(iChan, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadInc(const ZRef<YadSeqR>& iYadSeqR)
	{ return sReadInc(*iYadSeqR); }

// =================================================================================================
#pragma mark - YadSeqAtR

using ChanAtR_RefYad = DeriveFrom<ChanAspect_ReadAt<uint64,RefYad>,ChanAspect_Size>;
using YadSeqAtR = Channer<ChanAtR_RefYad>;

inline RefYad sReadAt(const ChanReadAt<uint64,RefYad>& iChan, uint64 iLoc)
	{
	RefYad result;
	if (1 == sReadAt(iChan, iLoc, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadAt(const ZRef<YadSeqAtR>& iYadSeqAtR, uint64 iLoc)
	{ return sReadAt(*iYadSeqAtR, iLoc); }

// =================================================================================================
#pragma mark - YadSeqAtRPos

using ChanRPos_RefYad = DeriveFrom<ChanAspect_Read<RefYad>,ChanAspect_ReadAt<uint64,RefYad>,ChanAspect_Size>;
using YadSeqAtRPos = Channer<ChanRPos_RefYad>;

// =================================================================================================
#pragma mark - YadMapR

typedef std::pair<Name,RefYad> NameRefYad;

using ChanR_NameRefYad = DeriveFrom<ChanAspect_Read<NameRefYad>>;

using YadMapR = Channer<ChanR_NameRefYad>;

inline RefYad sReadInc(const ChanR_NameRefYad& iChan, Name& oName)
	{
	NameRefYad result;
	if (1 == sRead(iChan, &result, 1))
		{
		oName = result.first;
		return result.second;
		}
	return null;
	}

inline RefYad sReadInc(const ZRef<YadMapR>& iYadMapR, Name& oName)
	{ return sReadInc(*iYadMapR, oName); }

// =================================================================================================
#pragma mark - YadMapAtR

using ChanAtR_NameRefYad = DeriveFrom<ChanAspect_ReadAt<Name,RefYad>>;
using YadMapAtR = Channer<ChanAtR_NameRefYad>;

inline RefYad sReadAt(const ChanReadAt<Name,RefYad>& iChan, const Name& iLoc)
	{
	RefYad result;
	if (1 == sReadAt(iChan, iLoc, &result, 1))
		return result;
	return null;
	}

inline RefYad sReadAt(const ZRef<YadMapAtR>& iYadMapAtR, const Name& iLoc)
	{ return sReadAt(*iYadMapAtR, iLoc); }

// =================================================================================================
#pragma mark - YadMapAtRPos

using ChanRPos_NameRefYad = DeriveFrom<ChanAspect_Read<NameRefYad>,ChanAspect_ReadAt<Name,RefYad>>;
using YadMapAtRPos = Channer<ChanRPos_NameRefYad>;

} // namespace ZooLib

#endif // __ZooLib_Yad_h__
