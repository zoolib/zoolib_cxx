// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_Embed_h__
#define __ZooLib_QueryEngine_Walker_Embed_h__ 1
#include "zconfig.h"

#include "zoolib/QueryEngine/Walker.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker_Embed

class Walker_Embed : public Walker
	{
public:
	Walker_Embed(const ZP<Walker>& iWalker_Parent, const RelationalAlgebra::RelHead& iBoundNames,
		const string8& iColName, const ZP<Walker>& iWalker_Embedee);

	virtual ~Walker_Embed();

// From QueryEngine::Walker
	virtual void Rewind();

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool QReadInc(Val_DB* ioResults);

// Our protocol
	ZP<Walker> GetParent()
		{ return fWalker_Parent; }

	ZP<Walker> GetEmbedee()
		{ return fWalker_Embedee; }

private:
	ZP<Walker> fWalker_Parent;
	const RelationalAlgebra::RelHead fBoundNames;
	const string8 fColName;
	ZP<Walker> fWalker_Embedee;

	size_t fOutputOffset;
	RelationalAlgebra::RelHead fEmbedeeRelHead;
	std::vector<size_t> fEmbedeeOffsets;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_Embed_h__
