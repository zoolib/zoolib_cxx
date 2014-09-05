#ifndef __ZStdIO_h__
#define __ZStdIO_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZStrim.h"

namespace ZooLib {
namespace ZStdIO {

extern const ZStreamR& stream_in;
extern const ZStreamW& stream_out;
extern const ZStreamW& stream_err;

extern const ZStrimR& strim_in;
extern const ZStrimW& strim_out;
extern const ZStrimW& strim_err;

} // namespace ZStdIO
} // namespace ZooLib

#endif // __ZStdIO_h__
