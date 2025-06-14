#ifndef MEMORY_H
#define MEMORY_H

// Enable debug memory allocation tracking in Visual Studio
#define CRTDBG_MAP_ALLOC
#include <memory>     // (Optional) For smart pointers if needed
#include <iostream>   // (Optional) For logging/debug output
#include <crtdbg.h>   // For memory leak detection and debugging

// Static variable to count memory allocations
// Note: 'static' makes this variable local to each translation unit (.cpp file)
// If you want a global counter across all files, use 'extern' instead
static uint32_t s_allocation = 0;

// Custom global 'new' operator
// Allows tracking or overriding memory allocations
void* operator new(const size_t size);

// Custom global 'delete' operator (sized version, available in C++14 and later)
// Allows tracking or overriding memory deallocation
void operator delete(void* ptr, const size_t size) noexcept;

#endif // MEMORY_H