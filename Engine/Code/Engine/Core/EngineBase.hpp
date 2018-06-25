#pragma once

typedef unsigned char		byte_t;

typedef unsigned int		uint;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

//Universal macros
#define STATIC 
#define BIT(x)		1 << x
#define UNUSED(x)	(void)(x)
#define SAFE_DELETE(ptr)   if ((ptr) != nullptr) { delete ptr; ptr = nullptr; }

#define KB *1024
#define MB *1024*1024
#define GB *1024*1024*1024

#define MS *0.001f
#define US *0.000001f


static void(*NullFuncPtr)() {};

template <typename Type>
inline void MemSetZero(Type* pointer)
{
	memset(pointer, 0, sizeof(Type));
}