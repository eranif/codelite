#ifndef CLMAKEGENERATORAPP_H
#define CLMAKEGENERATORAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>
// -----------------------------------------------------------
// -----------------------------------------------------------
class clMakeGeneratorApp : public wxAppConsole
{
public:
    enum eCommandType { kBuild, kClean, kRebuild };

protected:
    wxString m_workspaceFile;
    wxString m_project;
    wxString m_workingDirectory;
    wxString m_configuration;
    bool m_verbose;
    bool m_executeCommand;
    int m_exitCode;
    eCommandType m_commandType;
    wxString m_buildSettingsXml;
    bool m_generateCompileCommands;
	bool m_generateCompilerFlags = false;

protected:
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void DoExecCommand(const wxString& command);
    void DoGenerateCompileCommands();
    void Notice(const wxString& msg);
    void Error(const wxString& msg);
    void Info(const wxString& msg);
    void Out(const wxString& msg);
    void Bye();
    
public:
    clMakeGeneratorApp();
    virtual ~clMakeGeneratorApp();

    void DoExitApp();
    virtual bool OnInit();
    virtual int OnExit();
};

DECLARE_APP(clMakeGeneratorApp)

#endif // CLMAKEGENERATORAPP_H
