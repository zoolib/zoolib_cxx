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

#include "zoolib/ZUnicode_Normalize_CF.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZUtil_CFType.h"

#include ZMACINCLUDE2(CoreFoundation,CFString.h)

NAMESPACE_ZOOLIB_BEGIN
namespace ZUnicode {

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode, normalization, CF

namespace ZANONYMOUS {

#if defined(kCFCoreFoundationVersionNumber10_2)

	void spCFStringNormalize(CFMutableStringRef iString, CFStringNormalizationForm iNF)
		{ ::CFStringNormalize(iString, iNF); }

#else

	typedef enum
		{
		kCFStringNormalizationFormD = 0,
		kCFStringNormalizationFormKD,
		kCFStringNormalizationFormC,
		kCFStringNormalizationFormKC
		} CFStringNormalizationForm;

	extern "C" typedef
		void (*CFStringNormalize_Ptr)(CFMutableStringRef, CFStringNormalizationForm);

	void spCFStringNormalize(
		CFMutableStringRef iString, CFStringNormalizationForm iNF)
		{
		CFBundleRef bundleRef = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.CoreFoundation"));
		if (CFStringNormalize_Ptr theProc = (CFStringNormalize_Ptr)
			CFBundleGetFunctionPointerForName(bundleRef, CFSTR("CFStringNormalize")))
			{
			theProc(iString, iNF);
			}
		}

#endif

class Function
:	public ZFunctionChain_T<string16, const Param_Normalize&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (iParam.fString.empty())
			{
			oResult.clear();
			}
		else
			{
			CFStringNormalizationForm theNF;
			switch (iParam.fNormForm)
				{
				case eNormForm_D: theNF = kCFStringNormalizationFormD; break;
				case eNormForm_KD: theNF = kCFStringNormalizationFormKD; break;
				case eNormForm_C: theNF = kCFStringNormalizationFormC; break;
				case eNormForm_KC: theNF = kCFStringNormalizationFormKC; break;
				}

			ZRef<CFMutableStringRef> srm = ZUtil_CFType::sStringMutable(iParam.fString);
			spCFStringNormalize(srm, theNF);
			oResult = ZUtil_CFType::sAsUTF16(srm);
			}
		return true;		
		}	
	} sFunction0;

} // namespace ZANONYMOUS

} // namespace ZUnicode
NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
