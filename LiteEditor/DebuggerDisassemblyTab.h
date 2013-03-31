#ifndef DEBUGGERDISASSEMBLYTAB_H
#define DEBUGGERDISASSEMBLYTAB_H

#include "debuggersettingsbasedlg.h"
#include "debugger.h"

class DebuggerDisassemblyTab : public DebuggerDisassemblyTabBase
{
    wxString m_title;
    DisassembleEntryVec_t m_lines;
    
protected:
    void DoClear();
    
public:
    DebuggerDisassemblyTab(wxWindow* parent, const wxString &label);
    virtual ~DebuggerDisassemblyTab();
    
    void OnOutput(wxCommandEvent &e);
    void OnCurLine(wxCommandEvent &e);
    void OnDebuggerStopped(wxCommandEvent &e);
    
    void SetTitle(const wxString& title) {
        this->m_title = title;
    }
    const wxString& GetTitle() const {
        return m_title;
    }
};
#endif // DEBUGGERDISASSEMBLYTAB_H
