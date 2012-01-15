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

#ifndef __ZYad_ZooLibStream_h__
#define __ZYad_ZooLibStream_h__ 1
#include "zconfig.h"

#include "zoolib/ZYad_Std.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZYadMapR_ZooLibStreamOld

class ZYadMapR_ZooLibStreamOld : public ZYadMapR_Std
	{
public:
	ZYadMapR_ZooLibStreamOld(ZRef<ZStreamerR> iStreamerR);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStreamerR> fStreamerR;
	size_t fCountRemaining;
	};

// =================================================================================================
// MARK: - ZYad_ZooLibStream

namespace ZYad_ZooLibStream {

ZRef<ZYadR> sYadR(ZRef<ZStreamerR> iStreamerR);

void sToStream(const ZStreamW& iStreamW, ZRef<ZYadR> iYadR);

} // namespace ZYad_ZooLibStream
} // namespace ZooLib

#endif // __ZYad_ZooLibStream_h__
