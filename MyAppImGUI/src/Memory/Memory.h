#ifndef MEMORY_H
#define MEMORY_H


#define CRTDBG_MAP_ALLOC
#include <memory>
#include <iostream>
#include <crtdbg.h>


static uint32_t s_allocation = 0;

inline void* operator new(const size_t size)
{
	s_allocation++;
	std::cout << __TIME__	<< " " <<  "Allocation : " << s_allocation << '\n';
	std::cout << __FILE__ << __LINE__ << " " << " " << "Allocating " << size << " bytes" << '\n';
	return malloc(size);
}

inline void operator delete(void* ptr, const size_t size) noexcept {
	std::cout << "Freeing " << size << " bytes\n";
	free(ptr);
}

#endif