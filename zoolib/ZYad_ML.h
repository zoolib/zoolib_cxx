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

#ifndef __ZYad_ML__
#define __ZYad_ML__ 1
#include "zconfig.h"

#include "zoolib/ZML.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_ML

class ZYadParseException_ML : public ZYadParseException_Std
	{
public:
	ZYadParseException_ML(const std::string& iWhat);
	ZYadParseException_ML(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ML

class ZYadMapR_ML : public ZYadMapR_Std
	{
public:
	ZYadMapR_ML(ZML::Reader& iR);
	ZYadMapR_ML(ZML::Reader& iR, const ZTuple& iAttrs);
	ZYadMapR_ML(ZML::Reader& iR, const std::string& iTagName, const ZTuple& iAttrs);

// From ZYadMapR_Std
	virtual void Imp_Advance(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

// Our protocol
	ZTuple GetAttrs();

private:
	void pAdvance(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR);

	ZML::Reader& fR;

	const std::string fTagName;
	const ZTuple fAttrs;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYad_ML__
