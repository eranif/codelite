#ifndef CODELITE_LLDB_APP_H
#define CODELITE_LLDB_APP_H

#include <wx/app.h>
#include <wx/cmdline.h>

class CodeLiteLLDBApp : public wxAppConsole
{
protected:
    void DebugProcess(const wxString &exePath);
    
public:
    CodeLiteLLDBApp();
    virtual ~CodeLiteLLDBApp();
    
    /**
     * @brief intialize the application
     */
    virtual bool OnInit();
    /**
     * @brief perform cleanup before exiting
     */
    virtual int OnExit();
};

DECLARE_APP(CodeLiteLLDBApp)

#endif // CODELITE_LLDB_APP_H
