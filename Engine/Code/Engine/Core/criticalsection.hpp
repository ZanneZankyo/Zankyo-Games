#pragma once

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "EngineBase.hpp"

#define WIN32_LEAN_AND_MEAN

#include "Win32Include.hpp"

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
// Infoknowledge Management System

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

// creates a scoped critical section object named __lock_<LINE#>( &cs )
#define COMBINE1(X,Y) X##Y		
#define COMBINE(X, Y) COMBINE1(X, Y)
#define SCOPE_LOCK( cs ) ScopeCriticalSection COMBINE(__lock_,__LINE__)(&cs)
//#define SCOPE_LOCK( csp ) ScopeCriticalSection COMBINE(__scs_,__LINE__)(csp)

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Mutual Exclusive [mutex]: 
// Fair:  Serviced in order of request.
// Recursive: Someone is allowed to lock multiple times.
class CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

	void Initialize();

	// Enteres the lock - only one thread is allowed to be inside this lock at a time, every
	// other caller will block on this call until they're allowed to enter.
	void lock();

	// Tries to enter the lock - returns TRUE if the lock was entered, false if it failed.
	bool try_lock();

	// Leave the lock - allowing the next person to lock to be able to enter.
	void unlock();

public:
	CRITICAL_SECTION cs;
};

//------------------------------------------------------------------------
class ScopeCriticalSection
{
public:
	ScopeCriticalSection(CriticalSection *ptr);
	~ScopeCriticalSection();

public:
	CriticalSection *cs_ptr;
};

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/