#ifndef WXCEDITMANAGER_H
#define WXCEDITMANAGER_H

#include "unredobase.h"

#include <list>
#include <wx/string.h>

wxDECLARE_EVENT(wxEVT_MULTIPLE_UNREDO, wxCommandEvent);

struct State {
    wxString project_json;
    wxString selection;
    wxString parentTLW;
    wxString label;

    typedef wxSharedPtr<State> Ptr_t;
    typedef std::list<State::Ptr_t> List_t;

public:
    State() {}

    void Clear()
    {
        project_json.Clear();
        selection.Clear();
        parentTLW.Clear();
    }

    bool IsValid() const { return !project_json.IsEmpty(); }
};

class wxcEditManager : public CommandProcessorBase
{
protected:
    bool m_isModified;
    State::List_t m_undoList;
    State::List_t m_redoList;
    State::Ptr_t m_initialState;

public:
    static wxcEditManager& Get();

private:
    wxcEditManager();
    ~wxcEditManager() override = default;
    void SaveState(State::Ptr_t state);

    // Events
    void OnProjectLoaded(wxCommandEvent& event);
    void OnProjectSaved(wxCommandEvent& event);
    void OnProjectClosed(wxCommandEvent& event);
    void OnProjectMetadataChanged(wxCommandEvent& event);
    void OnPropertyChanged(wxCommandEvent& event);
    void OnUndoDropdownItem(wxCommandEvent& event) override;
    void OnRedoDropdownItem(wxCommandEvent& event) override;
    bool DoUndo() override { return true; }
    bool DoRedo() override { return true; }

    // Helpers
    void NotifyProjectModified();
    void NotifyProjectSynched();
    void SetModified(bool modified);

public:
    void Clear();
    State::Ptr_t Undo();
    State::Ptr_t Redo();

    bool CanUndo() const;
    bool CanRedo() const;
    bool IsDirty() const;

    State::Ptr_t GetCurrentState() const;
    void SaveInitialState(State::Ptr_t state) { m_initialState = state; }

    /**
     * @brief save the current state in the queue
     */
    void PushState(const wxString& label);

    void DoPopulateUnRedoMenu(wxMenu& menu, bool undoing) override;
};

#endif // WXCEDITMANAGER_H
