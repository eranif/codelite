#ifndef NODEJSDEBUGGERDLG_H
#define NODEJSDEBUGGERDLG_H
#include "WebToolsBase.h"

class NodeJSDebuggerDlg : public NodeJSDebuggerDlgBase
{
public:
    NodeJSDebuggerDlg(wxWindow* parent);
    virtual ~NodeJSDebuggerDlg();
    wxString GetCommand();
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NODEJSDEBUGGERDLG_H
