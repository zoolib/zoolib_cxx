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

#ifndef __ZGRgn_h__
#define __ZGRgn_h__ 1
#include "zconfig.h"

#include "zoolib/Accumulator_T.h"
#include "zoolib/FunctionChain.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZGeomPOD.h"

#include <vector>

namespace ZooLib {

class ZGRgnRep;

// =================================================================================================
// MARK: - ZGRgnRepCreator_T

template <typename Native>
class ZGRgnRepCreator_T
	{
public:
	static ZRef<ZGRgnRep> sCreate(Native iNative, bool iAdopt);
	};

// =================================================================================================
// MARK: - ZGRgnRep

class ZGRgnRep : public ZCounted
	{
protected:
	ZGRgnRep();

public:
	typedef bool (*DecomposeProc)(const ZRectPOD& iRect, void* iRefcon);
	virtual size_t Decompose(DecomposeProc iProc, void* iRefcon) = 0;

	virtual bool Contains(ZCoord iH, ZCoord iV) = 0;
	virtual bool IsEmpty() = 0;
	virtual ZRectPOD Bounds() = 0;
	virtual bool IsSimpleRect() = 0;
	virtual bool IsEqualTo(const ZRef<ZGRgnRep>& iRep) = 0;

	virtual void Inset(ZCoord iH, ZCoord iV) = 0;
	virtual ZRef<ZGRgnRep> Insetted(ZCoord iH, ZCoord iV) = 0;

	virtual void Offset(ZCoord iH, ZCoord iV) = 0;
	virtual ZRef<ZGRgnRep> Offsetted(ZCoord iH, ZCoord iV) = 0;

	virtual void Include(const ZRectPOD& iRect) = 0;
	virtual ZRef<ZGRgnRep> Including(const ZRectPOD& iRect) = 0;

	virtual void Intersect(const ZRectPOD& iRect) = 0;
	virtual ZRef<ZGRgnRep> Intersecting(const ZRectPOD& iRect) = 0;

	virtual void Exclude(const ZRectPOD& iRect) = 0;
	virtual ZRef<ZGRgnRep> Excluding(const ZRectPOD& iRect) = 0;

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
// MARK: - ZGRgn

class ZGRgn
	{
public:
	ZMACRO_operator_bool(ZGRgn, operator_bool) const
		{ return operator_bool_gen::translate(fRep && !fRep->IsEmpty()); }

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
	ZGRgn(const ZRectPOD& iRect);
	ZGRgn(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom);
	ZGRgn& operator=(const ZRectPOD& iRect);

	void Fresh();
	ZRef<ZGRgnRep> GetRep() const;

// Procedural API
	typedef ZGRgnRep::DecomposeProc DecomposeProc;
	size_t Decompose(DecomposeProc iProc, void* iRefcon) const;

	void Clear();

	bool IsEmpty() const;

	ZRectPOD Bounds() const;

	bool IsSimpleRect() const;

	bool Contains(ZCoord iH, ZCoord iV) const;

	bool Contains(const ZPointPOD& iPoint) const;

	bool IsEqualTo(const ZGRgn& iOther) const;

// Manipulation
	void Outline();
	ZGRgn Outlined() const;

	void Inset(ZCoord iH, ZCoord iV);
	ZGRgn Insetted(ZCoord iH, ZCoord iV) const;

	void Inset(const ZPointPOD& iPoint);
	ZGRgn Insetted(const ZPointPOD& iPoint) const;

	void Offset(ZCoord iOffsetH, ZCoord iOffsetV);
	ZGRgn Offsetted(ZCoord iOffsetH, ZCoord iOffsetV) const;

	void Offset(const ZPointPOD& iOffset);
	ZGRgn Offsetted(const ZPointPOD& iOffset) const;

	void Include(const ZRectPOD& iRect);
	ZGRgn Including(const ZRectPOD& iRect) const;

	void Intersect(const ZRectPOD& iRect);
	ZGRgn Intersecting(const ZRectPOD& iRect) const;

	void Exclude(const ZRectPOD& iRect);
	ZGRgn Excluding(const ZRectPOD& iRect) const;

	void Include(const ZGRgn& iOther);
	ZGRgn Including(const ZGRgn& iOther) const;

	void Intersect(const ZGRgn& iOther);
	ZGRgn Intersecting(const ZGRgn& iOther) const;

