#include "NodeJSDebuggerDlg.h"
#include <wx/filename.h>
#include "cl_config.h"
#include "clTernServer.h"
#include "globals.h"
#include "imanager.h"
#include "NodeJSWorkspaceUserConfiguration.h"
#include "NoteJSWorkspace.h"

NodeJSDebuggerDlg::NodeJSDebuggerDlg(wxWindow* parent, eDialogType type)
    : NodeJSDebuggerDlgBase(parent)
    , m_type(type)
{
    if(m_type == kDebug) {
        SetLabel(_("Debug script"));
    } else {
        SetLabel(_("Execute script"));
        m_staticTextDebuggerPort->Hide();
        m_textCtrlPort->Hide();
    }

    wxFileName fnNodejs;
    wxString nodejs = clConfig::Get().Read("webtools/nodejs/debugger/executable", wxString());
    if(nodejs.IsEmpty()) {
        if(clTernServer::LocateNodeJS(fnNodejs)) {
            nodejs = fnNodejs.GetFullPath();
        }
    }

    NodeJSWorkspaceUser userConf(NodeJSWorkspace::Get()->GetFilename().GetFullPath());
    userConf.Load();
    wxString script = userConf.GetScriptToExecute();
    if(script.IsEmpty()) {
        if(clGetManager()->GetActiveEditor()) {
            script = clGetManager()->GetActiveEditor()->GetFileName().GetFullPath();
        }
    }

    m_filePickerNodeJS->SetPath(nodejs);
    m_filePickerScript->SetPath(script);
    m_textCtrlPort->ChangeValue(wxString() << userConf.GetDebuggerPort());
}

NodeJSDebuggerDlg::~NodeJSDebuggerDlg()
{
    clConfig::Get().Write("webtools/nodejs/debugger/executable", m_filePickerNodeJS->GetPath());
    NodeJSWorkspaceUser userConf(NodeJSWorkspace::Get()->GetFilename().GetFullPath());
    userConf.Load();
    userConf.SetScriptToExecute(m_filePickerScript->GetPath());
    
    long nPort;
    m_textCtrlPort->GetValue().ToCLong(&nPort);
    userConf.SetDebuggerPort(nPort);
    userConf.Save();
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

    if(m_type == kDebug) {
        wxString sport = m_textCtrlPort->GetValue();
        long port = 5858;
        if(!sport.Trim().ToCLong(&port)) {
            port = 5858;
        }
        command << nodejs << " --debug-brk=" << port << " " << script;
        ::WrapInShell(command);
    } else {
        command << nodejs << " " << script;
        ::WrapInShell(command);
    }
    return command;
}
