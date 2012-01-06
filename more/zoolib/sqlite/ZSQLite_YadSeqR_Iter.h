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

#ifndef __ZSQLite_YadSeqR_Iter_h__
#define __ZSQLite_YadSeqR_Iter_h__ 1
#include "zconfig.h"

#include "zoolib/ZYad_Std.h"
#include "zoolib/sqlite/ZSQLite.h"

namespace ZooLib {
namespace ZSQLite {

// =================================================================================================
#pragma mark -
#pragma mark * ZSQLite::YadSeqR_Iter

class YadSeqR_Iter : public ZYadSeqR_Std
	{
public:
	YadSeqR_Iter(ZRef<Iter> iIter);
	virtual ~YadSeqR_Iter();

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<Iter> fIter;
	};

} // namespace ZSQLite
} // namespace ZooLib

#endif // __ZSQLite_YadSeqR_Iter_h__
