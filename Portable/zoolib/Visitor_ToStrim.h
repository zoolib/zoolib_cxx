// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Visitor_ToStrim_h__
#define __ZooLib_Visitor_ToStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"
#include "zoolib/Visitor.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_ToStrim

class Visitor_ToStrim
:	public virtual Visitor
	{
public:
	struct Options
		{
		Options();

		std::string fEOLString;
		std::string fIndentString;
		size_t fInitialIndent;
		bool fDebuggingOutput;
		};

	Visitor_ToStrim();

// From Visitor
	virtual void Visit(const ZP<Visitee>& iRep);

// Our protocol
	void ToStrim(const ChanW_UTF& iStrimW, const Options& iOptions, const ZP<Visitee>& iRep);

protected:
	void pToStrim(const ZP<Visitee>& iRep);

	const Options& pOptions();
	const ChanW_UTF& pStrimW();

	void pWriteLFIndent();

private:
	const Options* fOptions;
	const ChanW_UTF* fStrimW;

protected:
	size_t fIndent;
	};

} // namespace ZooLib

#endif // __ZooLib_Visitor_ToStrim_h__
