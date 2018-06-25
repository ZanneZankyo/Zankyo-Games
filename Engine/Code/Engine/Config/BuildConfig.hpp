#pragma once

// #define DETECT_MEMORY_OVERRUN
#define TRACK_MEMORY_NONE		(-1)
#define TRACK_MEMORY_BASIC		(0)
#define TRACK_MEMORY_VERBOSE	(1)

// If not defined, we will not track memory at all
// BASIC will track bytes used, and count
// VERBOSE will track individual callstacks
#if defined(PROFILING)
#define TRACK_MEMORY          TRACK_MEMORY_NONE
//#define PROFILER_ENABLED
#elif defined(FINAL_BUILD)
// undedfined
#else 
#define TRACK_MEMORY          TRACK_MEMORY_NONE
#endif


#define MAX_PROFILER_FRAMES 1