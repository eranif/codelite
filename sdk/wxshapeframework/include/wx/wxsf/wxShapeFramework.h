/***************************************************************
 * Name:      wxShapeFramework.h
 * Purpose:   Main header file
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSF_H_
#define _WXSF_H_

// main library classes
#include <wx/wxsf/DiagramManager.h>
#include <wx/wxsf/ShapeCanvas.h>
#include <wx/wxsf/Thumbnail.h>
#include <wx/wxsf/AutoLayout.h>

// shapes' declarations
#include <wx/wxsf/RectShape.h>
#include <wx/wxsf/RoundRectShape.h>
#include <wx/wxsf/FixedRectShape.h>
#include <wx/wxsf/EllipseShape.h>
#include <wx/wxsf/CircleShape.h>
#include <wx/wxsf/DiamondShape.h>
#include <wx/wxsf/TextShape.h>
#include <wx/wxsf/EditTextShape.h>
#include <wx/wxsf/BitmapShape.h>
#include <wx/wxsf/PolygonShape.h>
#include <wx/wxsf/ControlShape.h>
#include <wx/wxsf/GridShape.h>
#include <wx/wxsf/FlexGridShape.h>

// arrows' declarations
#include <wx/wxsf/OpenArrow.h>
#include <wx/wxsf/SolidArrow.h>
#include <wx/wxsf/DiamondArrow.h>
#include <wx/wxsf/CircleArrow.h>

// connection lines' declarations
#include <wx/wxsf/LineShape.h>
#include <wx/wxsf/CurveShape.h>
#include <wx/wxsf/OrthoShape.h>
#include <wx/wxsf/RoundOrthoShape.h>

// library events
#include <wx/wxsf/SFEvents.h>

// printing support
#include <wx/wxsf/Printout.h>

// common functions
#include <wx/wxsf/CommonFcn.h>

// serialize/deserialize functionality
#include <wx/wxxmlserializer/XmlSerializer.h>

#endif //_WXSF_H_
