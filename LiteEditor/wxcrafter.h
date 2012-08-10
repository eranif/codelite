#ifndef WXCRAFTER_H
#define WXCRAFTER_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/button.h>

class NewProjectDlgBaseClass : public wxDialog
{
protected:
    wxSplitterWindow* m_splitter5;
    wxPanel* m_splitterPageRight;
    wxStaticText* m_staticText34;
    wxChoice* m_chCategories;
    wxListCtrl* m_listTemplates;
    wxPanel* m_splitterPageLeft;
    wxStaticText* m_staticText16;
    wxTextCtrl* m_txtProjName;
    wxStaticText* m_staticText18;
    wxTextCtrl* m_textCtrlProjectPath;
    wxBitmapButton* m_bmpButton33;
    wxStaticText* m_staticText21;
    wxChoice* m_chCompiler;
    wxCheckBox* m_cbSeparateDir;
    wxStaticText* m_stxtFullFileName;
    wxTextCtrl* m_txtDescription;
    wxButton* m_button3;
    wxButton* m_button4;

protected:
    virtual void OnCategorySelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnTemplateSelected(wxListEvent& event) { event.Skip(); }
    virtual void OnProjectNameChanged(wxCommandEvent& event) { event.Skip(); }
    virtual void OnProjectPathUpdated(wxCommandEvent& event) { event.Skip(); }
    virtual void OnBrowseProjectPath(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOKUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    NewProjectDlgBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Project"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~NewProjectDlgBaseClass();
};

#endif
