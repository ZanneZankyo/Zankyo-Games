#pragma once
#include "Time.hpp"
#include <vector>
#include <queue>
#include "Engine\Utils\ZTree.hpp"
#include "Engine\Config\BuildConfig.hpp"
#include <wtypes.h>

				
#ifdef PROFILER_ENABLED						
#define PROFILE_SCOPE(s) ProfileLogScope __pscope_##__LINE__##(s)
#define PROFILE_FUNCTION ProfileLogScope __pscope_##__LINE__##(__FUNCTION__)
#else
#define PROFILE_SCOPE(s)
#define PROFILE_FUNCTION
#endif				

class ProfileLogScope
{
private:
	const char* m_scopeMessage;
	double m_timeAtStart;
public:
	ProfileLogScope(const char* scopeMessage);
	~ProfileLogScope();
};

struct ProfilerTag
{
	std::string m_tag;
	float m_time;
	size_t m_allocateBytes;
	size_t m_allocateCount;
	size_t m_freeBytes;
	size_t m_freeCount;
	ProfilerTag(
		const std::string& tag, 
		float time = 0.f,
		size_t allocateBytes = 0,
		size_t allocateCount = 0,
		size_t freeBytes = 0,
		size_t freeCount = 0
	) 
		: m_tag(tag)
		, m_time(time) 
		, m_allocateBytes(allocateBytes)
		, m_allocateCount(allocateCount)
		, m_freeBytes(freeBytes)
		, m_freeCount(freeCount)
	{}
};

class Profiler
{
public:
	static void Push(const std::string& tag);
	static void Push(ProfilerTag node);
	static void Pop();
	static TreeNode<ProfilerTag>* GetActiveNode();
	static ProfilerTag& GetActive();
	static void Start();
	static void End();

	static void AddAllocation(size_t byteSize, size_t allocateCount = 1);
	static void AddFree(size_t byteSize, size_t freeCount = 1);

	static void Pause(const std::string& ignoreParm = "");
	static void Resume(const std::string& ignoreParm = "");
	static void SnapShot(const std::string& ignoreParm = "");

	static Tree<ProfilerTag>* ProfilerGetPreviousFrameTree(int framesBefore);
	static float GetTotalTime(TreeNode<ProfilerTag>* node);

	static void SetTreeView(const std::string& ignoredParm = "");
	static void SetFlatView(const std::string& ignoredParm = "");

	static void Log(const std::string& ignoredParm = "");

	static bool IsValid();
	static bool IsPaused();
	static bool IsTreeView();

	static bool CompNodeByTotalTime(TreeNode<ProfilerTag>* a, TreeNode<ProfilerTag>* b);

	Profiler();

	bool m_isPaused;
	bool m_isTreeView;
	bool m_isSnapShot;

	Tree<ProfilerTag> m_tree;
	TreeNode<ProfilerTag>* m_activeNode;

	std::deque<Tree<ProfilerTag>> m_compeletedFrames;

	DWORD m_threadId;

	static Profiler* s_instance;
	
};

inline bool Profiler::IsValid()
{
	return s_instance != nullptr;
}

inline bool Profiler::IsPaused()
{
	return s_instance->m_isPaused;
}

inline bool Profiler::IsTreeView()
{
	return s_instance->m_isTreeView;
}

inline void Profiler::Pause(const std::string&)
{
	s_instance->m_isPaused = true;
	//s_instance->m_tree.Destroy();
	//s_instance->m_activeNode = nullptr;
}

inline void Profiler::Resume(const std::string&)
{
	s_instance->m_isPaused = false;
	//s_instance->m_tree.OpenNew();
}

inline void Profiler::SnapShot(const std::string&)
{
	s_instance->m_isPaused = false;
	s_instance->m_isSnapShot = true;
}

struct ProfilerReportInfo
{
	std::string tagName;
	int calls;
	float totalPercentage;
	float totalTime;
	float selfPercentage;
	float selfTime;
	int callDepth;
	size_t allocateBytes;
	size_t allocateCount;
	size_t freeBytes;
	size_t freeCount;

	static bool CompByTotalTime(const ProfilerReportInfo& a, const ProfilerReportInfo& b);
	static bool CompBySelfTime(const ProfilerReportInfo& a, const ProfilerReportInfo& b);
};

class ProfilerReport
{
public:
	ProfilerReport(int frameBefore = 0)
		: m_tree(Profiler::ProfilerGetPreviousFrameTree(frameBefore))
		, m_reports()
	{}

	Tree<ProfilerTag>* m_tree;
	
	float m_totalTime;
	std::vector<ProfilerReportInfo> m_reports;

	void CreateTreeView();
	void CreateFlatView();

	std::string ToString();

public:
	void CreateTreeView(std::vector<ProfilerReportInfo>& reports, TreeNode<ProfilerTag>* node, int callDepth = 0);
	void CreateFlatView(std::vector<ProfilerReportInfo>& reports, TreeNode<ProfilerTag>* node);
};