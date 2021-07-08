#include "ColoursAndFontsManager.h"
#include "EclipseThemeImporterManager.h"
#include "file_logger.h"
#include "wx/arrstr.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include <cl_standard_paths.h>
#include <iostream>
#include <wx/crt.h>
#include <wx/dir.h>
#include <wx/init.h>
#include <wx/string.h>

int main(int argc, char** argv)
{
    // initialize wxWidgets
    wxInitializer init;

    if(argc < 3) {
        std::cout << "Usage: " << wxFileName(argv[0]).GetFullName() << " <path-to-xml-files> <output-folder>" << endl;
        return 1;
    }

    wxString xml_files_path = argv[1];
    wxString output_path = argv[2];

    clStandardPaths::Get().SetUserDataDir(output_path);
    FileLogger::OpenLog("codelite-theme-importer.log", FileLogger::Dbg);
    EclipseThemeImporterManager importer;

    // get list XML files
    wxArrayString files;
    wxDir::GetAllFiles(xml_files_path, &files, "*.xml");
    for(const wxString& file : files) {
        clDEBUG() << "Importing" << file << "..." << importer.Import(file) << endl;
    }

    // save the changes
    ColoursAndFontsManager::Get().Save(true);
    return 0;
}
