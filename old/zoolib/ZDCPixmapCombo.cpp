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

#include "zoolib/ZDCPixmapCombo.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCPixmapCombo

ZDCPixmapCombo::ZDCPixmapCombo()
	{}

ZDCPixmapCombo::ZDCPixmapCombo(const ZDCPixmap& iColorPixmap,
	const ZDCPixmap& iMonoPixmap,
	const ZDCPixmap& iMaskPixmap)
:	fColorPixmap(iColorPixmap),
	fMonoPixmap(iMonoPixmap),
	fMaskPixmap(iMaskPixmap)
	{}

ZDCPixmapCombo::ZDCPixmapCombo(const ZDCPixmap& iMainPixmap, const ZDCPixmap& iMaskPixmap)
:	fColorPixmap(iMainPixmap),
	fMaskPixmap(iMaskPixmap)
	{}

ZDCPixmapCombo& ZDCPixmapCombo::operator=(const ZDCPixmapCombo& iOther)
	{
	fColorPixmap = iOther.fColorPixmap;
	fMonoPixmap = iOther.fMonoPixmap;
	fMaskPixmap = iOther.fMaskPixmap;
	return *this;
	}

bool ZDCPixmapCombo::operator==(const ZDCPixmapCombo& iOther) const
	{
	return fColorPixmap == iOther.fColorPixmap
		&& fMonoPixmap == iOther.fMonoPixmap
		&& fMaskPixmap == iOther.fMaskPixmap;
	}

void ZDCPixmapCombo::SetPixmaps(const ZDCPixmap& iColorPixmap,
	const ZDCPixmap& iMonoPixmap,
	const ZDCPixmap& iMaskPixmap)
	{
	fColorPixmap = iColorPixmap;
	fMonoPixmap = iMonoPixmap;
	fMaskPixmap = iMaskPixmap;
	}

void ZDCPixmapCombo::SetPixmaps(const ZDCPixmap& iMainPixmap, const ZDCPixmap& iMaskPixmap)
	{
	fColorPixmap = iMainPixmap;
	fMonoPixmap = ZDCPixmap();
	fMaskPixmap = iMaskPixmap;
	}

void ZDCPixmapCombo::GetPixmaps(ZDCPixmap& oColorPixmap,
	ZDCPixmap& oMonoPixmap,
	ZDCPixmap& oMaskPixmap) const
	{
	oColorPixmap = fColorPixmap;
	oMonoPixmap = fMonoPixmap;
	oMaskPixmap = fMaskPixmap;
	}

ZPoint ZDCPixmapCombo::Size() const
	{
	using std::max;

	ZPoint theSize = ZPoint::sZero;
	ZPoint tempSize = fColorPixmap.Size();
	theSize.h = max(theSize.h, tempSize.h);
	theSize.v = max(theSize.v, tempSize.v);

	tempSize = fMonoPixmap.Size();
	theSize.h = max(theSize.h, tempSize.h);
	theSize.v = max(theSize.v, tempSize.v);

	tempSize = fMaskPixmap.Size();
	theSize.h = max(theSize.h, tempSize.h);
	theSize.v = max(theSize.v, tempSize.v);

	return theSize;
	}

ZDCPixmapCombo::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fColorPixmap || fMonoPixmap); }
	
// =================================================================================================

} // namespace ZooLib
