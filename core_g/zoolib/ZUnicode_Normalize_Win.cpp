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

#include "zoolib/ZUnicode_Normalize_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

ZMACRO_MSVCStaticLib_cpp(Unicode_Normalize_Win)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {
namespace ZUnicode {

// =================================================================================================
// MARK: - ZUnicode, normalization, Win

namespace { // anonymous

#if WINVER >= 0x0600

static string16 spNormalized_C_Win(const string16& iString, ENormForm iNormForm)
	{
	string16 result = iString;
	if (not iString.empty())
		{
		NORM_FORM theNF;
		switch (iNormForm)
			{
			case eNormForm_D: theNF = NormalizationD; break;
			case eNormForm_KD: theNF = NormalizationKD; break;
			case eNormForm_C: theNF = NormalizationC; break;
			case eNormForm_KC: theNF = NormalizationKC; break;
			}

		for (;;)
			{
			int reqEstimate = ::NormalizeString(theNF,
				iString.data(), iString.size(),
				const_cast<UTF16*>(result.data()), result.size());

			if (reqEstimate > 0)
				{
				result.resize(reqEstimate);
				break;
				}

			DWORD err = ::GetLastError();
			if (err != ERROR_INSUFFICIENT_BUFFER)
				{
				result.clear();
				break;
				}
			result.resize(size_t(-1.2 * reqEstimate));
			}
		}
	return result;
	}

#elif WINVER >= 0x0500 || _WIN32_WINNT >= 0x0500

static string16 spNormalized_C_Win(const string16& iString, ENormForm iNormForm)
	{
	// Note that FoldString uses old tables which do not match the unicode standard behavior.
	string16 result = iString;
	if (not iString.empty())
		{
		DWORD theFlags;
		switch (iNormForm)
			{
			case eNormForm_D: theFlags = MAP_COMPOSITE; break;
			case eNormForm_KD: theFlags = MAP_COMPOSITE | MAP_FOLDCZONE; break;
			case eNormForm_C: theFlags = MAP_PRECOMPOSED; break;
			case eNormForm_KC: theFlags = MAP_PRECOMPOSED | MAP_FOLDCZONE; break;
			}

		for (;;)
			{
			int reqEstimate = ::FoldStringW(theFlags,
				iString.data(), iString.size(),
				const_cast<UTF16*>(result.data()), result.size());

			if (reqEstimate > 0)
				{
				result.resize(reqEstimate);
				break;
				}

			DWORD err = ::GetLastError();
			if (err != ERROR_INSUFFICIENT_BUFFER)
				{
				result.clear();
				break;
				}
			result.resize(size_t(-1.2 * reqEstimate));
			}
		}
	return result;
	}

#endif // WINVER

#if WINVER >= 0x0500 || _WIN32_WINNT >= 0x0500

class Function
:	public FunctionChain<string16, const Param_Normalize&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = spNormalized_C_Win(iParam.fString, iParam.fNormForm);
		return true;
		}
	} sFunction0;

#endif // WINVER >= 0x0500

} // anonymous namespace

} // namespace ZUnicode
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
