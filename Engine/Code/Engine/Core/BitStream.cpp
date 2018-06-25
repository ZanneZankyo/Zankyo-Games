#include "BitStream.hpp"


const uint BitStream::BITS_PER_BYTE = 8;

void BitStream::InsertBit(bool bit)
{
	uint byteOffset = m_bitOffset / BITS_PER_BYTE;
	uint bitOffset = m_bitOffset % BITS_PER_BYTE;
	byte_t* bp = m_stream.data() + byteOffset;
	byte_t mask = 1 << bitOffset;
	byte_t offMask = ~mask;
	*bp = (*bp) & offMask;
	if (bit)
		*bp |= mask;
	++m_bitOffset;
}

bool BitStream::WriteBits(void* data, uint bitCount)
{
	uint byteCount = bitCount / BITS_PER_BYTE;
	uint remainingBits = bitCount % BITS_PER_BYTE;
	byte_t* buffer = (byte_t*)data;
	for (uint byteIndex = 0; byteIndex < byteCount; byteIndex++)
	{
		byte_t b = buffer[byteIndex];
		for (uint bitIndex = 0; bitIndex < BITS_PER_BYTE; bitIndex++)
		{
			InsertBit((b & 1 << bitIndex) != 0);
		}
	}
	byte_t lastByte = buffer[byteCount];
	for (uint bitIndex = 0; bitIndex < BITS_PER_BYTE; bitIndex++)
	{
		InsertBit((lastByte & 1 << bitIndex) != 0);
	}
	return true;
}

uint BitStream::GetByteCountToWrite() const
{
	return (m_bitOffset + (BITS_PER_BYTE - 1)) / BITS_PER_BYTE;
}

bool BitStream::ReadBit()
{
	uint byteOffset = m_bitOffset / BITS_PER_BYTE;
	uint bitOffset = m_bitOffset % BITS_PER_BYTE;
	byte_t b = *(m_stream.data() + byteOffset);
	byte_t mask = 1 << bitOffset;
	return b & mask != 0;
}

byte_t BitStream::ReadByte(uint bitCount)
{
	byte_t b = 0;
	for (uint bitIndex = 0; bitIndex < bitCount; bitIndex++)
	{
		bool bit = ReadBit();
		if (bit)
		{
			byte_t mask = 1 << bitIndex;
			b |= mask;
		}
	}
	return b;
}

bool BitStream::ReadBits(void* data, uint byteSize, uint bitsToRead)
{
	byte_t* buffer = (byte_t*)data;
	memset(buffer, 0, BITS_PER_BYTE);
	uint byteCount = bitsToRead / BITS_PER_BYTE;
	uint remainingBitCount = bitsToRead % BITS_PER_BYTE;
	for (uint byteIndex = 0; byteIndex < byteCount; byteIndex++)
	{
		byte_t b = ReadByte(BITS_PER_BYTE);
		buffer[byteIndex] = b;
	}
	buffer[byteCount] = ReadByte(remainingBitCount);
	return true;
}
