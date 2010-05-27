/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZGRgn__
#define __ZGRgn__ 1
#include "zconfig.h"

#include "zoolib/ZAccumulator_T.h"
#include "zoolib/ZGeom.h"
#include "zoolib/ZRef_Counted.h"

#include <vector>

namespace ZooLib {

class ZGRgnRep;

// =================================================================================================
#pragma mark -
#pragma mark * ZGRgnRepCreator_T

template <typename Native>
class ZGRgnRepCreator_T
	{
public:
	static ZRef<ZGRgnRep> sCreate(Native iNative, bool iAdopt);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGRgnRep

class ZGRgnRep : public ZCounted
	{
protected:
	ZGRgnRep();

public:
	typedef bool (*DecomposeProc)(const ZRect& iRect, void* iRefcon);
	virtual size_t Decompose(DecomposeProc iProc, void* iRefcon) = 0;

	virtual bool Contains(ZCoord iH, ZCoord iV) = 0;
	virtual bool IsEmpty() = 0;
	virtual ZRect Bounds() = 0;
	virtual bool IsSimpleRect() = 0;
	virtual bool IsEqualTo(const ZRef<ZGRgnRep>& iRep) = 0;

	virtual void Inset(ZCoord iH, ZCoord iV) = 0;
	virtual ZRef<ZGRgnRep> Insetted(ZCoord iH, ZCoord iV) = 0;

	virtual void Offset(ZCoord iH, ZCoord iV) = 0;
	virtual ZRef<ZGRgnRep> Offsetted(ZCoord iH, ZCoord iV) = 0;

	virtual void Include(const ZRect& iRect) = 0;
	virtual ZRef<ZGRgnRep> Including(const ZRect& iRect) = 0;

	virtual void Intersect(const ZRect& iRect) = 0;
	virtual ZRef<ZGRgnRep> Intersecting(const ZRect& iRect) = 0;

	virtual void Exclude(const ZRect& iRect) = 0;
	virtual ZRef<ZGRgnRep> Excluding(const ZRect& iRect) = 0;

	virtual void Include(const ZRef<ZGRgnRep>& iRep) = 0;
	virtual ZRef<ZGRgnRep> Including(const ZRef<ZGRgnRep>& iRep) = 0;

	virtual void Intersect(const ZRef<ZGRgnRep>& iRep) = 0;
	virtual ZRef<ZGRgnRep> Intersecting(const ZRef<ZGRgnRep>& iRep) = 0;

	virtual void Exclude(const ZRef<ZGRgnRep>& iRep) = 0;
	virtual ZRef<ZGRgnRep> Excluding(const ZRef<ZGRgnRep>& iRep) = 0;

	virtual void Xor(const ZRef<ZGRgnRep>& iRep) = 0;
	virtual ZRef<ZGRgnRep> Xoring(const ZRef<ZGRgnRep>& iRep) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZGRgn

class ZGRgn
	{
	ZRef<ZGRgnRep> fRep;

public:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZGRgn, operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fRep && !fRep->IsEmpty()); }

	template <typename Native>
	static ZGRgn sFromNative(Native iNative, bool iAdopt)
		{ return ZGRgnRepCreator_T<Native>::sCreate(iNative, iAdopt); }

	ZGRgn();

	~ZGRgn();

	ZGRgn(const ZGRgn& iOther);

	ZGRgn& operator=(const ZGRgn& iOther);

	ZGRgn(const ZRef<ZGRgnRep>& iRep);

	ZGRgn& operator=(const ZRef<ZGRgnRep>& iRep);

// Convenience construction, assignment from rects
	ZGRgn(const ZRect& iRect);
	ZGRgn(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom);
	ZGRgn& operator=(const ZRect& iRect);

	void Fresh();
	ZRef<ZGRgnRep> GetRep() const;

// Procedural API
	typedef ZGRgnRep::DecomposeProc DecomposeProc;
	size_t Decompose(DecomposeProc iProc, void* iRefcon) const;

	void Clear();

	bool IsEmpty() const;

	ZRect Bounds() const;

	bool IsSimpleRect() const;

	bool Contains(ZCoord iH, ZCoord iV) const;

	bool Contains(const ZPoint& iPoint) const;

	bool IsEqualTo(const ZGRgn& iOther) const;

// Equality
	bool operator==(const ZGRgn& iOther) const;
	bool operator!=(const ZGRgn& iOther) const;

// Manipulation
	void Outline();
	ZGRgn Outlined() const;

	void Inset(ZCoord iH, ZCoord iV);
	ZGRgn Insetted(ZCoord iH, ZCoord iV) const;
	
	void Inset(const ZPoint& iPoint);
	ZGRgn Insetted(const ZPoint& iPoint) const;

	void Offset(ZCoord iOffsetH, ZCoord iOffsetV);
	ZGRgn Offsetted(ZCoord iOffsetH, ZCoord iOffsetV) const;

	void Offset(const ZPoint& iOffset);
	ZGRgn Offsetted(const ZPoint& iOffset) const;

	void Include(const ZRect& iRect);
	ZGRgn Including(const ZRect& iRect) const;

	void Intersect(const ZRect& iRect);
	ZGRgn Intersecting(const ZRect& iRect) const;

