#include "Profiler.hpp"
#include <processthreadsapi.h>
#include "Engine\Math\MathUtils.hpp"
#include <sstream>
#include "Engine\Tools\Memory.hpp"
#include "Console.hpp"
#include "Logger.hpp"

Profiler* Profiler::s_instance = nullptr;

ProfileLogScope::ProfileLogScope(const char* scopeMessage)
	: m_scopeMessage(scopeMessage)
	, m_timeAtStart(GetCurrentTimeSeconds())
{
	Profiler::Push(scopeMessage);
}

ProfileLogScope::~ProfileLogScope()
{
	double timeAtEnd = GetCurrentTimeSeconds();
	double timeElapsed = timeAtEnd - m_timeAtStart;
	if (Profiler::IsValid() && Profiler::GetActiveNode())
	{
		Profiler::GetActive().m_time = (float)timeElapsed;
		Profiler::Pop();
	}
	
	//ProfilerTag tag(m_scopeMessage, timeElapsed);
	//DebuggerPrintf("ProfileLogScope \"%s\" took %.02f ms\n ", m_scopeMessage, (timeAtEnd - m_timeAtStart) * 1000);
}

void Profiler::Push(ProfilerTag node)
{
	if (!s_instance)
		return;

	auto currentThreadId = GetCurrentThreadId();
	if (currentThreadId != s_instance->m_threadId)
		return;

	if (s_instance->m_activeNode)
	{
		s_instance->m_activeNode->Push(node);
		s_instance->m_activeNode = *(s_instance->m_activeNode->Children().end() - 1);
	}
	else// if (!s_instance->m_isPaused)
	{
		s_instance->m_tree.SetHead(node);
		s_instance->m_activeNode = s_instance->m_tree.Head();
	}
}

void Profiler::Push(const std::string& tag)
{
	ProfilerTag nodeTag(tag);
	Push(nodeTag);
}

void Profiler::Pop()
{
	if (!s_instance)
		return;

	auto currentThreadId = GetCurrentThreadId();
	if (currentThreadId != s_instance->m_threadId)
		return;

	if (s_instance->m_activeNode)
	{
		s_instance->m_activeNode = s_instance->m_activeNode->Parent();
		if (!s_instance->m_activeNode)
		{
			if (!s_instance->m_isPaused)
			{
				s_instance->m_compeletedFrames.push_front(s_instance->m_tree);
				if (s_instance->m_compeletedFrames.size() > MAX_PROFILER_FRAMES)
					s_instance->m_compeletedFrames.pop_back();
				s_instance->m_tree.OpenNew();
				if (s_instance->m_isSnapShot)
				{
					s_instance->m_isPaused = true;
					s_instance->m_isSnapShot = false;
				}
			}
			else
			{
				s_instance->m_tree.Destroy();
				s_instance->m_tree.OpenNew();
			}
		}
	}
		
}

TreeNode<ProfilerTag>* Profiler::GetActiveNode()
{
	return s_instance->m_activeNode;
}

ProfilerTag& Profiler::GetActive()
{
	return s_instance->m_activeNode->Data();
}

void Profiler::Start()
{
	
	s_instance = new Profiler();
	Console::AssignCommand("log_profiler", Profiler::Log);
	Console::AssignCommand("profiler_pause", Profiler::Pause);
	Console::AssignCommand("profiler_resume", Profiler::Resume);
	Console::AssignCommand("profiler_snapshot", Profiler::SnapShot);
	Console::AssignCommand("set_profiler_tree_view", Profiler::SetTreeView);
	Console::AssignCommand("set_profiler_flat_view", Profiler::SetFlatView);
}

void Profiler::End()
{
	SAFE_DELETE(s_instance);
}



void Profiler::AddAllocation(size_t byteSize, size_t count /*= 1*/)
{
	if (!s_instance || !s_instance->m_activeNode || s_instance->m_isPaused)
		return;

	auto currentThreadId = GetCurrentThreadId();
	if (currentThreadId != s_instance->m_threadId)
		return;

	s_instance->m_activeNode->Data().m_allocateBytes += byteSize;
	s_instance->m_activeNode->Data().m_allocateCount += count;

	TreeNode<ProfilerTag>* currentNode = s_instance->m_activeNode->Parent();
	while (currentNode)
	{
		currentNode->Data().m_allocateBytes += byteSize;
		currentNode->Data().m_allocateCount += count;
		currentNode = currentNode->Parent();
	}
}

void Profiler::AddFree(size_t byteSize, size_t count /*= 1*/)
{
	if (!s_instance || !s_instance->m_activeNode || s_instance->m_isPaused)
		return;

	auto currentThreadId = GetCurrentThreadId();
	if (currentThreadId != s_instance->m_threadId)
		return;

	s_instance->m_activeNode->Data().m_freeBytes += byteSize;
	s_instance->m_activeNode->Data().m_freeCount += count;

	TreeNode<ProfilerTag>* currentNode = s_instance->m_activeNode->Parent();
	while (currentNode)
	{
		currentNode->Data().m_freeBytes += byteSize;
		currentNode->Data().m_freeCount += count;
		currentNode = currentNode->Parent();
	}
}



