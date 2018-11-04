#ifndef MAINAPP_H
#define MAINAPP_H

#include "NodeStdinThread.h"
#include "clNodeProcess.h"
#include <cl_command_event.h>
#include <wx/app.h>
#include <wx/cmdline.h>

class MainApp : public wxAppConsole
{
    wxString m_nodejs;
    wxString m_scriptPath;
    clNodeProcess m_process;
    NodeStdinThread* m_stdin = nullptr;

protected:
    /**
     * @brief parse the command line here
     * @return true on success, false otherwise
     */
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void OnStdin(clCommandEvent& event);
    void OnStdinExit(clCommandEvent& event);

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