	void Exclude(const ZRect& iRect);
	ZGRgn Excluding(const ZRect& iRect) const;

	void Include(const ZGRgn& iOther);
	ZGRgn Including(const ZGRgn& iOther) const;

	void Intersect(const ZGRgn& iOther);
	ZGRgn Intersecting(const ZGRgn& iOther) const;

	void Exclude(const ZGRgn& iOther);
	ZGRgn Excluding(const ZGRgn& iOther) const;

	void Xor(const ZGRgn& iOther);
	ZGRgn Xoring(const ZGRgn& iOther) const;

// Algebraic API
	void operator+=(const ZPoint& iOffset);

	ZGRgn operator+(const ZPoint& iOffset) const;

	void operator-=(const ZPoint& iOffset);

	ZGRgn operator-(const ZPoint& iOffset) const;

// Combining with rectangles
	void operator|=(const ZRect& iRect);

	ZGRgn operator|(const ZRect& iRect) const;

	void operator&=(const ZRect& iRect);

	ZGRgn operator&(const ZRect& iRect) const;

	void operator-=(const ZRect& iRect);

	ZGRgn operator-(const ZRect& iRect) const;

// Combining with regions
	void operator|=(const ZGRgn& iOther);

	ZGRgn operator|(const ZGRgn& iOther) const;

	void operator&=(const ZGRgn& iOther);

	ZGRgn operator&(const ZGRgn& iOther) const;

	void operator-=(const ZGRgn& iOther);

	ZGRgn operator-(const ZGRgn& iOther) const;

	void operator^=(const ZGRgn& iOther);

	ZGRgn operator^(const ZGRgn& iOther) const;
	};

inline bool ZGRgn::operator==(const ZGRgn& iOther) const
	{ return this->IsEqualTo(iOther); }

inline bool ZGRgn::operator!=(const ZGRgn& iOther) const
	{ return ! this->IsEqualTo(iOther); }

inline void ZGRgn::Inset(const ZPoint& iInset)
	{ return this->Inset(iInset.h, iInset.v); }

inline ZGRgn ZGRgn::Insetted(const ZPoint& iInset) const
	{ return this->Insetted(iInset.h, iInset.v); }

inline void ZGRgn::Offset(const ZPoint& iOffset)
	{ return this->Offset(iOffset.h, iOffset.v); }

inline ZGRgn ZGRgn::Offsetted(const ZPoint& iOffset) const
	{ return this->Offsetted(iOffset.h, iOffset.v); }

inline void ZGRgn::operator+=(const ZPoint& iOffset)
	{ return this->Offset(iOffset.h, iOffset.v); }

inline ZGRgn ZGRgn::operator+(const ZPoint& iOffset) const
	{ return this->Offsetted(iOffset.h, iOffset.v); }

inline void ZGRgn::operator-=(const ZPoint& iOffset)
	{ return this->Offset(-iOffset.h, -iOffset.v); }

inline ZGRgn ZGRgn::operator-(const ZPoint& iOffset) const
	{ return this->Offsetted(-iOffset.h, -iOffset.v); }

inline void ZGRgn::operator|=(const ZRect& iRect)
	{ return this->Include(iRect); }

inline ZGRgn ZGRgn::operator|(const ZRect& iRect) const
	{ return this->Including(iRect); }

inline void ZGRgn::operator&=(const ZRect& iRect)
	{ return this->Intersect(iRect); }

inline ZGRgn ZGRgn::operator&(const ZRect& iRect) const
	{ return this->Intersecting(iRect); }

inline void ZGRgn::operator-=(const ZRect& iRect)
	{ return this->Exclude(iRect); }

inline ZGRgn ZGRgn::operator-(const ZRect& iRect) const
	{ return this->Excluding(iRect); }

inline void ZGRgn::operator|=(const ZGRgn& iOther)
	{ return this->Include(iOther); }

inline ZGRgn ZGRgn::operator|(const ZGRgn& iOther) const
	{ return this->Including(iOther); }

inline void ZGRgn::operator&=(const ZGRgn& iOther)
	{ return this->Intersect(iOther); }

inline ZGRgn ZGRgn::operator&(const ZGRgn& iOther) const
	{ return this->Intersecting(iOther); }

inline void ZGRgn::operator-=(const ZGRgn& iOther)
	{ return this->Exclude(iOther); }

inline ZGRgn ZGRgn::operator-(const ZGRgn& iOther) const
	{ return this->Excluding(iOther); }

inline void ZGRgn::operator^=(const ZGRgn& iOther)
	{ return this->Xor(iOther); }

inline ZGRgn ZGRgn::operator^(const ZGRgn& iOther) const
	{ return this->Xoring(iOther); }

// =================================================================================================
#pragma mark -
#pragma mark * ZGRgnAccumulator

class ZGRgnUnioner_t
	{
public:
	void operator()(ZGRgn& ioRgn, const ZGRgn& iOther) const
		{ ioRgn.Include(iOther); }
	};

typedef ZAccumulator_T<ZGRgn, ZGRgnUnioner_t, std::vector<ZGRgn> > ZGRgnAccumulator;

} // namespace ZooLib

#endif // __ZGRgn__
