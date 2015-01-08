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

#ifndef __ZooLib_Yad_Any_h__
#define __ZooLib_Yad_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Val_Any.h"
#include "zoolib/Yad.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sYadR

ZRef<YadR> sYadR(const Any& iVal);

ZRef<YadStreamerR> sYadR(const Data_Any& iData);

ZRef<YadSeqR> sYadR(const Seq_Any& iSeq);

ZRef<YadMapR> sYadR(const Map_Any& iMap);

// =================================================================================================
// MARK: - Yad_Any

namespace Yad_Any {

ZQ<Val_Any> sQFromYadR(ZRef<YadR> iYadR);

ZQ<Val_Any> sQFromYadR(bool iRepeatedPropsAsSeq, ZRef<YadR> iYadR);

} // namespace Yad_Any

// =================================================================================================
// MARK: - sFromYadR

inline
Val_Any sFromYadR(const Val_Any& iDefault, ZRef<YadR> iYadR)
	{ return Yad_Any::sQFromYadR(iYadR).DGet(iDefault); }

} // namespace ZooLib

#endif // __ZooLib_Yad_Any_h__
