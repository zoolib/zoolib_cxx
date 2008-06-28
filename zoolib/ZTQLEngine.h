/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTQLEngine__
#define __ZTQLEngine__ 1
#include "zconfig.h"

#include "ZTQL_Node.h"
//#include "ZTuple.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZTQLEngine

class ZTQLEngine
	{
public:
	class Iterator
		{
	protected:
		Iterator();

	public:
		virtual ~Iterator();

		virtual bool HasValue() = 0;
		virtual ZTuple Current() = 0;
		virtual void Advance() = 0;
		};


	ZTQLEngine();

	void Execute(ZRef<ZTQL::Node> iNode, std::vector<ZTuple>& oResults);

	virtual Iterator* Search();//ZTSpec::CriterionSect iSect);


//		const ZTSpec& iTSpec, std::vector<std::pair<uint64, ZTuple> >& oResults) = 0;

private:	
	void pExecute(ZRef<ZTQL::Node> iNode, std::vector<ZTuple>& oResults);
	};

#endif // __ZTQLEngine__
