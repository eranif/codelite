// wxcgen: headless wxCrafter code generator.
// Uses wxAppConsole (no DISPLAY required) and drives code generation
// directly through the widget model, bypassing GUICraftMainPanel entirely.

#include "JSON.h"
#include "file_logger.h"
#include "fileutils.h"
#include "json_utils.h"
#include "top_level_win_wrapper.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_project_metadata.h"
#include "wxgui_helpers.h"

#include <memory>
#include <vector>
#include <wx/app.h>
#include <wx/arrstr.h>
#include <wx/cmdline.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/sstream.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/wxcrtvararg.h>
#include <wx/xml/xml.h>

static const wxCmdLineEntryDesc s_cmdDesc[] = {
    {wxCMD_LINE_SWITCH, "v", "version", "Print version and exit", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "h", "help", "Print usage and exit", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "o", "output", "Override output directory", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH,
     nullptr,
     "verbose",
     "Print the output directories and the generated files",
     wxCMD_LINE_VAL_NONE,
     wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_PARAM,
     nullptr,
     nullptr,
     "Input .wxcp file",
     wxCMD_LINE_VAL_STRING,
     wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL},
    wxCMD_LINE_DESC_END};

extern const char* GIT_REVISION;

// Return `file` relative to `dir` when it is located under `dir`. Otherwise, return its full path.
static wxString RelativeToDir(const wxString& file, const wxString& dir)
{
    wxFileName fn(file);
    wxFileName rel(fn);
    if (rel.MakeRelativeTo(dir) && !(rel.GetDirCount() > 0 && rel.GetDirs()[0] == "..")) {
        return rel.GetFullPath();
    }
    return fn.GetFullPath();
}

// Print the directories and the files that were generated, one item per line:
//
//   Base class output directory:<full-path>
//   Base class files:<comma separated list of files>
//   Subclass output directory:<full-path>
//   Subclass files:<comma separated list of files>
//
// The file names are relative to the directory printed before them (when they are located under it).
static void PrintGeneratedFiles(const wxString& projectFile,
                                const wxFileName& baseOutputDir,
                                const wxString& baseCpp,
                                const wxStringMap_t& additionalFiles)
{
    wxFileName baseDir(baseOutputDir);
    baseDir.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE);
    const wxString baseDirPath = baseDir.GetPath();

    wxArrayString baseFiles;
    if (wxcProjectMetadata::Get().GetGenerateCPPCode()) {
        if (!baseCpp.IsEmpty()) {
            baseFiles.Add(RelativeToDir(wxcProjectMetadata::Get().BaseHeaderFile().GetFullPath(), baseDirPath));
            baseFiles.Add(RelativeToDir(wxcProjectMetadata::Get().BaseCppFile().GetFullPath(), baseDirPath));
        }
        for (const auto& p : additionalFiles) {
            baseFiles.Add(p.first);
        }
        const wxFileName& bitmapsCpp = wxcCodeGeneratorHelper::Get().GetBitmapsCppFile();
        if (bitmapsCpp.IsOk()) {
            baseFiles.Add(RelativeToDir(bitmapsCpp.GetFullPath(), baseDirPath));
        }
    }

    // Subclass files are generated next to the .wxcp file
    wxFileName subclassDir(projectFile);
    subclassDir.MakeAbsolute();
    const wxString subclassDirPath = subclassDir.GetPath();

    wxArrayString subclassFiles;
    for (const wxString& file : wxcProjectMetadata::Get().GetSubclassFiles()) {
        subclassFiles.Add(RelativeToDir(file, subclassDirPath));
    }

    wxPrintf("Base class output directory:%s\n", baseDirPath.c_str());
    wxPrintf("Base class files:%s\n", wxJoin(baseFiles, ',', '\0').c_str());
    wxPrintf("Subclass output directory:%s\n", subclassDirPath.c_str());
    wxPrintf("Subclass files:%s\n", wxJoin(subclassFiles, ',', '\0').c_str());
}

