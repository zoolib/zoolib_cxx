///* -------------------------------------------------------------------------------------------------
//Copyright (c) 2009 Andrew Green
//http://www.zoolib.org
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software
//and associated documentation files (the "Software"), to deal in the Software without restriction,
//including without limitation the rights to use, copy, modify, merge, publish, distribute,
//sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
//is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be
//included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
//BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
//OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
//OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//------------------------------------------------------------------------------------------------- */
//
//#include "zoolib/Chan_UTF_string.h" // For ChanW_UTF_string8
//#include "zoolib/Chan_Bin_Data.h"
//#include "zoolib/Visitor_Do_T.h"
//#include "zoolib/Yad_Any.h"
//#include "zoolib/Yad_Val_T.h"
//
//namespace ZooLib {
//
//namespace { // anonymous
//
//class AnyHolder
//	{
//public:
//	AnyHolder(const Any& iAny)
//	:	fAny(iAny)
//		{}
//
//	virtual ~AnyHolder()
//		{}
//
//// Our protocol
//	const Any& Get()
//		{ return fAny; }
//
//protected:
//	Any fAny;
//	};
//
//class YadAtomR_Any;
//
//#if defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//class SafeStackLink_YadAtomR_Any
//:	public SafePtrStackLink<YadAtomR_Any,SafeStackLink_YadAtomR_Any>
//	{};
//
//SafePtrStack_WithDestroyer<YadAtomR_Any,SafeStackLink_YadAtomR_Any> spSafeStack_YadAtomR_Any;
//#endif // defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//
//class YadAtomR_Any
//:	public AnyHolder
//,	public virtual YadAtomR
//#if defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//,	public SafeStackLink_YadAtomR_Any
//#endif // defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//	{
//public:
//	YadAtomR_Any(const Any& iAny)
//	:	AnyHolder(iAny)
//		{}
//
//	virtual ~YadAtomR_Any()
//		{}
//
//// From ZCounted
//#if defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//	virtual void Finalize()
//		{
//		bool finalized = this->FinishFinalize();
//		ZAssert(finalized);
//		ZAssert(not this->IsReferenced());
//		fAny.Clear();
//
//		spSafeStack_YadAtomR_Any.Push(this);
//		}
//#endif // defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//
//// From YadAtomR
//	virtual Any AsAny()
//		{ return AnyHolder::Get(); }
//
//// Our protocol
//	void SetAny(const Any& iAny)
//		{ fAny = iAny; }
//	};
//
//class YadStrimmerR_string
//:	public AnyHolder
//,	public Channer_T<ChanRU_UTF_string8>
//	{
//public:
//	YadStrimmerR_string(const Any& iAny)
//	:	AnyHolder(iAny)
//	,	Channer_T<ChanRU_UTF_string8>(fAny.Get<string8>())
//		{}
//
//	YadStrimmerR_string(const std::string& iString)
//	:	AnyHolder(Any(iString))
//	,	Channer_T<ChanRU_UTF_string8>(fAny.Get<string8>())
//		{}
//	};
//
//class YadStreamerRPos_Any
//:	public AnyHolder
//,	public Channer_T<ChanRPos_Bin_Data<Data_Any>>
//	{
//public:
//	YadStreamerRPos_Any(const Data_Any& iData)
//	:	AnyHolder(Any(iData))
//	,	Channer_T<ChanRPos_Bin_Data<Data_Any>>(iData)
//		{}
//	};
//
//class YadSeqAtRPos_Any
//:	public AnyHolder
//,	public Channer_T<Chan_YadSeqAtRPos_T<Seq_Any>>
//	{
//public:
//	YadSeqAtRPos_Any(const Seq_Any& iSeq)
//	:	AnyHolder(Any(iSeq))
//	,	Channer_T<Chan_YadSeqAtRPos_T<Seq_Any>>(iSeq)
//		{}
//	};
//
//class YadMapR_Any
//:	public AnyHolder
//,	public Channer_T<Chan_YadMapAtRPos_T<Map_Any>>
//	{
//public:
//	YadMapR_Any(const Map_Any& iMap)
//	:	AnyHolder(Any(iMap))
//	,	Channer_T<Chan_YadMapAtRPos_T<Map_Any>>(iMap)
//		{}
//	};
//
//} // anonymous namespace
//
//// =================================================================================================
//#pragma mark - sYadR
//
//ZRef<YadAtomR> sYadAtomR_Any(const Any& iAny)
//	{
//#if defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//	if (YadAtomR_Any* result = spSafeStack_YadAtomR_Any.PopIfNotEmpty<YadAtomR_Any>())
//		{
//		result->SetAny(iAny);
//		return result;
//		}
//#endif // defined(ZMACRO_UseSafeStack) && ZMACRO_UseSafeStack
//	return new YadAtomR_Any(iAny);
//	}
//
//ZRef<YadStrimmerR> sYadR(const string8& iVal)
//	{ return new YadStrimmerR_string(iVal); }
//
//ZRef<YadR> sYadR(const Any& iVal)
//	{
//	if (const string8* theVal = iVal.PGet<string8>())
//		return sYadR(*theVal);
//
//	if (const Data_Any* theVal = iVal.PGet<Data_Any>())
//		return sYadR(*theVal);
//
//	if (const Seq_Any* theVal = iVal.PGet<Seq_Any>())
//		return sYadR(*theVal);
//
//	if (const Map_Any* theVal = iVal.PGet<Map_Any>())
//		return sYadR(*theVal);
//
//	return sYadAtomR_Any(iVal);
//	}
//
//ZRef<YadStreamerR> sYadR(const Data_Any& iData)
//	{ return new YadStreamerRPos_Any(iData); }
//
//ZRef<YadSeqAtRPos> sYadR(const Seq_Any& iSeq)
//	{ return new YadSeqAtRPos_Any(iSeq); }
//
//ZRef<YadMapAtR> sYadR(const Map_Any& iMap)
//	{ return new YadMapR_Any(iMap); }
//
//// =================================================================================================
//#pragma mark - sFromYadR
//
//namespace { // anonymous
//
//class ValFromYadR
//	{
//public:
//	ValFromYadR(bool iRepeatedPropsAsSeq)
//	:	fRepeatedPropsAsSeq(iRepeatedPropsAsSeq)
//		{}
//
//	ZQ<Val_Any> QVisit(const ZRef<ZCounted>& iCounted)
//		{
//		if (false)
//			{}
//
//		if (AnyHolder* theAnyHolder = iCounted.DynamicCast<AnyHolder>())
//			return theAnyHolder->Get();
//
//		if (ZRef<YadAtomR> theYadAtomR = iCounted.DynamicCast<YadAtomR>())
//			return theYadAtomR->AsAny();
//
//		if (ZRef<ChannerR_UTF> theChanner = iCounted.DynamicCast<ChannerR_UTF>())
//			{
//			string8 theString;
//			sCopyAll(*theChanner, ChanW_UTF_string8(&theString));
//			return theString;
//			}
//
//		if (ZRef<ChannerR_Bin> theChanner = iCounted.DynamicCast<ChannerR_Bin>())
//			return sReadAll_T<Data_Any>(*theChanner);
//
//		if (ZRef<YadSeqR> theYadSeqR = iCounted.DynamicCast<YadSeqR>())
//			{
//			Seq_Any theSeq;
//
//			while (ZRef<YadR> theChild = sReadInc(theYadSeqR))
//				{
//				if (NotQ<Val_Any> theQ = this->QVisit(theChild))
//					return null;
//				else
//					theSeq.Append(*theQ);
//				}
//
//			return theSeq;
//			}
//
//		if (ZRef<YadMapR> theYadMapR = iCounted.DynamicCast<YadMapR>())
//			{
//			Map_Any theMap;
//
//			Name theName;
//			while (ZRef<YadR> theChild = sReadInc(theYadMapR, theName))
//				{
//				if (NotQ<Val_Any> theQ = this->QVisit(theChild))
//					{ return false; }
//				else
//					{
//					if (fRepeatedPropsAsSeq)
//						{
//						if (Val_Any* prior = theMap.PMut(theName))
//							{
//							if (Seq_Any* priorSeq = prior->PMut<Seq_Any>())
//								{
//								priorSeq->Append(*theQ);
//								continue;
//								}
//							Seq_Any theSeq;
//							theSeq.Append(*prior);
//							theSeq.Append(*theQ);
//							theQ = theSeq;
//							}
//						}
//					theMap.Set(theName, *theQ);
//					}
//				}
//
//			return theMap;
//			}
//
//		return null;
//		}
//
//private:
//	bool fRepeatedPropsAsSeq;
//	};
//
//} // anonymous namespace
//
//// =================================================================================================
//#pragma mark - Yad_Any
//
//namespace Yad_Any {
//
//ZQ<Val_Any> sQFromYadR(ZRef<YadR> iYadR)
//	{ return sQFromYadR(false, iYadR); }
//
//ZQ<Val_Any> sQFromYadR(bool iRepeatedPropsAsSeq, ZRef<YadR> iYadR)
//	{ return ValFromYadR(iRepeatedPropsAsSeq).QVisit(iYadR); }
//
//} // namespace Yad_Any
//
//} // namespace ZooLib
