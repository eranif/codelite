#ifndef NODEJSDEBUGGERDLG_H
#define NODEJSDEBUGGERDLG_H
#include "WebToolsBase.h"
#include <wx/tokenzr.h>

class NodeJSDebuggerDlg : public NodeJSDebuggerDlgBase
{
public:
    enum eDialogType {
        kDebug,
        kExecute,
    };
    eDialogType m_type;

public:
    NodeJSDebuggerDlg(wxWindow* parent, eDialogType type);
    NodeJSDebuggerDlg(wxWindow* parent, eDialogType type, const wxFileName& script, const wxArrayString& args);
    virtual ~NodeJSDebuggerDlg();
    wxString GetCommand();
    wxArrayString GetArgs() const
    {
        return wxStringTokenize(m_stcCommandLineArguments->GetText(), "\n", wxTOKEN_STRTOK);
    }
    wxString GetScript() const { return m_filePickerScript->GetPath(); }

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NODEJSDEBUGGERDLG_H
