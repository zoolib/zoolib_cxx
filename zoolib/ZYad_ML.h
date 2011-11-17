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

namespace ZooLib {

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
#pragma mark * ZYadStrimR_ML

class ZYadStrimR_ML
:	public virtual ZYadStrimR
	{
public:
	ZYadStrimR_ML(ZRef<ZML::StrimmerU> iStrimmerU);

// From ZYadR
	virtual void Finish();

// From ZStrimmerU via ZYadStrimR
	virtual const ZStrimR& GetStrimR();

// Our protocol
	ZML::StrimU& GetStrim();

private:
	ZRef<ZML::StrimmerU> fStrimmerU;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_ML

class ZYadMapR_ML
:	public virtual ZYadMapR_Std
	{
public:
	ZYadMapR_ML(ZRef<ZML::StrimmerU> iStrimmerU);
	ZYadMapR_ML(ZRef<ZML::StrimmerU> iStrimmerU, const ZML::Attrs_t& iAttrs);
	ZYadMapR_ML
		(ZRef<ZML::StrimmerU> iStrimmerU, const std::string& iTagName, const ZML::Attrs_t& iAttrs);

// From ZYadR
	virtual ZRef<ZYadR> Meta();

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

// Our protocol
	ZML::Attrs_t GetAttrs();

private:
	void pAdvance(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

	ZRef<ZML::StrimmerU> fStrimmerU;

	const std::string fTagName;
	const ZML::Attrs_t fAttrs;
	};

} // namespace ZooLib

#endif // __ZYad_ML__
