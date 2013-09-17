#ifndef CLMAKEGENERATORAPP_H
#define CLMAKEGENERATORAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>

class clMakeGeneratorApp : public wxAppConsole
{
    wxString m_workspaceFile;
    wxString m_project;
    wxString m_workingDirectory;
    wxString m_configuration;
    
protected:
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void DoExitApp();
    
public:
    clMakeGeneratorApp();
    virtual ~clMakeGeneratorApp();
    
    virtual bool OnInit();
    virtual int OnExit();
};

DECLARE_APP(clMakeGeneratorApp)

#endif // CLMAKEGENERATORAPP_H
