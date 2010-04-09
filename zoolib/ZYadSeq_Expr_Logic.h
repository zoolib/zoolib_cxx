/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZYadSeq_Expr_Logic__
#define __ZYadSeq_Expr_Logic__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/ZYad.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_Expr_Logic

class ZYadSeqR_Expr_Logic : public ZYadSeqR
	{
public:
	ZYadSeqR_Expr_Logic(ZRef<ZYadSeqR> iYadSeqR, ZRef<ZExpr_Logic> iExpr_Logic);
	virtual ~ZYadSeqR_Expr_Logic();

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc();

private:
	ZRef<ZYadSeqR> fYadSeqR;
	ZRef<ZExpr_Logic> fExpr_Logic;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_Expr_Logic

class ZYadSeqRPos_Expr_Logic : public ZYadSeqRPos
	{
public:
	ZYadSeqRPos_Expr_Logic(ZRef<ZYadSeqRPos> iYadSeqRPos, ZRef<ZExpr_Logic> iExpr_Logic);
	virtual ~ZYadSeqRPos_Expr_Logic();

// From ZYadSeqR via ZYadSeqRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadSeqRPos
	virtual ZRef<ZYadSeqRPos> Clone();

	virtual uint64 GetPosition();
	virtual void SetPosition(uint64 iPosition);

	virtual uint64 GetSize();

private:
	ZRef<ZYadSeqRPos> fYadSeqRPos;
	ZRef<ZExpr_Logic> fExpr_Logic;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYadSeq_Expr_Logic__
