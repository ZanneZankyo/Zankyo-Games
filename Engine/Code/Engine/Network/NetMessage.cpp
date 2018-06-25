#include "NetMessage.hpp"

NetMessage::NetMessage()
	: BinaryStream(LITTLE_ENDIAN)
	, m_sender(nullptr)
	, m_messageTypeIndex(0)
	, m_payloadBytesUsed(0)
{
	m_stream.reserve(MAX_PAYLOAD_SIZE);
}

NetMessage::NetMessage(uint8_t typeIndex)
	: BinaryStream(LITTLE_ENDIAN)
	, m_sender(nullptr)
	, m_messageTypeIndex(typeIndex)
	, m_payloadBytesUsed(0)
{
	m_stream.reserve(MAX_PAYLOAD_SIZE);
}

NetMessage::NetMessage(byte_t* bytes, size_t size)
	: BinaryStream(bytes,size,LITTLE_ENDIAN)
	, m_sender(nullptr)
	, m_messageTypeIndex(0)
	, m_payloadBytesUsed(0)
{
	m_payloadBytesUsed = ReadBuffer<uint16_t>() - 1;
	m_messageTypeIndex = ReadBuffer<byte_t>();
}

NetMessage::NetMessage(const NetMessage& copy)
	: BinaryStream(copy)
	, m_sender(copy.m_sender)
	, m_messageTypeIndex(copy.m_messageTypeIndex)
	, m_payloadBytesUsed(copy.m_payloadBytesUsed)
{

}

bool NetMessage::WriteString(const std::string& string)
{
	WriteBuffer((uint16_t)string.size());
	m_stream.insert(m_stream.end(), string.begin(), string.end());
	m_payloadBytesUsed += sizeof(uint16_t) + string.size();
	return true;
}

std::string NetMessage::ReadString(size_t stringLength /*= -1*/)
{
	if (stringLength == -1)
		stringLength = (size_t)ReadBuffer<uint16_t>();
	if (stringLength == 0 || stringLength == -1)
		return "";

	std::string string(m_stream.begin() + m_bufferOffset, m_stream.begin() + m_bufferOffset + stringLength);
	m_bufferOffset += stringLength;
	return string;
}
