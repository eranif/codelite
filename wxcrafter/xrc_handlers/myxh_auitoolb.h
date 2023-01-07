/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/xh_toolb.h
// Purpose:     XML resource handler for wxAuiToolBar
// Author:      Vaclav Slavik
// Created:     2000/08/11
// RCS-ID:      $Id: xh_toolb.h 60522 2009-05-05 18:23:04Z VS $
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MY_WX_XH_AUITOOLB_H_
#define _MY_WX_XH_AUITOOLB_H_
#include <wx/bitmap.h>
#include "wx/aui/auibar.h"
#include "wx/menu.h"
#include <wx/xrc/xmlres.h>

class wxAuiToolBar;

class MyWxAuiToolBarXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxAuiToolBarXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

private:
    bool m_isInside;
    wxAuiToolBar* m_toolbar;
    wxSize m_toolSize;

    class MenuHandler : public wxEvtHandler // From the official wx3.1 handler
    {
    public:
        void OnDropDown(wxAuiToolBarEvent& event);
        unsigned RegisterMenu(wxAuiToolBar* toobar, int id, wxMenu* menu);

    private:
        wxVector<wxMenu*> m_menus;
    };

    MenuHandler m_menuHandler;
};

#endif // _WX_XH_TOOLB_H_
