#include "RemoteCommandService.hpp"
#include "NetMessage.hpp"
#include "NetConnection.hpp"
#include "TCPConnection.hpp"
#include "TCPSession.hpp"
#include "NetSession.hpp"
#include "Engine\Core\Console.hpp"
#include "..\Core\StringUtils.hpp"

RemoteCommandService* RemoteCommandService::s_instance = nullptr;

RemoteCommandService* RemoteCommandService::GetInstance()
{
	return s_instance;
}

void RemoteCommandService::Setup()
{
	s_instance = new RemoteCommandService();
	NetMessageEvent onCommandEvent = std::bind(&RemoteCommandService::OnCommand, s_instance, std::placeholders::_1);
	NetMessageEvent onMessageEvent = std::bind(&RemoteCommandService::OnMessage, s_instance, std::placeholders::_1);
	s_instance->m_session.RegisterMessageEvent(SEND_COMMAND, onCommandEvent);
	s_instance->m_session.RegisterMessageEvent(SEND_MESSAGE, onMessageEvent);

	Console::AssignCommand("send_message", SendBackMessage);

	Console::AssignCommand("remote_join", RemoteCommandService::Join);
	Console::AssignCommand("remote_command", RemoteCommandService::SendRemoteCommand);
	Console::AssignCommand("remote_command_all", RemoteCommandService::SendRemoteCommandToAll);
	Console::AssignCommand("remote_command_id", RemoteCommandService::SendRemoteCommandToId);
	Console::AssignCommand("remote_use_echo", RemoteCommandService::SetAllowRemoteEcho);
}

void RemoteCommandService::OnCommand(NetMessage* message)
{
	s_instance->m_currentSender = (TCPConnection*)message->m_sender;
	std::string commandAndArguments = message->ReadString();
	Console::Log("[" + NetAddressToString(message->m_sender->m_address) + "]: " + commandAndArguments);
	if(s_instance->m_allowRemoteEcho)
		Console::RegisterEvent(Console::e_onPrintMessage, RemoteCommandService::SendBackMessage);
	Console::RunCommand(commandAndArguments);
	Console::UnRegisterEvent(Console::e_onPrintMessage, RemoteCommandService::SendBackMessage);
	s_instance->m_currentSender = nullptr;

}

void RemoteCommandService::OnMessage(NetMessage* message)
{
	std::string response = message->ReadString();
	Console::Log("[" + NetAddressToString(message->m_sender->m_address) + "]: " + response);
}

void RemoteCommandService::SendCommandToOthers(const std::string& commandAndArguments)
{
	NetMessage message(SEND_COMMAND);
	message.WriteString(commandAndArguments);
	s_instance->m_session.SendMessageToOthers(message);
}

void RemoteCommandService::SendCommandToAll(const std::string& commandAndArguments)
{
	NetMessage message(SEND_COMMAND);
	message.WriteString(commandAndArguments);
	s_instance->m_session.SendMessageToAll(message);
}

void RemoteCommandService::SendBackMessage(const std::string& string)
{
	NetMessage message(SEND_MESSAGE);
	message.WriteString(string);
	s_instance->m_currentSender->Send(&message);
}

void RemoteCommandService::Update()
{
	//if(m_session.IsRunning())
	if(s_instance->m_session.IsRunning())
		s_instance->m_session.Update();
	else
	{
		if (!s_instance->m_session.Join(GetMyAddress(RCS_PORT)))
		{
			s_instance->m_session.Host( RCS_PORT );
			Console::Log("RemoteCommandService hosted: " + NetAddressToString(s_instance->m_session.m_hostConnection->m_address));
			//m_session.StartListening(); // process of start listening is done in Host() ?
		}
		else
			Console::Log("RemoteCommandService joined: " + NetAddressToString(s_instance->m_session.m_hostConnection->m_address));
	}

}

void RemoteCommandService::Join(const net_address_t& address)
{
	s_instance->m_session.Leave();
	s_instance->m_session.Join(address);
}

void RemoteCommandService::Join(const std::string& address)
{
	Join(StringToNetAddress(address));
}

void RemoteCommandService::SendRemoteCommand(const std::string& commandAndArguments)
{
	NetMessage message(SEND_COMMAND);
	message.WriteString(commandAndArguments);
	s_instance->m_session.SendMessageToOthers(message);
}

void RemoteCommandService::SendRemoteCommandToAll(const std::string& commandAndArguments)
{
	NetMessage message(SEND_COMMAND);
	message.WriteString(commandAndArguments);
	s_instance->m_session.SendMessageToAll(message);
}

void RemoteCommandService::SendRemoteCommandToId(const std::string& idAndCommandAndArguments)
{
	auto idAndComAndArgs = StringUtils::SplitAtFirstToken(idAndCommandAndArguments, ' ');
	uint8_t id = (uint8_t)stoi(idAndComAndArgs.first);
	std::string commandAndArguments = idAndComAndArgs.second;
	NetMessage message(SEND_COMMAND);
	message.WriteString(commandAndArguments);
	s_instance->m_session.SendMessageToId(message, id);
}

void RemoteCommandService::SetAllowRemoteEcho(const std::string& boolString)
{
	s_instance->m_allowRemoteEcho = StringUtils::ToBool(boolString);
}

