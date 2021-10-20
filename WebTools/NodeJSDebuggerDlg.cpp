#include "ColoursAndFontsManager.h"
#include "NodeJSDebuggerDlg.h"
#include "NodeJSWorkspaceUserConfiguration.h"
#include "NoteJSWorkspace.h"
#include "cl_config.h"
#include "globals.h"
#include "imanager.h"
#include <wx/filename.h>
#include "clNodeJS.h"

NodeJSDebuggerDlg::NodeJSDebuggerDlg(wxWindow* parent, eDialogType type)
    : NodeJSDebuggerDlgBase(parent)
    , m_type(type)
{
    if(m_type == kDebug || m_type == kDebugCLI) {
        SetLabel(_("Debug script"));
        m_staticTextScript->SetLabel(_("Script to debug:"));
    } else {
        SetLabel(_("Execute script"));
        m_staticTextScript->SetLabel(_("Script to execute:"));
        m_staticTextDebuggerPort->Disable();
        m_textCtrlPort->Disable();
    }

    m_stcCommandLineArguments->SetEOLMode(wxSTC_EOL_LF);
    wxFileName fnNodejs;
    wxString nodejs = clNodeJS::Get().GetNode().GetFullPath();

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
    m_dirPickerWorkingDirectory->SetPath(userConf.GetWorkingDirectory().IsEmpty()
                                             ? NodeJSWorkspace::Get()->GetFilename().GetPath()
                                             : userConf.GetWorkingDirectory());

    m_stcCommandLineArguments->SetText(::wxJoin(userConf.GetCommandLineArgs(), '\n'));
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("javascript");
    if(lexer) { lexer->Apply(m_stcCommandLineArguments); }
    CenterOnParent();
}

NodeJSDebuggerDlg::NodeJSDebuggerDlg(wxWindow* parent, eDialogType type, const wxFileName& script,
                                     const wxArrayString& args)
    : NodeJSDebuggerDlgBase(parent)
    , m_type(type)
{
    if(m_type == kDebug || m_type == kDebugCLI) {
        SetLabel(_("Debug script"));
        m_staticTextScript->SetLabel(_("Script to debug:"));
    } else {
        SetLabel(_("Execute script"));
        m_staticTextScript->SetLabel(_("Script to execute:"));
        m_staticTextDebuggerPort->Disable();
        m_textCtrlPort->Disable();
    }

    m_stcCommandLineArguments->SetEOLMode(wxSTC_EOL_LF);
    wxFileName fnNodejs;
    wxString nodejs = clNodeJS::Get().GetNode().GetFullPath();

    NodeJSWorkspaceUser userConf(NodeJSWorkspace::Get()->GetFilename().GetFullPath());
    userConf.Load();

    m_filePickerNodeJS->SetPath(nodejs);
    m_filePickerScript->SetPath(script.GetFullPath());
    m_textCtrlPort->ChangeValue(wxString() << userConf.GetDebuggerPort());
    m_stcCommandLineArguments->SetText(::wxJoin(args, '\n'));
    m_dirPickerWorkingDirectory->SetPath(userConf.GetWorkingDirectory().IsEmpty()
                                             ? NodeJSWorkspace::Get()->GetFilename().GetPath()
                                             : userConf.GetWorkingDirectory());

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("javascript");
    if(lexer) { lexer->Apply(m_stcCommandLineArguments); }
    CenterOnParent();
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
    wxArrayString commandLineArgs = ::wxStringTokenize(m_stcCommandLineArguments->GetText(), "\n", wxTOKEN_STRTOK);
    userConf.SetCommandLineArgs(commandLineArgs);
    userConf.SetWorkingDirectory(m_dirPickerWorkingDirectory->GetPath());
    userConf.Save();
}

void NodeJSDebuggerDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(wxFileName::Exists(m_filePickerNodeJS->GetPath()) &&
                 wxFileName::Exists(m_filePickerScript->GetPath()));
}

void NodeJSDebuggerDlg::GetCommand(wxString& command, wxString& command_args)
{
    wxString script;
    command << m_filePickerNodeJS->GetPath();
    script << m_filePickerScript->GetPath();
    ::WrapWithQuotes(script);
    wxString sport = m_textCtrlPort->GetValue();
    long port = 5858;
    if(!sport.Trim().ToCLong(&port)) { port = 5858; }
    
    if(m_type == kDebug) {
        command_args << "--debug-brk=" << port << " " << script;
    } else if(m_type == kDebugCLI) {
        command_args << "--inspect-brk=" << port << " " << script;
    } else {
        command_args << script;
    }

    wxArrayString args = ::wxStringTokenize(m_stcCommandLineArguments->GetText(), "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < args.size(); ++i) {
        command_args << " " << ::WrapWithQuotes(args.Item(i));
    }
}

wxString NodeJSDebuggerDlg::GetWorkingDirectory() const
{
    return m_dirPickerWorkingDirectory->GetPath().IsEmpty() ? NodeJSWorkspace::Get()->GetFilename().GetPath()
                                                            : m_dirPickerWorkingDirectory->GetPath();
}
