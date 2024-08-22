//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdView.h
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

#ifndef ERDVIEW_H
#define ERDVIEW_H

#include "view.h"

#include <wx/wx.h>
#include <wx/wxsf/DiagramManager.h>
#include <wx/wxsf/FlexGridShape.h>
#include <wx/wxsf/RoundRectShape.h> // Base class: wxSFRoundRectShape
#include <wx/wxsf/TextShape.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

/*! \brief Class representing graphical wiev in ERD diagram, for more info see wxSF manual. */
class ErdView : public wxSFRoundRectShape {

public:
	XS_DECLARE_CLONABLE_CLASS(ErdView);
	ErdView();
	ErdView(const ErdView& obj);
	ErdView(View* pView);
	virtual ~ErdView();
	
	View* GetView() { return (View*) this->GetUserData(); }
	
	void UpdateView();
	
protected:
	void Initialize();
	
	virtual void DrawHighlighted(wxDC& dc);
	virtual void DrawHover(wxDC& dc);
	virtual void DrawNormal(wxDC& dc);
	
	void DrawDetails(wxDC& dc);
	
	wxSFTextShape *m_pLabel;
	wxSFTextShape *m_pSelect;
	wxSFFlexGridShape* m_pGrid;
};

#endif // ERDVIEW_H
