//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : drawingutils.cpp
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
#include "wx/settings.h"
#include "drawingutils.h"
#include "wx/dc.h"

//////////////////////////////////////////////////
// Colour methods to convert HSL <-> RGB
//////////////////////////////////////////////////
static float __min(float x, float y, float z)
{
	float m = x < y ? x : y;
	m = m < z ? m : z;
	return m;
}

static float __max(float x, float y, float z)
{
	float m = x > y ? x : y;
	m = m > z ? m : z;
	return m;
}

static void RGB_2_HSL(float r, float g, float b, float *h, float *s, float *l)
{
	float var_R = ( r / 255.0 );                     //RGB from 0 to 255
	float var_G = ( g / 255.0 );
	float var_B = ( b / 255.0 );

	float var_Min = __min( var_R, var_G, var_B );    //Min. value of RGB
	float var_Max = __max( var_R, var_G, var_B );    //Max. value of RGB
	float del_Max = var_Max - var_Min;             //Delta RGB value

	*l = ( var_Max + var_Min ) / 2.0;

	if ( del_Max == 0 ) {                   //This is a gray, no chroma...
		*h = 0;                                //HSL results from 0 to 1
		*s = 0;
	} else {                                 //Chromatic data...
		if ( *l < 0.5 ) *s = del_Max / ( var_Max + var_Min );
		else *s = del_Max / ( 2.0 - var_Max - var_Min );

		float del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
		float del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
		float del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;

		if      ( var_R == var_Max ) *h = del_B - del_G;
		else if ( var_G == var_Max ) *h = ( 1.0 / 3.0 ) + del_R - del_B;
		else if ( var_B == var_Max ) *h = ( 2.0 / 3.0 ) + del_G - del_R;

		if ( *h < 0 ) *h += 1;
		if ( *h > 1 ) *h -= 1;
	}
}

static float Hue_2_RGB( float v1, float v2, float vH )             //Function Hue_2_RGB
{
	if ( vH < 0 ) vH += 1;
	if ( vH > 1 ) vH -= 1;
	if ( ( 6.0 * vH ) < 1 ) return ( v1 + ( v2 - v1 ) * 6.0 * vH );
	if ( ( 2.0 * vH ) < 1 ) return ( v2 );
	if ( ( 3.0 * vH ) < 2 ) return ( v1 + ( v2 - v1 ) * ( ( 2.0 / 3.0 ) - vH ) * 6.0 );
	return ( v1 );
}

static void HSL_2_RGB(float h, float s, float l, float *r, float *g, float *b)
{
	if ( s == 0 ) {                     //HSL from 0 to 1
		*r = l * 255.0;                      //RGB results from 0 to 255
		*g = l * 255.0;
		*b = l * 255.0;
	} else {
		float var_2;
		if ( l < 0.5 ) var_2 = l * ( 1.0 + s );
		else           var_2 = ( l + s ) - ( s * l );

		float var_1 = 2.0 * l - var_2;

		*r = 255.0 * Hue_2_RGB( var_1, var_2, h + ( 1.0 / 3.0 ) );
		*g = 255.0 * Hue_2_RGB( var_1, var_2, h );
		*b = 255.0 * Hue_2_RGB( var_1, var_2, h - ( 1.0 / 3.0 ) );
	}
}

//-------------------------------------------------------------------------------------------------
// helper functions
//-------------------------------------------------------------------------------------------------

