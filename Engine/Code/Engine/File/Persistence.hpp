#pragma once
#include <string>
#include <vector>
#include "Engine\Core\EngineBase.hpp"

class Persistence
{
public:
	static bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector< byte_t >& out_buffer);
	static bool SaveBufferToBinaryFile(const std::string& filePath, const std::vector< byte_t >& buffer);
	static bool SaveStringToFile(const std::string& filePath, const std::string& string);
	template<typename Type>
	static Type ReadBuffer(const std::vector< byte_t >& buffer, size_t& bufferOffset);
	template<>
	static bool ReadBuffer(const std::vector< byte_t >& buffer, size_t& bufferOffset);

	//first byte should be size_t size
	template<typename Type>
	static std::vector<Type> ReadBufferArray(const std::vector< byte_t >& buffer, size_t& bufferOffset);
	template<typename Type>
	static std::vector<Type> ToBuffer(const Type& object);
	template<typename Type>
	static bool PushBuffer(const Type& object, std::vector<byte_t>& buffer, bool preReserved = false);
	template<>
	static bool PushBuffer(const bool& object, std::vector<byte_t>& buffer, bool preReserved);
	//also push size info to buffer
	template<typename Type>
	static bool PushBuffer(const Type* objectArray, size_t arraySize, std::vector<byte_t>& buffer, bool preReserved = false);
	static std::string ReadBufferAsString(const std::vector< byte_t >& buffer, size_t& bufferOffset, size_t stringLength = -1);
};

template<typename Type>
inline Type Persistence::ReadBuffer(const std::vector<byte_t>& buffer, size_t& bufferOffset)
{
	//return *(Type*)(buffer.data() + bufferOffset += sizeof(Type));
	Type value = *(Type*)(buffer.data() + bufferOffset);
	bufferOffset += sizeof(Type);
	return value;
}

template<typename Type>
std::vector<Type> Persistence::ReadBufferArray(const std::vector< byte_t >& buffer, size_t& bufferOffset)
{
	size_t size = ReadBuffer<size_t>(buffer, bufferOffset);
	std::vector<Type> array;
	for (size_t count = 0; count < size; count++)
		array.push_back(ReadBuffer<Type>(buffer, bufferOffset));
	return array;
}

template<typename Type>
std::vector<Type> Persistence::ToBuffer(const Type& object)
{
	std::vector<byte_t> buffer;
	unsigned int typeByteSize = sizeof(Type);
	buffer.resize(typeByteSize, 0);
	byte_t *bufferPtr = (byte_t*)&object;
	for (int i = 0; i < typeByteSize; i++)
		buffer[i] = bufferPtr[i];
	return buffer;
}

template<typename Type>
bool Persistence::PushBuffer(const Type& object, std::vector<byte_t>& buffer, bool preReserved/* = false*/)
{
	unsigned int typeByteSize = sizeof(Type);
	byte_t *bufferPtr = (byte_t*)&object;
	if(!preReserved)
		buffer.reserve(buffer.size() + typeByteSize);
	for (size_t i = 0; i < typeByteSize; i++)
		buffer.push_back(bufferPtr[i]);
	return true;
}

template<typename Type>
bool Persistence::PushBuffer(const Type* objectArray, size_t arraySize, std::vector<byte_t>& buffer, bool preReserved/* = false*/)
{
	bool result = true;
	buffer.reserve(buffer.size() + sizeof(arraySize) + sizeof(Type) * arraySize);
	result = PushBuffer(arraySize, buffer, preReserved);
	if (!result)
		return result;
	for (size_t i = 0; i < arraySize; i++)
	{
		result = PushBuffer(objectArray[i],buffer, preReserved);
		if (!result)
			return result;
	}
	return result;
}

/*
template<typename Type>
inline Type Persistence::ReadBufferArray(const std::vector<byte_t>& buffer, size_t & bufferOffset, size_t arrayLength)
{
	Type value = *(Type*)(buffer.data() + bufferOffset);
	bufferOffset += sizeof(Type);
	return value;
}*/


template<>
inline bool Persistence::ReadBuffer(const std::vector<byte_t>& buffer, size_t& bufferOffset)
{
	char value = ReadBuffer<char>(buffer, bufferOffset);
	return value == 0 ? false : true;
}

template<>
inline bool Persistence::PushBuffer(const bool& object, std::vector<byte_t>& buffer, bool preReserved/* = false*/)
{
	char boolByte = object ? 1 : 0;
	return PushBuffer(boolByte, buffer, preReserved);
}