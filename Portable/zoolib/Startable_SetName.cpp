// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Startable_SetName.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"

namespace ZooLib {

class Startable_SetName
:	public Startable
	{
public:
	Startable_SetName(const std::string& iName, const ZP<Startable>& iStartable)
	:	fName(iName)
	,	fStartable(iStartable)
		{}

	virtual bool QCall()
		{
		ZThread::sSetName(fName.c_str());
		fStartable->QCall();
		return true;
		}

	const std::string fName;
	const ZP<Startable> fStartable;
	};

ZP<Startable> sStartable_SetName(const std::string& iName, const ZP<Startable>& iStartable)
	{
	if (iStartable)
		return new Startable_SetName(iName, iStartable);
	return null;
	}

} // namespace ZooLib
