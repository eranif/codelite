#include "MainDialog.hpp"
#include <wx/app.h>
#include <wx/event.h>
#include <wx/image.h>

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() = default;
    ~MainApp() override = default;

    bool OnInit() override
    {
        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        MainDialog mainDialog{ nullptr };
        mainDialog.ShowModal();
        return false;
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
