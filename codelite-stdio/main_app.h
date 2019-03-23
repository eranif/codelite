#ifndef MAINAPP_H
#define MAINAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>
#include <cl_command_event.h>

class ChildProcess;
class MainApp : public wxAppConsole
{
    ChildProcess* m_child = nullptr;

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
