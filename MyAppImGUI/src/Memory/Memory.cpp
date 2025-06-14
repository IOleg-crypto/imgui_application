#include "Memory.h"

void* operator new(const size_t size)
{
	s_allocation++;
	std::cout << __TIME__ << " " << "Allocation : " << s_allocation << '\n';
	std::cout << __FILE__ << __LINE__ << " " << " " << "Allocating " << size << " bytes" << '\n';
	return malloc(size);
}

void operator delete(void* ptr, const size_t size) noexcept
{
	std::cout << "Freeing " << size << " bytes\n";
	free(ptr);
}
