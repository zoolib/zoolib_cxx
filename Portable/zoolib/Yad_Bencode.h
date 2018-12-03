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

#ifndef __ZooLib_Yad_Bencode_h__
#define __ZooLib_Yad_Bencode_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Yad_Std.h"

namespace ZooLib {

typedef ChanRU<byte> ChanRU_Bin;
typedef Channer<ChanRU_Bin> ChannerRU_Bin;

namespace Yad_Bencode {

// =================================================================================================
#pragma mark - ChanR_RefYad

class ChanR_RefYad
:	public ChanR_RefYad_Std
	{
public:
	ChanR_RefYad(ZRef<ChannerRU_Bin> iChanner);

// From ChanR_RefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR);

private:
	ZRef<ChannerRU_Bin> fChanner;
	};

// =================================================================================================
#pragma mark - ChanR_NameRefYad

class ChanR_NameRefYad
:	public ChanR_NameRefYad_Std
	{
public:
	ChanR_NameRefYad(ZRef<ChannerRU_Bin> iChanner);

// From ChanR_RefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR);

private:
	ZRef<ChannerRU_Bin> fChanner;
	};

// =================================================================================================
#pragma mark - sYadR

ZRef<YadR> sYadR(ZRef<ChannerRU_Bin> iChanner);

} // namespace Bencode

} // namespace ZooLib

#endif // __ZooLib_Yad_Bencode_h__