Tree<ProfilerTag>* Profiler::ProfilerGetPreviousFrameTree(int framesBefore)
{
	if ((int)s_instance->m_compeletedFrames.size() <= framesBefore)
		return nullptr;
	return &s_instance->m_compeletedFrames.at(framesBefore);
}

float Profiler::GetTotalTime(TreeNode<ProfilerTag>* node)
{
	float time = 0.f;
	time += node->Data().m_time;
	for (auto& child : node->Children())
		time += GetTotalTime(child);
	return time;
}

void Profiler::SetTreeView(const std::string&)
{
	s_instance->m_isTreeView = true;
}

void Profiler::SetFlatView(const std::string&)
{
	s_instance->m_isTreeView = false;
}

void Profiler::Log(const std::string&)
{
	ProfilerReport profilerReport;

	if (Profiler::IsTreeView())
		profilerReport.CreateTreeView();
	else
		profilerReport.CreateFlatView();

	std::string text = profilerReport.ToString();

	//Console::Log(text, Rgba::GREEN);
	Logger::LogTagged("Profiler", text);
}

Profiler::Profiler()
	: m_activeNode(nullptr)
	, m_tree()
	, m_compeletedFrames()
	, m_isPaused(false)
	, m_threadId(GetCurrentThreadId())
	, m_isTreeView(true)
	, m_isSnapShot(false)
{}

void ProfilerReport::CreateTreeView()
{
	PROFILE_FUNCTION;
	if (!m_tree) return;
	m_totalTime = m_tree->Head()->Data().m_time;
	CreateTreeView(m_reports, m_tree->Head());
}

void ProfilerReport::CreateTreeView(std::vector<ProfilerReportInfo>& reports, TreeNode<ProfilerTag>* node, int callDepth)
{
	
	ProfilerReportInfo reportInfo;
	reportInfo.tagName = node->Data().m_tag;
	reportInfo.selfTime = node->Data().m_time;
	reportInfo.totalTime = node->Data().m_time;
	reportInfo.totalPercentage = reportInfo.totalTime / m_totalTime;
	reportInfo.callDepth = callDepth;
	reportInfo.calls = 1;

	reportInfo.allocateBytes = node->Data().m_allocateBytes;
	reportInfo.allocateCount = node->Data().m_allocateCount;
	reportInfo.freeBytes = node->Data().m_freeBytes;
	reportInfo.freeCount = node->Data().m_freeCount;

	bool haveSame = false;

	for (auto& child : node->Children())
		reportInfo.selfTime -= child->Data().m_time;
	reportInfo.selfPercentage = reportInfo.selfTime / m_totalTime;

	for (int index = reports.size() - 1; index >= 0; index--)
	{
		ProfilerReportInfo& currentReport = reports[index];

		if (currentReport.callDepth != reportInfo.callDepth)
			break;

		if (currentReport.tagName == reportInfo.tagName)
		{
			currentReport.selfTime = MAX(reportInfo.selfTime, reports[index].selfTime);
			currentReport.selfPercentage = MAX(reportInfo.selfPercentage, reports[index].selfPercentage);
			currentReport.totalTime = MAX(reportInfo.totalTime, reports[index].totalTime);
			currentReport.totalPercentage = MAX(reportInfo.totalPercentage, reports[index].totalPercentage);
			currentReport.calls += reportInfo.calls;

			currentReport.allocateBytes += reportInfo.allocateBytes;
			currentReport.allocateCount += reportInfo.allocateCount;
			currentReport.freeBytes += reportInfo.freeBytes;
			currentReport.freeCount += reportInfo.freeCount;

			haveSame = true;
		}
	}

	if (!haveSame)
	{
		//reportInfo.totalTime = reportInfo.selfTime;
		//reportInfo.totalPercentage = reportInfo.selfPercentage;
		reports.push_back(reportInfo);
	}

	

	if (node->Children().size() > 0)
	{
		sort(node->Children().begin(), node->Children().end(), Profiler::CompNodeByTotalTime);

		for (auto& child : node->Children())
			CreateTreeView(reports, child, callDepth + 1);
	}
	

}

void ProfilerReport::CreateFlatView()
{
	if (!m_tree) return;
	m_totalTime = m_tree->Head()->Data().m_time;
	CreateFlatView(m_reports, m_tree->Head());
	sort(m_reports.begin(), m_reports.end(), ProfilerReportInfo::CompBySelfTime);
}


