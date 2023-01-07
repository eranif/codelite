#ifndef WXCEDITMANAGER_H
#define WXCEDITMANAGER_H

#include "unredobase.h"
#include <list>
#include <vector>
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
    virtual ~wxcEditManager();
    void SaveState(State::Ptr_t state);

    // Events
    void OnProjectLoaded(wxCommandEvent& event);
    void OnProjectSaved(wxCommandEvent& event);
    void OnProjectClosed(wxCommandEvent& event);
    void OnProjectMetadataChanged(wxCommandEvent& event);
    void OnPropertyChanged(wxCommandEvent& event);
    virtual void OnUndoDropdownItem(wxCommandEvent& event);
    virtual void OnRedoDropdownItem(wxCommandEvent& event);
    virtual bool DoUndo() { return true; }
    virtual bool DoRedo() { return true; }

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

    virtual void DoPopulateUnRedoMenu(wxMenu& menu, bool undoing);
};

#endif // WXCEDITMANAGER_H
