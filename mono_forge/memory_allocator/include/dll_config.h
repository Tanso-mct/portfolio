#pragma once

#ifdef memory_allocator_EXPORTS
#define MEMORY_ALLOCATOR_DLL __declspec(dllexport)
#else
#define MEMORY_ALLOCATOR_DLL __declspec(dllimport)
#endif