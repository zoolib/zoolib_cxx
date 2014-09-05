#include "zoolib/ZStdIO.h"

#include "zoolib/ZStream_POSIX.h"
#include "zoolib/ZStrim_Stream.h"

namespace ZooLib {

static ZStreamR_FILE spStreamR_In(stdin);
static ZStreamW_FILE spStreamW_Out(stdout);
static ZStreamW_FILE spStreamW_Err(stderr);

static ZStrimR_StreamUTF8 spStrimR_In(spStreamR_In);
static ZStrimW_StreamUTF8 spStrimW_Out(spStreamW_Out);
static ZStrimW_StreamUTF8 spStrimW_Err(spStreamW_Err);

namespace ZStdIO {

const ZStreamR& stream_in = spStreamR_In;
const ZStreamW& stream_out = spStreamW_Out;
const ZStreamW& stream_err = spStreamW_Err;

const ZStrimR& strim_in = spStrimR_In;
const ZStrimW& strim_out = spStrimW_Out;
const ZStrimW& strim_err = spStrimW_Err;

} // namespace ZStdIO

} // namespace ZooLib
