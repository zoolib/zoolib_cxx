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

#include "zoolib/ZStackCrawl.h"
#include "zoolib/ZMemory.h"

#include <stdexcept>
#include <vector>

using std::min;
using std::runtime_error;
using std::string;
using std::vector;

using namespace ZooLib;

#if ZCONFIG_API_Enabled(StackCrawl)

// =================================================================================================
// MARK: - Win32 Helpers

#if ZCONFIG_SPI_Enabled(Win)
#include <csetjmp>

#include "zoolib/ZWinHeader.h"
#include <imagehlp.h>

void ZStackCrawl::sPopulateStackFrames(vector<ZStackCrawl::pFrame_t>& oFrames)
	{
	// We can't use GetThreadContext on the running thread from the running
	// thread, it returns useless values. Instead we do more or less the
	// same as in linux.

	struct NativeStackFrame
		{
		NativeStackFrame* fNext;
		const void* fReturnAddress;
		size_t fParams[0];
		};

	jmp_buf theJmpBuf;
	setjmp(theJmpBuf);

	// We use frameNumber to ensure we don't get into an infinite loop.
	size_t frameNumber = 0;

	// Extract the program counter and stack frame pointer.
	static const int JB_PC = 5;
	static const int JB_BP = 4;
	const void* thePC = reinterpret_cast<const void*>(theJmpBuf[JB_PC]);
	NativeStackFrame* theNSF = reinterpret_cast<NativeStackFrame*>(theJmpBuf[JB_BP]);

	while (theNSF && frameNumber < 200)
		{
		pFrame_t theFrame;
		theFrame.fPC = thePC;

		// We don't actually know how many arguments the stack frame contains, and can't
		// derive it without unmangling the function name. We'll just use the difference
		// between the frame pointers (as size_t*) less the space for the return address
		// and the frame pointer, but no more than 6. If a stack frame is for a method
		// invocation the first argument will be the 'this' pointer. Subsequent values will
		// be parameters or local variables.
		size_t stackFrameSize = reinterpret_cast<size_t*>(theNSF->fNext)
			- reinterpret_cast<size_t*>(theNSF) - 2;

		theFrame.fCountParams = min(stackFrameSize, size_t(ZStackCrawl::kMaxCountParams));

		std::copy(&theNSF->fParams[0],
			&theNSF->fParams[theFrame.fCountParams],
			&theFrame.fParams[0]);

		oFrames.push_back(theFrame);

		thePC = theNSF->fReturnAddress;
		theNSF = theNSF->fNext;

		if (IsBadWritePtr(theNSF,
			sizeof(NativeStackFrame) + sizeof(uint32) * ZStackCrawl::kMaxCountParams))
			{
			// Use 0, not nullptr, because MingGW gcc 3.4.5 chokes with nil.
			theNSF = 0;
//			theNSF = nullptr;
			}

		++frameNumber;
		}
	}

static bool spGetLogicalAddress(const void* iAddr, char* oModuleName,
	size_t iModuleNameLen, size_t& oSection, size_t& oOffset)
	{
	MEMORY_BASIC_INFORMATION theMBI;
	if (!::VirtualQuery(iAddr, &theMBI, sizeof(theMBI)))
		return false;

	if (!theMBI.AllocationBase)
		return false;

	if (!::GetModuleFileNameA(static_cast<HMODULE>(theMBI.AllocationBase),
		oModuleName, iModuleNameLen))
		{
		return false;
		}

	// Point to the DOS header in memory
	const IMAGE_DOS_HEADER* theDOSHeader =
		static_cast<const IMAGE_DOS_HEADER*>(theMBI.AllocationBase);

	// The PE header is some bytes after theDOSHeader.
	const void* thePEHeader =
		static_cast<const char*>(theMBI.AllocationBase) + theDOSHeader->e_lfanew;

	const IMAGE_NT_HEADERS* theNTHeaders = static_cast<const IMAGE_NT_HEADERS*>(thePEHeader);

	const IMAGE_SECTION_HEADER* theSectionPtr = IMAGE_FIRST_SECTION(theNTHeaders);

	// theRVA is the offset from module load address.
	size_t theRVA = static_cast<const char*>(iAddr)
		- static_cast<const char*>(theMBI.AllocationBase);

	// Iterate through the section table, looking for the one that encompasses theRVA.
	for (size_t x = 0; x < theNTHeaders->FileHeader.NumberOfSections; ++x, ++theSectionPtr)
		{
		size_t sectionStart = theSectionPtr->VirtualAddress;
		if (theRVA >= sectionStart)
			{
			size_t sectionEnd = sectionStart
				+ std::max(theSectionPtr->SizeOfRawData, theSectionPtr->Misc.VirtualSize);
			if (theRVA <= sectionEnd)
				{
				oSection = x;
				oOffset = theRVA - sectionStart;
				return true;
				}
			}
		}

 	// Can't get here
	return false;
	}

