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

#ifndef __ZooLib_Yad_ML_h__
#define __ZooLib_Yad_ML_h__ 1
#include "zconfig.h"

#include "zoolib/ML.h"
#include "zoolib/Yad_Std.h"

namespace ZooLib {
namespace Yad_ML {

// =================================================================================================
#pragma mark - YadParseException_ML

class YadParseException_ML : public YadParseException_Std
	{
public:
	YadParseException_ML(const std::string& iWhat);
	YadParseException_ML(const char* iWhat);
	};

// =================================================================================================
#pragma mark - ChanR_NameRefYad

class ChanR_NameRefYad
:	public ChanR_NameRefYad_Std
	{
public:
	ChanR_NameRefYad(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF);
	ChanR_NameRefYad(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF, const ML::Attrs_t& iAttrs);
	ChanR_NameRefYad(
		ZRef<ML::ChannerRU_UTF> iChannerRU_UTF, const std::string& iTagName, const ML::Attrs_t& iAttrs);

// From ZYadR
	virtual ZRef<YadR> Meta();

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR);

// Our protocol
	ML::Attrs_t GetAttrs();

private:
	ZRef<ML::ChannerRU_UTF> fChannerRU_UTF;
	const std::string fTagName;
	const ML::Attrs_t fAttrs;
	bool fLastWasText;
	};

// =================================================================================================
#pragma mark - 

ZRef<YadR> sYadR(ZRef<ML::ChannerRU_UTF> iChannerRU_UTF);

} // namespace Yad_ML
} // namespace ZooLib

#endif // __ZooLib_Yad_ML_h__
