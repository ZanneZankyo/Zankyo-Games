#include "BinaryStream.hpp"
#include <string>

bool BinaryStream::ShouldFlipBytes()
{
	return m_endianness != GetHostOrder();
}

const byte_t* BinaryStream::GetBuffer() const
{
	return m_stream.data();
}

const size_t BinaryStream::GetSize() const
{
	return m_stream.size();
}

void BinaryStream::ReserveMore(const size_t additionalBytes)
{
	m_stream.reserve(m_stream.size() + additionalBytes);
}

void BinaryStream::MergeByteStream(const BinaryStream& source)
{
	m_stream.insert(m_stream.end(), source.m_stream.begin(), source.m_stream.end());
}

BinaryStream::BinaryStream(Endianness endianness)
	: m_stream()
	, m_bufferOffset(0)
	, m_endianness(endianness)
	, m_shouldFlip(ShouldFlipBytes())
{}

BinaryStream::BinaryStream(byte_t* bytes, size_t size, Endianness endianness)
	: m_stream(bytes, bytes + size)
	, m_bufferOffset(0)
	, m_endianness(endianness)
	, m_shouldFlip(ShouldFlipBytes())
{}

BinaryStream::BinaryStream(const BinaryStream& copy)
	: m_stream(copy.m_stream)
	, m_bufferOffset(copy.m_bufferOffset)
	, m_endianness(copy.m_endianness)
	, m_shouldFlip(copy.m_shouldFlip)
{

}

std::string BinaryStream::ReadAllAsString()
{
	return std::string(m_stream.begin(), m_stream.end());
}

std::string BinaryStream::ReadString(size_t stringLength /*= -1*/)
{
	if(stringLength == -1)
		stringLength = ReadBuffer<size_t>();
	if (stringLength == 0)
		return "";
	std::string string(m_stream.begin() + m_bufferOffset, m_stream.begin() + m_bufferOffset + stringLength);
	m_bufferOffset += stringLength;
	return string;
}

bool BinaryStream::WriteString(const std::string& string)
{
	WriteBuffer(string.size());
	m_stream.insert(m_stream.end(), string.begin(), string.end());
	return true;
}
