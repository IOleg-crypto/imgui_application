#ifndef MEMORY_H
#define MEMORY_H


#define _CRTDBG_MAP_ALLOC
#include <memory>
#include <iostream>
#include <crtdbg.h>


static uint32_t s_allocation = 0;

void* operator new(size_t size)
{
	s_allocation++;
	std::cout << "Allocation : " << s_allocation << std::endl;
	std::cout << "Allocating " << size << " bytes" << std::endl;
	return malloc(size);
}

void operator delete(void* ptr, size_t size) noexcept {
	std::cout << "Freeing " << size << " bytes\n";
	free(ptr);
}

#endif