#ifndef MEMORY_H
#define MEMORY_H

#include <memory>
#include <iostream>

void* operator new(size_t size)
{
	std::cout << "Allocating " << size << " bytes" << std::endl;
	return malloc(size);
}

void operator delete(void* ptr)
{
	std::cout << "Freeing memory" << std::endl;
	free(ptr);
}

#endif