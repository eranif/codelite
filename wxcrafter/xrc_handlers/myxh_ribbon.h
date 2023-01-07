/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_ribbon.h
// Purpose:     XML resource handler for wxRibbon related classes
// Author:      Armel Asselin
// Created:     2010-04-23
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Armel Asselin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XRC_XH_RIBBON_H_
#define _WX_XRC_XH_RIBBON_H_

#include "wx/xrc/xmlres.h"

class wxRibbonControl;

class MyWxRibbonXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxRibbonXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

private:
    const wxClassInfo* m_isInside;

    bool IsRibbonControl(wxXmlNode* node);

    wxObject* Handle_buttonbar();
    wxObject* Handle_toolbar();
    wxObject* Handle_button();
    wxObject* Handle_tool();
    wxObject* Handle_control();
    wxObject* Handle_page();
    wxObject* Handle_gallery();
    wxObject* Handle_galleryitem();
    wxObject* Handle_panel();
    wxObject* Handle_bar();

    void Handle_RibbonArtProvider(wxRibbonControl* control);

    wxDECLARE_DYNAMIC_CLASS(MyWxRibbonXmlHandler);
};

#endif // wxUSE_XRC && wxUSE_RIBBON
