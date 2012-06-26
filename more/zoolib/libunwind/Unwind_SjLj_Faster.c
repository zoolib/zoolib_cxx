/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

/*
Functions that have cleanup to be done in the event of an exception being thrown are very common.
On iOS and other systems 'setjmp/longjmp' (SjLj) exception handling is used. To support the
mechanism the compiler inserts into every exception-savvy function a call to _Unwind_SjLj_Register
as part of the function prologue and a matching call to _Unwind_SjLj_Unregister in the epilogue.

_Unwind_SjLj_Register and _Unwind_SjLj_Unregister are part of libunwind, the version of which used
prior to iOS 5.0 has a nasty performance problem. See lines 80 to 118 of
	<http://www.opensource.apple.com/source/libunwind/libunwind-30/src/Unwind-sjlj.c>

In pseudo code, Register(theNewContext) does this:
	theNewContext->prev = GetCurrentContext()
	SetCurrentContext(theNewContext)
	
and Unregister(theCurrentContext) does this
	SetCurrentContext(theCurrentContext->prev)

That's perfectly reasonable. Unfortunately GetCurrentContext looks like this:
	pthread_once(SetupKey)
	return pthread_get_specific(sKey)

and SetCurrentContext(theContext):
	pthread_once(SetupKey)
	pthread_Set_specific(sKey, theContext)

It's calling pthread_once(SetupKey) to ensure that there is single pthread_key_t allocated for
libunwind's use. And although pthread_once is quick, it's being called three times for *every*
exception-savvy function.

An alternative would be to allocate that key as part of the process' static initialization,
and since iOS 5 a more extreme version of that strategy used -- key 18 is hardcoded for use
by libunwind. See line 86 in
	<http://www.opensource.apple.com/source/Libc/Libc-763.13/pthreads/pthread_machdep.h>

Unwind_SjLj_Faster.c provides implementations of _Unwind_SjLj_Register and
_Unwind_SjLj_Unregister that do not use pthread_once. Instead, we identify the key the system
code has allocated and then do only what the pseudo code above does -- get the prior value,
put it in the new context's prev and set the new context as the current.

The system-provided _Unwind_SjLj_Register/_Unwind_SjLj_Unregister in iOS 5 are more efficient than
these replacements. So if we detect that the allocated key is 18 we assume we're on such a system,
and call through to the original code.

*/

#if defined(__arm__)

#if __APPLE__
	#include <Availability.h>
	#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
		#define Unwind_SjLj_Faster_DISABLED
	#endif
#endif

#ifndef Unwind_SjLj_Faster_DISABLED

#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>

// =================================================================================================

// A minimal definition of struct _Unwind_FunctionContext. The real thing has a lot
// of stuff in it, but we only care about the 'prev' field that should be its first entry.
// We use 'UFC' locally to keep things brief.

typedef struct _Unwind_FunctionContext UFC;

struct _Unwind_FunctionContext
	{ UFC* prev; };

// _Unwind_SjLj_Register and _Unwind_SjLj_Unregister both conform to this type definition.
typedef void (*SjLj_t)(UFC*);

// =================================================================================================

// This will take on the value of the thread-local key allocated by libunwind,
// efficient access to which is really the whole point of this file.

static pthread_key_t spKey;

// =================================================================================================

static pthread_key_t spRecursiveCheck
	(SjLj_t iRegister, SjLj_t iUnregister, pthread_key_t iKey, pthread_key_t iKey_Max)
	{
	// Make a note of the putative UFC* stored at iKey It may well be null, or else
	// something weird, but we don't actually deref it, so no foul.
	UFC* prior = (UFC*)pthread_getspecific(iKey);

	// Register a UFC. Note that we're doing a recursive traversal precisely to reduce the
	// likelhood of aliasing -- a value being stored in a thread-specific-key that just happens
	// to match a probed-for value.
	UFC theUFC;
	iRegister(&theUFC);

	// Get the value (now) stored under iKey.
	UFC* theVal = (UFC*)pthread_getspecific(iKey);

	// and undo the registration
	iUnregister(&theUFC);

	UFC* subsequent = (UFC*)pthread_getspecific(iKey);
	
	if (subsequent == prior
		&& theVal == &theUFC
		&& (theUFC.prev == prior || theUFC.prev == 0))
		{
		// Looks good. Register/Unregister preserved the prior value, set the current value
		// and stuffed the prior value into the current's struct (or zeroed it).
		return iKey;
		}

	if (iKey == iKey_Max)
		{
		// We've gone too deep without finding anything.
		return 0;
		}

	return spRecursiveCheck(iRegister, iUnregister, iKey + 1, iKey_Max);
	}

// =================================================================================================
// MARK: -

static void spRegister_Fast(UFC* fc)
	{
	fc->prev = (UFC*)pthread_getspecific(spKey);
	pthread_setspecific(spKey, fc);	
	}

static void spUnregister_Fast(UFC* fc)
	{ pthread_setspecific(spKey, fc->prev); }

static void spRegister_Initial(UFC* fc);

static SjLj_t spRegister = spRegister_Initial;
static SjLj_t spUnregister;

static void spRegister_Initial(UFC* fc)
	{
	SjLj_t systemRegister = (SjLj_t)dlsym(RTLD_NEXT, "_Unwind_SjLj_Register");
	SjLj_t systemUnregister = (SjLj_t)dlsym(RTLD_NEXT, "_Unwind_SjLj_Unregister");

	spKey = spRecursiveCheck(
		systemRegister, systemUnregister,
		0, // Starting key to try.
		512 // Largest key to try. NB. We may create this many stack frames.
		);

	if (spKey == 0 || spKey == 18)
		{
		spRegister = systemRegister;
		spUnregister = systemUnregister;
		}
	else
		{
		spRegister = spRegister_Fast;
		spUnregister = spUnregister_Fast;
		}

	spRegister(fc);
	}

// =================================================================================================
// MARK: -

void _Unwind_SjLj_Register(UFC* fc)
	{ spRegister(fc); }

void _Unwind_SjLj_Unregister(UFC* fc)
	{ spUnregister(fc); }

#endif // Unwind_SjLj_Faster_DISABLED

#endif // defined(__arm__)
