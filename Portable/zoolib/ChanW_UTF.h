// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanW_UTF_h__
#define __ZooLib_ChanW_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"
#include "zoolib/ChanW.h"
#include "zoolib/UnicodeString.h"

#include <cstdarg>

namespace ZooLib {

// =================================================================================================
#pragma mark -

/** \name String buffers, limiting and reporting both CU and CP
*/	//@{

void sWrite(const ChanW_UTF& iChanW,
	const UTF32* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

// Returns false if the source ends with an incomplete CU sequence
bool sWrite(const ChanW_UTF& iChanW,
	const UTF16* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

// Returns false if the source ends with an incomplete CU sequence
bool sWrite(const ChanW_UTF& iChanW,
	const UTF8* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);
//@}

// =================================================================================================
#pragma mark -

/** \name Pointer-and-length
*/	//@{
//size_t sWrite(const ChanW_UTF& iChanW, const UTF32* iString, size_t iLength);
size_t sWrite(const ChanW_UTF& iChanW, const UTF16* iString, size_t iLength);
size_t sWrite(const ChanW_UTF& iChanW, const UTF8* iString, size_t iLength);

bool sQWrite(const ChanW_UTF& iChanW, const UTF32* iString, size_t iLength);
bool sQWrite(const ChanW_UTF& iChanW, const UTF16* iString, size_t iLength);
bool sQWrite(const ChanW_UTF& iChanW, const UTF8* iString, size_t iLength);

void sEWrite(const ChanW_UTF& iChanW, const UTF32* iString, size_t iLength);
void sEWrite(const ChanW_UTF& iChanW, const UTF16* iString, size_t iLength);
void sEWrite(const ChanW_UTF& iChanW, const UTF8* iString, size_t iLength);
//@}

// =================================================================================================
#pragma mark -

/** \name Zero-terminated strings
*/	//@{
bool sQWrite(const ChanW_UTF& iChanW, const UTF32* iString);
bool sQWrite(const ChanW_UTF& iChanW, const UTF16* iString);
bool sQWrite(const ChanW_UTF& iChanW, const UTF8* iString);

void sEWrite(const ChanW_UTF& iChanW, const UTF32* iString);
void sEWrite(const ChanW_UTF& iChanW, const UTF16* iString);
void sEWrite(const ChanW_UTF& iChanW, const UTF8* iString);
//@}

// =================================================================================================
#pragma mark -

/** \name Standard library strings
*/	//@{
bool sQWrite(const ChanW_UTF& iChanW, const string32& iString);
bool sQWrite(const ChanW_UTF& iChanW, const string16& iString);
bool sQWrite(const ChanW_UTF& iChanW, const string8& iString);

void sEWrite(const ChanW_UTF& iChanW, const string32& iString);
void sEWrite(const ChanW_UTF& iChanW, const string16& iString);
void sEWrite(const ChanW_UTF& iChanW, const string8& iString);
//@}

// =================================================================================================
#pragma mark -

/** \name Formatted strings
*/	//@{
void sEWritef(const ChanW_UTF& iChanW,
	const UTF8* iString, ...)
	ZMACRO_Attribute_Format_Printf(2,3);

bool sWritef(const ChanW_UTF& iChanW, size_t* oCount_CUProduced, size_t* oCount_CUWritten,
	const UTF8* iString, ...)
	ZMACRO_Attribute_Format_Printf(4,5);

void sEWritev(const ChanW_UTF& iChanW,
	const UTF8* iString, va_list iArgs);

bool sWritev(const ChanW_UTF& iChanW, size_t* oCount_CUProduced, size_t* oCount_CUWritten,
	const UTF8* iString, va_list iArgs);
//@}

// =================================================================================================
#pragma mark - ChanW_UTF_Native32

typedef ChanW_UTF ChanW_UTF_Native32;

// =================================================================================================
#pragma mark - ChanW_UTF_Native16

class ChanW_UTF_Native16
:	public ChanW_UTF
	{
public:
// From ChanW_UTF (aka ChanW<UTF32>)
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

// Our protocol
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU) = 0;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_Native8

class ChanW_UTF_Native8
:	public ChanW_UTF
	{
public:
// From ChanW_UTF (aka ChanW<UTF32>)
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

// Our protocol
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU) = 0;
	};
} // namespace ZooLib

#endif // __ZooLib_ChanW_UTF_h__
