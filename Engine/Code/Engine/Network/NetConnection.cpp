#include "NetConnection.hpp"

NetConnection::NetConnection(NetSession* owner, net_address_t address, uint8_t connectionIndex)
	: m_owner(owner)
	, m_address(address)
	, m_connectionIndex(connectionIndex)
	, m_prev(nullptr)
	, m_next(nullptr)
{}

NetConnection::~NetConnection()
{

}
