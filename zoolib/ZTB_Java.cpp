/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#pragma export on
#include "org/zoolib/tuplebase/ZTB.h"
#include "org/zoolib/tuplebase/ZTB_RAM.h"
#include "org/zoolib/tuplebase/ZTB_Client_Net.h"
#include "org/zoolib/tuplebase/ZTBIter.h"
#include "org/zoolib/tuplebase/ZTBQuery.h"
#include "org/zoolib/tuplebase/ZTBSpec.h"
#include "org/zoolib/ZTxn.h"
#pragma export reset

#include "foundation/ZTuple_Java.h"
#include "foundation/ZUtil_Tuple.h"
#include "net/ZNet_Internet.h"
#include "stream/ZStream_JNI.h"
#include "stream/ZStream_Buffered.h"
#include "stream/ZStrim.h"
#include "stream/ZStrim_Stream.h"
#include "thread/ZThreadSimple.h"
#include "tuplebase/ZTB.h"
#include "tuplebase/ZTBRep_TS.h"
#include "tuplebase/ZTBRep_Client_Net.h"
#include "tuplebase/ZTBSpec.h"
#include "tuplebase/ZTS_RAM.h"
#include "tuplebase/ZTupleIndex.h"

#if 0
/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sAsTuple
 * Signature: (Ljava/util/Map;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sAsTuple
	(JNIEnv* iEnv, jclass, jobject iMap)
	{
	return reinterpret_cast<jint>(new ZTuple(ZTuple_Java::sMapToTuple(iEnv, iMap)));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sAsMap
 * Signature: (I)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sAsMap
	(JNIEnv* iEnv, jclass, jint iNativeTuple)
	{
	return ZTuple_Java::sTupleToMap(iEnv, *reinterpret_cast<ZTuple*>(iNativeTuple));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sTupleAsString
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sTupleAsString
	(JNIEnv* iEnv, jclass, jint iNativeTuple)
	{
	string theString = ZUtil_Tuple::sAsString(*reinterpret_cast<ZTuple*>(iNativeTuple), true);
	return iEnv->NewStringUTF(theString.c_str());
	}
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static string sGetString(JNIEnv* iEnv, jstring iString)
	{
	jboolean isCopy;
	return iEnv->GetStringUTFChars(iString, &isCopy);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTB

/*
 * Class:     org_zoolib_tuplebase_ZTB
 * Method:    sDispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTB_sDispose
	(JNIEnv* iEnv, jclass, jint iNativeTB)
	{
	delete reinterpret_cast<ZTB*>(iNativeTB);
	}

/*
 * Class:     org_zoolib_tuplebase_ZTB
 * Method:    sAllocateID
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_org_zoolib_tuplebase_ZTB_sAllocateID
	(JNIEnv* iEnv, jclass, jint iNativeTB)
	{
	if (iNativeTB)
		return reinterpret_cast<ZTB*>(iNativeTB)->AllocateID();
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTB
 * Method:    sAdd
 * Signature: (IILjava/util/Map;)J
 */
JNIEXPORT jlong JNICALL Java_org_zoolib_tuplebase_ZTB_sAdd
	(JNIEnv* iEnv, jclass, jint iNativeTB, jint iNativeTxn, jobject iMap)
	{
	if (iNativeTB && iNativeTxn)
		{
		ZTuple theTuple = ZTuple_Java::sMapToTuple(iEnv, iMap);
		return reinterpret_cast<ZTB*>(iNativeTB)->Add(*reinterpret_cast<ZTxn*>(iNativeTxn), theTuple);
		}
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTB
 * Method:    sSet
 * Signature: (IIJLjava/util/Map;)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTB_sSet
	(JNIEnv* iEnv, jclass, jint iNativeTB, jint iNativeTxn, jlong iID, jobject iMap)
	{
	if (iNativeTB && iNativeTxn && iID)
		{
		ZTuple theTuple = ZTuple_Java::sMapToTuple(iEnv, iMap);
		return reinterpret_cast<ZTB*>(iNativeTB)->Set(*reinterpret_cast<ZTxn*>(iNativeTxn), iID, theTuple);
		}
	}

/*
 * Class:     org_zoolib_tuplebase_ZTB
 * Method:    sGet
 * Signature: (IIJ)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_org_zoolib_tuplebase_ZTB_sGet
	(JNIEnv* iEnv, jclass, jint iNativeTB, jint iNativeTxn, jlong iID)
	{
	if (iNativeTB && iNativeTxn && iID)
		{
		ZTuple theTuple = reinterpret_cast<ZTB*>(iNativeTB)->Get(*reinterpret_cast<ZTxn*>(iNativeTxn), iID);
		return ZTuple_Java::sTupleToMap(iEnv, theTuple);
		}
	return NULL;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTB_RAM

/*
 * Class:     org_zoolib_tuplebase_ZTB_1RAM
 * Method:    sConstruct
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTB_1RAM_sConstruct
	(JNIEnv* iEnv, jclass)
	{
	vector<ZRef<ZTupleIndexFactory> > theFactories;
	ZTB* theTB = new ZTB(new ZTBRep_TS(theFactories, new ZTS_RAM(theFactories)));
	return reinterpret_cast<jint>(theTB);
	}

/*
 * Class:     org_zoolib_tuplebase_ZTB_1RAM
 * Method:    sConstruct_Stream
 * Signature: (Ljava/io/InputStream;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTB_1RAM_sConstruct_1Stream
	(JNIEnv* iEnv, jclass, jobject iStream)
	{
	vector<ZRef<ZTupleIndexFactory> > theFactories;
	try
		{
		ZTS_RAM* theTS = new ZTS_RAM(theFactories, ZStrimU_Unreader(ZStrimR_StreamUTF8(ZStreamR_Buffered(4096, ZStreamR_JNI(iEnv, iStream)))));
		ZTB* theTB = new ZTB(new ZTBRep_TS(theFactories, theTS));
		return reinterpret_cast<jint>(theTB);
		}
	catch (...)
		{
		return 0;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTB_Client_Net

static void sRunConnection(ZRef<ZTBRep_Client_Net> iTBRep)
	{
	iTBRep->Run();
	}

/*
 * Class:     org_zoolib_tuplebase_ZTB_1Client_1Net
 * Method:    sConstruct
 * Signature: (IS)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTB_1Client_1Net_sConstruct
	(JNIEnv* iEnv, jclass, jint iHost, jshort iPort)
	{
	ZRef<ZTBRep_Client_Net> theTBRep = new ZTBRep_Client_Net(new ZNetAddress_Internet(iHost, iPort));
	(new ZThreadSimple<ZRef<ZTBRep_Client_Net> >(sRunConnection, theTBRep))->Start();
	ZTB* theTB = new ZTB(theTBRep);
	return reinterpret_cast<jint>(theTB);	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTxn
/*
 * Class:     org_zoolib_ZTxn
 * Method:    sConstruct
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_ZTxn_sConstruct
	(JNIEnv* iEnv, jclass)
	{
	return reinterpret_cast<jint>(new ZTxn);
	}

/*
 * Class:     org_zoolib_ZTxn
 * Method:    sDispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_ZTxn_sDispose
	(JNIEnv* iEnv, jclass, jint iNativeTxn)
	{
	delete reinterpret_cast<ZTxn*>(iNativeTxn);
	}

/*
 * Class:     org_zoolib_ZTxn
 * Method:    sAbort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_ZTxn_sAbort
	(JNIEnv* iEnv, jclass, jint iNativeTxn)
	{
	if (iNativeTxn)
		reinterpret_cast<ZTxn*>(iNativeTxn)->Abort();
	}

/*
 * Class:     org_zoolib_ZTxn
 * Method:    sCommit
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_zoolib_ZTxn_sCommit
	(JNIEnv* iEnv, jclass, jint iNativeTxn)
	{
	if (iNativeTxn)
		return reinterpret_cast<ZTxn*>(iNativeTxn)->Commit();
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBIter

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sConstruct_Spec
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBIter_sConstruct_1Spec
	(JNIEnv* iEnv, jclass, jint iNative_Txn, jint iNative_TB, jint iNative_Spec)
	{
	ZTxn* theTxn = reinterpret_cast<ZTxn*>(iNative_Txn);
	ZTB* theTB = reinterpret_cast<ZTB*>(iNative_TB);
	ZTBSpec* theSpec = reinterpret_cast<ZTBSpec*>(iNative_Spec);
	return reinterpret_cast<jint>(new ZTBIter(*theTxn, *theTB, *theSpec));
	}
/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sConstruct_Query
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBIter_sConstruct_1Query
	(JNIEnv* iEnv, jclass, jint iNative_Txn, jint iNative_TB, jint iNative_Query)
	{
	ZTxn* theTxn = reinterpret_cast<ZTxn*>(iNative_Txn);
	ZTB* theTB = reinterpret_cast<ZTB*>(iNative_TB);
	ZTBQuery* theQuery = reinterpret_cast<ZTBQuery*>(iNative_Query);
	return reinterpret_cast<jint>(new ZTBIter(*theTxn, *theTB, *theQuery));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sDispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTBIter_sDispose
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	delete reinterpret_cast<ZTBIter*>(iNative_Iter);
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sClone
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBIter_sClone
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	return reinterpret_cast<jint>(new ZTBIter(*reinterpret_cast<ZTBIter*>(iNative_Iter)));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sHasValue
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_org_zoolib_tuplebase_ZTBIter_sHasValue
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	if (*reinterpret_cast<ZTBIter*>(iNative_Iter))
		return true;
	return false;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sAdvance
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTBIter_sAdvance
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	reinterpret_cast<ZTBIter*>(iNative_Iter)->Advance();
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sGet
 * Signature: (I)Ljava/util/Map;
 */
JNIEXPORT jobject JNICALL Java_org_zoolib_tuplebase_ZTBIter_sGet
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	return ZTuple_Java::sTupleToMap(iEnv, reinterpret_cast<ZTBIter*>(iNative_Iter)->Get());
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sGetID
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_org_zoolib_tuplebase_ZTBIter_sGetID
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	return reinterpret_cast<ZTBIter*>(iNative_Iter)->GetID();
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sSet
 * Signature: (ILjava/util/Map;)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTBIter_sSet
	(JNIEnv* iEnv, jclass, jint iNative_Iter, jobject iMap)
	{
	ZTuple theTuple = ZTuple_Java::sMapToTuple(iEnv, iMap);
	reinterpret_cast<ZTBIter*>(iNative_Iter)->Set(theTuple);
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBIter
 * Method:    sErase
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTBIter_sErase
	(JNIEnv* iEnv, jclass, jint iNative_Iter)
	{
	reinterpret_cast<ZTBIter*>(iNative_Iter)->Erase();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQuery

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sDispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sDispose
	(JNIEnv* iEnv, jclass, jint iNativeQuery)
	{
	delete reinterpret_cast<ZTBQuery*>(iNativeQuery);
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sConstruct_ID
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sConstruct_1ID
	(JNIEnv* iEnv, jclass, jlong iID)
	{
	if (iID)
		return reinterpret_cast<jint>(new ZTBQuery(iID));
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sConstruct_IDs
 * Signature: ([J)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sConstruct_1IDs
	(JNIEnv* iEnv, jclass, jlongArray iIDs)
	{
	if (size_t count = iEnv->GetArrayLength(iIDs))
		{
		jboolean isCopy;
		uint64* elems = static_cast<uint64*>(iEnv->GetPrimitiveArrayCritical(iIDs, &isCopy));
		ZTBQuery* theQuery = new ZTBQuery(elems, count);
		iEnv->ReleasePrimitiveArrayCritical(iIDs, elems, JNI_ABORT);
		return reinterpret_cast<jint>(theQuery);
		}
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sConstruct_Search
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sConstruct_1Search
	(JNIEnv* iEnv, jclass, jint iNativeSpec)
	{
	if (ZTBSpec* theSpec = reinterpret_cast<ZTBSpec*>(iNativeSpec))
		return reinterpret_cast<jint>(new ZTBQuery(*theSpec));
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sConstruct_Source
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sConstruct_1Source
	(JNIEnv* iEnv, jclass, jint iNativeSourceQuery, jstring iSourcePropName)
	{
	if (ZTBQuery* theSourceQuery = reinterpret_cast<ZTBQuery*>(iNativeSourceQuery))
		{
		string theSourcePropName = sGetString(iEnv, iSourcePropName);
		return reinterpret_cast<jint>(new ZTBQuery(*theSourceQuery, theSourcePropName));
		}
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sConstruct_Property_Query
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sConstruct_1Property_1Query
	(JNIEnv* iEnv, jclass, jstring iPropName, jint iNativeSourceQuery)
	{
	if (ZTBQuery* theSourceQuery = reinterpret_cast<ZTBQuery*>(iNativeSourceQuery))
		{
		string thePropName = sGetString(iEnv, iPropName);
		return reinterpret_cast<jint>(new ZTBQuery(thePropName, *theSourceQuery));
		}
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sConstruct_Property_Spec
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sConstruct_1Property_1Spec
	(JNIEnv* iEnv, jclass, jstring iPropName, jint iNativeSourceSpec)
	{
	if (ZTBSpec* theSourceSpec = reinterpret_cast<ZTBSpec*>(iNativeSourceSpec))
		{
		string thePropName = sGetString(iEnv, iPropName);
		return reinterpret_cast<jint>(new ZTBQuery(thePropName, *theSourceSpec));
		}
	return 0;
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sAnd_Query
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sAnd_1Query
	(JNIEnv* iEnv, jclass, jint iNativeQuery1, jint iNativeQuery2)
	{
	ZTBQuery* query1 = reinterpret_cast<ZTBQuery*>(iNativeQuery1);
	ZTBQuery* query2 = reinterpret_cast<ZTBQuery*>(iNativeQuery2);
	return reinterpret_cast<jint>(new ZTBQuery(*query1 & *query2));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sAnd_Spec
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sAnd_1Spec
	(JNIEnv* iEnv, jclass, jint iNativeQuery, jint iNativeSpec)
	{
	ZTBQuery* query = reinterpret_cast<ZTBQuery*>(iNativeQuery);
	ZTBSpec* spec = reinterpret_cast<ZTBSpec*>(iNativeSpec);
	return reinterpret_cast<jint>(new ZTBQuery(*query & *spec));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sOr_Query
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sOr_1Query
	(JNIEnv* iEnv, jclass, jint iNativeQuery1, jint iNativeQuery2)
	{
	ZTBQuery* query1 = reinterpret_cast<ZTBQuery*>(iNativeQuery1);
	ZTBQuery* query2 = reinterpret_cast<ZTBQuery*>(iNativeQuery2);
	return reinterpret_cast<jint>(new ZTBQuery(*query1 | *query2));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sOr_Spec
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sOr_1Spec
	(JNIEnv* iEnv, jclass, jint iNativeQuery, jint iNativeSpec)
	{
	ZTBQuery* query = reinterpret_cast<ZTBQuery*>(iNativeQuery);
	ZTBSpec* spec = reinterpret_cast<ZTBSpec*>(iNativeSpec);
	return reinterpret_cast<jint>(new ZTBQuery(*query | *spec));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBQuery
 * Method:    sImp_AsString
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_zoolib_tuplebase_ZTBQuery_sAsString
	(JNIEnv* iEnv, jclass, jint iNativeQuery)
	{
	ZTBQuery* theQuery = reinterpret_cast<ZTBQuery*>(iNativeQuery);
	string theString = ZUtil_Tuple::sAsString(theQuery->AsTuple(), true);
	return iEnv->NewStringUTF(theString.c_str());
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sMapAsString
 * Signature: (Ljava/util/Map;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sMapAsString
	(JNIEnv* iEnv, jclass, jobject iMap)
	{
	ZTuple theTuple = ZTuple_Java::sMapToTuple(iEnv, iMap);
	string theString = ZUtil_Tuple::sAsString(theTuple, true);
	return iEnv->NewStringUTF(theString.c_str());	
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Dispose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sDispose
	(JNIEnv* iEnv, jclass, jint iNativeSpec)
	{
	delete reinterpret_cast<ZTBSpec*>(iNativeSpec);
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sConstruct_Any
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Any
	(JNIEnv*, jclass)
	{
	return reinterpret_cast<jint>(new ZTBSpec(true));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Has
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Has
	(JNIEnv* iEnv, jclass, jstring iPropName)
	{
	string thePropName = sGetString(iEnv, iPropName);
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_Has, ZTupleValue()));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Equals
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Equals
	(JNIEnv* iEnv, jclass, jstring iPropName, jobject iValue)
	{
	string thePropName = sGetString(iEnv, iPropName);
	ZTupleValue theValue = ZTuple_Java::sObjectToTupleValue(iEnv, iValue);
	
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_Equal, theValue));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_NotEqual
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1NotEqual
	(JNIEnv* iEnv, jclass, jstring iPropName, jobject iValue)
	{
	string thePropName = sGetString(iEnv, iPropName);
	ZTupleValue theValue = ZTuple_Java::sObjectToTupleValue(iEnv, iValue);

	ZTBSpec lessSpec(thePropName, ZTBSpec::eRel_Less, theValue);
	ZTBSpec greaterSpec(thePropName, ZTBSpec::eRel_Greater, theValue);

	return reinterpret_cast<jint>(new ZTBSpec(lessSpec | greaterSpec));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Less
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Less
	(JNIEnv* iEnv, jclass, jstring iPropName, jobject iValue)
	{
	string thePropName = sGetString(iEnv, iPropName);
	ZTupleValue theValue = ZTuple_Java::sObjectToTupleValue(iEnv, iValue);
	
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_Less, theValue));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_LessEqual
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1LessEqual
	(JNIEnv* iEnv, jclass, jstring iPropName, jobject iValue)
	{
	string thePropName = sGetString(iEnv, iPropName);
	ZTupleValue theValue = ZTuple_Java::sObjectToTupleValue(iEnv, iValue);
	
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_LessEqual, theValue));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Greater
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Greater
	(JNIEnv* iEnv, jclass, jstring iPropName, jobject iValue)
	{
	string thePropName = sGetString(iEnv, iPropName);
	ZTupleValue theValue = ZTuple_Java::sObjectToTupleValue(iEnv, iValue);
	
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_Greater, theValue));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_GreaterEqual
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1GreaterEqual
	(JNIEnv* iEnv, jclass, jstring iPropName, jobject iValue)
	{
	string thePropName = sGetString(iEnv, iPropName);
	ZTupleValue theValue = ZTuple_Java::sObjectToTupleValue(iEnv, iValue);
	
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_GreaterEqual, theValue));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sConstruct_Contains
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Contains
	(JNIEnv* iEnv, jclass, jstring iPropName, jstring iString, jint iLevel)
	{
	string thePropName = sGetString(iEnv, iPropName);
	string theString = sGetString(iEnv, iString);
	return reinterpret_cast<jint>(new ZTBSpec(ZTBSpec::sStringContains(thePropName, theString, iLevel)));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Regex
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sConstruct_1Regex
	(JNIEnv* iEnv, jclass, jstring iPropName, jstring iRegex)
	{
	string thePropName = sGetString(iEnv, iPropName);
	string theRegex = sGetString(iEnv, iRegex);
	return reinterpret_cast<jint>(new ZTBSpec(thePropName, ZTBSpec::eRel_Regex, theRegex));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_And
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sAnd
	(JNIEnv* iEnv, jclass, jint iNativeSpec1, jint iNativeSpec2)
	{
	ZTBSpec* spec1 = reinterpret_cast<ZTBSpec*>(iNativeSpec1);
	ZTBSpec* spec2 = reinterpret_cast<ZTBSpec*>(iNativeSpec2);
	return reinterpret_cast<jint>(new ZTBSpec(*spec1 & *spec2));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Or
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sOr
	(JNIEnv* iEnv, jclass, jint iNativeSpec1, jint iNativeSpec2)
	{
	ZTBSpec* spec1 = reinterpret_cast<ZTBSpec*>(iNativeSpec1);
	ZTBSpec* spec2 = reinterpret_cast<ZTBSpec*>(iNativeSpec2);
	return reinterpret_cast<jint>(new ZTBSpec(*spec1 | *spec2));
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_AsString
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sAsString
	(JNIEnv* iEnv, jclass, jint iNativeSpec)
	{
	ZTBSpec* theSpec = reinterpret_cast<ZTBSpec*>(iNativeSpec);
	string theString = ZUtil_Tuple::sAsString(theSpec->AsTuple(), true);
	return iEnv->NewStringUTF(theString.c_str());
	}

/*
 * Class:     org_zoolib_tuplebase_ZTBSpec
 * Method:    sImp_Matches
 * Signature: (ILjava/util/Map;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_zoolib_tuplebase_ZTBSpec_sMatches
	(JNIEnv* iEnv, jclass, jint iNativeSpec, jobject iMap)
	{
	ZTBSpec* theSpec = reinterpret_cast<ZTBSpec*>(iNativeSpec);
	return theSpec->Matches(ZTuple_Java::sMapToTuple(iEnv, iMap));
	}