static const char* spTruncateFileName(const char* iFilename)
	{
	if (const char* truncatedFileName = strrchr(iFilename, '\\'))
		return truncatedFileName + 1;
	return iFilename;
	}

static bool spInited;
static void spGetSymNameAndOffset(const void* iAddress, string& oSymName, size_t& oOffset)
	{
	HANDLE currentProcess = ::GetCurrentProcess();
	if (!spInited)
		{
		bool inited1 = ::SymInitialize(currentProcess, nullptr, false);

		char moduleFileName[MAX_PATH];
		::GetModuleFileNameA(::GetModuleHandleA(nullptr), moduleFileName, sizeof(moduleFileName));

		bool inited2 = ::SymLoadModule(
						currentProcess, // process handle
						nullptr, // file handle
						moduleFileName, // path to exe
						nullptr, // module name
						0, // load address
						0); // module size
		spInited = true;
		}

	const uint32 kMaxSymbolName = 512;
	uint8 buffer[sizeof(IMAGEHLP_SYMBOL) + kMaxSymbolName];

	IMAGEHLP_SYMBOL* theSYMBOL = reinterpret_cast<IMAGEHLP_SYMBOL*>(buffer);
	theSYMBOL->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	theSYMBOL->MaxNameLength = kMaxSymbolName;
	theSYMBOL->Address = reinterpret_cast<DWORD>(iAddress);

	// Note that SymGetSymFromAddr only works with Win98 and NT 4 or later.
	DWORD theDWORDOffset;
	if (::SymGetSymFromAddr(currentProcess,
		reinterpret_cast<DWORD>(iAddress), &theDWORDOffset, theSYMBOL))
		{
		oOffset = theDWORDOffset;
		oSymName = theSYMBOL->Name;
		}
	else
		{
		int error = ::GetLastError();
		if (true)
			{
			// For now it is more useful to use the address rather than the actual offset.
			oOffset = reinterpret_cast<size_t>(iAddress);
			oSymName.clear();
			}
		else
			{
			char theModuleName[MAX_PATH];

			// It's possible to use adr with the map file to determine the
			// function name, but it's rather awkward since you have to use
			// the "Rva+Base" column in the map file. By calling GetLogicalAddress
			// we get the address used by the first column in the map file which
			// is much easier to handle.
			size_t theSection;
			size_t theOffset;
			if (!::spGetLogicalAddress(iAddress, theModuleName,
				sizeof(theModuleName), theSection, theOffset))
				{
				theModuleName[0] = '\0';
				}

			oOffset = theOffset;
			oSymName = spTruncateFileName(theModuleName);
			}
		}
	}

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
// MARK: - Linux helpers

#if defined(linux) && ZCONFIG(Processor, x86)
#include <execinfo.h> // <--Use this instead of our own code.

#include <csetjmp>
#include <stdexcept>

#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <unistd.h>

namespace { // anonymous

class Symbols
	{
public:
	Symbols(const char* iFileName, size_t iBaseAddr);
	~Symbols();

