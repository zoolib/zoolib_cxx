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

#ifndef __ZooLib_RelationalAlgebra_Util_Strim_RelHead_h__
#define __ZooLib_RelationalAlgebra_Util_Strim_RelHead_h__
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZQ.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_RelHead {

// =================================================================================================
#pragma mark - Util_Strim_RelHead

void sWrite_PropName(const string8& iName, const ChanW_UTF& s);
void sWrite_RelHead(const RelHead& iRelHead, const ChanW_UTF& s);

ZQ<ColName> sQRead_PropName(const ChanRU_UTF& iChanRU);

ZQ<RelHead> sQFromStrim_RelHead(const ChanRU_UTF& iChanRU);

ZQ<std::pair<ColName,ColName> > sQFromStrim_Rename(const ChanRU_UTF& iChanRU);

ZQ<ConcreteHead> sQFromStrim_ConcreteHead(const ChanRU_UTF& iChanRU);

void sWrite_RenameWithOptional(
	const RelationalAlgebra::Rename& iRename, const RelationalAlgebra::RelHead& iOptional,
	const ChanW_UTF& w);

} // namespace Util_Strim_RelHead
} // namespace RelationalAlgebra

const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelationalAlgebra::RelHead& iRH);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelationalAlgebra::Rename& iRename);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelationalAlgebra::ConcreteHead& iRH);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Util_Strim_RelHead_h__
