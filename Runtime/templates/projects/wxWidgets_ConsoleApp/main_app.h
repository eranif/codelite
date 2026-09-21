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
    ~MainApp() override;
    
    /**
     * @brief intialize the application
     */
    bool OnInit() override;
    /**
     * @brief perform cleanup before exiting
     */
    int OnExit() override;
};

DECLARE_APP(MainApp)

#endif // MAINAPP_H
