//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : advanced_settings.h
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

#ifndef __advanced_settings__
#define __advanced_settings__

#include "CompilerMainPage.h"
#include "advance_settings_base.h"
#include "compiler.h"

#include <CompilersDetectorManager.h>
#include <ICompilerLocator.h>
#include <map>
#include <vector>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
#include <wx/wx.h>

///////////////////////////////////////////////////////////////////////////
class BuildTabSetting;

///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedDlg
///////////////////////////////////////////////////////////////////////////////

class ICompilerSubPage
{
public:
    ICompilerSubPage() {}
    virtual ~ICompilerSubPage() {}
    virtual void Save(CompilerPtr cmp) = 0;
};

class BuildSettingsDialog : public AdvancedDlgBase
{
    std::map<wxString, std::vector<ICompilerSubPage*>> m_compilerPagesMap;

protected:
    virtual void OnApply(wxCommandEvent& event);
    virtual void OnApplyUI(wxUpdateUIEvent& event);

    wxPanel* m_compilersMainPanel;
    CompilerMainPage* m_compilersPage;

    wxStaticText* m_staticText1;
    wxStaticLine* m_staticline2;
    wxStaticLine* m_staticline10;
    BuildTabSetting* m_buildSettings;
    wxMenu* m_rightclickMenu;
    CompilersDetectorManager m_compilersDetector;

public:
    void OnButtonNewClicked();
    void OnScanAndSuggestCompilers();
    void OnAddExistingCompiler();
    void OnButtonOKClicked(wxCommandEvent&);
    void OnContextMenu(wxContextMenuEvent& e);

    void LoadCompilers();
    bool CreateNewCompiler(const wxString& name, const wxString& copyFrom);
    bool DeleteCompiler(const wxString& name);
    void OnCompilersDetected(const ICompilerLocator::CompilerVec_t& compilers);

public:
    BuildSettingsDialog(wxWindow* parent, size_t selected_page, int id = wxID_ANY, wxString title = _("Build Settings"),
                        wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize,
                        int style = wxDEFAULT_DIALOG_STYLE);
    ~BuildSettingsDialog();
    void OnAutoDetectCompilers(wxButton* btn);
};

#endif //__advanced_settings__
