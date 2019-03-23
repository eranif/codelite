#ifndef MAINAPP_H
#define MAINAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>
#include <cl_command_event.h>
#include <wx/arrstr.h>

#ifdef __WXOSX__
#define APP_BASE wxApp
#else
#define APP_BASE wxAppConsole
#endif

class wxEventLoopBase;
class ChildProcess;
class MainApp : public APP_BASE
{
    ChildProcess* m_child = nullptr;
    wxArrayString m_argv;

protected:
    void DoStartup();
    
protected:
    /**
     * @brief parse the command line here
     * @return true on success, false otherwise
     */
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

public:
    MainApp();
    virtual ~MainApp();

    /**
     * @brief intialize the application
     */
    virtual bool OnInit();
    /**
     * @brief perform cleanup before exiting
     */
    virtual int OnExit();
};

DECLARE_APP(MainApp)

#endif // MAINAPP_H
