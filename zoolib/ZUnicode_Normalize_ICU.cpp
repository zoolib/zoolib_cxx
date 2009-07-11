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

#include "zoolib/ZUnicode_Normalize_ICU.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(ICU)

#include "zoolib/ZFunctionChain.h"

#include "unicode/unorm.h"

NAMESPACE_ZOOLIB_BEGIN
namesapce ZUnicode {

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode, normalization, ICU

namespace ZANONYMOUS {

string16 sNormalized(const string16& iString, ENormForm iNormForm)
	{
	using namespace ZUnicode;

	UNormalizationMode theMode = UNORM_DEFAULT;
	switch (iNormForm)
		{
		case eNormForm_D: theMode = UNORM_NFD; break;
		case eNormForm_KD: theMode = UNORM_NFKD; break;
		case eNormForm_C: theMode = UNORM_NFC; break;
		case eNormForm_KC: theMode = UNORM_NFKC; break;
		}

	if (size_t sourceSize = iString.size())
		{
		UErrorCode status = U_ZERO_ERROR;
		size_t neededSize = ::unorm_normalize(
			iString.data(), sourceSize, theMode, 0,
			nullptr, 0, &status);

		if (!status)
			{
			string16 result(neededSize, UTF16());
			::unorm_normalize(
				iString.data(), sourceSize, theMode, 0,
				const_cast<UTF16*>(result.data()), result.size(), &status);
			if (!status)
				return result;
			}
		}
	return iString;
	}

class Function
:	public ZFunctionChain_T<string16, const Param_Normalize&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = sNormalized(iParam.fString, iParam.fNormForm);
		return true;		
		}	
	} sFunction0;

} // namespace ZANONYMOUS

} // namespace ZUnicode
NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(ICU)
