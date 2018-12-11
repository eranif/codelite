#include "clEditorEditEventsHandler.h"
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include <wx/textentry.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>

clEditEventsHandler::clEditEventsHandler(wxStyledTextCtrl* wnd)
    : m_stc(wnd)
    , m_textCtrl(nullptr)
    , m_combo(nullptr)
    , m_noUnbind(false)
{
    DoInitialize();
}

clEditEventsHandler::clEditEventsHandler(wxTextCtrl* wnd)
    : m_stc(nullptr)
    , m_textCtrl(wnd)
    , m_combo(nullptr)
    , m_noUnbind(false)
{
    DoInitialize();
}

clEditEventsHandler::clEditEventsHandler(wxComboBox* wnd)
    : m_stc(nullptr)
    , m_textCtrl(nullptr)
    , m_combo(wnd)
    , m_noUnbind(false)
{
    DoInitialize();
}

clEditEventsHandler::~clEditEventsHandler()
{
    if(!m_noUnbind && (m_stc || m_textCtrl || m_combo)) {
        EventNotifier::Get()->TopFrame()->Unbind(wxEVT_MENU, &clEditEventsHandler::OnCopy, this, wxID_COPY);
        EventNotifier::Get()->TopFrame()->Unbind(wxEVT_MENU, &clEditEventsHandler::OnPaste, this, wxID_PASTE);
        EventNotifier::Get()->TopFrame()->Unbind(wxEVT_MENU, &clEditEventsHandler::OnCut, this, wxID_CUT);
        EventNotifier::Get()->TopFrame()->Unbind(wxEVT_MENU, &clEditEventsHandler::OnSelectAll, this, wxID_SELECTALL);
        EventNotifier::Get()->TopFrame()->Unbind(wxEVT_MENU, &clEditEventsHandler::OnUndo, this, wxID_UNDO);
        EventNotifier::Get()->TopFrame()->Unbind(wxEVT_MENU, &clEditEventsHandler::OnRedo, this, wxID_REDO);
    }
}

#define CHECK_FOCUS_WINDOW()                                                     \
    wxWindow* focus = wxWindow::FindFocus();                                     \
    if(!focus) {                                                                 \
        event.Skip();                                                            \
        return;                                                                  \
    } else if((focus != m_stc) && (focus != m_textCtrl) && (focus != m_combo)) { \
        event.Skip();                                                            \
        return;                                                                  \
    }

#define CALL_FUNC(func)     \
    if(m_stc) {             \
        m_stc->func();      \
    } else if(m_combo) {    \
        m_combo->func();    \
    } else {                \
        m_textCtrl->func(); \
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
        EventNotifier::Get()->TopFrame()->Bind(wxEVT_MENU, &clEditEventsHandler::OnCopy, this, wxID_COPY);
        EventNotifier::Get()->TopFrame()->Bind(wxEVT_MENU, &clEditEventsHandler::OnPaste, this, wxID_PASTE);
        EventNotifier::Get()->TopFrame()->Bind(wxEVT_MENU, &clEditEventsHandler::OnCut, this, wxID_CUT);
        EventNotifier::Get()->TopFrame()->Bind(wxEVT_MENU, &clEditEventsHandler::OnSelectAll, this, wxID_SELECTALL);
        EventNotifier::Get()->TopFrame()->Bind(wxEVT_MENU, &clEditEventsHandler::OnUndo, this, wxID_UNDO);
        EventNotifier::Get()->TopFrame()->Bind(wxEVT_MENU, &clEditEventsHandler::OnRedo, this, wxID_REDO);
    }
}
