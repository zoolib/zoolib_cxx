/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStackCrawl_h__
#define __ZStackCrawl_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__StackCrawl
	#if ZCONFIG_SPI_Enabled(Linux) && ZCONFIG(Processor, x86)
		#define ZCONFIG_API_Avail__StackCrawl 1
	#endif
	#if ZCONFIG_SPI_Enabled(Win)
		#define ZCONFIG_API_Avail__StackCrawl ZCONFIG_SPI_Enabled(imagehlp)
	#endif
#endif

#ifndef ZCONFIG_API_Avail__StackCrawl
	#define ZCONFIG_API_Avail__StackCrawl 0
#endif

#ifndef ZCONFIG_API_Desired__StackCrawl
	#define ZCONFIG_API_Desired__StackCrawl 1
#endif

#if ZCONFIG_API_Enabled(StackCrawl)
#include "zoolib/ZTypes.h"

#include <string>
#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStackCrawl

class ZStackCrawl
	{
public:
	enum { kMaxCountParams = 8 };

	class Frame
		{
	public:
		const void* fPC;
		size_t fOffset;
		size_t fCountParams;
		uint32 fParams[kMaxCountParams];
		std::string fName;
		};

private:
	class pFrame_t
		{
	public:
		const void* fPC;
		size_t fOffset;
		size_t fCountParams;
		uint32 fParams[kMaxCountParams];
		};

public:

	ZStackCrawl();
	~ZStackCrawl();

	size_t Count() const;
	Frame At(size_t iIndex) const;

	#if defined(linux) && ZCONFIG(Processor, x86)
		static void sPopulateStackFrames(std::vector<ZStackCrawl::pFrame_t>& oFrames);
	#endif

	#if ZCONFIG_SPI_Enabled(Win)
		static void sPopulateStackFrames(std::vector<ZStackCrawl::pFrame_t>& oFrames);
	#endif

private:
	std::vector<pFrame_t> fFrames;

	#if defined(linux) && ZCONFIG(Processor, x86)
		class AllSymbols;
		mutable AllSymbols* fAllSymbols;
	#endif
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(StackCrawl)

#endif // __ZStackCrawl_h__
