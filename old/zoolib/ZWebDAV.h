/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZWebDAV_h__
#define __ZWebDAV_h__ 1
#include "zconfig.h"

#include "zoolib/ZNode.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZTuple.h"

namespace ZooLib {

namespace ZWebDAV {

bool sHandle_DELETE(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_GET(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_LOCK(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_MKCOL(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_MOVE(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_OPTIONS(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_PROPFIND(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_PROPPATCH(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_PUT(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

bool sHandle_UNLOCK(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

//	bool sHandle_COPY(
//		ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
//		const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

//	bool sHandle_HEAD(
//		ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
//		const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

//	bool sHandle_POST(
//	ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
//	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

//	bool sHandle_TRACE(
//		ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
//		const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam);

} // namespace ZWebDAV

} // namespace ZooLib

#endif // __ZWebDAV_h__
