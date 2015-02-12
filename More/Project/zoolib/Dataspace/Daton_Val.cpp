/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zooLib/dataspace/Daton_Val.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Util_Any_JSON.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark -
#pragma mark spAsVal (anonymous)

namespace { // anonymous

ZQ<Val_Any> spQAsVal(const Data_Any& iData)
	{
	try { return Util_Any_JSON::sQRead(new Channer_T<ChanRPos_Bin_Data<Data_Any>,ChanR_Bin>(iData)); }
	catch (...) {}
	return null;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Daton/Val conversion.

Val_Any sAsVal(const DatonSet::Daton& iDaton)
	{ return spQAsVal(iDaton.GetData()).Get(); }

DatonSet::Daton sAsDaton(const Val_Any& iVal)
	{
	Data_Any theData;
	Util_Any_JSON::sWrite(iVal, ChanW_UTF_Chan_Bin_UTF8(ChanRWPos_Bin_Data<Data_Any>(&theData)));
	return theData;
	}

} // namespace Dataspace
} // namespace ZooLib
