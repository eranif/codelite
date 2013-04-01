#ifndef DEBUGGERDISASSEMBLYTAB_H
#define DEBUGGERDISASSEMBLYTAB_H

#include "debuggersettingsbasedlg.h"
#include "debugger.h"

class DebuggerDisassemblyTab : public DebuggerDisassemblyTabBase
{
    wxString m_title;
    DisassembleEntryVec_t m_lines;

protected:
    virtual void OnMarginClicked(wxStyledTextEvent& event);
    void DoClear();
    void DoCentrLine(int line);

    void OnOutput(wxCommandEvent &e);
    void OnCurLine(wxCommandEvent &e);
    void OnDebuggerStopped(wxCommandEvent &e);
    void OnQueryFileLineDone(wxCommandEvent &e);
    void OnAllBreakpointsDeleted(wxCommandEvent &e);
    
public:
    DebuggerDisassemblyTab(wxWindow* parent, const wxString &label);
    virtual ~DebuggerDisassemblyTab();



    void SetTitle(const wxString& title) {
        this->m_title = title;
    }
    const wxString& GetTitle() const {
        return m_title;
    }
};
#endif // DEBUGGERDISASSEMBLYTAB_H