void ProfilerReport::CreateFlatView(std::vector<ProfilerReportInfo>& reports, TreeNode<ProfilerTag>* node)
{
	ProfilerReportInfo reportInfo;
	reportInfo.tagName = node->Data().m_tag;
	reportInfo.selfTime = node->Data().m_time;
	reportInfo.totalTime = node->Data().m_time;
	reportInfo.totalPercentage = reportInfo.totalTime / m_totalTime;
	reportInfo.callDepth = 0;
	reportInfo.calls = 1;

	reportInfo.allocateBytes = node->Data().m_allocateBytes;
	reportInfo.allocateCount = node->Data().m_allocateCount;
	reportInfo.freeBytes = node->Data().m_freeBytes;
	reportInfo.freeCount = node->Data().m_freeCount;

	bool haveSame = false;

	for (auto& child : node->Children())
		reportInfo.selfTime -= child->Data().m_time;
	reportInfo.selfPercentage = reportInfo.selfTime / m_totalTime;

	for (int index = reports.size() - 1; index >= 0; index--)
	{
		ProfilerReportInfo& currentReport = reports[index];

		if (currentReport.callDepth != reportInfo.callDepth)
			break;

		if (currentReport.tagName == reportInfo.tagName)
		{
			currentReport.selfTime = MAX(reportInfo.selfTime, reports[index].selfTime);
			currentReport.selfPercentage = MAX(reportInfo.selfPercentage, reports[index].selfPercentage);
			currentReport.totalTime = MAX(reportInfo.totalTime, reports[index].totalTime);
			currentReport.totalPercentage = MAX(reportInfo.totalPercentage, reports[index].totalPercentage);
			currentReport.calls += reportInfo.calls;

			currentReport.allocateBytes += reportInfo.allocateBytes;
			currentReport.allocateCount += reportInfo.allocateCount;
			currentReport.freeBytes += reportInfo.freeBytes;
			currentReport.freeCount += reportInfo.freeCount;

			haveSame = true;
		}
	}

	if (!haveSame)
	{
		//reportInfo.totalTime = reportInfo.selfTime;
		//reportInfo.totalPercentage = reportInfo.selfPercentage;
		reports.push_back(reportInfo);
	}

	for (auto& child : node->Children())
		CreateFlatView(reports, child);
}

std::string ProfilerReport::ToString()
{
	PROFILE_FUNCTION;

	if (m_reports.empty())
		return "No Recorded Report";

	size_t maxTagLength = 0;
	size_t maxCallDepth = 0;
	for (auto& info : m_reports)
	{
		if (info.tagName.size() > maxTagLength)
			maxTagLength = info.tagName.size();
		if (info.callDepth > (int)maxCallDepth)
			maxCallDepth = info.callDepth;
	}
		
	maxTagLength += maxCallDepth + 4;
	//maxTagLength = 50;

	std::stringstream ss;

	ss

		<< "CPU" << std::endl
		<< "    Previous Frame Time: " << m_reports[0].totalTime * 1000.f << " ms" << std::endl
		<< "    Frame Rate: " << 1.f / m_reports[0].totalTime << std::endl
		<< std::endl

		<< std::left << std::setw(maxTagLength) << "TAG NAME"

		<< std::left << std::setw(7) << "CALLS"
		<< std::left << std::setw(9) << "% TOTAL"
		<< std::left << std::setw(13) << "TOTAL TIME"
		<< std::left << std::setw(9) << "% SELF"
		<< std::left << std::setw(13) << "SELF TIME"

		<< std::left << std::setw(16) << "ALLCATED BYTES"
		<< std::left << std::setw(16) << "ALLCATED COUNT"
		<< std::left << std::setw(16) << "FREE BYTES"
		<< std::left << std::setw(16) << "FREE COUNT"

		<< std::endl;

	for (auto& info : m_reports)
	{
		ss  << std::left <<  std::setw(info.callDepth+1) << " "
			<< std::left <<  std::setw(maxTagLength - info.callDepth-1) << info.tagName

			<< std::left <<  std::setw(7)  << info.calls
			<< std::left <<  std::setw(9)  << std::setprecision(2) << std::fixed << info.totalPercentage * 100.f
			<< std::left <<  std::setw(13) << Time::ToString(info.totalTime)
			<< std::left <<  std::setw(9)  << std::setprecision(2) << std::fixed << info.selfPercentage * 100.f
			<< std::left <<  std::setw(13) << Time::ToString(info.selfTime)

			<< std::left << std::setw(16) << MemoryProfiler::ToByteSizeString(info.allocateBytes)
			<< std::left << std::setw(16) << info.allocateCount
			<< std::left << std::setw(16) << MemoryProfiler::ToByteSizeString(info.freeBytes)
			<< std::left << std::setw(16) << info.freeCount

			<< std::left <<  std::endl;
	}
	return ss.str();
}

bool ProfilerReportInfo::CompByTotalTime(const ProfilerReportInfo& a, const ProfilerReportInfo& b)
{
	return a.totalTime > b.totalTime;
}

bool ProfilerReportInfo::CompBySelfTime(const ProfilerReportInfo& a, const ProfilerReportInfo& b)
{
	return a.selfTime > b.selfTime;
}

bool Profiler::CompNodeByTotalTime(TreeNode<ProfilerTag>* a, TreeNode<ProfilerTag>* b)
{
	return GetTotalTime(a) > GetTotalTime(b);
}