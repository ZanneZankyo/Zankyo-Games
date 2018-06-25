#pragma once
#include "EngineBase.hpp"
#include <vector>

static constexpr uint32_t ENDIAN_CHECK = 0x01020304; 

enum Endianness
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

bool constexpr IsBigEndian() 
{ 
	return((byte_t*)(&ENDIAN_CHECK))[0] == 0x01; 
}

Endianness constexpr GetHostOrder()
{
	return IsBigEndian() ? BIG_ENDIAN : LITTLE_ENDIAN;
}



class BinaryStream
{
public:
	std::vector<byte_t> m_stream;
	size_t m_bufferOffset;
protected:
	Endianness m_endianness;
	bool m_shouldFlip;
private:
	bool ShouldFlipBytes();

public:

	BinaryStream(Endianness endianness = GetHostOrder());
	BinaryStream(byte_t* bytes, size_t size, Endianness endianness = GetHostOrder());
	BinaryStream(const BinaryStream& copy);

	const byte_t* GetBuffer() const;
	const size_t GetSize() const;

	void ReserveMore(const size_t additionalBytes);
	void MergeByteStream(const BinaryStream& source);

	template<typename Type>
	Type ReadBuffer();
	template<>
	bool ReadBuffer();

	template<typename Type>
	Type ReadBufferNoProgress();
	template<>
	bool ReadBufferNoProgress();

	//No size_t size at head
	virtual std::string ReadAllAsString();
	
	/*
	When stringLength is specified:
	....Read certain length of string.
	======================================
	When stringLength is NOT specified:
	....Read size_t size and head and read[size] length of string.
	*/
	virtual std::string ReadString(size_t stringLength = -1);

	//First byte must be size_t size
	template<typename Type>
	std::vector<Type> ReadBufferArray();

	//template<typename Type>
	//static std::vector<Type> ToBuffer(const Type& object, Endianness endianness = GetHostOrder());

	template<typename Type>
	bool WriteBuffer(const Type& object);
	template<>
	bool WriteBuffer(const bool& object);

	virtual bool WriteString(const std::string& string);

	template<typename Type>
	bool WriteBufferArray(const Type* objectArray, size_t arraySize);

	
};

template<typename Type>
Type BinaryStream::ReadBuffer()
{
	if (m_shouldFlip)
	{
		size_t reverseStartPosition = m_stream.size() - (m_bufferOffset + sizeof(Type));
		std::vector<byte_t> bytes(
			m_stream.rbegin() + reverseStartPosition, 
			m_stream.rbegin() + reverseStartPosition + sizeof(Type)
		);
		Type value = *(Type*)(bytes.data());
		m_bufferOffset += sizeof(Type);
		return value;
	}
	else
	{
		Type value = *(Type*)(m_stream.data() + m_bufferOffset);
		m_bufferOffset += sizeof(Type);
		return value;
	}
}

template<>
bool BinaryStream::ReadBuffer()
{
	byte_t value = m_stream[m_bufferOffset];
	m_bufferOffset += sizeof(byte_t);
	return value == 1;
}

template<typename Type>
Type BinaryStream::ReadBufferNoProgress()
{
	if (m_shouldFlip)
	{
		size_t reverseStartPosition = m_stream.size() - (m_bufferOffset + sizeof(Type));
		std::vector<byte_t> bytes(
			m_stream.rbegin() + reverseStartPosition,
			m_stream.rbegin() + reverseStartPosition + sizeof(Type)
		);
		Type value = *(Type*)(bytes.data());
		return value;
	}
	else
	{
		Type value = *(Type*)(m_stream.data() + m_bufferOffset);
		return value;
	}
}

template<>
bool BinaryStream::ReadBufferNoProgress()
{
	byte_t value = m_stream[m_bufferOffset];
	return value == 1;
}

template<typename Type>
std::vector<Type> BinaryStream::ReadBufferArray()
{
	size_t size = ReadBuffer<size_t>();
	std::vector<Type> array;
	for (size_t count = 0; count < size; count++)
		array.push_back(ReadBuffer<Type>());
	return array;
}

template<typename Type>
bool BinaryStream::WriteBuffer(const Type& object)
{
	size_t typeByteSize = sizeof(Type);
	byte_t *bufferPtr = (byte_t*)&object;
	if (m_shouldFlip)
		for (size_t i = typeByteSize - 1; i != -1; i--)
			m_stream.push_back(bufferPtr[i]);
	else
		for (size_t i = 0; i < typeByteSize; i++)
			m_stream.push_back(bufferPtr[i]);
	
	return true;
}

template<>
bool BinaryStream::WriteBuffer(const bool& object)
{
	byte_t byte = object ? 1 : 0;
	m_stream.push_back(byte);
	return true;
}

template<typename Type>
bool BinaryStream::WriteBufferArray(const Type* objectArray, size_t arraySize)
{
	WriteBuffer(arraySize);
	for (size_t index = 0; index < arraySize; index++)
		WriteBuffer(objectArray[index]);
	return true;
}
