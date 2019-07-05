// Copyright (c) 2019 Andrew Green
// http://www.zoolib.org

#ifndef __ZooLib_ChanRU_UTF_ML_h__
#define __ZooLib_ChanRU_UTF_ML_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ChanRU_UTF.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/ML.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRU_UTF_ML

class ChanRU_UTF_ML
:	public ChanRU_UTF
	{
public:
	explicit ChanRU_UTF_ML(const ChanRU_UTF& iChanRU);

	ChanRU_UTF_ML(const ChanRU_UTF& iChanRU,
		bool iRecognizeEntitiesInAttributeValues, ZP<ML::Callable_Entity> iCallable);
	~ChanRU_UTF_ML();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);

// Our protocol
//	ZMACRO_operator_bool(ChanU_UTF, operator_bool) const;
	ML::EToken Current() const;
	ChanRU_UTF_ML& Advance();

	const std::string& Name() const;
	ML::Attrs_t Attrs() const;

	ZQ<std::string> QAttr(const std::string& iAttrName) const;
	std::string Attr(const std::string& iAttrName) const;

private:
	void pAdvance() const;
	void pAdvance();

	const ChanRU_UTF& fChanRU;

	bool fRecognizeEntitiesInAttributeValues;

	ZP<ML::Callable_Entity> fCallable;

	string32 fBuffer;
	size_t fBufferStart;

	ML::EToken fToken;

	std::string fTagName;
	ML::Attrs_t fTagAttributes;
	};

// =================================================================================================
#pragma mark - ML::ChannerRU

typedef Channer_T<ChanRU_UTF_ML> ChannerRU_UTF_ML;

ZP<ChannerRU_UTF_ML> sChannerRU_UTF_ML(const ZP<ChannerRU<UTF32>>& iChanner);

// =================================================================================================
#pragma mark - ML parsing support

void sSkipText(ChanRU_UTF_ML& r);

bool sSkip(ChanRU_UTF_ML& r, const std::string& iTagName);
bool sSkip(ChanRU_UTF_ML& r, std::vector<std::string>& ioTags);

bool sTryRead_Begin(ChanRU_UTF_ML& r, const std::string& iTagName);

bool sTryRead_Empty(ChanRU_UTF_ML& r, const std::string& iTagName);

bool sTryRead_End(ChanRU_UTF_ML& r, const std::string& iTagName);

} // namespace ZooLib

#endif // __ZooLib_ChanRU_UTF_ML_h__
