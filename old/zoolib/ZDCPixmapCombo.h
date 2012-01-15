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

#ifndef __ZDCPixmapCombo_h__
#define __ZDCPixmapCombo_h__ 1
#include "zconfig.h"

#include "zoolib/ZDCPixmap.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCPixmapCombo

class ZDCPixmapCombo
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZDCPixmapCombo,
    	operator_bool_generator_type, operator_bool_type);

public:
	ZDCPixmapCombo();

	ZDCPixmapCombo(const ZDCPixmap& iColorPixmap,
		const ZDCPixmap& iMonoPixmap,
		const ZDCPixmap& iMaskPixmap);

	ZDCPixmapCombo(const ZDCPixmap& iMainPixmap, const ZDCPixmap& iMaskPixmap);

	ZDCPixmapCombo& operator=(const ZDCPixmapCombo& iOther);

	bool operator==(const ZDCPixmapCombo& other) const;
	bool operator!=(const ZDCPixmapCombo& other) const
		{ return ! (*this == other); }

	void SetPixmaps(const ZDCPixmap& iColorPixmap,
		const ZDCPixmap& iMonoPixmap,
		const ZDCPixmap& iMaskPixmap);

	void SetPixmaps(const ZDCPixmap& iMainPixmap, const ZDCPixmap& iMaskPixmap);

	void GetPixmaps(ZDCPixmap& oColorPixmap, ZDCPixmap& oMonoPixmap, ZDCPixmap& oMaskPixmap) const;

	void SetColor(const ZDCPixmap& iPixmap) { fColorPixmap = iPixmap; }
	const ZDCPixmap& GetColor() const { return fColorPixmap; }

	void SetMono(const ZDCPixmap& iPixmap) { fMonoPixmap = iPixmap; }
	const ZDCPixmap& GetMono() const { return fMonoPixmap; }

	void SetMask(const ZDCPixmap& iPixmap) { fMaskPixmap = iPixmap; }
	const ZDCPixmap& GetMask() const { return fMaskPixmap; }

	ZPoint Size() const;

// Are we valid, i.e. do we have any pixels at all?
	operator operator_bool_type() const;

protected:
	ZDCPixmap fColorPixmap;
	ZDCPixmap fMonoPixmap;
	ZDCPixmap fMaskPixmap;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZDCPixmapCombo_h__
