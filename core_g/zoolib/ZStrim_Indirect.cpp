/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZStrim_Indirect.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimR_Indirect

ZStrimR_Indirect::ZStrimR_Indirect(const ZStrimR* iStrim)
:	ZStrim_Indirect<ZStrimR>(iStrim)
	{}

ZStrimR_Indirect::~ZStrimR_Indirect()
	{}

void ZStrimR_Indirect::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (fStrim)
		fStrim->Imp_ReadUTF32(oDest, iCount, oCount);
	else if (oCount)
		*oCount = 0;
	}

void ZStrimR_Indirect::Imp_ReadUTF16(UTF16* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	if (fStrim)
		{
		fStrim->Imp_ReadUTF16(oDest, iCountCU, oCountCU, iCountCP, oCountCP);
		}
	else
		{
		if (oCountCU)
			*oCountCU = 0;
		if (oCountCP)
			*oCountCP = 0;
		}
	}

void ZStrimR_Indirect::Imp_ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	if (fStrim)
		{
		fStrim->Imp_ReadUTF8(oDest, iCountCU, oCountCU, iCountCP, oCountCP);
		}
	else
		{
		if (oCountCU)
			*oCountCU = 0;
		if (oCountCP)
			*oCountCP = 0;
		}
	}

void ZStrimR_Indirect::Imp_CopyToDispatch(const ZStrimW& iStrimW,
	uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten)
	{
	if (fStrim)
		{
		fStrim->Imp_CopyToDispatch(iStrimW, iCountCP, oCountCPRead, oCountCPWritten);
		}
	else
		{
		if (oCountCPRead)
			*oCountCPRead = 0;
		if (oCountCPWritten)
			*oCountCPWritten = 0;
		}
	}

void ZStrimR_Indirect::Imp_CopyTo(const ZStrimW& iStrimW,
	uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten)
	{
	if (fStrim)
		{
		fStrim->Imp_CopyTo(iStrimW, iCountCP, oCountCPRead, oCountCPWritten);
		}
	else
		{
		if (oCountCPRead)
			*oCountCPRead = 0;
		if (oCountCPWritten)
			*oCountCPWritten = 0;
		}
	}

bool ZStrimR_Indirect::Imp_ReadCP(UTF32& oCP)
	{
	if (fStrim)
		return fStrim->Imp_ReadCP(oCP);
	return false;
	}

void ZStrimR_Indirect::Imp_Skip(uint64 iCountCP, uint64* oCountCPSkipped)
	{
	if (fStrim)
		fStrim->Imp_Skip(iCountCP, oCountCPSkipped);
	else if (oCountCPSkipped)
		*oCountCPSkipped = 0;
	}

// =================================================================================================
// MARK: - ZStrimU_Indirect

ZStrimU_Indirect::ZStrimU_Indirect(const ZStrimU* iStrim)
:	ZStrim_Indirect<ZStrimU>(iStrim)
	{}

ZStrimU_Indirect::~ZStrimU_Indirect()
	{}

void ZStrimU_Indirect::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (fStrim)
		fStrim->Imp_ReadUTF32(oDest, iCount, oCount);
	else if (oCount)
		*oCount = 0;
	}

void ZStrimU_Indirect::Imp_ReadUTF16(UTF16* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	if (fStrim)
		{
		fStrim->Imp_ReadUTF16(oDest, iCountCU, oCountCU, iCountCP, oCountCP);
		}
	else
		{
		if (oCountCU)
			*oCountCU = 0;
		if (oCountCP)
			*oCountCP = 0;
		}
	}

void ZStrimU_Indirect::Imp_ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	if (fStrim)
		{
		fStrim->Imp_ReadUTF8(oDest, iCountCU, oCountCU, iCountCP, oCountCP);
		}
	else
		{
		if (oCountCU)
			*oCountCU = 0;
		if (oCountCP)
			*oCountCP = 0;
		}
	}

void ZStrimU_Indirect::Imp_CopyToDispatch(const ZStrimW& iStrimW,
	uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten)
	{
	if (fStrim)
		{
		fStrim->Imp_CopyToDispatch(iStrimW, iCountCP, oCountCPRead, oCountCPWritten);
		}
	else
		{
		if (oCountCPRead)
			*oCountCPRead = 0;
		if (oCountCPWritten)
			*oCountCPWritten = 0;
		}
	}

void ZStrimU_Indirect::Imp_CopyTo(const ZStrimW& iStrimW,
	uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten)
	{
	if (fStrim)
		{
		fStrim->Imp_CopyTo(iStrimW, iCountCP, oCountCPRead, oCountCPWritten);
		}
	else
		{
		if (oCountCPRead)
			*oCountCPRead = 0;
		if (oCountCPWritten)
			*oCountCPWritten = 0;
		}
	}

bool ZStrimU_Indirect::Imp_ReadCP(UTF32& oCP)
	{
	if (fStrim)
		return fStrim->Imp_ReadCP(oCP);
	return false;
	}

void ZStrimU_Indirect::Imp_Skip(uint64 iCountCP, uint64* oCountCPSkipped)
	{
	if (fStrim)
		fStrim->Imp_Skip(iCountCP, oCountCPSkipped);
	else if (oCountCPSkipped)
		*oCountCPSkipped = 0;
	}

void ZStrimU_Indirect::Imp_Unread(UTF32 iCP)
	{
	if (fStrim)
		return fStrim->Imp_Unread(iCP);
	}

size_t ZStrimU_Indirect::Imp_UnreadableLimit()
	{
	if (fStrim)
		return fStrim->Imp_UnreadableLimit();
	return 0;
	}

// =================================================================================================
// MARK: - ZStrimW_Indirect

ZStrimW_Indirect::ZStrimW_Indirect(const ZStrimW* iStrim)
:	ZStrim_Indirect<ZStrimW>(iStrim)
	{}

ZStrimW_Indirect::~ZStrimW_Indirect()
	{}

void ZStrimW_Indirect::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (fStrim)
		fStrim->Imp_WriteUTF32(iSource, iCountCU, oCountCU);
	else if (oCountCU)
		*oCountCU = 0;
	}

void ZStrimW_Indirect::Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (fStrim)
		fStrim->Imp_WriteUTF16(iSource, iCountCU, oCountCU);
	else if (oCountCU)
		*oCountCU = 0;
	}

void ZStrimW_Indirect::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (fStrim)
		fStrim->Imp_WriteUTF8(iSource, iCountCU, oCountCU);
	else if (oCountCU)
		*oCountCU = 0;
	}

void ZStrimW_Indirect::Imp_CopyFromDispatch(const ZStrimR& iStrimR,
	uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten)
	{
	if (fStrim)
		{
		fStrim->Imp_CopyFromDispatch(iStrimR, iCountCP, oCountCPRead, oCountCPWritten);
		}
	else
		{
		if (oCountCPRead)
			*oCountCPRead = 0;
		if (oCountCPWritten)
			*oCountCPWritten = 0;
		}
	}

void ZStrimW_Indirect::Imp_CopyFrom(const ZStrimR& iStrimR,
	uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten)
	{
	if (fStrim)
		{
		fStrim->Imp_CopyFrom(iStrimR, iCountCP, oCountCPRead, oCountCPWritten);
		}
	else
		{
		if (oCountCPRead)
			*oCountCPRead = 0;
		if (oCountCPWritten)
			*oCountCPWritten = 0;
		}
	}

void ZStrimW_Indirect::Imp_Flush()
	{
	if (fStrim)
		fStrim->Imp_Flush();
	}

} // namespace ZooLib
