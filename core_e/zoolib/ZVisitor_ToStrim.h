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

#ifndef __ZVisitor_ToStrim_h__
#define __ZVisitor_ToStrim_h__
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

#include "zoolib/ZVisitor.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZVisitor_ToStrim

class ZVisitor_ToStrim
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

	ZVisitor_ToStrim();

// From ZVisitor
	virtual void Visit(const ZRef<ZVisitee>& iRep);

// Our protocol
	void ToStrim(const Options& iOptions, const ChanW_UTF& iStrimW, const ZRef<ZVisitee>& iRep);

protected:
	void pToStrim(const ZRef<ZVisitee>& iRep);

	const Options& pOptions();
	const ChanW_UTF& pStrimW();

	void pWriteLFIndent();

private:
	const Options* fOptions;
	const ChanW_UTF* fStrimW;
protected:
	size_t fIndent;
	};

} // namespace ZooLib

#endif // __ZVisitor_ToStrim_h__
