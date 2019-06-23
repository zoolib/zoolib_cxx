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

#ifndef __ZooLib_Dataspace_RowBoat_h__
#define __ZooLib_Dataspace_RowBoat_h__ 1
#include "zconfig.h"

#include "zoolib/Dataspace/RelsWatcher.h"
#include "zoolib/Dataspace/Types.h"
#include "zoolib/RelationalAlgebra/PseudoMap.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - RowBoat

class RowBoat
:	public Counted
	{
public:
	typedef RelationalAlgebra::PseudoMap PseudoMap;
	typedef RelationalAlgebra::Expr_Rel Expr_Rel;
	typedef QueryEngine::Result Result;

	typedef RelsWatcher::Callable_Register Callable_Register;

	typedef Callable<void(const PseudoMap* iOld, const PseudoMap* iNew)> Callable_Row;

	typedef Callable<ZP<Callable_Row>(const PseudoMap&)> Callable_Make_Callable_Row;

	RowBoat(const ZP<Callable_Register>& iCallable_Register,
		const ZP<Expr_Rel>& iRel,
		const RelHead& iIdentity,
		const RelHead& iSignificant,
		bool iEmitDummyChanges,
		const ZP<Callable_Make_Callable_Row>& iCallable);

	virtual ~RowBoat();

// From Counted
	virtual void Initialize();

// Our protocol
	const std::vector<ZP<Callable_Row> >& GetRows();

private:
	void pChanged(
		const ZP<Counted>& iRegistration,
		int64 iChangeCount,
		const ZP<Result>& iResult,
		const ZP<ResultDeltas>& iResultDeltas);

	const ZP<Callable_Register> fCallable_Register;
	const ZP<Expr_Rel> fRel;
	QueryEngine::ResultDiffer fResultDiffer;
	const ZP<Callable_Make_Callable_Row> fCallable;

	ZP<Counted> fRegistration;
	std::vector<ZP<Callable_Row> > fRows;
	std::map<string8,size_t> fBindings;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_RowBoat_h__

