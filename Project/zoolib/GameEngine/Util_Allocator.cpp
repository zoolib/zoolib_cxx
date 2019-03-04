#include "zoolib/GameEngine/Util_Allocator.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Non-releasing allocator

// RealAllocator and TTAllocator, names I'm not overly fond of.
// RealAllocator is a simple class that allocates space
// suballocated from pages that are not themselves disposed till the RealAllocator is
// disposed, and which makes no provision for taking back space once allocated. It's used
// to provide the backing store for the map of IDs to TransTuples, which only ever grows
// during the course of a transaction, and is then tossed in its entirety.
// TTAllocator is the actual class passed to the map<uint64, TransTuple> template.

RealAllocator::RealAllocator()
:	fCurOffset(kPageSize)
	{}

RealAllocator::RealAllocator(const RealAllocator&)
	{}

RealAllocator::~RealAllocator()
	{
	for (std::vector<char*>::iterator ii = fPages.begin(), end = fPages.end(); ii != end; ++ii)
		delete[] *ii;
	}

RealAllocator& RealAllocator::operator=(const RealAllocator&)
	{ return *this; }

void* RealAllocator::Allocate(size_t iSize)
	{
	fCurOffset += iSize;
	if (fCurOffset > kPageSize)
		{
		fPages.push_back(new char[kPageSize]);
		fCurOffset = iSize;
		}
	return fPages.back() + fCurOffset - iSize;
	}

void RealAllocator::swap(RealAllocator& ioOther)
	{
	using std::swap;
	swap(fPages, ioOther.fPages);
	swap(fCurOffset, ioOther.fCurOffset);
	}

} // namespace GameEngine
} // namespace ZooLib
