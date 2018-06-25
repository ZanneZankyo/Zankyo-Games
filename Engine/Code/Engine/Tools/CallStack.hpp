#pragma once
#include "Engine\Core\EngineBase.hpp"
#include <stdint.h>
#include <string>

#define MAX_FRAMES_PER_CALLSTACK (128)

struct callstack_line_t
{
	char filename[128];
	char function_name[1024];
	uint32_t line;
	uint32_t offset;
};

class CallStack
{
public:
	CallStack();
	CallStack(const CallStack& copy);
	uint32_t hash;
	uint frame_count;
	void* frames[MAX_FRAMES_PER_CALLSTACK];

	void Initialize();
	bool operator == (const CallStack& comp) const;
	bool operator < (const CallStack& comp) const;
};

bool CallStackSystemInit();
void CallStackSystemDeinit();

// Feel free to reorg this in a way you like - this is very C style.  
// Really, we just want to make sure these callstacks are not allocated on the heap.
// - You could creat them in-place in the meta-data if you prefer (provide memory to fill)
// - You could overload new on the Callstack class, cause "new Callstack(skip_frames)" to call that, a
//   and keeping this functionality.

// As this is - this will create a callstack using malloc (untracked allocation), skipping the first few frames.
CallStack* CreateCallStack(uint skip_frames);
void DestroyCallStack(CallStack *c);

std::string CallStackGetString(CallStack *cs, uint const max_lines);

uint CallStackGetLines(callstack_line_t *line_buffer, uint const max_lines, CallStack *cs);