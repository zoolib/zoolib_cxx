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
Summary
-------
This file should be first in your executable's link order. If that's not possible, see comments
in _Unwind_SjLj_Register.

SjLj-based exceptions on iOS require that _Unwind_SjLj_Register/_Unwind_SjLj_Unregister be
frequently called. This file provides replacement implementations which will be called
by an executable's own code (though not by system libraries) and which are simpler and
thus quicker than the standrd versions, but remain compatible with them.

Dicussion
---------
To support exceptions one needs at least two pieces of information.
	1. A way to identify the point at which a particular throw will be caught.
	2. A list of the work has to be (un)done between a throw and matched catch.

This file is concerned with making management of the latter more efficient on systems using
libunwind, for my purposes that means iPhone/iPad.

In most environments we have zero-overhead exceptions. In the normal course of affairs code just
runs, if you don't thow an exception there is no runtime cost. The compiler embeds additional meta
information which is inspected by the runtime throw and which provides instruction as to what
cleanup code need be executed.

Zero-overhead exceptions fundamentally rely on a robust call-chain architecture, so that at
runtime one needs only to use the program counter and the stack pointer to identify the appropriate
meta information.

There *is* a cost associated with all this. First, there's the meta information itself. This
can often double the size of code emitted. The other cost is the reliance on a robust call-chain.
Not every architecture is sufficienlty rigid in its call/return requirements to naturally support
this mechanism.

So on some systems, iOS being my immediate concern, a mechanism based on setjmp/longjmp is used.
The actual transfer of control is by longjmp, and each function that cares about this issue calls
_Unwind_SjLj_Register near entry, and _Unwind_SjLj_Unregister near exit. There *is* meta-information
emitted as well, so the size issue may remain, but we need the SjLj stuff to create a reliable
chain independent of CPU vagaries.

Every function that *may* need to participate in cleanup caused by the passage of an exception
calls _Unwind_SjLj_Register and _Unwind_SjLj_Unregister. The compiler sometimes does this even
when a function's cleanup is a no-op.

The problem is that the standard implementations of _Unwind_SjLj_Register and 
_Unwind_SjLj_Unregister in libunwind are not as efficient as they could be.

Register and unregister maintain a linked list of contextual information on a per-thread basis.
So they have to call pthread_set_specific and pthread_get_specific. They also have to know what
pthread_key_t to use.

In pseudo code,

Register (passed theNewContext) does this:
	theNewContext->prev = GetCurrentContext()
	SetCurrentContext(theNewContext)
	
and Unregister (passed theCurrentContext) does this
	SetCurrentContext(theCurrentContext->prev)

That's perfectly reasonable. Unfortunately GetCurrentContext looks like this:
	pthread_once(SetupKey)
	return pthread_get_specific(sKey)

and SetCurrentContext(theContext):
	pthread_once(SetupKey)
	return pthread_Set_specific(sKey, theContext)

So we're calling pthread_once three times per function that participates in the SjLj exception
mechanism. pthread_once is quick, but if you call it three times per function, the cost adds up.
And remember, all it's doing is ensuring that there's a pthread_key_t consistent across all threads.

This file identifies what pthread_key_y is being used by Register/Unregister and provides
replacement implementations of Register/Unregister that reference a static value presumed to
have been already setup.

Our own Register calls a function pointer that intially references the setup function, and
is then updated to reference the fast implementation.
*/

#if defined(__arm__)

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
// efficient accces to which is really the whole point of this file.

static pthread_key_t spKey;

// =================================================================================================

static pthread_key_t spRecursiveCheck
	(SjLj_t iRegister, SjLj_t iUnregister, pthread_key_t iKey, pthread_key_t iKey_Max)
	{
	// Make a note of the putative UFC* stored at iKey It may well be null, or else
	// something weird, but we don't actually deref it, so no foul.
	UFC* prior = (UFC*)pthread_getspecific(iKey);

	// Register a UFC. Not that we're doing a recursive traversal precisely to reduce the
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

	if (iKey < iKey_Max)
		{
		// We haven't tried too many keys, try the next.
		return spRecursiveCheck(iRegister, iUnregister, iKey + 1, iKey_Max);
		}

	// We've gone too deep without finding anything.
	return 0;
	}

// =================================================================================================
// MARK: -

static void spRegister_Fast(UFC* fc)
	{
	fc->prev = (UFC*)pthread_getspecific(spKey);
	pthread_setspecific(spKey, fc);	
	}

static void spRegister_Initial(UFC* fc);

static SjLj_t spRegister = spRegister_Initial;

static void spRegister_Initial(UFC* fc)
	{
	spKey = spRecursiveCheck(
		(SjLj_t)dlsym(RTLD_NEXT, "_Unwind_SjLj_Register"),
		(SjLj_t)dlsym(RTLD_NEXT, "_Unwind_SjLj_Unregister"),
		0, // Starting key to try.
		512 // Largest key to try. NB. We may create this many stack frames.
		);
	
	// If this assert trips, we couldn't find libunwind's key.
	assert(spKey);

	spRegister = spRegister_Fast;

	spRegister_Fast(fc);
	}

// =================================================================================================
// MARK: -

void _Unwind_SjLj_Register(UFC* fc)
	{
	// If this assert trips, then this file has not been placed early enough in the
	// link order. We could make it safe by conditionalizing execution on the value
	// of spRegister, calling (SjLj_t)dlsym(RTLD_NEXT, "_Unwind_SjLj_Register") if it's
	// null, but that would somewhat subvert the whole point, which is to make the
	// calling of Register/Unregister as efficient as possible.
	assert(spRegister);

	spRegister(fc);

	assert(spKey);
	}

void _Unwind_SjLj_Unregister(UFC* fc)
	{
	assert(spRegister && spKey);
	pthread_setspecific(spKey, fc->prev);
	}

#endif // defined(__arm__)
