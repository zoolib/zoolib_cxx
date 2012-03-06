/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTrail_h__
#define __ZTrail_h__ 1
#include "zconfig.h"

#include "zoolib/ZUnicodeString.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZTrail

class ZTrail
	{
	ZTrail(bool); // Not implemented
public:
	ZTrail();
	ZTrail(const ZTrail& iTrail);
	~ZTrail();
	ZTrail& operator=(const ZTrail& iTrail);

	ZTrail(const string8& iPOSIXTrail);
	ZTrail(const char* iPOSIXTrail);
	ZTrail(const char* iPOSIXTrail, size_t iSize);

	ZTrail(const string8& iSeparator, const string8& iIgnore,
		const string8& iBounce, const string8& iTrail);

	template <class I>
	ZTrail(const I& iBegin, const I& iEnd)
	:	fComps(iBegin, iEnd)
		{}

	bool operator==(const ZTrail& iOther) const;
	bool operator<(const ZTrail& iOther) const;

	ZTrail operator+(const ZTrail& iTrail) const;
	ZTrail& operator+=(const ZTrail& iTrail);

	template <class I>
	void AppendTrail(const I& iBegin, const I& iEnd)
		{ fComps.insert(fComps.end(), iBegin, iEnd); }
	void AppendTrail(const ZTrail& iTrail);
	void AppendComp(const string8& iComp);
	void AppendBounce();
	void AppendBounces(size_t iCount);

	template <class I>
	void PrependTrail(const I& iBegin, const I& iEnd)
		{ fComps.insert(fComps.begin(), iBegin, iEnd);}
	void PrependTrail(const ZTrail& iTrail);
	void PrependComp(const string8& iComp);
	void PrependBounce();
	void PrependBounces(size_t iCount);

	string8 AsString() const;
	string8 AsString(const string8& iSeparator, const string8& iBounce) const;

	ZTrail Branch() const;
	const string8& Leaf() const;

	size_t Count() const;
	const string8& At(size_t iIndex) const;
	ZTrail SubTrail(size_t iBegin, size_t iEnd) const;
	ZTrail SubTrail(size_t iBegin) const;

	ZTrail Normalized() const;
	ZTrail& Normalize();
	bool IsNormalized() const;

	const std::vector<string8>& GetComps() const
		{ return fComps; }

	static ZTrail sTrailFromTo(const std::vector<string8>& iSource,
		const std::vector<string8>& oDest);

	static void sParseStringAndAppend
		(const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
		const UTF8* iPath, size_t iPathSize,
		std::vector<string8>& ioComps);

	static void sParseStringAndAppend
		(const UTF16* iSeparator, const UTF16* iIgnore, const UTF16* iBounce,
		const UTF16* iPath, size_t iPathSize,
		std::vector<string16>& ioComps);

	static size_t sNormalize_ReturnLeadingBounces(const std::vector<string8>& iComps,
		std::vector<string8>& oComps);

private:
	std::vector<string8> fComps;
	};

ZTrail operator+(const string8& iPOSIXTrail, const ZTrail& iTrail);

} // namespace ZooLib

#endif // __ZTrail_h__
