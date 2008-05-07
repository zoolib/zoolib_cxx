#include "ZStdIO.h"

#include "ZStream_POSIX.h"
#include "ZStrim_Stream.h"

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


//ZStreamW_Locked out();

//ZStrimW_Locked sout();


//void sSetStreamOut(ZRef<ZStreamerW> iStreamer);
//void sSetStrimOut(ZRef<ZStrimmerW> iStrimmer);
} // namespace ZStdIO