wxColor DrawingUtils::LightColour(const wxColour& color, float percent)
{
	float h, s, l, r, g, b;
	RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

	// reduce the Lum value
	l += (float)((percent * 5.0)/100.0);
	if (l > 1.0) l = 1.0;

	HSL_2_RGB(h, s, l, &r, &g, &b);
	return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

void DrawingUtils::TruncateText(wxDC& dc, const wxString& text, const int &maxWidth, wxString& fixedText)
{
	int textH, textW;
	int rectSize = maxWidth + 4; //error size
	int textLen = (int)text.Length();
	wxString tempText = text;

	fixedText = wxT("");
	dc.GetTextExtent(text, &textW, &textH);
	if (rectSize >= textW) {
		fixedText = text;
		return;
	}

	// The text does not fit in the designated area,
	// so we need to truncate it a bit
	wxString suffix = wxT("..");
	int w, h;
	dc.GetTextExtent(suffix, &w, &h);
	rectSize -= w;

	for (int i=textLen; i>=0; i--) {
		dc.GetTextExtent(tempText, &textW, &textH);
		if (rectSize > textW) {
			fixedText = tempText;
			fixedText += wxT("..");
			return;
		}
		tempText = tempText.RemoveLast();
	}
}

void DrawingUtils::PaintStraightGradientBox(wxDC& dc,
        const wxRect& rect,
        const wxColour& startColor,
        const wxColour& endColor,
        bool  vertical)
{
	int rd, gd, bd, high = 0;
	rd = endColor.Red() - startColor.Red();
	gd = endColor.Green() - startColor.Green();
	bd = endColor.Blue() - startColor.Blue();

	/// Save the current pen and brush
	wxPen savedPen = dc.GetPen();
	wxBrush savedBrush = dc.GetBrush();

	if ( vertical )
		high = rect.GetHeight()-1;
	else
		high = rect.GetWidth()-1;

	if ( high < 1 )
		return;

	for (int i = 0; i <= high; ++i) {
		int r = startColor.Red() +  ((i*rd*100)/high)/100;
		int g = startColor.Green() + ((i*gd*100)/high)/100;
		int b = startColor.Blue() + ((i*bd*100)/high)/100;

		wxPen p(wxColor(r, g, b));
		dc.SetPen(p);

		if ( vertical )
			dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
		else
			dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
	}

	/// Restore the pen and brush
	dc.SetPen( savedPen );
	dc.SetBrush( savedBrush );
}

void DrawingUtils::DrawVerticalButton(wxDC& dc,
                                      const wxRect& rect,
                                      const bool &focus,
                                      const bool &leftTabs,
                                      bool vertical,
                                      bool hover  )
{
	wxColour lightGray = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), GetDdkShadowLightFactor());

	// Define the rounded rectangle base on the given rect
	// we need an array of 9 points for it
	wxColour topStartColor(wxT("WHITE"));
	wxColour topEndColor(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	// Define the middle points
	if ( focus ) {
		PaintStraightGradientBox(dc, rect, topStartColor, topEndColor, vertical);
	} else {
		wxRect r1;
		wxRect r2;

		topStartColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
		topEndColor = lightGray;

		if (leftTabs) {
			r1 = wxRect(rect.x, rect.y, rect.width, rect.height/4);
			r2 = wxRect(rect.x, rect.y+rect.height/4, rect.width, (rect.height*3)/4);
			PaintStraightGradientBox(dc, r1, topEndColor, topStartColor, vertical);
			PaintStraightGradientBox(dc, r2, topStartColor, topStartColor, vertical);

		} else {
			r1 = wxRect(rect.x, rect.y, rect.width, (rect.height*3)/4);
			r2 = wxRect(rect.x, rect.y+(rect.height*3)/4, rect.width, rect.height/4);
			PaintStraightGradientBox(dc, r1, topStartColor, topStartColor, vertical);
			PaintStraightGradientBox(dc, r2, topStartColor, topEndColor, vertical);
		}

	}

	dc.SetBrush( *wxTRANSPARENT_BRUSH );
}

void DrawingUtils::DrawHorizontalButton(wxDC& dc, const wxRect& rect, const bool &focus, const bool &upperTabs, bool vertical, bool hover)
{
	wxColour lightGray = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), GetDdkShadowLightFactor());
	wxColour topStartColor(wxT("WHITE"));
	wxColour topEndColor(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	// Define the middle points
	if ( focus ) {
		if (upperTabs) {
			PaintStraightGradientBox(dc, rect, topStartColor, topEndColor, vertical);
		} else {
			PaintStraightGradientBox(dc, rect, topEndColor, topStartColor, vertical);
		}
	} else {

		topStartColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
		topEndColor = lightGray;

		wxRect r1;
		wxRect r2;

		if (upperTabs) {
			r1 = wxRect(rect.x, rect.y, rect.width, rect.height/4);
			r2 = wxRect(rect.x, rect.y+rect.height/4, rect.width, (rect.height*3)/4);
			PaintStraightGradientBox(dc, r1, topEndColor, topStartColor, vertical);
			PaintStraightGradientBox(dc, r2, topStartColor, topStartColor, vertical);

		} else {
			r1 = wxRect(rect.x, rect.y, rect.width, (rect.height*3)/4);
			r2 = wxRect(rect.x, rect.y+(rect.height*3)/4, rect.width, rect.height/4);
			PaintStraightGradientBox(dc, r1, topStartColor, topStartColor, vertical);
			PaintStraightGradientBox(dc, r2, topStartColor, topEndColor, vertical);
		}
	}

	dc.SetBrush( *wxTRANSPARENT_BRUSH );
}

bool DrawingUtils::IsDark(const wxColour& color)
{
	int evg = (color.Red() + color.Green() + color.Blue())/3;
	if (evg < 127)
		return true;

	return false;
}

float DrawingUtils::GetDdkShadowLightFactor()
{
#if defined (__WXGTK__)
	return 10.0;
#else
	return 8.0;
#endif
}

float DrawingUtils::GetDdkShadowLightFactor2()
{
#if defined (__WXGTK__)
	return 5.0;
#else
	return 3.0;
#endif
}
