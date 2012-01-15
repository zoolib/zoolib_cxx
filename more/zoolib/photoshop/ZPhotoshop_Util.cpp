/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/photoshop/ZPhotoshop_Util.h"
#include "zoolib/photoshop/ZPhotoshop_Val.h"

#include "PITerminology.h"

namespace ZooLib {
namespace ZPhotoshop {

using namespace ZPhotoshop;

// =================================================================================================
// MARK: - ZUtil_Photoshop

void sGetHostVersion(int& oMajor, int& oMinor, int& oFix)
	{
	static bool sFetched;
	static int sMajor, sMinor, sFix;
	if (!sFetched)
		{
		Spec theSpec;
		theSpec += Spec::sEnum(classApplication, Enumerated(typeOrdinal, enumTarget));
		theSpec += Spec::sProperty(classProperty, keyHostVersion);

		if (const ZQ<Map> theQ = theSpec.QGet())
			{
			if (ZQ<Map> versionMapQ = theQ->Get(keyHostVersion).QGetMap())
				{
				sMajor = versionMapQ->Get(keyVersionMajor).GetInt32();
				sMinor = versionMapQ->Get(keyVersionMinor).GetInt32();
				sFix = versionMapQ->Get(keyVersionFix).GetInt32();
				}
			}
		sFetched = true;
		}
	oMajor = sMajor;
	oMinor = sMinor;
	oFix = sFix;
	}

int sGetHostVersion_Major()
	{
	int major, minor, fix;
	sGetHostVersion(major, minor, fix);
	return major;
	}

} // namespace ZUtil_Photoshop
} // namespace ZooLib
