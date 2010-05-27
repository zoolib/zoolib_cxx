/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZYad_Simple__
#define __ZYad_Simple__ 1
#include "zconfig.h"

#include "zoolib/ZStrimmer.h"
#include "zoolib/ZYad_Std.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Simple

class ZYadParseException_Simple : public ZYadParseException_Std
	{
public:
	ZYadParseException_Simple(const std::string& iWhat);
	ZYadParseException_Simple(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Simple

class ZYadMapR_Simple : public ZYadMapR_Std
	{
public:
	ZYadMapR_Simple(ZRef<ZStrimmerU> iStrimmerU);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZStrimmerU> fStrimmerU;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_Simple

namespace ZYad_Simple {

ZRef<ZYadR> sMakeYadR(ZRef<ZStrimmerU> iStrimmerU);

} // namespace ZYad_Simple

} // namespace ZooLib

#endif // __ZYad_Simple__
