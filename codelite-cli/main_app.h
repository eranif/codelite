#ifndef MAINAPP_H
#define MAINAPP_H

#include "csManager.h"
#include <wx/app.h>
#include <wx/cmdline.h>

class MainApp : public wxAppConsole
{
protected:
    csManager* m_manager;

protected:
    /**
     * @brief parse the command line here
     * @return true on success, false otherwise
     */
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void PrintUsage(const wxCmdLineParser& parser);

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
