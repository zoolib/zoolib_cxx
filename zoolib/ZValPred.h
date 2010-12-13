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

#ifndef __ZValPred__
#define __ZValPred__ 1
#include "zconfig.h"

#include "zoolib/ZValPred_T.h"

#include "zoolib/ZVal_Any.h"
//#include "zoolib/ZVal_CFType.h"
//#include "zoolib/ZVal_Zoolib.h"

namespace ZooLib {

typedef ZMap_Any ZMap_Expr;

//typedef ZMap_CFType ZMap_Expr;
//typedef ZMap_ZooLib ZMap_Expr;

typedef ZMap_Expr::Val_t ZVal_Expr;

// =================================================================================================
#pragma mark -
#pragma mark * ZValPred

typedef ZValPred_T<ZVal_Expr> ZValPred;
typedef ZValComparandPseudo_T<ZVal_Expr> ZValComparandPseudo;

inline ZValComparandPseudo CString(const std::string& iVal)
	{ return CConst_T<ZVal_Expr>(iVal); }

inline ZValComparandPseudo CConst(const ZVal_Expr& iVal)
	{ return CConst_T<ZVal_Expr>(iVal); }

inline ZValComparandPseudo CName(const std::string& iName)
	{ return CName_T<ZVal_Expr>(iName); }

inline ZValComparandPseudo CVar(const std::string& iVarName)
	{ return CVar_T<ZVal_Expr>(iVarName); }

} // namespace ZooLib

#endif // __ZValPred__