	const char* fStringTable;
	int fNumOffsets;
	vector<Elf32_Sym> fSyms;

private:
	bool ReadStabs(int iFD, const Elf32_Ehdr& iHdr, size_t iBaseAddr);
	};

static bool spVerifyELF(const Elf32_Ehdr& iHdr)
	{
	if (memcmp(&iHdr.e_ident[EI_MAG0], ELFMAG, SELFMAG))
		return false;

	if (iHdr.e_ident[EI_CLASS] != ELFCLASS32)
		return false;

	if (iHdr.e_ident[EI_DATA] != ELFDATA2LSB)
		return false;

	if (iHdr.e_ident[EI_VERSION] != EV_CURRENT || iHdr.e_version != EV_CURRENT)
		return false;

	if (iHdr.e_machine != EM_386)
		return false;

	return true;
	}

Symbols::Symbols(const char* iFileName, size_t iBaseAddr)
:	fStringTable(nullptr)
	{
	int theFD = open(iFileName, O_RDONLY);

	if (theFD < 0)
		{
		throw runtime_error(string("Couldn't open file to read stabs from \"") + iFileName + "\"");
		}
	else
		{
		try
			{
			Elf32_Ehdr theEHdr;
			read(theFD, &theEHdr, sizeof(theEHdr));
			if (!spVerifyELF(theEHdr))
				{
				throw runtime_error(string("\"")
					+ iFileName + "\" does not have a valid ELF header");
				}

			if (!this->ReadStabs(theFD, theEHdr, iBaseAddr))
				throw runtime_error(string("Couldn't read stabs from \"") + iFileName + "\"");
			}
		catch (...)
			{
			delete[] fStringTable;
			close(theFD);
			throw;
			}
		close(theFD);
		}
	}

Symbols::~Symbols()
	{
	delete[] fStringTable;
	}

bool Symbols::ReadStabs(int iFD, const Elf32_Ehdr& iHdr, size_t iBaseAddr)
	{
	int spot = iHdr.e_shoff;
	if (lseek(iFD, spot, SEEK_SET) != spot)
		return false;

	Elf32_Shdr shdr;
	memset(&shdr, 0, sizeof(shdr));

	fNumOffsets = iHdr.e_shnum;

	for (int idx = 0; idx < iHdr.e_shnum; idx++)
		{
		if (read(iFD, &shdr, iHdr.e_shentsize) != iHdr.e_shentsize)
			return false;

		spot += iHdr.e_shentsize;
		switch (shdr.sh_type)
			{
			case SHT_SYMTAB:
				{
				if (lseek(iFD, shdr.sh_offset, SEEK_SET) != shdr.sh_offset)
					return false;

				size_t countSyms = shdr.sh_size / sizeof(Elf32_Sym);
				fSyms.resize(countSyms);

				if (read(iFD, &fSyms[0], shdr.sh_size) != shdr.sh_size)
					return false;

				for (vector<Elf32_Sym>::iterator ii = fSyms.begin(); ii != fSyms.end(); ++ii)
					{
					if (ii->st_shndx && ii->st_shndx < fNumOffsets)
						ii->st_value += iBaseAddr;
					}

				int i = iHdr.e_shoff + shdr.sh_link * iHdr.e_shentsize;
				if (lseek(iFD, i, SEEK_SET) != i)
					return false;

				if (read(iFD, &shdr, iHdr.e_shentsize) != iHdr.e_shentsize)
					return false;

				if (lseek(iFD, shdr.sh_offset, SEEK_SET) != shdr.sh_offset)
					return false;

				fStringTable = new char[shdr.sh_size];

				if (read(iFD, const_cast<char*>(fStringTable), shdr.sh_size) != shdr.sh_size)
					return false;

				lseek(iFD, spot, SEEK_SET);
				break;
				}
			case SHT_DYNAMIC:
				{
				lseek(iFD, spot, SEEK_SET);
				break;
				}
			}
		}

	return true;
	}

} // anonymous namespace

class ZStackCrawl::AllSymbols
	{
public:
	AllSymbols();
	~AllSymbols();

	bool GetSymNameAndOffset(const void* iAddress, string& oSymName, size_t& oOffset) const;

private:
	vector<Symbols*> fSymbols;
	};

ZStackCrawl::AllSymbols::AllSymbols()
	{
	// Always load the symbols for the main executable.
	try
		{
		fSymbols.push_back(new Symbols("/proc/self/exe", 0));
		}
	catch (...)
		{}

	// If we're dynamically linked the symbol _DYNAMIC references the head of a linked
	// list of link maps. If we're statically linked _DYNAMIC is not defined and will
	// cause a link error. Bypassing all this nonsense we instead rely on the linux behavior
	// that the return value of dlopen is the pointer to the link map for the library
	// in question, and that for a statically linked executable dlopen will fail.

	if (void* us = dlopen(0, RTLD_LAZY | RTLD_NOLOAD))
		{
		struct link_map* theLinkMap = static_cast<struct link_map*>(us);
		while (theLinkMap)
			{
			// The executable's entry in the list will have a nil l_name field,
			// this check thus skips the executable, plus any malformed entries
			// that have a zero length name (shouldn't actually be any).
			if (theLinkMap->l_name && theLinkMap->l_name[0])
				{
				try
					{
					fSymbols.push_back(new Symbols(theLinkMap->l_name, theLinkMap->l_addr));
					}
				catch (...)
					{}
				}
			theLinkMap = theLinkMap->l_next;
			}
		dlclose(us);
		}
	}

ZStackCrawl::AllSymbols::~AllSymbols()
	{
	ZUtil_STL::sDeleteAll(fSymbols.begin(), fSymbols.end());
	}

