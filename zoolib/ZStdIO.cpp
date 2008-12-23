#include "zoolib/ZStdIO.h"

#include "zoolib/ZStream_POSIX.h"
#include "zoolib/ZStrim_Stream.h"

NAMESPACE_ZOOLIB_BEGIN

static ZStreamR_FILE sStreamR_In(stdin);
static ZStreamW_FILE sStreamW_Out(stdout);
static ZStreamW_FILE sStreamW_Err(stderr);

static ZStrimR_StreamUTF8 sStrimR_In(sStreamR_In);
static ZStrimW_StreamUTF8 sStrimW_Out(sStreamW_Out);
static ZStrimW_StreamUTF8 sStrimW_Err(sStreamW_Err);

namespace ZStdIO {

const ZStreamR& stream_in = sStreamR_In;
const ZStreamW& stream_out = sStreamW_Out;
const ZStreamW& stream_err = sStreamW_Err;

const ZStrimR& strim_in = sStrimR_In;
const ZStrimW& strim_out = sStrimW_Out;
const ZStrimW& strim_err = sStrimW_Err;

} // namespace ZStdIO

NAMESPACE_ZOOLIB_END
