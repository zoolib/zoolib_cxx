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

#ifndef __ZCompat_npapi__
#define __ZCompat_npapi__
#include "zconfig.h"

#include "ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Netscape)

// Pull in ZTypes so we see its int32 definition
#include "zoolib/ZTypes.h"

// And tell the npXXX headers that we have an int32 definition
#define _INT32 1

#if defined(ZProjectHeader_npapi)

#	include ZProjectHeader_npapi

#elif defined(__APPLE__)

#	include <WebKit/npfunctions.h>
#	if !defined(XP_MACOSX)
		typedef NPError (*NP_InitializeFuncPtr)(NPNetscapeFuncs*);
		typedef NPError (*NP_GetEntryPointsFuncPtr)(NPPluginFuncs*);
		typedef void (*BP_CreatePluginMIMETypesPreferencesFuncPtr)(void);
		typedef NPError (*MainFuncPtr)(NPNetscapeFuncs*, NPPluginFuncs*, NPP_ShutdownProcPtr*);
#	endif

#	define ZCONFIG_NPStringUpperCaseFieldNames 1
#	define NPEventType_GetFocusEvent getFocusEvent
#	define NPEventType_LoseFocusEvent loseFocusEvent
#	define NPEventType_AdjustCursorEvent loseFocusEvent

#else

#	include <npupp.h>

#endif

// =================================================================================================
// Fixup the NPString field name change

#ifndef ZCONFIG_NPStringUpperCaseFieldNames
#	define ZCONFIG_NPStringUpperCaseFieldNames 0
#endif


#if ZCONFIG_NPStringUpperCaseFieldNames

	inline const NPUTF8* const& sNPStringCharsConst(const NPString& iNPString)
		{ return iNPString.UTF8Characters; }

	inline NPUTF8*& sNPStringChars(NPString& iNPString)
		{ return const_cast<NPUTF8*&>(iNPString.UTF8Characters); }

	inline uint32_t sNPStringLengthConst(const NPString& iNPString)
		{ return iNPString.UTF8Length; }

	inline uint32_t& sNPStringLength(NPString& iNPString)
		{ return iNPString.UTF8Length; }

#else

	inline const NPUTF8* const& sNPStringCharsConst(const NPString& iNPString)
		{ return iNPString.utf8characters; }

	inline NPUTF8*& sNPStringChars(NPString& iNPString)
		{ return const_cast<NPUTF8*&>(iNPString.utf8characters); }

	inline uint32_t sNPStringLengthConst(const NPString& iNPString)
		{ return iNPString.utf8length; }

	inline uint32_t& sNPStringLength(NPString& iNPString)
		{ return iNPString.utf8length; }
#endif

#endif // ZCONFIG_SPI_Enabled(Netscape)

#endif // __ZCompat_npapi__
