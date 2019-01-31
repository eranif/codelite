#include "wxc_edit_manager.h"
#include "event_notifier.h"
#include "wxc_project_metadata.h"
#include "wxguicraft_main_view.h"
#include <wx/app.h>

wxDEFINE_EVENT(wxEVT_MULTIPLE_UNREDO, wxCommandEvent);

const int FIRST_MENU_ID = 10000;

wxcEditManager::wxcEditManager()
    : m_isModified(false)
{
    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_SAVED, wxCommandEventHandler(wxcEditManager::OnProjectSaved),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXC_PROJECT_LOADED, wxCommandEventHandler(wxcEditManager::OnProjectLoaded),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_CLOSED, wxCommandEventHandler(wxcEditManager::OnProjectClosed),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJECT_METADATA_MODIFIED,
                                  wxCommandEventHandler(wxcEditManager::OnProjectMetadataChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROPERTIES_MODIFIED, wxCommandEventHandler(wxcEditManager::OnPropertyChanged),
                                  NULL, this);
}

wxcEditManager::~wxcEditManager() {}

wxcEditManager& wxcEditManager::Get()
{
    static wxcEditManager editManager;
    return editManager;
}

State::Ptr_t wxcEditManager::Redo()
{
    State::Ptr_t state = m_redoList.back();
    m_redoList.pop_back();
    m_undoList.push_back(state);
    SetModified(true);
    return state;
}

State::Ptr_t wxcEditManager::Undo()
{
    // move the last item to the re-do list
    State::Ptr_t state = m_undoList.back();
    m_undoList.pop_back();
    m_redoList.push_back(state);
    SetModified(true);

    // and return the current state
    return GetCurrentState();
}

void wxcEditManager::OnUndoDropdownItem(wxCommandEvent& event)
{
    const int count = event.GetId() - FIRST_MENU_ID + 1;
    wxCHECK_RET(count > 0 && count <= (int)m_undoList.size(), "Invalid command index");
    for(int n = 0; n < count; ++n) {
        State::Ptr_t state = m_undoList.back();
        m_undoList.pop_back();
        m_redoList.push_back(state);
    }

    SetModified(true);

    wxCommandEvent evt(wxEVT_MULTIPLE_UNREDO,
                       wxID_UNDO); // This tells GUICraftMainPanel to reload the new 'current' state
    wxPostEvent(wxTheApp, evt);
}

void wxcEditManager::OnRedoDropdownItem(wxCommandEvent& event)
{
    const int count = event.GetId() - FIRST_MENU_ID + 1;
    wxCHECK_RET(count > 0 && count <= (int)m_redoList.size(), "Invalid command index");
    for(int n = 0; n < count; ++n) {
        State::Ptr_t state = m_redoList.back();
        m_redoList.pop_back();
        m_undoList.push_back(state);
    }

    SetModified(true);

    wxCommandEvent evt(wxEVT_MULTIPLE_UNREDO,
                       wxID_REDO); // This tells GUICraftMainPanel to reload the new 'current' state
    wxPostEvent(wxTheApp, evt);
}

void wxcEditManager::SaveState(State::Ptr_t state)
{
    m_redoList.clear();
    m_undoList.push_back(state);
}

void wxcEditManager::Clear()
{
    m_redoList.clear();
    m_undoList.clear();
    m_initialState.reset();
    SetModified(false);
}

State::Ptr_t wxcEditManager::GetCurrentState() const
{
    if(m_undoList.empty()) return m_initialState;
    return m_undoList.back();
}

bool wxcEditManager::IsDirty() const { return m_isModified; }

void wxcEditManager::OnProjectLoaded(wxCommandEvent& event)
{
    event.Skip();
    Clear();
    m_initialState = GUICraftMainPanel::m_MainPanel->CurrentState();
}

void wxcEditManager::OnProjectSaved(wxCommandEvent& event)
{
    event.Skip();
    SetModified(false);
}

void wxcEditManager::OnProjectClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

void wxcEditManager::OnProjectMetadataChanged(wxCommandEvent& event)
{
    event.Skip();
    SetModified(true);
    PushState("project metadata changed");
}

void wxcEditManager::OnPropertyChanged(wxCommandEvent& event)
{
    event.Skip();
    SetModified(true);
    PushState("property changed");
}

void wxcEditManager::PushState(const wxString& label)
{
    if(GUICraftMainPanel::m_MainPanel) {
        SetModified(true);
        State::Ptr_t state = GUICraftMainPanel::m_MainPanel->CurrentState();
        state->label = label;
        SaveState(state);
    }
}

void wxcEditManager::NotifyProjectModified()
{
    wxCommandEvent event(wxEVT_CMD_WXCRAFTER_PROJECT_MODIFIED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void wxcEditManager::NotifyProjectSynched()
{
    wxCommandEvent event(wxEVT_CMD_WXCRAFTER_PROJECT_SYNCHED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void wxcEditManager::SetModified(bool modified)
{
    m_isModified = modified;
    if(m_isModified) {
        NotifyProjectModified();
    } else {
        NotifyProjectSynched();
    }
}

bool wxcEditManager::CanRedo() const { return !m_redoList.empty(); }

bool wxcEditManager::CanUndo() const { return !m_undoList.empty(); }

void wxcEditManager::DoPopulateUnRedoMenu(wxMenu& menu, bool undoing)
{
    wxString prefix(undoing ? _("Undo ") : _("Redo "));
    int id = FIRST_MENU_ID;
    int count = 0;

    if(undoing) {
        if(m_undoList.size() > 0) {
            for(State::List_t::const_reverse_iterator iter = m_undoList.rbegin(); iter != m_undoList.rend(); ++iter) {
                State::Ptr_t command = *iter;
                if(command) { menu.Append(id++, wxString::Format("%i ", ++count) + prefix + command->label); }
            }
        }
    } else {
        for(State::List_t::const_reverse_iterator iter = m_redoList.rbegin(); iter != m_redoList.rend(); ++iter) {
            State::Ptr_t command = *iter;
            if(command) { menu.Append(id++, wxString::Format("%i ", ++count) + prefix + command->label); }
        }
    }
}
