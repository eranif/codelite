//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_page.h
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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __build_page__
#define __build_page__

#include <wx/wx.h>

#include "filepicker.h"
#include "wx/panel.h"
#include <wx/choicebk.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class BuildPage
///////////////////////////////////////////////////////////////////////////////
class BuildPage : public wxPanel
{
private:
protected:
    wxStaticText* m_staticText;
    wxChoicebook* m_bookBuildSystems;
    wxCheckBox* m_fixOnStartup;
    wxCheckBox* m_generateAsteriskCleanTarget;

    void CustomInit();
    wxPanel* CreateBuildSystemPage(const wxString& name);

public:
    BuildPage(wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize(500, 300),
              int style = wxTAB_TRAVERSAL);
    void Save();
};

class BuildSystemPage : public wxPanel
{
    wxStaticText* m_staticText17;
    FilePicker* m_filePicker;
    wxStaticText* m_staticText18;
    wxString m_name;
    wxStaticText* m_staticText19;

public:
    BuildSystemPage(wxWindow* parent, wxString name);
    virtual ~BuildSystemPage() {}

    void Save();
    void SetSelected();
};
#endif //__build_page__