bool ZStackCrawl::AllSymbols::GetSymNameAndOffset(const void* iAddress,
	string& oSymName, size_t& oOffset) const
	{
	const Elf32_Sym* foundSym = nullptr;
	const Symbols* foundTable = nullptr;
	size_t addressAsInt = reinterpret_cast<size_t>(iAddress);
	for (vector<Symbols*>::const_iterator ii = fSymbols.begin(); ii != fSymbols.end(); ++ii)
		{
		for (vector<Elf32_Sym>::const_iterator jj = (*ii)->fSyms.begin(); jj != (*ii)->fSyms.end(); ++j)
			{
			if (jj->st_value > addressAsInt)
				continue;
			if (jj->st_shndx == SHN_UNDEF)
				continue;
			if (jj->st_shndx >= (*i)->fNumOffsets)
				continue;
			if (ELF32_ST_TYPE(jj->st_info) != STT_FUNC)
				continue;
			if (foundSym && foundSym->st_value >= jj->st_value)
				continue;
			foundSym = &(*jj);
			foundTable = *ii;
			}
		}

	if (foundSym)
		{
		oSymName = foundTable->fStringTable + foundSym->st_name;
		oOffset = addressAsInt - foundSym->st_value;
		return true;
		}
	else
		{
		oOffset = 0;
		return false;
		}
	}

void ZStackCrawl::sPopulateStackFrames(vector<ZStackCrawl::pFrame_t>& oFrames)
	{
	struct NativeStackFrame
		{
		NativeStackFrame* fNext;
		size_t fReturnAddress;
		size_t fParams[0];
		};

	jmp_buf theJmpBuf;
	setjmp(theJmpBuf);

	// We use frameNumber to ensure we don't get into an infinite loop.
	size_t frameNumber = 0;

	// Extract the program counter and stack frame pointer.
	const void* thePC = reinterpret_cast<const void*>(theJmpBuf[0].__jmpbuf[JB_PC]);
	NativeStackFrame* theNSF = reinterpret_cast<NativeStackFrame*>(theJmpBuf[0].__jmpbuf[JB_BP]);

	while (theNSF && frameNumber < 200)
		{
		pFrame_t theFrame;
		theFrame.fPC = thePC;

		// We don't actually know how many arguments the stack frame contains, and can't
		// derive it without unmangling the function name. We'll just use the difference
		// between the frame pointers (as size_t*) less the space for the return address
		// and the frame pointer, but no more than 6. If a stack frame is for a method
		// invocation the first argument will be the 'this' pointer. Subsequent values will
		// be parameters or local variables.
		size_t stackFrameSize = reinterpret_cast<size_t*>(theNSF->fNext)
			- reinterpret_cast<size_t*>(theNSF) - 2;

		theFrame.fCountParams = min(stackFrameSize, size_t(ZStackCrawl::kMaxCountParams));

		std::copy(&theNSF->fParams[0],
			&theNSF->fParams[theFrame.fCountParams],
			&theFrame.fParams[0]);

		oFrames.push_back(theFrame);

		thePC = reinterpret_cast<const void*>(theNSF->fReturnAddress);
		theNSF = theNSF->fNext;
		++frameNumber;
		}
	}

#endif // defined(linux) && ZCONFIG(Processor, x86)

// =================================================================================================
// MARK: - ZStackCrawl
ZStackCrawl::ZStackCrawl()
	{
	fFrames.reserve(32);

	#if defined(linux) && ZCONFIG(Processor, x86)
		fAllSymbols = nullptr;
		sPopulateStackFrames(fFrames);
	#endif

	#if ZCONFIG_SPI_Enabled(Win)
		sPopulateStackFrames(fFrames);
	#endif
	}

ZStackCrawl::~ZStackCrawl()
	{
	#if defined(linux) && ZCONFIG(Processor, x86)
		delete fAllSymbols;
	#endif
	}

size_t ZStackCrawl::Count() const
	{ return fFrames.size(); }

ZStackCrawl::Frame ZStackCrawl::At(size_t iIndex) const
	{
	Frame result;
	if (iIndex < fFrames.size())
		{
		#if defined(linux) && ZCONFIG(Processor, x86)
			if (!fAllSymbols)
				fAllSymbols = new AllSymbols;

			const pFrame_t& theFrame = fFrames.at(iIndex);

			result.fPC = theFrame.fPC;
			result.fCountParams = theFrame.fCountParams;

			std::copy(&theFrame.fParams[0],
				&theFrame.fParams[0] + kMaxCountParams,
				&result.fParams[0]);

			fAllSymbols->GetSymNameAndOffset(result.fPC, result.fName, result.fOffset);
			return result;
		#endif

		#if ZCONFIG_SPI_Enabled(Win)
			const pFrame_t& theFrame = fFrames.at(iIndex);

			result.fPC = theFrame.fPC;
			result.fCountParams = theFrame.fCountParams;

			std::copy(&theFrame.fParams[0],
				&theFrame.fParams[0] + kMaxCountParams,
				&result.fParams[0]);

			spGetSymNameAndOffset(result.fPC, result.fName, result.fOffset);
			return result;
		#endif
		}

	ZMemZero_T(result);
	return result;
	}

#endif // ZCONFIG_API_Enabled(StackCrawl)
