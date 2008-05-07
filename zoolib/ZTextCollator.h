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

#ifndef __ZTextCollator__
#define __ZTextCollator__ 1
#include "zconfig.h"

#include "ZRefCount.h"
#include "ZUnicode.h"

class ZTextCollatorRep;
class ZTextCollationKeyRep;

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollator

/// Instances of ZTextCollator provide locale-sensitive collation and comparison services.

class ZTextCollator
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTextCollator,
    	operator_bool_generator_type, operator_bool_type);
public:
	ZTextCollator();
	ZTextCollator(const ZTextCollator& iOther);
	~ZTextCollator();
	ZTextCollator& operator=(const ZTextCollator& iOther);

	ZTextCollator(int iStrength);
	ZTextCollator(const string8& iLocaleName, int iStrength);
	ZTextCollator(const char* iLocaleName, int iStrength);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fRep && true); }

	bool Equals(const string8& iLeft, const string8& iRight) const;
	bool Equals(const string16& iLeft, const string16& iRight) const;
	bool Equals(const string32& iLeft, const string32& iRight) const;

	bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength) const;

	bool Equals(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength) const;

	bool Equals(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength) const;


	int Compare(const string8& iLeft, const string8& iRight) const;
	int Compare(const string16& iLeft, const string16& iRight) const;
	int Compare(const string32& iLeft, const string32& iRight) const;

	int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength) const;

	int Compare(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength) const;

	int Compare(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength) const;


	bool Contains(const string8& iPattern, const string8& iTarget) const;
	bool Contains(const string16& iPattern, const string16& iTarget) const;
	bool Contains(const string32& iPattern, const string32& iTarget) const;

	bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength) const;

	bool Contains(const UTF16* iPattern, size_t iPatternLength,
		const UTF16* iTarget, size_t iTargetLength) const;

	bool Contains(const UTF32* iPattern, size_t iPatternLength,
		const UTF32* iTarget, size_t iTargetLength) const;
	
private:
	void pMakeSafe() const;
	ZRef<ZTextCollatorRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollationKey

class ZTextCollationKey
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTextCollationKey,
    	operator_bool_generator_type, operator_bool_type);
public:
	ZTextCollationKey();
	ZTextCollationKey(const ZTextCollationKey& iOther);
	~ZTextCollationKey();
	ZTextCollationKey& operator=(const ZTextCollationKey& iOther);

	ZTextCollationKey(const ZTextCollator& iCollator, const string8& iPattern);
	ZTextCollationKey(const ZTextCollator& iCollator, const string16& iPattern);
	ZTextCollationKey(const ZTextCollator& iCollator, const string32& iPattern);

	ZTextCollationKey(const ZTextCollator& iCollator, const UTF8* iPattern, size_t iPatternLength);
	ZTextCollationKey(const ZTextCollator& iCollator, const UTF16* iPattern, size_t iPatternLength);
	ZTextCollationKey(const ZTextCollator& iCollator, const UTF32* iPattern, size_t iPatternLength);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fRep && true); }

//	bool Equals(const ZTextCollationKey& iRight) const;
	bool Equals(const string8& iRight) const;
	bool Equals(const string16& iRight) const;
	bool Equals(const string32& iRight) const;

	bool Equals(const UTF8* iRight, size_t iRightLength) const;
	bool Equals(const UTF16* iRight, size_t iRightLength) const;
	bool Equals(const UTF32* iRight, size_t iRightLength) const;


//	int Compare(const ZTextCollationKey& iRight) const;
	int Compare(const string8& iRight) const;
	int Compare(const string16& iRight) const;
	int Compare(const string32& iRight) const;

	int Compare(const UTF8* iRight, size_t iRightLength) const;
	int Compare(const UTF16* iRight, size_t iRightLength) const;
	int Compare(const UTF32* iRight, size_t iRightLength) const;


	bool ContainedBy(const string8& iTarget) const;
	bool ContainedBy(const string16& iTarget) const;
	bool ContainedBy(const string32& iTarget) const;

	bool ContainedBy(const UTF8* iTarget, size_t iTargetLength) const;
	bool ContainedBy(const UTF16* iTarget, size_t iTargetLength) const;
	bool ContainedBy(const UTF32* iTarget, size_t iTargetLength) const;

private:
	ZRef<ZTextCollationKeyRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollatorRep

class ZTextCollatorRep : public ZRefCounted
	{
protected:
	ZTextCollatorRep();
	ZTextCollatorRep(const ZTextCollatorRep& iOther);
	ZTextCollatorRep& operator=(const ZTextCollatorRep& iOther);

public:
	virtual ~ZTextCollatorRep();

// Our protocol
	virtual ZRef<ZTextCollatorRep> Clone() = 0;
	virtual ZRef<ZTextCollatorRep> SafeCopy();

	virtual bool Equals(const string8& iLeft, const string8& iRight) = 0;
	virtual bool Equals(const string16& iLeft, const string16& iRight) = 0;
	virtual bool Equals(const string32& iLeft, const string32& iRight) = 0;

	virtual bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength) = 0;

	virtual bool Equals(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength) = 0;

	virtual bool Equals(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength) = 0;


	virtual int Compare(const string8& iLeft, const string8& iRight) = 0;
	virtual int Compare(const string16& iLeft, const string16& iRight) = 0;
	virtual int Compare(const string32& iLeft, const string32& iRight) = 0;

	virtual int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength) = 0;

	virtual int Compare(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength) = 0;

	virtual int Compare(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength) = 0;


	virtual bool Contains(const string8& iPattern, const string8& iTarget) = 0;
	virtual bool Contains(const string16& iPattern, const string16& iTarget) = 0;
	virtual bool Contains(const string32& iPattern, const string32& iTarget) = 0;

	virtual bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength) = 0;

	virtual bool Contains(const UTF16* iPattern, size_t iPatternLength,
		const UTF16* iTarget, size_t iTargetLength) = 0;

	virtual bool Contains(const UTF32* iPattern, size_t iPatternLength,
		const UTF32* iTarget, size_t iTargetLength) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollatorRep_NativeUTF8

