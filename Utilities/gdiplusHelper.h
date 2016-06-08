#ifndef gdiplusConversions_h__
#define gdiplusConversions_h__

#include <gdiplus.h>

inline Gdiplus::Point ToPoint(const CPoint& point) 
{
	return Gdiplus::Point(static_cast<INT>(point.x), static_cast<INT>(point.y));
}

inline Gdiplus::PointF ToPointF(const CPoint& point)
{
	return Gdiplus::PointF(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
}

inline Gdiplus::PointF ToPointF(long x, long y)
{
	return Gdiplus::PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y));
}

inline Gdiplus::Rect ToRect(const CRect& rc)
{
	return Gdiplus::Rect(static_cast<INT>(rc.left), static_cast<INT>(rc.top), 
		static_cast<INT>(rc.Width()), static_cast<INT>(rc.Height()));
}

inline Gdiplus::RectF ToRectF(const CRect& rc)
{
	return Gdiplus::RectF(static_cast<Gdiplus::REAL>(rc.left), static_cast<Gdiplus::REAL>(rc.top), 
		static_cast<Gdiplus::REAL>(rc.Width()), static_cast<Gdiplus::REAL>(rc.Height()));
}
inline Gdiplus::Size ToSize(const CSize& size)
{
	return Gdiplus::Size(size.cx, size.cy);
}

inline Gdiplus::SizeF ToSizeF(const CSize& size)
{
	return Gdiplus::SizeF(static_cast<Gdiplus::REAL>(size.cx + 0.5f), 
		static_cast<Gdiplus::REAL>(size.cy + 0.5f));
}

inline CSize ToCSize(const Gdiplus::SizeF& size)
{
	return CSize(static_cast<long>(size.Width + 0.5f), static_cast<long>(size.Height + 0.5f));
}

inline CPoint ToCPoint(const Gdiplus::PointF& point)
{
	return CPoint(static_cast<int>(point.X + 0.5f), static_cast<int>(point.Y + 0.5f));
}


inline CRect ToCRect(const Gdiplus::Rect& rc)
{
	return CRect(rc.X, rc.Y, rc.X + rc.Width, rc.Y + rc.Height);
}

inline CRect ToCRect(const Gdiplus::RectF& rc)
{
	return CRect(static_cast<int>(rc.X + 0.5f), static_cast<int>(rc.Y + 0.5f), 
		static_cast<int>(rc.X + 0.5f + rc.Width), static_cast<int>(rc.Y + rc.Height + 0.5f));
}

namespace GdiplusHelper
{
	CString GetActualDisplayText(const CString& display_text, Gdiplus::Graphics& graphics, Gdiplus::Font& font, Gdiplus::StringFormat& fomat, LONG Width);
}

#define GDIPLUS_CHECK(A) {Status status = (A); if (status != Ok){status = (A);} ASSERT(status == Ok);}

#endif // gdiplusConversions_h__