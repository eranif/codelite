#include "ColoursAndFontsManager.h"
#include "ImportThemesDialog.h"
#include "ThemeImporterManager.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"

#include <iostream>
#include <wx/arrstr.h>
#include <wx/crt.h>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/imagjpeg.h>
#include <wx/imagpng.h>
#include <wx/init.h>
#include <wx/string.h>

#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif

namespace
{
void import_files(const wxString& input_dir, const wxString& output_dir)
{
    wxFileName output_file(output_dir, "lexers.json");
    if(output_file.FileExists()) {
        wxRenameFile(output_file.GetFullPath(), output_file.GetFullPath() + ".orig");
    }

    clStandardPaths::Get().SetUserDataDir(output_dir);
    FileLogger::OpenLog("codelite-theme-importer.log", FileLogger::Dbg);

    ThemeImporterManager importer;

    // get list XML files
    {
        wxArrayString xml_files;
        wxDir::GetAllFiles(input_dir, &xml_files, "*.xml");
        for(const wxString& file : xml_files) {
            clDEBUG() << "Importing Eclipse Theme:" << file << "..." << importer.Import(file) << endl;
        }
    }
    {
        wxArrayString json_files;
        wxDir::GetAllFiles(input_dir, &json_files, "*.json");
        for(const wxString& file : json_files) {
            clDEBUG() << "Importing VSCode Theme:" << file << "..." << importer.Import(file) << endl;
        }
    }

    // save the changes
    wxFileName lexer_json(output_dir, "lexers.json");
    ColoursAndFontsManager::Get().Save(lexer_json);
}
} // namespace
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
            if(pFunc) {
                pFunc();
            }
            FreeLibrary(user32Dll);
        }
#endif
        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        ImportThemesDialog dlg(nullptr);
        if(dlg.ShowModal() == wxID_OK) {
            wxString input_dir = dlg.GetInputDirectory();
            wxString output_dir = dlg.GetOutputDirectory();
            import_files(input_dir, output_dir);
            ::wxMessageBox("CodeLite themes generated successfully!");
        }
        return false;
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