class ZTextCollatorRep_NativeUTF8 : public ZTextCollatorRep
	{
public:
// From ZTextCollatorRep
	virtual bool Equals(const string8& iLeft, const string8& iRight) = 0;
	virtual bool Equals(const string16& iLeft, const string16& iRight);
	virtual bool Equals(const string32& iLeft, const string32& iRight);

	virtual bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength) = 0;

	virtual bool Equals(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength);

	virtual bool Equals(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength);


	virtual int Compare(const string8& iLeft, const string8& iRight) = 0;
	virtual int Compare(const string16& iLeft, const string16& iRight);
	virtual int Compare(const string32& iLeft, const string32& iRight);

	virtual int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength) = 0;

	virtual int Compare(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength);

	virtual int Compare(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength);


	virtual bool Contains(const string8& iPattern, const string8& iTarget) = 0;
	virtual bool Contains(const string16& iPattern, const string16& iTarget);
	virtual bool Contains(const string32& iPattern, const string32& iTarget);

	virtual bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength) = 0;

	virtual bool Contains(const UTF16* iPattern, size_t iPatternLength,
		const UTF16* iTarget, size_t iTargetLength);

	virtual bool Contains(const UTF32* iPattern, size_t iPatternLength,
		const UTF32* iTarget, size_t iTargetLength);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollatorRep_NativeUTF16

class ZTextCollatorRep_NativeUTF16 : public ZTextCollatorRep
	{
public:
// From ZTextCollatorRep
	virtual bool Equals(const string8& iLeft, const string8& iRight);
	virtual bool Equals(const string16& iLeft, const string16& iRight) = 0;
	virtual bool Equals(const string32& iLeft, const string32& iRight);

	virtual bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual bool Equals(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength) = 0;

	virtual bool Equals(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength);


	virtual int Compare(const string8& iLeft, const string8& iRight);
	virtual int Compare(const string16& iLeft, const string16& iRight) = 0;
	virtual int Compare(const string32& iLeft, const string32& iRight);

	virtual int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual int Compare(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength) = 0;

	virtual int Compare(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength);


	virtual bool Contains(const string8& iPattern, const string8& iTarget);
	virtual bool Contains(const string16& iPattern, const string16& iTarget) = 0;
	virtual bool Contains(const string32& iPattern, const string32& iTarget);

	virtual bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength);

	virtual bool Contains(const UTF16* iPattern, size_t iPatternLength,
		const UTF16* iTarget, size_t iTargetLength) = 0;

	virtual bool Contains(const UTF32* iPattern, size_t iPatternLength,
		const UTF32* iTarget, size_t iTargetLength);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollatorRep_NativeUTF32

class ZTextCollatorRep_NativeUTF32 : public ZTextCollatorRep
	{
public:
// From ZTextCollatorRep
	virtual bool Equals(const string8& iLeft, const string8& iRight);
	virtual bool Equals(const string16& iLeft, const string16& iRight);
	virtual bool Equals(const string32& iLeft, const string32& iRight) = 0;

	virtual bool Equals(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual bool Equals(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength);

	virtual bool Equals(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength) = 0;


	virtual int Compare(const string8& iLeft, const string8& iRight);
	virtual int Compare(const string16& iLeft, const string16& iRight);
	virtual int Compare(const string32& iLeft, const string32& iRight) = 0;

	virtual int Compare(const UTF8* iLeft, size_t iLeftLength,
		const UTF8* iRight, size_t iRightLength);

	virtual int Compare(const UTF16* iLeft, size_t iLeftLength,
		const UTF16* iRight, size_t iRightLength);

	virtual int Compare(const UTF32* iLeft, size_t iLeftLength,
		const UTF32* iRight, size_t iRightLength) = 0;


	virtual bool Contains(const string8& iPattern, const string8& iTarget);
	virtual bool Contains(const string16& iPattern, const string16& iTarget);
	virtual bool Contains(const string32& iPattern, const string32& iTarget) = 0;

	virtual bool Contains(const UTF8* iPattern, size_t iPatternLength,
		const UTF8* iTarget, size_t iTargetLength);

	virtual bool Contains(const UTF16* iPattern, size_t iPatternLength,
		const UTF16* iTarget, size_t iTargetLength);

	virtual bool Contains(const UTF32* iPattern, size_t iPatternLength,
		const UTF32* iTarget, size_t iTargetLength) = 0;
	};

#endif // __ZTextCollator__
