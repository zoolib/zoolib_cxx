#ifndef __ZooLib_GameEngine_Util_Allocator_h__
#define __ZooLib_GameEngine_Util_Allocator_h__ 1
#include "zconfig.h"

#include <vector>

namespace ZooLib {
namespace GameEngine {

using std::size_t;
using std::ptrdiff_t;

// =================================================================================================
#pragma mark - Non-releasing allocator

// RealAllocator and TTAllocator, names I'm not overly fond of.
// RealAllocator is a simple class that allocates space
// suballocated from pages that are not themselves disposed till the RealAllocator is
// disposed, and which makes no provision for taking back space once allocated. It's used
// to provide the backing store for the map of IDs to TransTuples, which only ever grows
// during the course of a transaction, and is then tossed in its entirety.
// TTAllocator is the actual class passed to the map<uint64, TransTuple> template.

class RealAllocator
	{
public:
	std::vector<char*> fPages;
	static const size_t kPageSize = 4096;
	size_t fCurOffset;

	RealAllocator();
	RealAllocator(const RealAllocator&);
	~RealAllocator();
	RealAllocator& operator=(const RealAllocator&);

	void* Allocate(size_t iSize);
	
	void swap(RealAllocator& ioOther);
	};

template <typename T>
class TTAllocator;

template <>
class TTAllocator<void>
	{
public:
	typedef void value_type;
	typedef TTAllocator<void> class_type;
	typedef void *pointer;
	typedef void const *const_pointer;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;

	template <typename U>
	struct rebind
		{
		typedef TTAllocator<U> other;
		};
	};

template <typename T>
class TTAllocator
	{
public:
	typedef T value_type;
	typedef TTAllocator<value_type> class_type;
	typedef value_type *pointer;
	typedef value_type const *const_pointer;
	typedef value_type &reference;
	typedef value_type const &const_reference;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef void *deallocate_pointer;

	template <typename U>
	struct rebind
		{
		typedef TTAllocator<U> other;
		};

	RealAllocator* fRealAllocator;

	TTAllocator(RealAllocator& iRealAllocator) throw ()
	:	fRealAllocator(&iRealAllocator)
		{}

	TTAllocator(class_type const & iOther) throw ()
	:	fRealAllocator(iOther.fRealAllocator)
		{}

	template <typename U>
	TTAllocator(TTAllocator<U> const & iOther) throw ()
	:	fRealAllocator(iOther.fRealAllocator)
		{}

	~TTAllocator() throw ()
		{}

	TTAllocator& operator=(TTAllocator& ioOther)
		{
		fRealAllocator = ioOther.fRealAllocator;
		return *this;
		}

	size_type max_size() const throw ()
		{ return static_cast<size_type>(-1) / sizeof(value_type); }

	pointer address(reference x) const throw ()
		{ return &x; }

	const_pointer address(const_reference x) const throw ()
		{ return &x; }

	pointer allocate(size_type n, TTAllocator<void>::const_pointer pv = 0)
		{
		((void)(pv)) ;
		TTAllocator<void>::pointer p = fRealAllocator->Allocate(n * sizeof(value_type));
		if (p == 0)
			throw ::std::bad_alloc();

		return static_cast<pointer>(p);
		}

	void deallocate(deallocate_pointer p, size_type n)
		{}

	void deallocate(deallocate_pointer p)
		{}

	void construct(pointer p, value_type const &x)
		{ new(p) value_type(x); }

	void construct(pointer p)
		{ new(p) value_type(); }

	void destroy(pointer p) throw ()
		{ static_cast<pointer>(p)->~value_type(); }
	};

template <typename T>
inline bool operator==(const TTAllocator<T>& , const TTAllocator<T>&)
	{ return true; }

template <typename T>
inline bool operator!=(const TTAllocator<T>& , const TTAllocator<T>&)
	{ return false; }

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_Allocator_h__
