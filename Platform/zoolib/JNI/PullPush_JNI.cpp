// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/JNI/PullPush_JNI.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Chan_XX_Memory.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/ParseException.h"

#include "zoolib/PullPush_ZZ.h" // used by sAs_JNI

namespace ZooLib {

using namespace ZooLib;
using namespace JNI;

// =================================================================================================
#pragma mark -

namespace {  // anonymous

struct LoadHandler_PullPush : public JNI::LoadHandler
	{
	void OnLoad(JNIEnv* iEnv);
	} spLoadHandler_PullPush;

jclass jclass_Class;
	jmethodID jmethodID_Class_getName;

jclass jclass_String;

jclass jclass_CharSequence;
	jmethodID jmethodID_CharSequence_length;
	jmethodID jmethodID_CharSequence_charAt;

jclass jclass_Number;
	jmethodID jmethodID_Number_doubleValue;

jclass jclass_Boolean;
	jfieldID jfieldID_Boolean_value;

jclass jclass_Byte;
	jfieldID jfieldID_Byte_value;

jclass jclass_Character;
	jfieldID jfieldID_Character_value;

jclass jclass_Short;
	jfieldID jfieldID_Short_value;

jclass jclass_Integer;
	jfieldID jfieldID_Integer_value;

jclass jclass_Long;
	jfieldID jfieldID_Long_value;

jclass jclass_Float;
	jfieldID jfieldID_Float_value;

jclass jclass_Double;
	jfieldID jfieldID_Double_value;

jclass jclass_Iterable;
	jmethodID jmethodID_Iterable_iterator;

jclass jclass_Map;
	jmethodID jmethodID_Map_entrySet;

jclass jclass_Map_Entry;
	jmethodID jmethodID_Map_Entry_getKey;
	jmethodID jmethodID_Map_Entry_getValue;

jclass jclass_Iterator;
	jmethodID jmethodID_Iterator_hasNext;
	jmethodID jmethodID_Iterator_next;

jclass jclass_JSONObject;
	jfieldID jfieldID_JSONObject_nameValuePairs;
	jfieldID jfieldID_JSONObject_NULL;

jclass jclass_JSONArray;
	jfieldID jfieldID_JSONArray_values;

jclass jclass_TreeMap;
	jmethodID jmethodID_TreeMap_Init;
	jmethodID jmethodID_TreeMap_put;

jclass jclass_ArrayList;
	jmethodID jmethodID_ArrayList_Init;
	jmethodID jmethodID_ArrayList_add;

// -------------------------------------------------------------------------------------------------

void LoadHandler_PullPush::OnLoad(JNIEnv* env)
	{
	jclass_Class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Class"));
		jmethodID_Class_getName = env->GetMethodID(jclass_Class, "getName", "()Ljava/lang/String;");

	jclass_String = (jclass) env->NewGlobalRef(env->FindClass("java/lang/String"));

	jclass_CharSequence = (jclass) env->NewGlobalRef(env->FindClass("java/lang/CharSequence"));
		jmethodID_CharSequence_length = env->GetMethodID(jclass_CharSequence, "length", "()I");
		jmethodID_CharSequence_charAt = env->GetMethodID(jclass_CharSequence, "charAt", "(I)C");

	jclass_Boolean = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Boolean"));
		jfieldID_Boolean_value = env->GetFieldID(jclass_Boolean, "value", "Z");

	jclass_Character = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Character"));
		jfieldID_Character_value = env->GetFieldID(jclass_Character, "value", "C");

	jclass_Number = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Number"));
		jmethodID_Number_doubleValue = env->GetMethodID(jclass_Number, "doubleValue", "()D");

	jclass_Byte = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Byte"));
		jfieldID_Byte_value = env->GetFieldID(jclass_Byte, "value", "B");

	jclass_Short = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Short"));
		jfieldID_Short_value = env->GetFieldID(jclass_Short, "value", "S");

	jclass_Integer = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Integer"));
		jfieldID_Integer_value = env->GetFieldID(jclass_Integer, "value", "I");

	jclass_Long = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Long"));
		jfieldID_Long_value = env->GetFieldID(jclass_Long, "value", "J");

	jclass_Float = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Float"));
		jfieldID_Float_value = env->GetFieldID(jclass_Float, "value", "F");

	jclass_Double = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Double"));
		jfieldID_Double_value = env->GetFieldID(jclass_Double, "value", "D");

	jclass_Iterable = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Iterable"));
		jmethodID_Iterable_iterator = env->GetMethodID(jclass_Iterable, "iterator", "()Ljava/util/Iterator;");

	jclass_Map = (jclass) env->NewGlobalRef(env->FindClass("java/util/Map"));
		jmethodID_Map_entrySet = env->GetMethodID(jclass_Map, "entrySet", "()Ljava/util/Set;");

	jclass_Map_Entry = (jclass) env->NewGlobalRef(env->FindClass("java/util/Map$Entry"));
		jmethodID_Map_Entry_getKey = env->GetMethodID(jclass_Map_Entry, "getKey", "()Ljava/lang/Object;");
		jmethodID_Map_Entry_getValue = env->GetMethodID(jclass_Map_Entry, "getValue", "()Ljava/lang/Object;");

	jclass_Iterator = (jclass) env->NewGlobalRef(env->FindClass("java/util/Iterator"));
		jmethodID_Iterator_hasNext = env->GetMethodID(jclass_Iterator, "hasNext", "()Z");
		jmethodID_Iterator_next = env->GetMethodID(jclass_Iterator, "next", "()Ljava/lang/Object;");

	jclass_JSONObject = (jclass) env->NewGlobalRef(env->FindClass("org/json/JSONObject"));
		jfieldID_JSONObject_nameValuePairs = env->GetFieldID(jclass_JSONObject, "nameValuePairs", "Ljava/util/LinkedHashMap;");
		jfieldID_JSONObject_NULL = env->GetStaticFieldID(jclass_JSONObject, "NULL", "Ljava/lang/Object;");

	jclass_JSONArray = (jclass) env->NewGlobalRef(env->FindClass("org/json/JSONArray"));
		jfieldID_JSONArray_values = env->GetFieldID(jclass_JSONArray, "values", "Ljava/util/List;");

	jclass_TreeMap = (jclass) env->NewGlobalRef(env->FindClass("java/util/TreeMap"));
		jmethodID_TreeMap_Init = env->GetMethodID(jclass_TreeMap, "<init>", "()V");
		jmethodID_TreeMap_put = env->GetMethodID(jclass_TreeMap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

	jclass_ArrayList = (jclass) env->NewGlobalRef(env->FindClass("java/util/ArrayList"));
		jmethodID_ArrayList_Init = env->GetMethodID(jclass_ArrayList, "<init>", "()V");
		jmethodID_ArrayList_add = env->GetMethodID(jclass_ArrayList, "add", "(Ljava/lang/Object;)Z");
	}

} // anonymous namespace

// =================================================================================================

static
PPT spFromJNumber(JNIEnv* env, jobject iObject)
	{
	if (env->IsInstanceOf(iObject, jclass_Byte))
		return byte(env->GetByteField(iObject, jfieldID_Byte_value));

	if (env->IsInstanceOf(iObject, jclass_Short))
		return int16(env->GetShortField(iObject, jfieldID_Short_value));

	if (env->IsInstanceOf(iObject, jclass_Integer))
		return int32(env->GetIntField(iObject, jfieldID_Integer_value));

	if (env->IsInstanceOf(iObject, jclass_Long))
		return int64(env->GetLongField(iObject, jfieldID_Long_value));

	if (env->IsInstanceOf(iObject, jclass_Float))
		return float(env->GetFloatField(iObject, jfieldID_Float_value));

	if (env->IsInstanceOf(iObject, jclass_Double))
		return double(env->GetDoubleField(iObject, jfieldID_Double_value));

	// Some non-standard Number subclass. Double is a reasonable choice for a fallback.
	double theValue = env->CallDoubleMethod(iObject, jmethodID_Number_doubleValue);
	if (not env->ExceptionCheck())
		return theValue;
	env->ExceptionClear();
	return null;
	}

template <class EE>
void spFromPaC_Push_PPT(const PaC<EE>& iPaC, const ChanW_PPT& iChanW)
	{
	sPush_Start_Seq(iChanW);
	for (size_t xx = 0; xx < sCount(iPaC); ++xx)
		sPush(sPtr(iPaC)[xx], iChanW);
	sPush_End(iChanW);
	}

template <class arraytype>
void spFromJArray_Push_PPT(jarray iJArray, const ChanW_PPT& iChanW)
	{ spFromPaC_Push_PPT(PaC_ConstArray<arraytype>((arraytype)iJArray), iChanW); }

static
void spFromJArray_Push_PPT(JNIEnv* env, char iType, jarray iArray, const ChanW_PPT& iChanW)
	{
	switch (iType)
		{
		case 'Z': // boolean
			{
			spFromJArray_Push_PPT<jbooleanArray>(iArray, iChanW);
			break;
			};
		case 'B': // byte
			{
			// Special case, goes across as a Bin.
			PaC_ConstArray<jbyteArray> thePaC((jbyteArray)iArray);
			sPull_Bin_Push_PPT(
				ChanRPos_XX_Memory<byte>(sCastPaC<const byte>(thePaC)),
				iChanW);
			break;
			};
		case 'C': // char
			{
			// Special case, send the uint16_t as UTF16.
			PaC_ConstArray<jcharArray> thePaC((jcharArray)iArray);
			spFromPaC_Push_PPT(sCastPaC<const UTF16>(thePaC), iChanW);
			break;
			};
		case 'S': // short
			{
			spFromJArray_Push_PPT<jshortArray>(iArray, iChanW);
			break;
			};
		case 'I': // int
			{
			spFromJArray_Push_PPT<jintArray>(iArray, iChanW);
			break;
			};
		case 'J': // long
			{
			spFromJArray_Push_PPT<jlongArray>(iArray, iChanW);
			break;
			};
		case 'F': // float
			{
			spFromJArray_Push_PPT<jfloatArray>(iArray, iChanW);
			break;
			};
		case 'D': // double
			{
			spFromJArray_Push_PPT<jdoubleArray>(iArray, iChanW);
			break;
			};
		}
	}

static
void spFromJNI_Push_PPT(JNIEnv* env, jobject iObject,
	const ChanW_PPT& iChanW)
	{
	if (not iObject)
		sPush(PPT(), iChanW);

	else if (env->IsSameObject(iObject, env->GetStaticObjectField(jclass_JSONObject, jfieldID_JSONObject_NULL)))
		{
		sPush(PPT(), iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_String))
		{
		sPush(sAsString8((jstring)iObject), iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_JSONObject))
		{
		jobject jNameValuePairs = env->GetObjectField(iObject, jfieldID_JSONObject_nameValuePairs);
		spFromJNI_Push_PPT(env, jNameValuePairs, iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_JSONArray))
		{
		jobject jValues = env->GetObjectField(iObject, jfieldID_JSONArray_values);
		spFromJNI_Push_PPT(env, jValues, iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_Number))
		{
		sPush(spFromJNumber(env, iObject), iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_Boolean))
		{
		sPush(bool(env->GetBooleanField(iObject, jfieldID_Boolean_value)), iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_Character))
		{
		sPush(UTF16(env->GetCharField(iObject, jfieldID_Character_value)), iChanW);
		}

// Not sure of the best way to handle CharSequence.
// 	else if (env->IsInstanceOf(iObject, jclass_CharSequence))
// 		{
// 		jint theLength = env->CallIntMethod(iObject, jmethodID_CharSequence_length);
// 		string8 theString;
// 		for (jint ii = 0; ii < theLength; ++ii)
// 			theString += (UTF16)env->CallCharMethod(iObject, jmethodID_CharSequence_charAt, ii);
// 		sPush(theString, iChanW);
// 		}

	else if (env->IsInstanceOf(iObject, jclass_Map))
		{
		jobject jEntrySet = env->CallObjectMethod(iObject, jmethodID_Map_entrySet);
		jobject jIter = env->CallObjectMethod(jEntrySet, jmethodID_Iterable_iterator);

		sPush_Start_Map(iChanW);
		for (;;)
			{
			if (not env->CallBooleanMethod(jIter, jmethodID_Iterator_hasNext))
				break;

			PushPopLocalFrame thePPLF;

			jobject jEntry = env->CallObjectMethod(jIter, jmethodID_Iterator_next);
			jobject jKey = env->CallObjectMethod(jEntry, jmethodID_Map_Entry_getKey);
			jobject jValue = env->CallObjectMethod(jEntry, jmethodID_Map_Entry_getValue);
			if (not env->IsInstanceOf(jKey, jclass_String))
				continue;

			sPush(sName(sAsString8((jstring)jKey)), iChanW);
			spFromJNI_Push_PPT(env, jValue, iChanW);
			}
		sPush_End(iChanW);
		}

	else if (env->IsInstanceOf(iObject, jclass_Iterable))
		{
		jobject jIter = env->CallObjectMethod(iObject, jmethodID_Iterable_iterator);

		sPush_Start_Seq(iChanW);
		for (;;)
			{
			if (not env->CallBooleanMethod(jIter, jmethodID_Iterator_hasNext))
				break;

			PushPopLocalFrame thePPLF;

			jobject jEntry = env->CallObjectMethod(jIter, jmethodID_Iterator_next);
			spFromJNI_Push_PPT(env, jEntry, iChanW);
			}
		sPush_End(iChanW);
		}

	else
		{
		jclass theClazz = env->GetObjectClass(iObject);
		string8 theName = sAsString8((jstring)env->CallObjectMethod(theClazz, jmethodID_Class_getName));
		if (theName[0] == '[')
			{
			// It's an array.
			if (theName.length() == 2)
				{
				// It's a primitive array
				spFromJArray_Push_PPT(env, theName[1], (jarray)iObject, iChanW);
				}
			else
				{
				// It's an array of objects.
				sPush_Start_Seq(iChanW);
				jobjectArray theJObjectArray = (jobjectArray)iObject;
				for (jsize xx = 0, theCount = env->GetArrayLength(theJObjectArray);
					xx < theCount; ++xx)
					{
					PushPopLocalFrame thePPLF;
					spFromJNI_Push_PPT(env, env->GetObjectArrayElement(theJObjectArray, xx), iChanW);
					}
				sPush_End(iChanW);
				}
			}
		else
			{
			sPush(UnhandledJNI(theName), iChanW);
			}
		}
	}

void sFromJNI_Push_PPT(jobject iObject,
	const ChanW_PPT& iChanW)
	{
	JNIEnv* env = EnvTV::sGet();

	CountedStringUniquifier theCountedStringUniquifier;

	spFromJNI_Push_PPT(env, iObject, iChanW);
	}

// =================================================================================================
#pragma mark -

static jbyteArray spAsJNI(JNIEnv* env, const Data_ZZ& iData)
	{
	jsize dataJSize = jsize(iData.GetSize());
	jbyteArray theByteArray = env->NewByteArray(dataJSize);
	env->SetByteArrayRegion(theByteArray, 0, dataJSize, static_cast<const jbyte*>(iData.GetPtr()));
	return theByteArray;
	}

static jobject spPrimitive_AsJNI(JNIEnv* env, const PPT& iPPT)
	{
	if (ZQ<bool> theQ = iPPT.QGet<bool>())
		return sMakeBoolean(env, *theQ);

	if (ZQ<int64> theQ = sQCoerceInt(iPPT))
		return sMakeLong(env, *theQ);

	if (ZQ<double> theQ = sQCoerceRat(iPPT))
		return sMakeDouble(env, *theQ);

	if (const string8* theString = sPGet<string8>(iPPT))
		return sMakeString(env, *theString);

	if (const string16* theString = sPGet<string16>(iPPT))
		return sMakeString(env, *theString);

	if (const Data_ZZ* theData = sPGet<Data_ZZ>(iPPT))
		return spAsJNI(env, *theData);

	if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(iPPT))
		return sMakeString(env, sReadAllUTF16(*theChanner));

