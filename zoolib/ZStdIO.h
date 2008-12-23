#ifndef __ZStdIO__
#define __ZStdIO__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZStrim.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZStdIO {

extern const ZStreamR& stream_in;
extern const ZStreamW& stream_out;
extern const ZStreamW& stream_err;

extern const ZStrimR& strim_in;
extern const ZStrimW& strim_out;
extern const ZStrimW& strim_err;

} // namespace ZStdIO

NAMESPACE_ZOOLIB_END

#endif // __ZStdIO__
