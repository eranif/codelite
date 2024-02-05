#include "clEditorEditEventsHandler.h"

#include "event_notifier.h"

#include <wx/app.h>
#include <wx/combobox.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/textentry.h>

clEditEventsHandler::clEditEventsHandler(wxStyledTextCtrl* wnd, const wxString& name)
    : m_stc(wnd)
    , m_textCtrl(nullptr)
    , m_combo(nullptr)
    , m_noUnbind(false)
    , m_name(name)
{
    DoInitialize();
}

clEditEventsHandler::clEditEventsHandler(wxTextCtrl* wnd, const wxString& name)
    : m_stc(nullptr)
    , m_textCtrl(wnd)
    , m_combo(nullptr)
    , m_noUnbind(false)
    , m_name(name)
{
    DoInitialize();
}

clEditEventsHandler::clEditEventsHandler(wxComboBox* wnd, const wxString& name)
    : m_stc(nullptr)
    , m_textCtrl(nullptr)
    , m_combo(wnd)
    , m_noUnbind(false)
    , m_name(name)
{
    DoInitialize();
}

clEditEventsHandler::~clEditEventsHandler()
{
    if(!m_noUnbind && (m_stc || m_textCtrl || m_combo)) {
        wxTheApp->Unbind(wxEVT_MENU, &clEditEventsHandler::OnCopy, this, wxID_COPY);
        wxTheApp->Unbind(wxEVT_MENU, &clEditEventsHandler::OnPaste, this, wxID_PASTE);
        wxTheApp->Unbind(wxEVT_MENU, &clEditEventsHandler::OnCut, this, wxID_CUT);
        wxTheApp->Unbind(wxEVT_MENU, &clEditEventsHandler::OnSelectAll, this, wxID_SELECTALL);
        wxTheApp->Unbind(wxEVT_MENU, &clEditEventsHandler::OnUndo, this, wxID_UNDO);
        wxTheApp->Unbind(wxEVT_MENU, &clEditEventsHandler::OnRedo, this, wxID_REDO);
    }
}

void clEditEventsHandler::OnCopy(wxCommandEvent& event)
{
    CHECK_FOCUS_WINDOW();
    CALL_FUNC(Copy);
}

void clEditEventsHandler::OnPaste(wxCommandEvent& event)
{
    CHECK_FOCUS_WINDOW();
    CALL_FUNC(Paste);
}

void clEditEventsHandler::OnCut(wxCommandEvent& event)
{
    CHECK_FOCUS_WINDOW();
    CALL_FUNC(Cut);
}

void clEditEventsHandler::OnSelectAll(wxCommandEvent& event)
{
    CHECK_FOCUS_WINDOW();
    CALL_FUNC(SelectAll);
}

void clEditEventsHandler::OnUndo(wxCommandEvent& event)
{
    CHECK_FOCUS_WINDOW();
    CALL_FUNC(Undo);
}

void clEditEventsHandler::OnRedo(wxCommandEvent& event)
{
    CHECK_FOCUS_WINDOW();
    CALL_FUNC(Redo);
}

void clEditEventsHandler::DoInitialize()
{
    if(m_textCtrl || m_stc || m_combo) {
        wxTheApp->Bind(wxEVT_MENU, &clEditEventsHandler::OnCopy, this, wxID_COPY);
        wxTheApp->Bind(wxEVT_MENU, &clEditEventsHandler::OnPaste, this, wxID_PASTE);
        wxTheApp->Bind(wxEVT_MENU, &clEditEventsHandler::OnCut, this, wxID_CUT);
        wxTheApp->Bind(wxEVT_MENU, &clEditEventsHandler::OnSelectAll, this, wxID_SELECTALL);
        wxTheApp->Bind(wxEVT_MENU, &clEditEventsHandler::OnUndo, this, wxID_UNDO);
        wxTheApp->Bind(wxEVT_MENU, &clEditEventsHandler::OnRedo, this, wxID_REDO);
    }
}
