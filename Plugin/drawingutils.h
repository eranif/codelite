//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : drawingutils.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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
	static bool IsDark(const wxColour &col);
};

#endif //DRAWINGUTILS_H
