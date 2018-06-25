#pragma once
#include "Network.hpp"
#include "Engine\Core\BinaryStream.hpp"



class NetMessage : public BinaryStream
{

public:
	NetMessage();
	NetMessage(uint8_t typeIndex);
	NetMessage(byte_t* bytes, size_t size);
	NetMessage(const NetMessage& copy);


	virtual bool WriteString(const std::string& string) override;
	virtual std::string ReadString(size_t stringLength = -1) override;

	//uint ReadByte();
	//uint WriteByte();
public:

	static const size_t MAX_PAYLOAD_SIZE = 2 KB;

	uint8_t m_messageTypeIndex;
	NetConnection *m_sender;
	//byte_t m_payload[MAX_PAYLOAD_SIZE]; // replaced with BinaryStream::m_stream
	uint m_payloadBytesUsed;
};