#ifndef MAINAPP_H
#define MAINAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>

class MainApp : public wxAppConsole
{
protected:
    /**
     * @brief parse the command line here
     * @return true on success, false otherwise
     */
    bool DoParseCommandLine(wxCmdLineParser &parser);
    
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