	if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(iPPT))
		return spAsJNI(env, sReadAll_T<Data_ZZ>(*theChanner));

	return nullptr;
	}

static jobject spPull_PPT_AsJNI(JNIEnv* env, const PPT& iPPT, const ChanR_PPT& iChanR)
	{
	PushPopLocalFrame thePPLF;

	if (jobject theObject = spPrimitive_AsJNI(env, iPPT))
		return thePPLF.PopReturn(theObject);

	if (sIsStart_Map(iPPT))
		{
		jobject theMap = env->NewObject(jclass_TreeMap, jmethodID_TreeMap_Init);
		for (;;)
			{
			PushPopLocalFrame thePPLFInner;
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<PPT> thePPTQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else
				{
				jobject theEntry = spPull_PPT_AsJNI(env, *thePPTQ, iChanR);
				env->CallObjectMethod(theMap, jmethodID_TreeMap_put, sMakeString(env, theNameQ->AsString8()), theEntry);
				}
			}
		return thePPLF.PopReturn(theMap);
		}

	// This could be just Start, to generically handle Start derivatives
	if (sIsStart_Seq(iPPT))
		{
		jobject theArray = env->NewObject(jclass_ArrayList, jmethodID_ArrayList_Init);
		for (;;)
			{
			PushPopLocalFrame thePPLFInner;
			if (NotQ<PPT> thePPTQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else
				{
				jobject theEntry = spPull_PPT_AsJNI(env, *thePPTQ, iChanR);
				env->CallBooleanMethod(theArray, jmethodID_ArrayList_add, theEntry);
				}
			}
		return thePPLF.PopReturn(theArray);
		}

	return thePPLF.PopReturn(env->GetStaticObjectField(jclass_JSONObject, jfieldID_JSONObject_NULL));
	}

jobject sPull_PPT_AsJNI(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		JNIEnv* env = EnvTV::sGet();
		return spPull_PPT_AsJNI(env, *theQ, iChanR);
		}
	return nullptr;
	}

jobject sAsJNI(const Val_ZZ& iVal)
	{ return sPull_PPT_AsJNI(*sChannerR_PPT(iVal)); }

// =================================================================================================
#pragma mark - sChannerR_PPT

static void spFromJNI_Push_PPT_Disconnect(jobject iObject,
	const ZP<ChannerWCon_PPT>& iChannerWCon)
	{
	EnsureAttachedToCurrentThread eatct(JNI::sJavaVM());
	sFromJNI_Push_PPT(iObject, *iChannerWCon);
	sDisconnectWrite(*iChannerWCon);
	}

ZP<ChannerR_PPT> sChannerR_PPT(jobject iJObject)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();

	sStartOnNewThread(
		sBindR(sCallable(spFromJNI_Push_PPT_Disconnect),
			iJObject, sGetClear(thePair.first)));

	return thePair.second;
	}

} // namespace ZooLib
