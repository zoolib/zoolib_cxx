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

#ifndef __ZValCondition__
#define __ZValCondition__ 1
#include "zconfig.h"

#include "zoolib/ZValCondition_T.h"

#include "zoolib/ZVal_Any.h"
//#include "zoolib/ZVal_CFType.h"
//#include "zoolib/ZVal_Zoolib.h"

NAMESPACE_ZOOLIB_BEGIN

typedef ZMap_Any ZMap_Expr;

//typedef ZMap_CFType ZMap_Expr;
//typedef ZMap_ZooLib ZMap_Expr;

typedef ZMap_Expr::Val_t ZVal_Expr;

// =================================================================================================
#pragma mark -
#pragma mark * ZValCondition

typedef ZValCondition_T<ZVal_Expr> ZValCondition;
typedef ZValComparandPseudo_T<ZVal_Expr> ZValComparandPseudo;

inline ZValComparandPseudo CString(const std::string& iVal)
	{ return CConst_T<ZVal_Expr>(string8(iVal)); }

inline ZValComparandPseudo CConst(const ZVal_Expr& iVal)
	{ return CConst_T<ZVal_Expr>(iVal); }

inline ZValComparandPseudo CName(const std::string& iName)
	{ return CName_T<ZVal_Expr>(iName); }

inline ZValComparandPseudo CTrail(const ZTrail& iTrail)
	{ return CTrail_T<ZVal_Expr>(iTrail); }

inline ZValComparandPseudo CVal()
	{ return CVal_T<ZVal_Expr>(); }

inline ZValComparandPseudo CVar(const std::string& iVarName)
	{ return CVar_T<ZVal_Expr>(iVarName); }

NAMESPACE_ZOOLIB_END

#endif // __ZValCondition__
