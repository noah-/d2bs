#pragma once

#include <windows.h>

template <typename T, typename Alloc = std::allocator<T> > class TimedAlloc;

template <> class TimedAlloc<void>
{
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void value_type;
	template <class U> struct rebind { typedef TimedAlloc<U> other; };
};

template <typename T, typename Alloc>
class TimedAlloc
{
private:
	Alloc allocator;
	int allocs, deallocs, ctors, dtors;
	double alloctime, dealloctime, ctortime, dtortime;

public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind { typedef TimedAlloc<U> other; };

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	TimedAlloc() : allocs(0), deallocs(0), ctors(0), dtors(0),
				    alloctime(0), dealloctime(0), ctortime(0), dtortime(0), allocator(Alloc()) {}
	template <typename U> TimedAlloc(const TimedAlloc<U>&){}
	~TimedAlloc() {}
	size_type max_size() const throw() { return allocator.max_size(); }

	pointer allocate(size_type count, const_pointer hint = 0)
	{
		allocs++;
		LARGE_INTEGER start, end;

		QueryPerformanceCounter(&start);
		pointer result = static_cast<pointer>(allocator.allocate(count, hint));
		QueryPerformanceCounter(&end);

		alloctime += end.QuadPart - start.QuadPart;
		return result;
	}
	void deallocate(pointer p, size_type n)
	{
		deallocs++;
		LARGE_INTEGER start, end;

		QueryPerformanceCounter(&start);
		allocator.deallocate(p, n);
		QueryPerformanceCounter(&end);

		dealloctime += end.QuadPart - start.QuadPart;
	}

	void construct(pointer p, const T& value)
	{
		ctors++;
		LARGE_INTEGER start, end;

		QueryPerformanceCounter(&start);
		new(static_cast<void*>(p)) T(value);
		QueryPerformanceCounter(&end);

		ctortime += end.QuadPart - start.QuadPart;
	}
	void construct(pointer p)
	{
		ctors++;
		LARGE_INTEGER start, end;

		QueryPerformanceCounter(&start);
		new(static_cast<void*>(p)) T();
		QueryPerformanceCounter(&end);

		ctortime += end.QuadPart - start.QuadPart;
	}
	void destroy(pointer p)
	{
		dtors++;
		LARGE_INTEGER start, end;

		QueryPerformanceCounter(&start);
		p->~T();
		QueryPerformanceCounter(&end);

		dtortime += end.QuadPart - start.QuadPart;
	}

	void DumpStats(FILE* f) const
	{
		LARGE_INTEGER totalfreq;
		QueryPerformanceFrequency(&totalfreq);
		double freq = (double)(totalfreq.QuadPart / 1000);

		double realalloctime = (alloctime / freq),
			   realdealloctime = (dealloctime / freq),
			   realctortime = (ctortime / freq),
			   realdtortime = (dtortime / freq),
			   allocperpsec = (allocs == 0 ? 0 : realalloctime*1000 / allocs),
			   deallocperpsec = (deallocs == 0 ? 0 : realdealloctime*1000 / deallocs),
			   ctorperobj = (ctors == 0 ? 0 : realctortime*1000 / ctors),
			   dtorperobj = (dtors == 0 ? 0 : realdtortime*1000 / dtors);

		fprintf(f, "Allocations: %d, Deallocations: %d, Objects constructed: %d, Objects destructed: %d\n"
				   "Time spent allocating: %gms (%f microseconds/alloc), Time spent deallocating: %gms (%f microseconds/dealloc)\n"
				   "Time spent constructing: %gms (%f microseconds/ctor), Time spent destructing: %gms (%f microseconds/dtor)\n",
				   allocs, deallocs, ctors, dtors,
				   realalloctime, allocperpsec, realdealloctime, deallocperpsec,
				   realctortime, realdtortime, ctorperobj, dtorperobj);
	}
};

template <typename T, typename U>
inline bool operator==(const TimedAlloc<T>&, const TimedAlloc<U>&) { return true; }
template <typename T, typename U>
inline bool operator!=(const TimedAlloc<T>&, const TimedAlloc<U>&) { return false; }
