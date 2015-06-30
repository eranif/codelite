#include "NodeJSDebuggerDlg.h"
#include <wx/filename.h>
#include "cl_config.h"
#include "clTernServer.h"
#include "globals.h"
#include "imanager.h"

NodeJSDebuggerDlg::NodeJSDebuggerDlg(wxWindow* parent)
    : NodeJSDebuggerDlgBase(parent)
{
    
    wxFileName fnNodejs;
    wxString nodejs = clConfig::Get().Read("webtools/nodejs/debugger/executable", wxString());
    if(nodejs.IsEmpty()) {
        if(clTernServer::LocateNodeJS(fnNodejs)) {
            nodejs = fnNodejs.GetFullPath();
        }
    }
    wxString script = clConfig::Get().Read("webtools/nodejs/debugger/script", wxString());
    if(script.IsEmpty()) {
        if(clGetManager()->GetActiveEditor()) {
            script = clGetManager()->GetActiveEditor()->GetFileName().GetFullPath();
        }
    }
    
    m_filePickerNodeJS->SetPath(nodejs);
    m_filePickerScript->SetPath(script);
}

NodeJSDebuggerDlg::~NodeJSDebuggerDlg() 
{
    clConfig::Get().Write("webtools/nodejs/debugger/executable", m_filePickerNodeJS->GetPath());
    clConfig::Get().Write("webtools/nodejs/debugger/script", m_filePickerScript->GetPath());
}

void NodeJSDebuggerDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(wxFileName::Exists(m_filePickerNodeJS->GetPath()) &&
                 wxFileName::Exists(m_filePickerScript->GetPath()));
}

wxString NodeJSDebuggerDlg::GetCommand()
{
    wxString command, nodejs, script;
    nodejs << m_filePickerNodeJS->GetPath();
    script << m_filePickerScript->GetPath();
    ::WrapWithQuotes(nodejs);
    ::WrapWithQuotes(script);
    
    command << nodejs << " --debug-brk " << script;
    ::WrapInShell(command);
    
    return command;
}
