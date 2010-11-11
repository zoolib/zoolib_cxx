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

#ifndef __ZYadSeq_Expr_Bool__
#define __ZYadSeq_Expr_Bool__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZYad.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_Expr_Bool

class ZYadSeqR_Expr_Bool : public ZYadSeqR
	{
public:
	ZYadSeqR_Expr_Bool(ZRef<ZYadSeqR> iYadSeqR, ZRef<ZExpr_Bool> iExpr_Bool);
	virtual ~ZYadSeqR_Expr_Bool();

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc();

private:
	ZRef<ZYadSeqR> fYadSeqR;
	ZRef<ZExpr_Bool> fExpr_Bool;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqRPos_Expr_Bool

class ZYadSeqRPos_Expr_Bool : public ZYadSeqRPos
	{
public:
	ZYadSeqRPos_Expr_Bool(ZRef<ZYadSeqRPos> iYadSeqRPos, ZRef<ZExpr_Bool> iExpr_Bool);
	virtual ~ZYadSeqRPos_Expr_Bool();

// From ZYadSeqR via ZYadSeqRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadSeqRPos
//##	virtual ZRef<ZYadSeqRPos> Clone();

	virtual uint64 GetPosition();
	virtual void SetPosition(uint64 iPosition);

	virtual uint64 GetSize();

private:
	ZRef<ZYadSeqRPos> fYadSeqRPos;
	ZRef<ZExpr_Bool> fExpr_Bool;
	};

} // namespace ZooLib

#endif // __ZYadSeq_Expr_Bool__
