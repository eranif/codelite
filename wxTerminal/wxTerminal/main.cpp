#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit()
    {
#ifdef __WXMSW__
        typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
        HINSTANCE user32Dll = LoadLibrary(L"User32.dll");
        if(user32Dll) {
            SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(user32Dll, "SetProcessDPIAware");
            if(pFunc) { pFunc(); }
            FreeLibrary(user32Dll);
        }
#endif

        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        MainFrame* mainFrame = new MainFrame(NULL);
        SetTopWindow(mainFrame);
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
