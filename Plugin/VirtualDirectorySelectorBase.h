//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: VirtualDirectorySelectorBase.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_PLUGIN_VIRTUALDIRECTORYSELECTORBASE_BASE_CLASSES_H
#define _CODELITE_PLUGIN_VIRTUALDIRECTORYSELECTORBASE_BASE_CLASSES_H

#include "clThemedTreeCtrl.h"

#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/xrc/xmlres.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
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

#include "codelite_exports.h"

class WXDLLIMPEXP_SDK VirtualDirectorySelectorDlgBaseClass : public wxDialog
{
protected:
    wxStaticText* m_staticText1;
    clThemedTreeCtrl* m_treeCtrl;
    wxStaticText* m_staticTextPreview;
    wxButton* m_buttonOk;
    wxButton* m_buttonCancel;
    wxButton* m_button1;

protected:
    virtual void OnItemSelected(wxTreeEvent& event) { event.Skip(); }
    virtual void OnButtonOK(wxCommandEvent& event) { event.Skip(); }
    virtual void OnButtonOkUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnButtonCancel(wxCommandEvent& event) { event.Skip(); }
    virtual void OnNewVD(wxCommandEvent& event) { event.Skip(); }
    virtual void OnNewVDUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText1() { return m_staticText1; }
    clThemedTreeCtrl* GetTreeCtrl() { return m_treeCtrl; }
    wxStaticText* GetStaticTextPreview() { return m_staticTextPreview; }
    wxButton* GetButtonOk() { return m_buttonOk; }
    wxButton* GetButtonCancel() { return m_buttonCancel; }
    wxButton* GetButton1() { return m_button1; }
    VirtualDirectorySelectorDlgBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY,
                                         const wxString& title = _("Virtual Directory Selector"),
                                         const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                                         long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~VirtualDirectorySelectorDlgBaseClass();
};

#endif
