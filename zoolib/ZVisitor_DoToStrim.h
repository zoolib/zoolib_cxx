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

#ifndef __ZVisitor_DoToStrim__
#define __ZVisitor_DoToStrim__
#include "zconfig.h"

#include "zoolib/ZStrim.h"
#include "zoolib/ZVisitor.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_DoToStrim

class ZVisitor_DoToStrim
:	public virtual ZVisitor
	{
public:
	struct Options
		{
		Options();

		std::string fEOLString;
		std::string fIndentString;
		size_t fInitialIndent;
		bool fDebuggingOutput;
		};

	ZVisitor_DoToStrim();

// From ZVisitor
	virtual void Visit(ZRef<ZVisitee> iRep);

// Our protocol
	void DoToStrim(const Options& iOptions, const ZStrimW& iStrimW, ZRef<ZVisitee> iRep);

protected:
	void pDoToStrim(ZRef<ZVisitee> iRep);

	const Options& pOptions();
	const ZStrimW& pStrimW();

	void pWriteLFIndent();

private:
	const Options* fOptions;
	const ZStrimW* fStrimW;

	size_t fIndent;
	};

} // namespace ZooLib

#endif // __ZVisitor_DoToStrim__
