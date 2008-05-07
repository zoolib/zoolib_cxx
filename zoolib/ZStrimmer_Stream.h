/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStrimmer_Stream__
#define __ZStrimmer_Stream__ 1
#include "zconfig.h"

#include "ZStream.h"
#include "ZStrimmer.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerR_StreamR_T

template <class Strim_t>
class ZStrimmerR_StreamR_T : public ZStrimmerR
	{
public:
	ZStrimmerR_StreamR_T(const ZStreamR& iStreamR)
	:	fStrimR(iStreamR)
		{}

	template <class P>
	ZStrimmerR_StreamR_T(P& iParam, const ZStreamR& iStreamR)
	:	fStrimR(iParam, iStreamR)
		{}

	template <class P>
	ZStrimmerR_StreamR_T(const P& iParam, const ZStreamR& iStreamR)
	:	fStrimR(iParam, iStreamR)
		{}

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR()
		{ return fStrimR; }

private:
	Strim_t fStrimR;
	};

#endif // __ZStrimmer_Stream__
