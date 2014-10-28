/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTextCollator_ICU_h__
#define __ZTextCollator_ICU_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__TextCollator_ICU
	#define ZCONFIG_API_Avail__TextCollator_ICU ZCONFIG_SPI_Enabled(ICU)
#endif

#ifndef ZCONFIG_API_Desired__TextCollator_ICU
	#define ZCONFIG_API_Desired__TextCollator_ICU 1
#endif

#include "zoolib/ZTextCollator.h"

#if ZCONFIG_API_Enabled(TextCollator_ICU)

ZMACRO_MSVCStaticLib_Reference(TextCollator_ICU)

struct UCollator;

namespace ZooLib {

// =================================================================================================
// MARK: - ZTextCollatorRep_ICU

class ZTextCollatorRep_ICU : public ZTextCollatorRep_NativeUTF16
	{
public:
	ZTextCollatorRep_ICU(const char* iLocaleName, int iStrength);
	ZTextCollatorRep_ICU(UCollator* iCollator);
	virtual ~ZTextCollatorRep_ICU();

	virtual ZRef<ZTextCollatorRep> Clone();

// From ZTextCollatorRep via ZTextCollatorRep_NativeUTF16
	using ZTextCollatorRep::Equals;
	using ZTextCollatorRep::Compare;
	using ZTextCollatorRep::Contains;

	virtual bool Equals(const string16& iLeft, const string16& iRight);
	virtual bool Equals(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength);

	virtual int Compare(const string16& iLeft, const string16& iRight);
	virtual int Compare(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength);

	virtual bool Contains(const string16& iPattern, const string16& iTarget);
	virtual bool Contains(const UTF16* iPattern, size_t iPatternLength,
		const UTF16* iTarget, size_t iTargetLength);

private:
	UCollator* fCollator;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCollator_ICU)

#endif // __ZTextCollator_ICU_h__
