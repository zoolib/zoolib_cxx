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

#ifndef __ZQE_Walker_Embed__
#define __ZQE_Walker_Embed__ 1
#include "zconfig.h"

#include "zoolib/zqe/ZQE_Walker.h"
#include "zoolib/zra/ZRA_RelHead.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Embed

class Walker_Embed : public Walker_Nullary
	{
public:
	Walker_Embed(const string8& iRelName,
		const ZRA::Rename iBindings, const ZRef<Walker>& iWalker_Child);

	virtual ~Walker_Embed();

// From ZQE::Walker
	virtual ZRef<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(
		ZVal_Any* ioResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

private:
	const string8 fRelName;
	const ZRA::Rename fBindings;
	ZRef<Walker> fWalker_Child;

	size_t fOutputOffset;
	ZRA::RelHead fChildRelHead;
	std::vector<size_t> fChildOffsets;
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Walker_Embed__
