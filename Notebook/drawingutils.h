#ifndef DRAWINGUTILS_H
#define DRAWINGUTILS_H

#include "wx/dc.h"
#include "wx/colour.h"

class DrawingUtils
{
public:
	static wxColor LightColour(const wxColour& color, int percent);
	static void TruncateText(wxDC& dc, const wxString& text, const int &maxWidth, wxString& fixedText);
	static void PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool vertical);
	static void DrawVerticalButton(wxDC& dc, const wxRect& rect, const bool &focus, const bool &upperTabs, bool vertical, bool hover = false);
	static void DrawHorizontalButton(wxDC& dc, const wxRect& rect, const bool &focus, const bool &upperTabs, bool vertical, bool hover = false);
};

#endif //DRAWINGUTILS_H
