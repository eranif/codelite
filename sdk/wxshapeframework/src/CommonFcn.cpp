/***************************************************************
 * Name:      CommonFcn.cpp
 * Purpose:   Implements set of global (auxiliary) functions
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <math.h>

#include <wx/tokenzr.h>

#include "wx/wxsf/CommonFcn.h"

namespace wxSFCommonFcn
{
	//----------------------------------------------------------------------------------//
	// conversion functions
	//----------------------------------------------------------------------------------//

	wxPoint Conv2Point(const wxRealPoint& pt)
	{
		return wxPoint((int)pt.x, (int)pt.y);
	}

	wxSize Conv2Size(const wxRealPoint& pt)
	{
		return wxSize((int)pt.x, (int)pt.y);
	}

	wxRealPoint Conv2RealPoint(const wxPoint& pt)
	{
		return wxRealPoint((double)pt.x, (double)pt.y);
	}

	//----------------------------------------------------------------------------------//
	// graphical functions
	//----------------------------------------------------------------------------------//

	wxColour GetHybridColour(const wxColour &orig, const wxColour &modificator)
	{
		int r, g, b;

		r = orig.Red() - (255 - modificator.Red())/20;
		g = orig.Green() - (255 - modificator.Green())/20;
		b = orig.Blue() - (255 - modificator.Blue())/20;

		if(r < 0) r = 0;
		if(g < 0) g = 0;
		if(b < 0) b = 0;

		return wxColour(r, g, b);
	}

	bool LinesIntersection(const wxRealPoint& from1, const wxRealPoint& to1, const wxRealPoint& from2, const wxRealPoint& to2, wxRealPoint& i)
	{
		double a1, b1, c1, a2, b2, c2, ka, kb;

		// bug in GCC ???
		volatile double xi, yi;

	   // create line 1 info
		a1 = to1.y - from1.y;
		b1 = from1.x - to1.x;
		c1 = -a1*from1.x - b1*from1.y;

		// create line 2 info
		a2 = to2.y - from2.y;
		b2 = from2.x - to2.x;
		c2 = -a2*from2.x - b2*from2.y;

		// check, whether the lines are parallel...
		ka = a1 / a2;
		kb = b1 / b2;

		if(ka == kb) return false;

		// find intersection point
		#ifdef __WXMSW__
		xi = floor(((b1*c2 - c1*b2) / (a1*b2 - a2*b1)) + 0.5);
		yi = floor((-(a1*c2 - a2*c1) / (a1*b2 - a2*b1)) + 0.5);
		#else
		xi = (b1*c2 - c1*b2) / (a1*b2 - a2*b1);
		yi = -(a1*c2 - a2*c1) / (a1*b2 - a2*b1);
		#endif

		if( ((from1.x - xi)*(xi - to1.x) >= 0) &&
			((from2.x - xi)*(xi - to2.x) >= 0) &&
			((from1.y - yi)*(yi - to1.y) >= 0) &&
			((from2.y - yi)*(yi - to2.y) >= 0) )
			{
				i.x = xi;
				i.y = yi;
				return true;
			}
		else
			return false;
	}

	double Distance(const wxRealPoint& pt1, const wxRealPoint& pt2)
	{
		return sqrt((pt2.x - pt1.x)*(pt2.x - pt1.x) + (pt2.y - pt1.y)*(pt2.y - pt1.y));
	}
}

namespace wxSF
{
	const double PI = 3.14159265;
}
