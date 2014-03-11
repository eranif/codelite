#ifndef UNREDOBASE_H
#define UNREDOBASE_H


#include <vector>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include <wx/aui/auibar.h>


enum CLC_types { CLC_insert, CLC_delete, CLC_unknown };

class CLCommand
{
public:
    CLCommand(CLC_types type = CLC_unknown, const wxString& name="") : m_commandType(type), m_commandName(name), m_isOpen(true)
    {}
    virtual ~CLCommand() {}

    int GetCommandType() const {
        return m_commandType;
    }
    wxString GetName() const {
        return m_commandName;
    }
    void SetName(const wxString& name) {
        m_commandName = name;
    }
    wxString GetText() const {
        return m_text;
    }
    void SetText(const wxString& text) {
        m_text = text;
    }
    bool IsOpen() const {
        return m_isOpen;
    }
    void Close() {
        m_isOpen = false;
    }
    wxString GetUserLabel() const {
        return m_userLabel;
    }
    void SetUserLabel(const wxString& label) {
        m_userLabel = label;
    }
    void AppendText(const wxString& text, int position) {
        m_text << text;
    }

    virtual bool GetIsAppendable() const = 0;
    virtual bool Do() {}
    virtual bool Undo() {}

protected:
    const int m_commandType;
    wxString m_commandName;
    wxString m_text;
    wxString m_userLabel;
    bool     m_isOpen;
};

typedef std::vector<CLCommand*> vCLCommands;

class CommandProcessorBase : public wxEvtHandler
{
public:
    CommandProcessorBase();
    virtual ~CommandProcessorBase();

    bool CanAppend(CLC_types type) {
        return GetOpenCommand()->GetIsAppendable() && GetOpenCommand()->GetCommandType() == type;
    }

    virtual void ProcessOpenCommand();

    void PopulateUnRedoMenu(wxWindow* win, wxPoint& pt, bool undoing);

    void PrepareLabelledStatesMenu(wxMenu* menu);
    void PopulateLabelledStatesMenu(wxMenu* menu);

    void UnBindLabelledStatesMenu(wxMenu* menu);
    void DoUnBindLabelledStatesMenu(wxMenu* menu);

    CLCommand* GetInitialCommand() const {
        return m_initialCommand;
    }

    int GetCurrentCommand() const {
        return m_currentCommand;
    }

    void IncrementCurrentCommand();
    void DecrementCurrentCommand();

    int GetNextUndoCommand() const {
        return GetCommands().size() - m_currentCommand - 1;
    }

    void Add(CLCommand* command) {
        m_commands.push_back(command);
        m_currentCommand = m_commands.size()-1;
    }

    CLCommand* GetOpenCommand();
    bool HasOpenCommand() {
        return (GetOpenCommand() != NULL);
    }

    virtual void CloseOpenCommand();

    vCLCommands& GetCommands() {
        return m_commands;
    }

    const vCLCommands& GetCommands() const {
        return m_commands;
    }

    void OnTBUnRedo(wxAuiToolBarEvent& event);

    virtual bool DoUndo() = 0;

    virtual bool DoRedo() = 0;

    bool CanUndo() const {
        return (GetCurrentCommand() > -1);
    }

    bool CanRedo() const {
        return (GetCommands().size() > 0) && (GetCurrentCommand() < (int)(GetCommands().size() - 1));
    }

    void ClearRedos() { // Remove any redos invalidated by a new addition
        while (GetCurrentCommand() < (int)(GetCommands().size() - 1)) {
            GetCommands().pop_back();        
        }
    }

    void SetUserLabel(const wxString& label);

    CLCommand* GetActiveCommand() const; // The command indexed by m_currentCommand

protected:
    void Clear() {
        m_commands.clear();
        delete m_initialCommand;
        m_initialCommand = NULL;
    }

    virtual void OnUndoDropdownItem(wxCommandEvent& event);
    virtual void OnRedoDropdownItem(wxCommandEvent& event);
    virtual void OnLabelledStatesMenuItem(wxCommandEvent& event);

    CLCommand* m_initialCommand;    // A command to hold any initial-state user-label, and to store any initial state if we're state-storing
    vCLCommands m_commands;
    int m_currentCommand;           // The next one to be undone   
};

#endif // UNREDOBASE_H
