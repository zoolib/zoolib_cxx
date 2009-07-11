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

void ZooLib_CFStringNormalize(CFMutableStringRef theString, ENormForm theForm);

#if defined(kCFCoreFoundationVersionNumber10_2)

	void ZooLib_CFStringNormalize(CFMutableStringRef theString, ENormForm theForm)
		{
		::CFStringNormalize(theString, (CFStringNormalizationForm)theForm);
		}

#else

	typedef enum {} CFStringNormalizationForm;

	extern "C" typedef
		void (*CFStringNormalize_Ptr)(CFMutableStringRef, CFStringNormalizationForm);

	void ZooLib_CFStringNormalize(
		CFMutableStringRef theString, ENormForm theForm)
		{
		CFBundleRef bundleRef = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.CoreFoundation"));
		CFStringNormalize_Ptr theProc = (CFStringNormalize_Ptr)
			CFBundleGetFunctionPointerForName(bundleRef, CFSTR("CFStringNormalize"));
		theProc(theString, (CFStringNormalizationForm)theForm);
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
			ZRef<CFMutableStringRef> srm = ZUtil_CFType::sStringMutable(iParam.fString);
			ZooLib_CFStringNormalize(srm, iParam.fNormForm);
			oResult = ZUtil_CFType::sAsUTF16(srm);
			}
		return true;		
		}	
	} sFunction0;

} // namespace ZANONYMOUS

} // namespace ZUnicode
NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