	void Exclude(const ZGRgn& iOther);
	ZGRgn Excluding(const ZGRgn& iOther) const;

	void Xor(const ZGRgn& iOther);
	ZGRgn Xoring(const ZGRgn& iOther) const;

private:
	ZRef<ZGRgnRep> fRep;
	};

inline void ZGRgn::Inset(const ZPointPOD& iInset)
	{ return this->Inset(iInset.h, iInset.v); }

inline ZGRgn ZGRgn::Insetted(const ZPointPOD& iInset) const
	{ return this->Insetted(iInset.h, iInset.v); }

inline void ZGRgn::Offset(const ZPointPOD& iOffset)
	{ return this->Offset(iOffset.h, iOffset.v); }

inline ZGRgn ZGRgn::Offsetted(const ZPointPOD& iOffset) const
	{ return this->Offsetted(iOffset.h, iOffset.v); }

// Operator API

inline bool operator==(const ZGRgn& l, const ZGRgn& r)
	{ return l.IsEqualTo(r); }

inline bool operator!=(const ZGRgn& l, const ZGRgn& r)
	{ return !l.IsEqualTo(r); }

inline void operator+=(ZGRgn& ioRgn, const ZPointPOD& iOffset)
	{ ioRgn.Offset(iOffset.h, iOffset.v); }

inline ZGRgn operator+(const ZGRgn& iRgn, const ZPointPOD& iOffset)
	{ return iRgn.Offsetted(iOffset.h, iOffset.v); }

inline void operator-=(ZGRgn& ioRgn, const ZPointPOD& iOffset)
	{ ioRgn.Offset(-iOffset.h, -iOffset.v); }

inline ZGRgn operator-(const ZGRgn& iRgn, const ZPointPOD& iOffset)
	{ return iRgn.Offsetted(-iOffset.h, -iOffset.v); }

inline void operator|=(ZGRgn& ioRgn, const ZRectPOD& iRect)
	{ ioRgn.Include(iRect); }

inline ZGRgn operator|(const ZGRgn& iRgn, const ZRectPOD& iRect)
	{ return iRgn.Including(iRect); }

inline void operator&=(ZGRgn& ioRgn, const ZRectPOD& iRect)
	{ ioRgn.Intersect(iRect); }

inline ZGRgn operator&(const ZGRgn& iRgn, const ZRectPOD& iRect)
	{ return iRgn.Intersecting(iRect); }

inline void operator-=(ZGRgn& ioRgn, const ZRectPOD& iRect)
	{ ioRgn.Exclude(iRect); }

inline ZGRgn operator-(const ZGRgn& iRgn, const ZRectPOD& iRect)
	{ return iRgn.Excluding(iRect); }

inline void operator|=(ZGRgn& ioRgn, const ZGRgn& iOther)
	{ ioRgn.Include(iOther); }

inline ZGRgn operator|(const ZGRgn& iRgn, const ZGRgn& iOther)
	{ return iRgn.Including(iOther); }

inline void operator&=(ZGRgn& ioRgn, const ZGRgn& iOther)
	{ ioRgn.Intersect(iOther); }

inline ZGRgn operator&(const ZGRgn& iRgn, const ZGRgn& iOther)
	{ return iRgn.Intersecting(iOther); }

inline void operator-=(ZGRgn& ioRgn, const ZGRgn& iOther)
	{ ioRgn.Exclude(iOther); }

inline ZGRgn operator-(const ZGRgn& iRgn, const ZGRgn& iOther)
	{ return iRgn.Excluding(iOther); }

inline void operator^=(ZGRgn& ioRgn, const ZGRgn& iOther)
	{ ioRgn.Xor(iOther); }

inline ZGRgn operator^(const ZGRgn& iRgn, const ZGRgn& iOther)
	{ return iRgn.Xoring(iOther); }

// =================================================================================================
// MARK: - ZGRgnAccumulator

class ZGRgnUnioner_t
	{
public:
	void operator()(ZGRgn& ioRgn, const ZGRgn& iOther) const
		{ ioRgn.Include(iOther); }
	};

typedef Accumulator_T<ZGRgn, ZGRgnUnioner_t, std::vector<ZGRgn> > ZGRgnAccumulator;

} // namespace ZooLib

#endif // __ZGRgn_h__
