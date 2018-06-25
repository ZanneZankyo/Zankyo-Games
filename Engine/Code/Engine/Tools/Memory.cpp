#include <sstream>
#include "Memory.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\RGBA.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\File\Persistence.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Core\Logger.hpp"
#include "Engine\Core\Profiler.hpp"

void* operator new (size_t count, void* ptr);

MemoryProfiler::MemoryProfiler()
	: m_allocations()
	, m_callStacks()
	, m_frameAllocateCount(0)
	, m_frameFreeCount(0)
	, m_memoryByteCount(0)
	, m_allocationCount(0)
	, m_highWaterMemory(0)
	, m_threadHandle(nullptr)
{}

MemoryProfiler* MemoryProfiler::s_instance = nullptr;

/*
void MemoryProfiler::Thread(void*)
{
	if (!s_instance)
		return;

	allocation_t* pointerToNew = nullptr;

	while (s_instance->m_isValid) {
		if (s_instance->m_pointersToNew.pop(&pointerToNew)) {
			MemoryProfiler::AddAllocationCount();
			MemoryProfiler::AddFrameAllocationCount();
			MemoryProfiler::AddMemoryByte(pointerToNew->m_byteSize);
#if (TRACK_MEMORY == TRACK_MEMORY_VERBOSE)
			CallStack* callStack = CreateCallStack(0);
			MemoryProfiler::LogAllocationCallStack(pointerToNew, callStack);
#endif
		}
		ThreadSleep(1);
	}
}*/

void MemoryProfiler::Initialize()
{
	
	s_instance = (MemoryProfiler *) ::malloc(sizeof(MemoryProfiler));
	s_instance->m_isValid = false;
	s_instance->m_frameAllocateCount = 0;
	s_instance->m_frameFreeCount = 0;
	s_instance->m_memoryByteCount = 0;
	s_instance->m_allocationCount = 0;
	s_instance->m_highWaterMemory = 0;
	//new (&s_instance->m_callStacks) std::forward_list<AllocationInfo*>();
	//new (&s_instance->m_allocations) std::forward_list<void*>();
	//s_instance->m_callStacks = ::std::map<CallStack, std::vector<void*>>();
	//s_instance->m_allocations = std::forward_list<void*>();
	s_instance->m_callStacks = (ThreadSafeLinkedPtrList<AllocationInfo>*) ::malloc(sizeof(ThreadSafeLinkedPtrList<AllocationInfo>));
	s_instance->m_callStacks->Initialize();
	s_instance->m_lock.Initialize();
	s_instance->m_isValid = true;

	CallStackSystemInit();

	Console::AssignCommand("log_memory", MemoryProfiler::LogCurrentAllocation);
	Console::AssignCommand("log_memory_callstack", MemoryProfiler::LogAllocationWithCallStack);
	//s_instance->m_threadHandle = ThreadCreate(Thread, nullptr);
}

void MemoryProfiler::Destroy()
{
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return;
	//std::string output = OutputAllocationAndCallStack();
	//Logger::LogTagged("memory", output);
	s_instance->m_isValid = false;
	s_instance->m_callStacks->Destroy();
	//ThreadJoin(s_instance->m_threadHandle);
	free(s_instance->m_callStacks);
	free(s_instance);
	s_instance = nullptr;
}

void MemoryProfiler::LogAllocationCallStack(void* ptr, CallStack* callStack)
{
	if (!s_instance || !s_instance->m_isValid)
		return;
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return;
	AllocationInfo* info = (AllocationInfo*)::malloc(sizeof(AllocationInfo));
	info->cs = callStack;
	info->ptr = ptr;
	if (!s_instance || !s_instance->m_isValid)
	{
		free(info);
		free(callStack);
		return;
	}
	s_instance->m_callStacks->PushFront(info);

}

void MemoryProfiler::EraseAllocationCallStack(void* ptr)
{
	if (!s_instance || !s_instance->m_isValid)
		return;
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return;
	auto node = s_instance->m_callStacks->Head();
	while (node)
	{
		auto allocationInfo = node->Data();
		if (allocationInfo->ptr == ptr)
		{
			free(node->Data()->cs);
			s_instance->m_callStacks->Delete(node);
			break;
		}
		node = node->m_next;
	}
/*
	for(auto& it : s_instance->m_callStacks)
		if (it->ptr == ptr)
		{
			s_instance->m_callStacks.remove(it);
			break;
		}*/
}

void MemoryProfiler::LogCurrentAllocation(const std::string&)
{
	if (!s_instance || !s_instance->m_isValid)
		return;
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return;
	Console::Log("Total allocations : " + std::to_string(s_instance->m_allocationCount),Rgba::GREEN);
	std::string byteAllocated = "Bytes allocated : ";
	byteAllocated += ToByteSizeString(s_instance->m_memoryByteCount);
	Console::Log(byteAllocated, Rgba::GREEN);
	Console::Log("Allocations last frame : " + std::to_string(s_instance->m_frameAllocateCount), Rgba::GREEN);
	Console::Log("Frees last frame : " + std::to_string(s_instance->m_frameFreeCount), Rgba::GREEN);
}

void MemoryProfiler::LogAllocationWithCallStack(const std::string&)
{
	if (!s_instance || !s_instance->m_isValid)
		return;
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return;
	std::string output = OutputAllocationAndCallStack();
	Logger::LogTagged("memory", output);
	//std::string fileName = "Data/log/memoryLog_" + std::to_string((int)Time::s_currentTimeSeconds) + ".log";
	//Persistence::SaveBufferToBinaryFile(fileName, std::vector<byte_t>(output.begin(),output.begin()+output.size()));

}

