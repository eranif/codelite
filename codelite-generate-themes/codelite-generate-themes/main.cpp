#include "ColoursAndFontsManager.h"
#include "ThemeImporters/ThemeImporterManager.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"

#include <wx/app.h>
#include <wx/arrstr.h>
#include <wx/cmdline.h>
#include <wx/crt.h>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/init.h>
#include <wx/msgout.h>
#include <wx/string.h>

// The default input and output directories are set by CMake (the themes folder of this tool, and Runtime/lexers)
#ifndef CGT_DEFAULT_THEMES_DIR
#define CGT_DEFAULT_THEMES_DIR "themes"
#endif

#ifndef CGT_DEFAULT_OUTPUT_DIR
#define CGT_DEFAULT_OUTPUT_DIR "."
#endif

namespace
{
/// Import all the themes found in `input_dir` and write `lexers.json` into `output_dir`.
/// Return the number of theme files that were found.
size_t import_files(const wxString& input_dir, const wxString& output_dir)
{
    wxFileName output_file(output_dir, "lexers.json");
    if (output_file.FileExists()) {
        wxRenameFile(output_file.GetFullPath(), output_file.GetFullPath() + ".orig");
    }

    clStandardPaths::Get().SetUserDataDir(output_dir);
    FileLogger::OpenLog("codelite-theme-importer.log", FileLogger::Dbg);

    ThemeImporterManager importer;

    size_t count = 0;
    // get list XML files
    {
        wxArrayString xml_files;
        wxDir::GetAllFiles(input_dir, &xml_files, "*.xml");
        for (const wxString& file : xml_files) {
            clDEBUG() << "Importing Eclipse Theme:" << file << "..." << importer.Import(file) << endl;
        }

        count += xml_files.size();
    }
    {
        wxArrayString json_files;
        wxDir::GetAllFiles(input_dir, &json_files, "*.json");
        for (const wxString& file : json_files) {
            clDEBUG() << "Importing VSCode Theme:" << file << "..." << importer.Import(file) << endl;
        }
        count += json_files.size();
    }

    {
        wxArrayString toml_files;
        wxDir::GetAllFiles(input_dir, &toml_files, "*.toml");
        for (const wxString& file : toml_files) {
            clDEBUG() << "Importing Alacritty Theme (toml):" << file << "..." << importer.Import(file) << endl;
        }
        count += toml_files.size();
    }

    // save the changes
    wxFileName lexer_json(output_dir, "lexers.json");
    ColoursAndFontsManager::Get().Save(lexer_json);
    return count;
}
} // namespace

/// Command line tool: generate `lexers.json` from a folder of themes (Eclipse XML, VSCode JSON and Alacritty TOML).
class MainApp : public wxApp
{
public:
    MainApp() = default;
    ~MainApp() override = default;

    bool OnInit() override
    {
        // This is a command line tool: print the usage and the errors to the console (not in a dialog)
        delete wxMessageOutput::Set(new wxMessageOutputStderr(stdout));
        return wxApp::OnInit();
    }

    void OnInitCmdLine(wxCmdLineParser& parser) override
    {
        parser.AddOption(
            "i", "input", "The themes folder (default: " CGT_DEFAULT_THEMES_DIR ")", wxCMD_LINE_VAL_STRING);
        parser.AddOption("o",
                         "output",
                         "The folder to write lexers.json into (default: " CGT_DEFAULT_OUTPUT_DIR ")",
                         wxCMD_LINE_VAL_STRING);
        parser.AddSwitch("h", "help", "Show this help", wxCMD_LINE_OPTION_HELP);
    }

    bool OnCmdLineParsed(wxCmdLineParser& parser) override
    {
        m_inputDir = CGT_DEFAULT_THEMES_DIR;
        m_outputDir = CGT_DEFAULT_OUTPUT_DIR;
        parser.Found("i", &m_inputDir);
        parser.Found("o", &m_outputDir);
        return true;
    }

    int OnRun() override
    {
        if (!wxDirExists(m_inputDir)) {
            wxFprintf(stderr, "Error: themes folder '%s' does not exist\n", m_inputDir);
            return 1;
        }

        if (!wxFileName::Mkdir(m_outputDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
            wxFprintf(stderr, "Error: could not create the output folder '%s'\n", m_outputDir);
            return 1;
        }

        const size_t count = import_files(m_inputDir, m_outputDir);
        wxPrintf("Successfully generated %lu themes into '%s'\n",
                 static_cast<unsigned long>(count),
                 wxFileName(m_outputDir, "lexers.json").GetFullPath());
        return 0;
    }

private:
    wxString m_inputDir;
    wxString m_outputDir;
};

wxIMPLEMENT_APP_CONSOLE(MainApp);
