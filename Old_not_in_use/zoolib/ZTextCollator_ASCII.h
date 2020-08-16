// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZTextCollator_ASCII_h__
#define __ZTextCollator_ASCII_h__ 1
#include "zconfig.h"

#include "zoolib/ZTextCollator.h"

ZMACRO_MSVCStaticLib_Reference(TextCollator_ASCII)

namespace ZooLib {

// =================================================================================================
#pragma mark - ZTextCollatorRep_ASCII

class ZTextCollatorRep_ASCII : public ZTextCollatorRep_NativeUTF8
	{
public:
	ZTextCollatorRep_ASCII(int iStrength);
	virtual ~ZTextCollatorRep_ASCII();

// From ZTextCollatorRep via ZTextCollatorRep_NativeUTF8
	virtual ZRef<ZTextCollatorRep> Clone();
	virtual ZRef<ZTextCollatorRep> SafeCopy();

	using ZTextCollatorRep::Equals;
	using ZTextCollatorRep::Compare;
	using ZTextCollatorRep::Contains;

	virtual bool Equals(const string8& iLeft, const string8& iRight);
	virtual bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual int Compare(const string8& iLeft, const string8& iRight);
	virtual int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual bool Contains(const string8& iPattern, const string8& iTarget);
	virtual bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength);

private:
	int fStrength;
	};

} // namespace ZooLib

#endif // __ZTextCollator_ASCII_h__
