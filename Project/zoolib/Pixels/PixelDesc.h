/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __ZooLib_Pixels_PixelDesc_h__
#define __ZooLib_Pixels_PixelDesc_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"

#include "zoolib/Pixels/Pixval2RGBA.h"
#include "zoolib/Pixels/RGBA2Pixval.h"

namespace ZooLib {
namespace Pixels {

typedef bool (*MungeProc)(Ord iH, Ord iV, RGBA& ioColor, void* iRefcon);

class PixelDescRep;

// =================================================================================================
#pragma mark - PixelDesc

class PixelDesc
	{
public:
	PixelDesc();
	PixelDesc(const PixelDesc& iOther);
	~PixelDesc();
	PixelDesc& operator=(const PixelDesc& iOther);

	PixelDesc(const ZP<PixelDescRep>& iPixelDescRep);
	PixelDesc& operator=(const ZP<PixelDescRep>& iPixelDescRep);

	ZP<PixelDescRep> GetRep() const;

	bool HasAlpha() const;
	PixelDesc WithoutAlpha() const;

	RGBA AsRGBA(Pixval iPixval) const;
	void AsRGBAs(const Pixval* iPixvals, size_t iCount, RGBA* oColors) const;

	Comp AsAlpha(Pixval iPixval) const;

	Pixval AsPixval(const RGBA& iRGBA) const;
	void AsPixvals(const RGBA* iColors, size_t iCount, Pixval* oPixvals) const;

private:
	ZP<PixelDescRep> fRep;
	};

// =================================================================================================
#pragma mark - PixelDescRep

class PixelDescRep
:	public ZCounted
	{
protected:
	PixelDescRep() {}
	PixelDescRep(const PixelDescRep& iOther) {}
	PixelDescRep& operator=(const PixelDescRep&) { return *this; }

public:
	virtual ~PixelDescRep() {}

	virtual bool HasAlpha() = 0;
	virtual ZP<PixelDescRep> WithoutAlpha() = 0;

	virtual RGBA Imp_AsRGBA(Pixval iPixval) const = 0;
	virtual void Imp_AsRGBAs(const Pixval* iPixvals, size_t iCount, RGBA* oColors) const = 0;

	virtual Comp Imp_AsAlpha(Pixval iPixval) const = 0;

	virtual Pixval Imp_AsPixval(const RGBA& iRGBA) const = 0;
	virtual void Imp_AsPixvals(const RGBA* iColors, size_t iCount, Pixval* oPixvals) const = 0;
	};

// =================================================================================================
#pragma mark - PixelDescRep_Indexed

class PixelDescRep_Indexed
:	public PixelDescRep
,	public Pixval2RGBA_Indexed
,	public RGBA2Pixval_Indexed
	{
public:
	PixelDescRep_Indexed(const RGBA* iColors, size_t iCount);
	PixelDescRep_Indexed(const RGBA* iColors, size_t iCount, bool iStripAlpha);

	PixelDescRep_Indexed(const RGBA* iColors, uint32* iPixvals, size_t iCount);
	PixelDescRep_Indexed(const RGBA* iColors,
		uint32* iPixvals, size_t iCount, bool iStripAlpha);

//	PixelDescRep_Indexed(const ZRGBAMap* iColorMap, size_t iCount);

	virtual ~PixelDescRep_Indexed();

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZP<PixelDescRep> WithoutAlpha();

	virtual RGBA Imp_AsRGBA(Pixval iPixval) const
		{ return Pixval2RGBA_Indexed::AsRGBA(iPixval); }

	virtual void Imp_AsRGBAs(const Pixval* iPixvals, size_t iCount, RGBA* oColors) const
		{ return Pixval2RGBA_Indexed::AsRGBAs(iPixvals, iCount, oColors); }

	virtual Comp Imp_AsAlpha(Pixval iPixval) const
		{ return Pixval2RGBA_Indexed::AsAlpha(iPixval); }

	virtual Pixval Imp_AsPixval(const RGBA& iRGBA) const
		{ return RGBA2Pixval_Indexed::AsPixval(iRGBA); }

	virtual void Imp_AsPixvals(const RGBA* iColors, size_t iCount, Pixval* oPixvals) const
		{ return RGBA2Pixval_Indexed::AsPixvals(iColors, iCount, oPixvals); }

// Our protocol
	void BuildReverseLookupIfNeeded() const;

	void GetColors(const RGBA*& oColors, size_t& oCount) const;

	bool Matches(const PixelDescRep_Indexed* iOther);

protected:
	bool fCheckedAlpha;
	bool fHasAlpha;
	};

// =================================================================================================
#pragma mark - PixelDescRep_Gray

class PixelDescRep_Gray
:	public PixelDescRep
,	public Pixval2RGBA_Gray
,	public RGBA2Pixval_Gray
	{
public:
	PixelDescRep_Gray(uint32 iMaskGray, uint32 iMaskAlpha);

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZP<PixelDescRep> WithoutAlpha();

	virtual RGBA Imp_AsRGBA(Pixval iPixval) const
		{ return Pixval2RGBA_Gray::AsRGBA(iPixval); }

	virtual void Imp_AsRGBAs(const Pixval* iPixvals, size_t iCount, RGBA* oColors) const
		{ return Pixval2RGBA_Gray::AsRGBAs(iPixvals, iCount, oColors); }

	virtual Comp Imp_AsAlpha(Pixval iPixval) const
		{ return Pixval2RGBA_Gray::AsAlpha(iPixval); }

	virtual Pixval Imp_AsPixval(const RGBA& iRGBA) const
		{ return RGBA2Pixval_Gray::AsPixval(iRGBA); }

	virtual void Imp_AsPixvals(const RGBA* iColors, size_t iCount, Pixval* oPixvals) const
		{ return RGBA2Pixval_Gray::AsPixvals(iColors, iCount, oPixvals); }

// Our protocol
	void GetMasks(uint32& oMaskL, uint32& oMaskA) const;

	bool Matches(const PixelDescRep_Gray* iOther);
	};

// =================================================================================================
#pragma mark - PixelDescRep_Color

class PixelDescRep_Color
:	public PixelDescRep
,	public Pixval2RGBA_Color
,	public RGBA2Pixval_Color
	{
public:
	PixelDescRep_Color(uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha);

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZP<PixelDescRep> WithoutAlpha();

	virtual RGBA Imp_AsRGBA(Pixval iPixval) const
		{ return Pixval2RGBA_Color::AsRGBA(iPixval); }

	virtual void Imp_AsRGBAs(const Pixval* iPixvals, size_t iCount, RGBA* oColors) const
		{ return Pixval2RGBA_Color::AsRGBAs(iPixvals, iCount, oColors); }

	virtual Comp Imp_AsAlpha(Pixval iPixval) const
		{ return Pixval2RGBA_Color::AsAlpha(iPixval); }

	virtual Pixval Imp_AsPixval(const RGBA& iRGBA) const
		{ return RGBA2Pixval_Color::AsPixval(iRGBA); }

	virtual void Imp_AsPixvals(const RGBA* iColors, size_t iCount, Pixval* oPixvals) const
		{ return RGBA2Pixval_Color::AsPixvals(iColors, iCount, oPixvals); }

	void GetMasks(uint32& oMaskR, uint32& oMaskG, uint32& oMaskB, uint32& oMaskA) const;

	bool Matches(const PixelDescRep_Color* iOther);
	};

// =================================================================================================
#pragma mark - PixelDesc

PixelDesc sPixelDesc(const RGBA* iColors, size_t iCount);
//PixelDesc sPixelDesc(const RGBAMap* iColorMap, size_t iCount)

PixelDesc sPixelDesc(uint32 iMaskGray, uint32 iMaskAlpha);

PixelDesc sPixelDesc(uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixelDesc_h__
