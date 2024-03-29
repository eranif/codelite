//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: RemotyUI.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_REMOTY_REMOTYUI_BASE_CLASSES_H
#define _CODELITE_REMOTY_REMOTYUI_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include "clChoice.h"
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/textctrl.h>
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

class RemotyWorkspaceViewBase : public wxPanel
{
protected:
protected:
public:
    RemotyWorkspaceViewBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL);
    virtual ~RemotyWorkspaceViewBase();
};

class RemotySwitchToWorkspaceDlgBase : public wxDialog
{
protected:
    wxStaticText* m_staticText77;
    wxChoice* m_choiceWorkspaceType;
    wxStaticText* m_staticText75;
    wxComboBox* m_comboBoxPath;
    wxButton* m_button29;
    wxStaticText* m_staticText81;
    wxChoice* m_choiceAccount;
    wxStdDialogButtonSizer* m_stdBtnSizer9;
    wxButton* m_button11;
    wxButton* m_button13;

protected:
    virtual void OnChoiceWorkspaceType(wxCommandEvent& event) { event.Skip(); }
    virtual void OnPathChanged(wxCommandEvent& event) { event.Skip(); }
    virtual void OnBrowse(wxCommandEvent& event) { event.Skip(); }
    virtual void OnRemoteUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnOKUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText77() { return m_staticText77; }
    wxChoice* GetChoiceWorkspaceType() { return m_choiceWorkspaceType; }
    wxStaticText* GetStaticText75() { return m_staticText75; }
    wxComboBox* GetComboBoxPath() { return m_comboBoxPath; }
    wxButton* GetButton29() { return m_button29; }
    wxStaticText* GetStaticText81() { return m_staticText81; }
    wxChoice* GetChoiceAccount() { return m_choiceAccount; }
    RemotySwitchToWorkspaceDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                                   const wxString& title = _("Switch to workspace..."),
                                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                                   long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~RemotySwitchToWorkspaceDlgBase();
};

class RemotyNewWorkspaceDlgBase : public wxDialog
{
protected:
    wxStaticText* m_staticText45;
    wxTextCtrl* m_textCtrlPath;
    wxButton* m_button49;
    wxStaticText* m_staticText59;
    wxTextCtrl* m_textCtrlName;
    wxStdDialogButtonSizer* m_stdBtnSizer37;
    wxButton* m_button39;
    wxButton* m_button41;

protected:
    virtual void OnBrowse(wxCommandEvent& event) { event.Skip(); }
    virtual void OnOKUI(wxUpdateUIEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText45() { return m_staticText45; }
    wxTextCtrl* GetTextCtrlPath() { return m_textCtrlPath; }
    wxButton* GetButton49() { return m_button49; }
    wxStaticText* GetStaticText59() { return m_staticText59; }
    wxTextCtrl* GetTextCtrlName() { return m_textCtrlName; }
    RemotyNewWorkspaceDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                              const wxString& title = _("New File System Workspace (remote)"),
                              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                              long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~RemotyNewWorkspaceDlgBase();
};

#endif