static bool GenerateFromProject(const wxString& filename,
                                const wxString& fileContent,
                                const wxString& outputDirOverride,
                                bool verbose)
{
    wxcProjectMetadata::Get().SetProjectFile(filename);

    JSON json(fileContent);
    wxcProjectMetadata::Get().FromJSON(json.toElement().namedObject("metadata"));
    wxcProjectMetadata::Get().UpdatePaths();

    if (!outputDirOverride.IsEmpty()) {
        wxcProjectMetadata::Get().SetGeneratedFilesDir(outputDirOverride);
    }

    wxFileName outputDir(wxcProjectMetadata::Get().GetGeneratedFilesDir(), "");
    wxCrafter::MakeAbsToProject(outputDir);
    if (!outputDir.DirExists()) {
        if (!outputDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
            wxFprintf(stderr, "wxcgen: cannot create output directory: %s\n", outputDir.GetFullPath().c_str());
            return false;
        }
    }

    // Deserialize all top-level windows once into an in-memory widget tree.
    // Own them as wxcWidget (public virtual dtor) and dynamic_cast where needed —
    // TopLevelWinWrapper has a protected destructor.
    JSONItem windows = json.toElement().namedObject("windows");
    int nCount = windows.arraySize();

    std::vector<std::unique_ptr<wxcWidget>> wrappers;
    wrappers.reserve(nCount);
    for (int i = 0; i < nCount; i++) {
        wxcWidget* w = wxcWidget::CreateFromJSON(windows.arrayItem(i));
        if (w) {
            wrappers.emplace_back(w);
        }
    }

    wxArrayString headers;
    wxString baseCpp, baseHeader;
    wxStringMap_t additionalFiles;

    wxcCodeGeneratorHelper::Get().Clear();
    wxcProjectMetadata::Get().ClearAggregatedData();

    for (auto& w : wrappers) {
        TopLevelWinWrapper* tlw = dynamic_cast<TopLevelWinWrapper*>(w.get());
        if (!tlw) {
            continue;
        }
        tlw->GenerateCode(wxcProjectMetadata::Get(),
                          /*promptUser=*/false,
                          /*baseOnly=*/false,
                          baseCpp,
                          baseHeader,
                          headers,
                          additionalFiles);
    }

    wxcProjectMetadata::Get().SetAdditionalFiles(additionalFiles);

    // Match the banner emitted by the GUI path verbatim so files don't churn
    // when a developer regenerates locally with the GUI after a wxcgen build.
    wxString autoGenComment;
    autoGenComment << "//////////////////////////////////////////////////////////////////////\n"
                   << "// This file was auto-generated by CodeLite's wxCrafter Plugin\n"
                   << "// wxCrafter project file: " << wxcProjectMetadata::Get().GetProjectFileName().GetFullName()
                   << "\n"
                   << "// Do not modify this file by hand!\n"
                   << "//////////////////////////////////////////////////////////////////////\n\n";

    wxCrafter::WriteGeneratedOutput(baseCpp, baseHeader, headers, additionalFiles, autoGenComment, nullptr);

    // Write XRC output
    if (wxcProjectMetadata::Get().GetGenerateXRC()) {
        wxString xrcFilePath = wxcProjectMetadata::Get().GetXrcFileName();
        if (!xrcFilePath.IsEmpty()) {
            wxString xrcOutput;
            for (auto& w : wrappers) {
                TopLevelWinWrapper* tlw = dynamic_cast<TopLevelWinWrapper*>(w.get());
                if (!tlw) {
                    continue;
                }
                wxString text;
                tlw->ToXRC(text, wxcWidget::XRC_LIVE);
                xrcOutput << text;
            }
            if (!xrcOutput.IsEmpty()) {
                TopLevelWinWrapper::WrapXRC(xrcOutput);
                wxStringInputStream str(xrcOutput);
                wxStringOutputStream out;
                wxXmlDocument doc(str);
                if (doc.Save(out)) {
                    wxFileName fnXrc(xrcFilePath);
                    wxCrafter::MakeAbsToProject(fnXrc);
                    wxCrafter::WriteFile(fnXrc.GetFullPath(), out.GetString(), true);
                }
            }
        }
    }

    const auto ret = wxcCodeGeneratorHelper::Get().CreateXRC(nullptr, nullptr, nullptr, nullptr);
    if (!ret.ok()) {
        wxPrintf("%s\n", ret.message());
        return false;
    }

    if (verbose) {
        PrintGeneratedFiles(filename, outputDir, baseCpp, additionalFiles);
    }
    return true;
}

