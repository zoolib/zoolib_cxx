// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_PixvalIters_h__
#define __ZooLib_Pixels_PixvalIters_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/PixvalAccessor.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixvalIterR

class PixvalIterR
	{
public:
	PixvalIterR(const PixvalDesc& iPixvalDesc, const void* iAddress, int iH);

	Pixval ReadInc();
	Pixval Read();

	void Inc();

	void Reset(const void* iAddress, int iH);

protected:
	PixvalAccessor fAccessor;
	const void* fAddress;

	int fH;
	};

// =================================================================================================
#pragma mark - PixvalIterW

class PixvalIterW
	{
public:
	PixvalIterW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH);

	void WriteInc(Pixval iPixval);
	void Write(Pixval iPixval);

	void Inc();

	void Reset(void* iAddress, int iH);

private:
	PixvalAccessor fAccessor;
	void* fAddress;

	int fH;
	};

// =================================================================================================
#pragma mark - PixvalIterRW

class PixvalIterRW
	{
public:
	PixvalIterRW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH);

	Pixval ReadInc();
	Pixval Read();

	void WriteInc(Pixval iPixval);
	void Write(Pixval iPixval);

	void Inc();

	void Reset(void* iAddress, int iH);

private:
	PixvalAccessor fAccessor;
	void* fAddress;

	int fH;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixvalIters_h__
