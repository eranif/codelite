#ifndef NODEJSDEBUGGERDLG_H
#define NODEJSDEBUGGERDLG_H
#include "WebToolsBase.h"

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
    virtual ~NodeJSDebuggerDlg();
    wxString GetCommand();
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NODEJSDEBUGGERDLG_H
