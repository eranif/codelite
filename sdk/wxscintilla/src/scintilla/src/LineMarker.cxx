// Scintilla source code edit control
/** @file LineMarker.cxx
 ** Defines the look of a line marker in the margin .
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>

#include "Platform.h"

#include "Scintilla.h"
#include "XPM.h"
#include "LineMarker.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

//////////////////////////////////////////////////
// Colour methods to convert HSL <-> RGB
//////////////////////////////////////////////////
float cl_min(float x, float y, float z)
{
	float m = x < y ? x : y;
	m = m < z ? m : z;
	return m;
}

float cl_max(float x, float y, float z)
{
	float m = x > y ? x : y;
	m = m > z ? m : z;
	return m;
}

void RGB_2_HSL(float r, float g, float b, float *h, float *s, float *l)
{
	float var_R = ( r / 255.0 );                     //RGB from 0 to 255
	float var_G = ( g / 255.0 );
	float var_B = ( b / 255.0 );

	float var_Min = cl_min( var_R, var_G, var_B );    //Min. value of RGB
	float var_Max = cl_max( var_R, var_G, var_B );    //Max. value of RGB
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

float Hue_2_RGB( float v1, float v2, float vH )             //Function Hue_2_RGB
{
	if ( vH < 0 ) vH += 1;
	if ( vH > 1 ) vH -= 1;
	if ( ( 6.0 * vH ) < 1 ) return ( v1 + ( v2 - v1 ) * 6.0 * vH );
	if ( ( 2.0 * vH ) < 1 ) return ( v2 );
	if ( ( 3.0 * vH ) < 2 ) return ( v1 + ( v2 - v1 ) * ( ( 2.0 / 3.0 ) - vH ) * 6.0 );
	return ( v1 );
}

void HSL_2_RGB(float h, float s, float l, float *r, float *g, float *b)
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

long darkColour(long color, float level)
{
	float r, g, b;
	ColourDesired col(color);

	// convert the HSL
	float h, s, l;
	RGB_2_HSL((float)col.GetRed(), (float)col.GetGreen(), (float)col.GetBlue(), &h, &s, &l);

	// reduce the Lum value
	l -= (float)((level * 5.0)/100.0);
	if (l < 0) l = 0.0;

	// convert back from HSL to RGB
	HSL_2_RGB(h, s, l, &r, &g, &b);

	return ColourDesired((unsigned int)r, (unsigned int)g, (unsigned int)b).AsLong();
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

void LineMarker::RefreshColourPalette(Palette &pal, bool want) {
	pal.WantFind(fore, want);
	pal.WantFind(back, want);
	if (pxpm) {
		pxpm->RefreshColourPalette(pal, want);
	}
}

void LineMarker::SetXPM(const char *textForm) {
	delete pxpm;
	pxpm = new XPM(textForm);
	markType = SC_MARK_PIXMAP;
}

void LineMarker::SetXPM(const char * const *linesForm) {
	delete pxpm;
	pxpm = new XPM(linesForm);
	markType = SC_MARK_PIXMAP;
}

static void DrawBox(Surface *surface, int centreX, int centreY, int armSize, ColourAllocated fore, ColourAllocated back) {
	PRectangle rc;
	rc.left = centreX - armSize;
	rc.top = centreY - armSize;
	rc.right = centreX + armSize + 1;
	rc.bottom = centreY + armSize + 1;
	surface->RectangleDraw(rc, back, fore);
}

static void DrawCircle(Surface *surface, int centreX, int centreY, int armSize, ColourAllocated fore, ColourAllocated back) {
	PRectangle rcCircle;
	rcCircle.left = centreX - armSize;
	rcCircle.top = centreY - armSize;
	rcCircle.right = centreX + armSize + 1;
	rcCircle.bottom = centreY + armSize + 1;
	surface->Ellipse(rcCircle, back, fore);
}

static void DrawPlus(Surface *surface, int centreX, int centreY, int armSize, ColourAllocated fore) {
	PRectangle rcV(centreX, centreY - armSize + 2, centreX + 1, centreY + armSize - 2 + 1);
	surface->FillRectangle(rcV, fore);
	PRectangle rcH(centreX - armSize + 2, centreY, centreX + armSize - 2 + 1, centreY+1);
	surface->FillRectangle(rcH, fore);
}

static void DrawMinus(Surface *surface, int centreX, int centreY, int armSize, ColourAllocated fore) {
	PRectangle rcH(centreX - armSize + 2, centreY, centreX + armSize - 2 + 1, centreY+1);
	surface->FillRectangle(rcH, fore);
}

void LineMarker::Draw(Surface *surface, PRectangle &rcWhole, Font &fontForCharacter, int foldLevel) {
	if ((markType == SC_MARK_PIXMAP) && (pxpm)) {
		pxpm->Draw(surface, rcWhole);
		return;
	}
	// Restrict most shapes a bit
	PRectangle rc = rcWhole;
	rc.top++;
	rc.bottom--;
	int minDim = Platform::Minimum(rc.Width(), rc.Height());
	minDim--;	// Ensure does not go beyond edge
	int centreX = (rc.right + rc.left) / 2;
	int centreY = (rc.bottom + rc.top) / 2;
	int dimOn2 = minDim / 2;
	int dimOn4 = minDim / 4;
	int blobSize = dimOn2-1;
	int armSize = dimOn2-2;
	if (rc.Width() > (rc.Height() * 2)) {
		// Wide column is line number so move to left to try to avoid overlapping number
		centreX = rc.left + dimOn2 + 1;
	}
	if (markType == SC_MARK_ROUNDRECT) {
		PRectangle rcRounded = rc;
		rcRounded.left = rc.left + 1;
		rcRounded.right = rc.right - 1;
		surface->RoundedRectangle(rcRounded, fore.allocated, back.allocated);
	} else if (markType == SC_MARK_CIRCLE) {
		PRectangle rcCircle;
		rcCircle.left = centreX - dimOn2;
		rcCircle.top = centreY - dimOn2;
		rcCircle.right = centreX + dimOn2;
		rcCircle.bottom = centreY + dimOn2;
		surface->Ellipse(rcCircle, fore.allocated, back.allocated);
	} else if (markType == SC_MARK_ARROW) {
		Point pts[] = {
    		Point(centreX - dimOn4, centreY - dimOn2),
    		Point(centreX - dimOn4, centreY + dimOn2),
    		Point(centreX + dimOn2 - dimOn4, centreY),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_ARROW_IN_BOX) {
		Point pts[] = {
    		Point(centreX - dimOn4, centreY - dimOn2),
    		Point(centreX - dimOn4, centreY + dimOn2),
    		Point(centreX + dimOn2 - dimOn4, centreY),
		};

		if(foldLevel == 0){
			long c = darkColour(back.allocated.AsLong(), foldLevel+1);
			surface->RoundedRectangle(rcWhole, c, c);
		} else {
			long c = darkColour(back.allocated.AsLong(), foldLevel);
			surface->FillRectangle(rcWhole, c);
		}

		ColourDesired des;
		des.Set(0, 0, 0);
		ColourAllocated alloc(des.AsLong());
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		back.allocated, alloc);


	} else if (markType == SC_MARK_ARROWDOWN) {
		Point pts[] = {
    		Point(centreX - dimOn2, centreY - dimOn4),
    		Point(centreX + dimOn2, centreY - dimOn4),
    		Point(centreX, centreY + dimOn2 - dimOn4),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_ARROWDOWN_IN_BOX) {
		Point pts[] = {
    		Point(centreX - dimOn2, centreY - dimOn4),
    		Point(centreX + dimOn2, centreY - dimOn4),
    		Point(centreX, centreY + dimOn2 - dimOn4),
		};

		ColourDesired des;
		des.Set(0, 0, 0);
		ColourAllocated alloc(des.AsLong());

		if(foldLevel > 0){
			long prev_c = darkColour(back.allocated.AsLong(), foldLevel);
			PRectangle filler = rcWhole;
			filler.bottom = filler.top + 2;
			surface->FillRectangle(filler, prev_c);
		}

		long c = darkColour(back.allocated.AsLong(), foldLevel+1);
		rcWhole.bottom += 2;
		surface->RoundedRectangle(rcWhole, c, c);

		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		back.allocated, alloc);

	} else if (markType == SC_MARK_MINUS_IN_BOX) {
		ColourDesired des;
		des.Set(0, 0, 0);
		ColourAllocated alloc(des.AsLong());

		if(foldLevel > 0){
			long prev_c = darkColour(back.allocated.AsLong(), foldLevel);
			PRectangle filler = rcWhole;
			filler.bottom = filler.top + 2;
			surface->FillRectangle(filler, prev_c);
		}

		long c = darkColour(back.allocated.AsLong(), foldLevel+1);
		rcWhole.bottom += 2;
		surface->RoundedRectangle(rcWhole, c, c);

		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX - armSize, centreY + 1),
		};

		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, alloc);

	} else if (markType == SC_MARK_PLUS_IN_BOX) {
		ColourDesired des;
		des.Set(0, 0, 0);
		ColourAllocated alloc(des.AsLong());

		if(foldLevel > 0){
			long prev_c = darkColour(back.allocated.AsLong(), foldLevel);
			PRectangle filler = rcWhole;
			filler.bottom = filler.top + 2;
			surface->FillRectangle(filler, prev_c);
		}

		long c = darkColour(back.allocated.AsLong(), foldLevel+1);
		rcWhole.bottom += 2;
		surface->RoundedRectangle(rcWhole, c, c);

		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX - 1, centreY - 1),
    		Point(centreX - 1, centreY - armSize),
    		Point(centreX + 1, centreY - armSize),
    		Point(centreX + 1, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX + 1, centreY + 1),
    		Point(centreX + 1, centreY + armSize),
    		Point(centreX - 1, centreY + armSize),
    		Point(centreX - 1, centreY + 1),
    		Point(centreX - armSize, centreY + 1),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, alloc);

	} else if (markType == SC_MARK_PLUS) {
		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX - 1, centreY - 1),
    		Point(centreX - 1, centreY - armSize),
    		Point(centreX + 1, centreY - armSize),
    		Point(centreX + 1, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX + 1, centreY + 1),
    		Point(centreX + 1, centreY + armSize),
    		Point(centreX - 1, centreY + armSize),
    		Point(centreX - 1, centreY + 1),
    		Point(centreX - armSize, centreY + 1),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_MINUS) {
		Point pts[] = {
    		Point(centreX - armSize, centreY - 1),
    		Point(centreX + armSize, centreY -1),
    		Point(centreX + armSize, centreY +1),
    		Point(centreX - armSize, centreY + 1),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
                 		fore.allocated, back.allocated);

	} else if (markType == SC_MARK_SMALLRECT) {
		PRectangle rcSmall;
		rcSmall.left = rc.left + 1;
		rcSmall.top = rc.top + 2;
		rcSmall.right = rc.right - 1;
		rcSmall.bottom = rc.bottom - 2;
		surface->RectangleDraw(rcSmall, fore.allocated, back.allocated);

	} else if (markType == SC_MARK_EMPTY || markType == SC_MARK_BACKGROUND) {
		// An invisible marker so don't draw anything

	} else if (markType == SC_MARK_VLINE) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rcWhole.bottom);

	} else if (markType == SC_MARK_LCORNER) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rc.top + dimOn2);
		surface->LineTo(rc.right - 2, rc.top + dimOn2);

	} else if (markType == SC_MARK_TCORNER) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rcWhole.bottom);
		surface->MoveTo(centreX, rc.top + dimOn2);
		surface->LineTo(rc.right - 2, rc.top + dimOn2);

	} else if (markType == SC_MARK_LCORNERCURVE) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rc.top + dimOn2-3);
		surface->LineTo(centreX+3, rc.top + dimOn2);
		surface->LineTo(rc.right - 1, rc.top + dimOn2);

	} else if (markType == SC_MARK_TCORNERCURVE) {
		surface->PenColour(back.allocated);
		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, rcWhole.bottom);

		surface->MoveTo(centreX, rc.top + dimOn2-3);
		surface->LineTo(centreX+3, rc.top + dimOn2);
		surface->LineTo(rc.right - 1, rc.top + dimOn2);

	} else if (markType == SC_MARK_BOXPLUS) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);

	} else if (markType == SC_MARK_BOXPLUSCONNECTED) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);

		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);

		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);

	} else if (markType == SC_MARK_BOXMINUS) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);

		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);

	} else if (markType == SC_MARK_BOXMINUSCONNECTED) {
		surface->PenColour(back.allocated);
		DrawBox(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);

		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);

		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);

	} else if (markType == SC_MARK_CIRCLEPLUS) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);

	} else if (markType == SC_MARK_CIRCLEPLUSCONNECTED) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawPlus(surface, centreX, centreY, blobSize, back.allocated);

		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);

		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);

	} else if (markType == SC_MARK_CIRCLEMINUS) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);

		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);

	} else if (markType == SC_MARK_CIRCLEMINUSCONNECTED) {
		DrawCircle(surface, centreX, centreY, blobSize, fore.allocated, back.allocated);
		surface->PenColour(back.allocated);
		DrawMinus(surface, centreX, centreY, blobSize, back.allocated);

		surface->MoveTo(centreX, centreY + blobSize);
		surface->LineTo(centreX, rcWhole.bottom);

		surface->MoveTo(centreX, rcWhole.top);
		surface->LineTo(centreX, centreY - blobSize);

	} else if (markType >= SC_MARK_CHARACTER) {
		char character[1];
		character[0] = static_cast<char>(markType - SC_MARK_CHARACTER);
		int width = surface->WidthText(fontForCharacter, character, 1);
		rc.left += (rc.Width() - width) / 2;
		rc.right = rc.left + width;
		surface->DrawTextClipped(rc, fontForCharacter, rc.bottom - 2,
			character, 1, fore.allocated, back.allocated);

	} else if (markType == SC_MARK_DOTDOTDOT) {
		int right = centreX - 6;
		for (int b=0; b<3; b++) {
			PRectangle rcBlob(right, rc.bottom - 4, right + 2, rc.bottom-2);
			surface->FillRectangle(rcBlob, fore.allocated);
			right += 5;
		}
	} else if (markType == SC_MARK_ARROWS) {
		surface->PenColour(fore.allocated);
		int right = centreX - 2;
		for (int b=0; b<3; b++) {
			surface->MoveTo(right - 4, centreY - 4);
			surface->LineTo(right, centreY);
			surface->LineTo(right - 5, centreY + 5);
			right += 4;
		}
	} else if (markType == SC_MARK_SHORTARROW) {
		Point pts[] = {
			Point(centreX, centreY + dimOn2),
			Point(centreX + dimOn2, centreY),
			Point(centreX, centreY - dimOn2),
			Point(centreX, centreY - dimOn4),
			Point(centreX - dimOn4, centreY - dimOn4),
			Point(centreX - dimOn4, centreY + dimOn4),
			Point(centreX, centreY + dimOn4),
			Point(centreX, centreY + dimOn2),
		};
		surface->Polygon(pts, sizeof(pts) / sizeof(pts[0]),
				fore.allocated, back.allocated);
	} else if (markType == SC_MARK_LEFTRECT) {
		PRectangle rcLeft = rcWhole;
		rcLeft.right = rcLeft.left + 4;
		surface->FillRectangle(rcLeft, back.allocated);

	} else if(markType == SC_MARK_FULLRECT_TAIL){
		if(foldLevel > 1){
			long prev_c = darkColour(back.allocated.AsLong(), foldLevel-1);
			PRectangle filler = rcWhole;
			filler.top = filler.bottom - 3;
			surface->FillRectangle(rcWhole, prev_c);
		}

		long c = darkColour(back.allocated.AsLong(), foldLevel);
		surface->RoundedRectangle(rcWhole, c, c);

		PRectangle filler = rcWhole;
		filler.bottom = filler.top + 2;
		surface->FillRectangle(filler, c);

	} else {// SC_MARK_FULLRECT
		surface->FillRectangle(rcWhole, darkColour(back.allocated.AsLong(), foldLevel));
	}
}
