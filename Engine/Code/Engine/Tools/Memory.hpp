#pragma once

#include <stack>
#include <string>
#include <utility>
#include <forward_list>
#include <map>
#include <vector>

#include "Engine/Config/BuildConfig.hpp"
#include "CallStack.hpp"
#include "Engine/Utils/LinkedPtrList.hpp"
#include "Engine/Core/Thread.hpp"
#include "Engine/Core/ThreadSafeLinkedPtrList.hpp"
#include "Engine/Core/criticalsection.hpp"

struct allocation_t
{
	size_t m_byteSize;
};

struct AllocationInfo
{
	CallStack* cs;
	void* ptr;
};

struct AllocationSortingInfo
{
	CallStack* cs;
	size_t memory;
	size_t count;
	bool operator < (const AllocationSortingInfo& comp) { return memory < comp.memory; }
};

#ifdef TRACK_MEMORY

//void* operator new (const size_t size);
//void operator delete(void *ptr);

#endif

class MemoryProfiler
{
public:

	static void Thread(void*);

	static void Initialize();
	static void Destroy();
	
	static void AddMemoryByte(const size_t size);
	static void SubMemoryByte(const size_t size);

	static void AddAllocationCount(const size_t count = 1);
	static void SubAllocationCount(const size_t count = 1);

	static void AddFrameAllocationCount(const size_t count = 1);
	static void AddFrameFreeCount(const size_t count = 1);

	static void BeginFrame();
	static void EndFrame();

	static size_t GetMemoryByte();
	static size_t GetAllicationCount();
	static size_t GetFrameAllocationCount();
	static size_t GetFrameFreeCount();
	static size_t GetHighWater();

	static void LogAllocationCallStack(void* ptr, CallStack* callStack);
	static void EraseAllocationCallStack(void* ptr);

	static void LogCurrentAllocation(const std::string& ignoredParm = "");
	static void LogAllocationWithCallStack(const std::string& ignoredParm = "");

	static void LogNewAllocation(void * ptr);
	static std::string ToByteSizeString(size_t byteSize);

	static std::string OutputAllocationAndCallStack();

	static void NewPointer(allocation_t* pointer);
	static void DeletePointer(allocation_t* pointer);

	static MemoryProfiler* GetInstance();


	
private:
	MemoryProfiler();

	size_t m_memoryByteCount;
	size_t m_allocationCount;
	size_t m_highWaterMemory;
	size_t m_frameAllocateCount;
	size_t m_frameFreeCount;
	std::forward_list<void*> m_allocations;
	ThreadSafeLinkedPtrList<AllocationInfo>* m_callStacks;

	thread_handle_t m_threadHandle;

	CriticalSection m_lock;
	//AllocationInfo* m_callStacks;

	bool m_isValid;

	static MemoryProfiler* s_instance;
};

inline void MemoryProfiler::AddMemoryByte(const size_t size)
{
	if (!s_instance)
		return;
	s_instance->m_memoryByteCount += size;
	if (s_instance->m_memoryByteCount > s_instance->m_highWaterMemory)
		s_instance->m_highWaterMemory = s_instance->m_memoryByteCount;
}

inline void MemoryProfiler::SubMemoryByte(const size_t size)
{
	if (!s_instance)
		return;
	s_instance->m_memoryByteCount -= size;
}

inline void MemoryProfiler::AddAllocationCount(const size_t count)
{
	if (!s_instance)
		return;
	s_instance->m_allocationCount += count;
}

inline void MemoryProfiler::SubAllocationCount(const size_t count)
{
	if (!s_instance)
		return;
	s_instance->m_allocationCount -= count;
}

inline void MemoryProfiler::AddFrameAllocationCount(const size_t count)
{
	if (!s_instance)
		return;
	s_instance->m_frameAllocateCount += count;
}

inline void MemoryProfiler::AddFrameFreeCount(const size_t count)
{
	if (!s_instance)
		return;
	s_instance->m_frameFreeCount += count;
}

inline void MemoryProfiler::BeginFrame()
{
	if (!s_instance)
		return;
	s_instance->m_frameAllocateCount = 0;
	s_instance->m_frameFreeCount = 0;
}

inline void MemoryProfiler::EndFrame()
{
	if (!s_instance)
		return;
	//log profile;
}

inline size_t MemoryProfiler::GetMemoryByte()
{
	if (!s_instance)
		return 0;
	return s_instance->m_memoryByteCount;
}

inline size_t MemoryProfiler::GetAllicationCount()
{
	if (!s_instance)
		return 0;
	return s_instance->m_allocationCount;
}

inline size_t MemoryProfiler::GetFrameAllocationCount()
{
	if (!s_instance)
		return 0;
	return s_instance->m_frameAllocateCount;
}

inline size_t MemoryProfiler::GetFrameFreeCount()
{
	if (!s_instance)
		return 0;
	return s_instance->m_frameFreeCount;
}

inline size_t MemoryProfiler::GetHighWater()
{
	if (!s_instance)
		return 0;
	return s_instance->m_highWaterMemory;
}