// wxcgen is a console application: declaring a wxAppConsole subclass tells
// wxWidgets to use the console traits and skip GUI initialisation entirely.
// On GTK builds this avoids gtk_init being called without a DISPLAY, which
// would emit "GLib-GObject-CRITICAL: invalid (NULL) pointer instance" before
// failing.
class wxcgenApp : public wxAppConsole
{
public:
    bool OnInit() override { return true; }
    int OnRun() override;
};

wxIMPLEMENT_APP_CONSOLE(wxcgenApp);

int wxcgenApp::OnRun()
{
    // Silence wxLog. EditorConfig's first-run path probes for files that may
    // not exist yet ("can't open .../wxcrafter.conf"); we don't want that
    // (or any other framework log) on a CLI tool's stderr — wxcgen prints
    // its own diagnostics directly with wxFprintf.
    wxLog::EnableLogging(false);

    wxImage::AddHandler(new wxPNGHandler);

    wxCmdLineParser parser;
    parser.SetDesc(s_cmdDesc);
    parser.SetLogo(
        "wxcgen: generate the wxCrafter base classes from .wxcp files.\n"
        "\n"
        "The base classes are placed next to the .wxcp file, unless an output directory is found.\n"
        "The output directory is selected using the first match of:\n"
        "  1. The -o option.\n"
        "  2. The environment variable WXCGEN_FOLDER_MAP=<base_dir>=<target_folder>\n"
        "     (several entries are separated by ':' on Unix and ';' on Windows).\n"
        "  3. The same WXCGEN_FOLDER_MAP=<base_dir>=<target_folder> line in a '.wxcrafter-environment'\n"
        "     file, searched in the .wxcp folder and then in each of its parent folders.\n"
        "If the .wxcp file is under <base_dir>, the base classes are generated in <target_folder>\n"
        "plus the relative path of the .wxcp folder. Subclass files are always generated next to\n"
        "the .wxcp file.\n"
        "\n"
        "With --verbose, wxcgen prints the following lines for each input file after a successful generation:\n"
        "  Base class output directory:<full-path>\n"
        "  Base class files:<comma separated list of files>\n"
        "  Subclass output directory:<full-path>\n"
        "  Subclass files:<comma separated list of files>\n");
    parser.SetCmdLine(argc, argv);
    if (parser.Parse(/*giveUsage=*/false) != 0) {
        parser.Usage();
        return 1;
    }

    if (parser.Found("h")) {
        parser.Usage();
        return 0;
    }

    if (parser.Found("v")) {
        wxPrintf("wxcgen %s\n", GIT_REVISION);
        return 0;
    }

    if (parser.GetParamCount() == 0) {
        parser.Usage();
        return 1;
    }

    const bool verbose = parser.Found("verbose");

    wxString outputDirStr;
    if (parser.Found("o", &outputDirStr)) {
        wxFileName outputDir(outputDirStr, "");
        outputDir.MakeAbsolute();
        outputDirStr = outputDir.GetPath();
        if (!outputDir.DirExists()) {
            outputDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
    }

    // Ensure the user data dir exists before anything tries to write into
    // it (FileLogger expect it).
    wxFileName user_data_dir{wxStandardPaths::Get().GetUserDataDir(), wxEmptyString};
    user_data_dir.AppendDir("wxcrafter");
    user_data_dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    FileLogger::OpenLog("wxcgen.log", FileLogger::System);

    int rc = 0;
    for (size_t i = 0; i < parser.GetParamCount(); i++) {
        wxString filename = parser.GetParam(i);
        wxFileName fn(filename);
        fn.MakeAbsolute();
        filename = fn.GetFullPath();

        wxString fileContent;
        if (!FileUtils::ReadFileContent(filename, fileContent)) {
            wxFprintf(stderr, "wxcgen: cannot read file: %s\n", filename.c_str());
            rc = 1;
            continue;
        }

        // -o has priority, then WXCGEN_FOLDER_MAP
        wxString outputDir = outputDirStr;
        if (outputDir.empty()) {
            outputDir = wxCrafter::GetOutputDirFromEnv(fn).value_or(wxEmptyString);
        }

        if (!GenerateFromProject(filename, fileContent, outputDir, verbose)) {
            rc = 1;
        }
    }
    return rc;
}
