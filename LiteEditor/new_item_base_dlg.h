//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: new_item_dlg.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_FORMBUILDER_NEW_ITEM_DLG_BASE_CLASSES_H
#define _CODELITE_FORMBUILDER_NEW_ITEM_DLG_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/statline.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

// clang-format on

class NewItemBaseDlg : public wxDialog
{
protected:
    wxStaticText* m_staticText6;
    wxTextCtrl* m_fileName;
    wxStaticText* m_staticText3;
    wxTextCtrl* m_location;
    wxButton* m_browseBtn;
    wxStaticLine* m_staticline1;
    wxStdDialogButtonSizer* m_stdBtnSizer2;
    wxButton* m_button4;
    wxButton* m_button6;

protected:
    virtual void OnBrowseButton(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOKUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText6() { return m_staticText6; }
    wxTextCtrl* GetFileName() { return m_fileName; }
    wxStaticText* GetStaticText3() { return m_staticText3; }
    wxTextCtrl* GetLocation() { return m_location; }
    wxButton* GetBrowseBtn() { return m_browseBtn; }
    wxStaticLine* GetStaticline1() { return m_staticline1; }
    NewItemBaseDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add New File"),
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                   long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~NewItemBaseDlg();
};

#endif
