// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compat_NonCopyable_h__
#define __ZooLib_Compat_NonCopyable_h__ 1
#include "zconfig.h"

//  Boost noncopyable.hpp header file  --------------------------------------//

//  (C) Copyright Beman Dawes 1999-2003. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/utility for documentation.

namespace ZooLib {

//  Private copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.

//  Contributed by Dave Abrahams

namespace noncopyable_  // protection from unintended ADL
	{
	class noncopyable
		{
		protected:
			// The following ensure that subclasses must provide
			// their own accessible constructor and destructor.
			inline noncopyable() {}
			inline ~noncopyable() {}
		private:
			// The following are inaccessible and not implemented.
			noncopyable(const noncopyable&);
			const noncopyable& operator=(const noncopyable&);
		};
	}

typedef noncopyable_::noncopyable NonCopyable;

} // namespace ZooLib

#endif // __ZooLib_Compat_NonCopyable_h__
