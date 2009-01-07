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

#include "zoolib/ZUnicode_Normalize.h"

#if ZCONFIG_API_Enabled(Unicode_Normalize)

#include "zoolib/ZDebug.h"
#include "zoolib/ZUtil_CFType.h"


#if ZCONFIG_SPI_Enabled(CFType)
#	include ZMACINCLUDE2(CoreFoundation,CFString.h)
#endif

#if ZCONFIG_SPI_Enabled(Win)
#	include "zoolib/ZWinHeader.h"
#endif

#if ZCONFIG_SPI_Enabled(ICU)
#	include "unicode/unorm.h"
#endif


NAMESPACE_ZOOLIB_USING

// =================================================================================================

#if ZCONFIG_SPI_Enabled(CFType)

void ZooLib_CFStringNormalize(CFMutableStringRef theString, ZUnicode::ENormForm theForm);

#if defined(kCFCoreFoundationVersionNumber10_2)

	void ZooLib_CFStringNormalize(CFMutableStringRef theString, ZUnicode::ENormForm theForm)
		{
		// CFStringNormalize is available
		::CFStringNormalize(theString, (CFStringNormalizationForm)theForm);
		}

#else

	typedef enum {} CFStringNormalizationForm;

	extern "C" typedef
		void (*CFStringNormalize_Ptr)(CFMutableStringRef, CFStringNormalizationForm);

	void ZooLib_CFStringNormalize(
		CFMutableStringRef theString, ZUnicode::ENormForm theForm)
		{
		CFBundleRef bundleRef = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.CoreFoundation"));
		CFStringNormalize_Ptr theProc = (CFStringNormalize_Ptr)
			CFBundleGetFunctionPointerForName(bundleRef, CFSTR("CFStringNormalize"));
		theProc(theString, (CFStringNormalizationForm)theForm);
		}

#endif

static string16 sNormalized_C_CFType(const string16& iString, ZUnicode::ENormForm iNormForm)
	{
	if (iString.empty())
		return iString;

	CFMutableStringRef srm = ZUtil_CFType::sCreateMutableCFString_UTF16(iString);
	ZooLib_CFStringNormalize(srm, iNormForm);
	const string16 result = ZUtil_CFType::sAsUTF16(srm);
	::CFRelease(srm);
	return result;
	}

#endif // ZCONFIG_SPI_Enabled(CFType)

// =================================================================================================

#if 0 && ZCONFIG_SPI_Enabled(Win)

static string16 sNormalized_C_Win(const string16& iString, ZUnicode::ENormForm iNormForm)
	{
	// See FoldString with these flags:
	// FormC      MAP_PRECOMPOSED
	// FormD      MAP_COMPOSITE
	// FormKC     MAP_PRECOMPOSED | MAP_FOLDCZONE
	// FormKD     MAP_COMPOSITE | MAP_FOLDCZONE 
	// That said, FoldString uses old tables which do not match the
	// unicode standard behavior.

	// Vista has NormalizeString, which is what we should really use.
	}

#endif // ZCONFIG_SPI_Enabled(CFType)

// =================================================================================================

#if ZCONFIG_SPI_Enabled(ICU)

static string16 sNormalized_C_ICU(const string16& iString, ZUnicode::ENormForm iNormForm)
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
			nil, 0, &status);

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

#endif // ZCONFIG_SPI_Enabled(ICU)

NAMESPACE_ZOOLIB_BEGIN

namespace ZUnicode {

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode, normalization -- very incomplete for now

string16 sNormalized(const string16& iString, ENormForm iNormForm)
	{
	#if ZCONFIG_SPI_Enabled(CFType)
		return sNormalized_C_CFType(iString, iNormForm);
//	#elif ZCONFIG_SPI_Enabled(Win)
//		return sNormalized_C_Win(iString);
	#elif ZCONFIG_SPI_Enabled(ICU)
		return sNormalized_C_ICU(iString, iNormForm);
	#else
		ZUnimplemented();
		return iString;
	#endif
	}

} // namespace ZUnicode

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Unicode_Normalize)
