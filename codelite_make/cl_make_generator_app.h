#ifndef CLMAKEGENERATORAPP_H
#define CLMAKEGENERATORAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>
// -----------------------------------------------------------
// -----------------------------------------------------------
class clMakeGeneratorApp : public wxAppConsole
{
    wxString  m_workspaceFile;
    wxString  m_project;
    wxString  m_workingDirectory;
    wxString  m_configuration;
    bool      m_verbose;
    bool      m_executeCommand;

protected:
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void DoExecCommand(const wxString &command);

    void Notice(const wxString &msg);
    void Error(const wxString &msg);
    void Info(const wxString &msg);
    void Out(const wxString &msg);

public:
    clMakeGeneratorApp();
    virtual ~clMakeGeneratorApp();

    void DoExitApp();
    virtual bool OnInit();
    virtual int OnExit();
};

DECLARE_APP(clMakeGeneratorApp)

#endif // CLMAKEGENERATORAPP_H
