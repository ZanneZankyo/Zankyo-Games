#pragma once
#include "EngineBase.hpp"

template <typename ...ARGS>
class Event
{
public:
	struct EventRegisterInfo;
	typedef void(*EventCallback)(EventRegisterInfo*, ARGS...);
	typedef void(*GlobalFunctionCallback)(void*, ARGS...);
	struct EventRegisterInfo
	{
		EventCallback callback;
		void* userCallback;
		void* userArgument;
	};

public:
	std::vector<EventRegisterInfo> m_registrations;

public:
	Event() : m_registrations() {}
	Event(void *userArguments, GlobalFunctionCallback callback)
		: m_registrations()
	{
		RegisterGlobalCall(userArguments, callback);
	}
	template <typename ObjClass>
	Event(ObjClass *obj, void (ObjClass::*MemberCallBack)(ARGS...)) 
		: m_registrations()
	{
		RegisterMemberCall(obj, MemberCallBack);
	}
	Event(const Event &copy)
		: m_registrations(copy.m_registrations)
	{}
	
	template <typename ObjClass, typename MemberCallback>
	static void CallMethod(EventRegisterInfo *info, ARGS ...args);
	static void CallGlobal(EventRegisterInfo *info, ARGS ...args);
	
	void RegisterGlobalCall(void *userArguments, GlobalFunctionCallback callback)
	{
		EventRegisterInfo info;
		info.callback = CallGlobal;
		info.userCallback = callback;
		info.userArgument = userArguments;
		m_registrations.push_back(info);
	}
	template <typename ObjClass>
	void RegisterMemberCall(ObjClass *obj, void (ObjClass::*MemberCallBack)(ARGS...))
	{
		EventRegisterInfo info;
		info.callback = CallMethod<ObjClass, decltype(MemberCallBack)>;
		info.userCallback = *(void**)(&MemberCallBack);
		info.userArgument = obj;
		m_registrations.push_back(info);
	}
	void Trigger(ARGS ...arguments)
	{
		for (uint index = 0; index < m_registrations.size(); ++index) {
			EventRegisterInfo &info = m_registrations[index];
			info.callback(&info, arguments...);
		}
	}
};

//--------------------------------------------------------------------
template <typename ...ARGS>
void Event<ARGS...>::CallGlobal(EventRegisterInfo *info, ARGS ...args)
{
	GlobalFunctionCallback callback = (GlobalFunctionCallback)(info->userCallback);
	callback(info->userArgument, args...);
}

//--------------------------------------------------------------------
template <typename ...ARGS>
template <typename ObjClass, typename MemberCallback>
void Event<ARGS...>::CallMethod(EventRegisterInfo *info, ARGS ...args)
{
	MemberCallback memberCallback = *(MemberCallback*)&(info->userCallback);
	ObjClass *obj = (ObjClass*)(info->userArgument);
	(obj->*memberCallback)(args...);
}