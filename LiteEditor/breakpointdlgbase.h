//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: breakpointdlg.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CODELITE_LITEEDITOR_BREAKPOINTDLG_BASE_CLASSES_H
#define _CODELITE_LITEEDITOR_BREAKPOINTDLG_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/dataview.h>
#include "breakpointslistctrl.h"
#include <wx/button.h>
#include "clThemedButton.h"
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/stattext.h>
#include <wx/choicebk.h>
#include <wx/imaglist.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
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

class BreakpointTabBase : public wxPanel
{
protected:
    BreakpointsListctrl* m_dvListCtrlBreakpoints;
    clThemedButton* m_buttonAdd;
    clThemedButton* m_buttonEdit;
    clThemedButton* m_buttonDelete;
    clThemedButton* m_buttonDeleteAll;
    clThemedButton* m_buttonApplyPending;

protected:
    virtual void OnContextMenu(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnBreakpointActivated(wxDataViewEvent& event) { event.Skip(); }
    virtual void OnAdd(wxCommandEvent& event) { event.Skip(); }
    virtual void OnEdit(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDelete(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDeleteAll(wxCommandEvent& event) { event.Skip(); }
    virtual void OnApplyPending(wxCommandEvent& event) { event.Skip(); }

public:
    BreakpointsListctrl* GetDvListCtrlBreakpoints() { return m_dvListCtrlBreakpoints; }
    clThemedButton* GetButtonAdd() { return m_buttonAdd; }
    clThemedButton* GetButtonEdit() { return m_buttonEdit; }
    clThemedButton* GetButtonDelete() { return m_buttonDelete; }
    clThemedButton* GetButtonDeleteAll() { return m_buttonDeleteAll; }
    clThemedButton* GetButtonApplyPending() { return m_buttonApplyPending; }
    BreakpointTabBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    virtual ~BreakpointTabBase();
};

class BreakpointPropertiesDlgBase : public wxDialog
{
protected:
    wxStaticText* m_staticText9;
    wxChoicebook* m_choicebook;
    wxPanel* m_panel1;
    wxStaticText* m_staticText51;
    wxCheckBox* m_checkLineno;
    wxTextCtrl* m_textLineno;
    wxCheckBox* m_checkBreakFunction;
    wxTextCtrl* m_textFunctionname;
    wxCheckBox* m_checkRegex;
    wxCheckBox* m_checkBreakMemory;
    wxTextCtrl* m_textBreakMemory;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textFilename;
    wxButton* m_buttonBrowse;
    wxStaticLine* m_staticline5;
    wxPanel* m_panel2;
    wxRadioBox* m_radioWatchtype;
    wxStaticText* m_staticText5;
    wxTextCtrl* m_textWatchExpression;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCond;
    wxStaticText* m_staticText11;
    wxTextCtrl* m_textCommands;
    wxCheckBox* m_checkDisable;
    wxCheckBox* m_checkTemp;
    wxStaticText* m_staticText7;
    wxSpinCtrl* m_spinIgnore;
    wxButton* m_button6;
    wxButton* m_button7;

protected:
    virtual void OnPageChanged(wxChoicebookEvent& event) { event.Skip(); }
    virtual void OnPageChanging(wxChoicebookEvent& event) { event.Skip(); }
    virtual void OnCheckBreakLineno(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCheckBreakLinenoUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnCheckBreakFunction(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCheckBreakFunctionUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnCheckBreakMemory(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCheckBreakMemoryUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnBrowse(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText9() { return m_staticText9; }
    wxStaticText* GetStaticText51() { return m_staticText51; }
    wxCheckBox* GetCheckLineno() { return m_checkLineno; }
    wxTextCtrl* GetTextLineno() { return m_textLineno; }
    wxCheckBox* GetCheckBreakFunction() { return m_checkBreakFunction; }
    wxTextCtrl* GetTextFunctionname() { return m_textFunctionname; }
    wxCheckBox* GetCheckRegex() { return m_checkRegex; }
    wxCheckBox* GetCheckBreakMemory() { return m_checkBreakMemory; }
    wxTextCtrl* GetTextBreakMemory() { return m_textBreakMemory; }
    wxStaticText* GetStaticText6() { return m_staticText6; }
    wxTextCtrl* GetTextFilename() { return m_textFilename; }
    wxButton* GetButtonBrowse() { return m_buttonBrowse; }
    wxStaticLine* GetStaticline5() { return m_staticline5; }
    wxPanel* GetPanel1() { return m_panel1; }
    wxRadioBox* GetRadioWatchtype() { return m_radioWatchtype; }
    wxStaticText* GetStaticText5() { return m_staticText5; }
    wxTextCtrl* GetTextWatchExpression() { return m_textWatchExpression; }
    wxPanel* GetPanel2() { return m_panel2; }
    wxChoicebook* GetChoicebook() { return m_choicebook; }
    wxStaticText* GetStaticText1() { return m_staticText1; }
    wxTextCtrl* GetTextCond() { return m_textCond; }
    wxStaticText* GetStaticText11() { return m_staticText11; }
    wxTextCtrl* GetTextCommands() { return m_textCommands; }
    wxCheckBox* GetCheckDisable() { return m_checkDisable; }
    wxCheckBox* GetCheckTemp() { return m_checkTemp; }
    wxStaticText* GetStaticText7() { return m_staticText7; }
    wxSpinCtrl* GetSpinIgnore() { return m_spinIgnore; }
    wxButton* GetButton6() { return m_button6; }
    wxButton* GetButton7() { return m_button7; }
    BreakpointPropertiesDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                                const wxString& title = _("Breakpoint and Watchpoint Properties"),
                                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~BreakpointPropertiesDlgBase();
};

#endif
