cmake_minimum_required(VERSION 3.4.1)

set(ZOOLIB_CXX ../..)

set(ZDIR ${ZOOLIB_CXX}/Portable)

set (CppFiles	
	${ZDIR}/zoolib/Callable_Bool.cpp
	${ZDIR}/zoolib/Cancellable.cpp
	${ZDIR}/zoolib/ChanRU_UTF_ML.cpp
	${ZDIR}/zoolib/ChanRU_UTF_Std.cpp
	${ZDIR}/zoolib/ChanR_Bin_HexStrim.cpp
	${ZDIR}/zoolib/ChanR_Bin_More.cpp
	${ZDIR}/zoolib/ChanW_Bin_HexStrim.cpp
	${ZDIR}/zoolib/ChanW_Bin_More.cpp
	${ZDIR}/zoolib/ChanW_UTF_InsertSeparator.cpp
	${ZDIR}/zoolib/ChanW_UTF_ML.cpp
	${ZDIR}/zoolib/Chan_Bin_ASCIIStrim.cpp
	${ZDIR}/zoolib/Chan_Bin_Base64.cpp
	${ZDIR}/zoolib/Chan_Bin_FILE.cpp
	${ZDIR}/zoolib/Chan_Bin_string.cpp
	${ZDIR}/zoolib/Chan_UTF_CRLF.cpp
	${ZDIR}/zoolib/Chan_UTF_Chan_Bin.cpp
	${ZDIR}/zoolib/Chan_UTF_Escaped.cpp
	${ZDIR}/zoolib/Chan_UTF_TextCoder.cpp
	${ZDIR}/zoolib/Chan_UTF_string.cpp
	${ZDIR}/zoolib/Chan_XX_RateCapped.cpp
	${ZDIR}/zoolib/Coerce_Any.cpp
	${ZDIR}/zoolib/Compare_Integer.cpp
	${ZDIR}/zoolib/Compare_Rational.cpp
	${ZDIR}/zoolib/Compare_string.cpp
	${ZDIR}/zoolib/Data_Any.cpp
	${ZDIR}/zoolib/File.cpp
	${ZDIR}/zoolib/Log.cpp
	${ZDIR}/zoolib/ML.cpp
	${ZDIR}/zoolib/Matrix.cpp
	${ZDIR}/zoolib/Name.cpp
	${ZDIR}/zoolib/NamePriv.cpp
	${ZDIR}/zoolib/NameUniquifier.cpp
	${ZDIR}/zoolib/Net.cpp
	${ZDIR}/zoolib/Net_Internet.cpp
	${ZDIR}/zoolib/PullPush.cpp
	${ZDIR}/zoolib/PullPush_Any.cpp
	${ZDIR}/zoolib/PullPush_JSON.cpp
	${ZDIR}/zoolib/PullPush_JSONB.cpp
	${ZDIR}/zoolib/PullPush_XMLPList.cpp
	${ZDIR}/zoolib/Pull_Basic.cpp
	${ZDIR}/zoolib/Pull_Bencode.cpp
	${ZDIR}/zoolib/Pull_ML.cpp
	${ZDIR}/zoolib/Pull_SeparatedValues.cpp
	${ZDIR}/zoolib/Pull_XMLAttr.cpp
	${ZDIR}/zoolib/StartOnNewThread.cpp
	${ZDIR}/zoolib/StartScheduler.cpp
	${ZDIR}/zoolib/Startable_SetName.cpp
	${ZDIR}/zoolib/Starter_EachOnNewThread.cpp
	${ZDIR}/zoolib/Starter_EventLoopBase.cpp
	${ZDIR}/zoolib/Starter_ThreadLoop.cpp
	${ZDIR}/zoolib/StdIO.cpp
	${ZDIR}/zoolib/TextCoder.cpp
	${ZDIR}/zoolib/TextCoderAliases.cpp
	${ZDIR}/zoolib/TextCoder_Std.cpp
	${ZDIR}/zoolib/TextCoder_Unicode.cpp
	${ZDIR}/zoolib/Trail.cpp
	${ZDIR}/zoolib/Util_Any_JSON.cpp
	${ZDIR}/zoolib/Util_Any_JSONB.cpp
	${ZDIR}/zoolib/Util_Chan_Bin_Operators.cpp
	${ZDIR}/zoolib/Util_Chan_JSON.cpp
	${ZDIR}/zoolib/Util_Chan_UTF.cpp
	${ZDIR}/zoolib/Util_Chan_UTF_Operators.cpp
	${ZDIR}/zoolib/Util_Debug.cpp
	${ZDIR}/zoolib/Util_File.cpp
	${ZDIR}/zoolib/Util_Time.cpp
	${ZDIR}/zoolib/Util_string.cpp
	${ZDIR}/zoolib/Val.cpp
	${ZDIR}/zoolib/Val_Any.cpp
	${ZDIR}/zoolib/Visitor.cpp
	${ZDIR}/zoolib/Visitor_ToStrim.cpp
)

add_library(
	# Specifies the name of the library.
	ZooLib_Portable

	# STATIC or SHARED
	STATIC

	${CppFiles}
	)

# Specifies a path to native header files.
include_directories(${ZDIR} ${ZOOLIB_CXX}/Core ${ZOOLIB_CXX}/Platform ${ZOOLIB_CXX}/default_config)
