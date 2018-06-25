#pragma once
#include <vector>
#include "EngineBase.hpp"

class BitStream
{
public:
	void InsertBit(bool bit);
	bool WriteBits(void* data, uint bitCount);
	uint GetByteCountToWrite() const;
	bool ReadBit();
	byte_t ReadByte(uint bitCount);
	bool ReadBits(void* data, uint byteSize, uint bitsToRead);
public:
	uint m_bitOffset;
	std::vector<byte_t> m_stream;
public:
	static const uint BITS_PER_BYTE;
};