void MemoryProfiler::LogNewAllocation(void * ptr)
{
	if (!s_instance || !s_instance->m_isValid)
		return;
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return;
	s_instance->m_allocations.push_front(ptr);
}

std::string MemoryProfiler::ToByteSizeString(size_t byteSize)
{
	std::stringstream ss;

	if (s_instance->m_memoryByteCount >= 1 GB)
		ss << std::setprecision(2) << std::fixed << byteSize / (float)(1 GB) << " GB";
	else if (s_instance->m_memoryByteCount >= 1 MB)
		ss << std::setprecision(2) << std::fixed << byteSize / (float)(1 MB) << " MB";
	else if (s_instance->m_memoryByteCount >= 1 KB)
		ss << std::setprecision(2) << std::fixed << byteSize / (float)(1 KB) << " KB";
	else
		ss << std::setprecision(2) << std::fixed << byteSize << " B";
	return ss.str();
}

std::string MemoryProfiler::OutputAllocationAndCallStack()
{
	if (!s_instance || !s_instance->m_isValid)
		return "";
	SCOPE_LOCK(s_instance->m_lock);
	if (!s_instance || !s_instance->m_isValid)
		return "";
	size_t totalAllocation = s_instance->m_callStacks->Size();
	UNUSED(totalAllocation);
	LinkedPtrList<AllocationSortingInfo> list;
	list.Initialize();

	//group allocations
	{
		auto current = s_instance->m_callStacks->Head();
		
		while (current)
		{
			AllocationSortingInfo* info = (AllocationSortingInfo*)malloc(sizeof(AllocationSortingInfo));
			info->cs = current->Data()->cs;
			info->memory = ((allocation_t*)current->Data()->ptr)->m_byteSize;
			info->count = 1;

			auto listIterator = list.Head();
			while (listIterator)
			{
				if (*(listIterator->Data()->cs) == *(info->cs))
				{
					listIterator->Data()->memory += info->memory;
					listIterator->Data()->count += info->count;
					free(info);
					info = nullptr;
					break;
				}
				listIterator = listIterator->m_next;
			}
			if (info)
			{
				list.PushFront(info);
			}
			current = current->m_next;
		}
	}
	

	//sort descending
	LinkedPtrList<AllocationSortingInfo> sortedList;
	sortedList.Initialize();
	{
		auto current = list.Head();
		while (current)
		{
			sortedList.SortingPush(current->Data(), false);
			current = current->m_next;
		}
	}

	std::stringstream ss;
	//outputing string
	{
		auto current = sortedList.Head();

		while (current)
		{
			std::string allocationSize = "Allocation size: " + ToByteSizeString(current->Data()->memory) 
				+ ", Allocation count: " + std::to_string(current->Data()->count);
			ss << allocationSize << std::endl;
			Console::Log(allocationSize, Rgba::GREEN);

			// Printing a callstack, happens when making report
			char line_buffer[1024];
			callstack_line_t lines[128];
			uint line_count = CallStackGetLines(lines, 128, current->Data()->cs);
			for (uint i = 0; i < line_count; ++i) {
				// this specific format will make it double clickable in an output window 
				// taking you to the offending line.
				//sprintf_s(line_buffer, 512, "%s(%u): %s\n",
				//	lines[i].filename, lines[i].line, lines[i].function_name);
				sprintf_s(line_buffer, 1024, "%s(%u): %s\n",
					lines[i].filename, lines[i].line, lines[i].function_name);

				// print to output and console
				Console::Log(line_buffer);
				printf(line_buffer);
				ss << line_buffer << std::endl;
			}

			current = current->m_next;
		}
	}

	return ss.str();
}

void MemoryProfiler::NewPointer(allocation_t* pointer)
{
	if (!s_instance || !s_instance->m_isValid)
		return;
	MemoryProfiler::AddAllocationCount();
	MemoryProfiler::AddFrameAllocationCount();
	MemoryProfiler::AddMemoryByte(pointer->m_byteSize);
#if (TRACK_MEMORY == TRACK_MEMORY_VERBOSE)
	CallStack* callStack = CreateCallStack(0);
	MemoryProfiler::LogAllocationCallStack(pointer, callStack);
#endif
}

void MemoryProfiler::DeletePointer(allocation_t* pointer)
{
	if (s_instance && s_instance->m_isValid)
	{
		MemoryProfiler::SubAllocationCount();
		MemoryProfiler::AddFrameFreeCount();
		MemoryProfiler::SubMemoryByte(pointer->m_byteSize);
#if (TRACK_MEMORY == TRACK_MEMORY_VERBOSE)
		MemoryProfiler::EraseAllocationCallStack(pointer);
#endif
	}

	free(pointer);
}

MemoryProfiler* MemoryProfiler::GetInstance()
{
	return s_instance;
}

#if TRACK_MEMORY != TRACK_MEMORY_NONE

void* operator new (const size_t size)
{

	size_t alloc_size = size + sizeof(allocation_t);
	allocation_t *ptr = (allocation_t*)malloc(alloc_size);
	ptr->m_byteSize = size;

	MemoryProfiler::NewPointer(ptr);
	Profiler::AddAllocation(size);

	return ptr + 1;

}



void operator delete(void *ptr)
{

	allocation_t *size_ptr = (allocation_t*)ptr;
	size_ptr--;

	Profiler::AddFree(size_ptr->m_byteSize);
	MemoryProfiler::DeletePointer(size_ptr);
}

#endif