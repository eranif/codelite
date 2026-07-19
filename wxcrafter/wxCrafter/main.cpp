#include "ColoursAndFontsManager.h"
#include "UI/wxguicraft_main_view.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "wxc_project_metadata.h"
#include "wxcrafter_plugin.h"

#include <wx/cmdline.h>
#include <wx/filefn.h>
#include <wx/socket.h>
#include <wx/sysopt.h>

class wxcApp : public wxApp
{
public:
    wxcApp() = default;
    ~wxcApp() override = default;
    bool OnInit() override;
    int OnExit() override;
    bool OnCmdLineParsed(wxCmdLineParser& parser) override;

private:
    wxCrafterPlugin* m_wxcPlugin = nullptr;
    bool m_hiddenMainFrame = false;
};

DECLARE_APP(wxcApp)

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    {wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH,
     "g",
     "generate",
     "Generate code from input files and close",
     wxCMD_LINE_VAL_NONE,
     wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "o", "output", "Overwrite output directory", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH,
     "s",
     "server",
     "Start in server mode (hidden)",
     wxCMD_LINE_VAL_STRING,
     wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_PARAM,
     nullptr,
     nullptr,
     "Input file",
     wxCMD_LINE_VAL_STRING,
     wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL},
    wxCMD_LINE_DESC_END};

IMPLEMENT_APP(wxcApp)

#ifdef __WXMSW__
using SetProcessDPIAwareFunc = BOOL(WINAPI*)();
#endif

bool wxcApp::OnInit()
{
    // parse command line
    wxCmdLineParser parser;
    parser.SetDesc(cmdLineDesc);
    parser.SetCmdLine(wxAppBase::argc, wxAppBase::argv);
    if (parser.Parse() != 0) {
        return false;
    }

    if (parser.Found("h")) {
        // print usage
        parser.Usage();
        return false;
    }
    m_hiddenMainFrame = parser.Found("s");

    wxSystemOptions::SetOption(_T("msw.remap"), 0);
    wxSystemOptions::SetOption("msw.notebook.themed-background", 0);
    wxXmlResource::Get()->InitAllHandlers();
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxCURHandler);
    wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxGIFHandler);
    // InitXmlResource();

    wxLog::EnableLogging(false);

#ifdef __WXGTK__
    // Redirect stdout/error to a file
    wxFileName stdout_err(wxStandardPaths::Get().GetUserDataDir(), "wxcrafter-stdout-stderr.log");
#ifndef NDEBUG
    stdout_err.SetPath(stdout_err.GetPath() + "-dbg");
#endif
    FILE* new_stdout = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stdout);
    FILE* new_stderr = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stderr);
    wxUnusedVar(new_stderr);
    wxUnusedVar(new_stdout);
#endif

#ifdef __WXGTK__
    wxString installPrefix = INSTALL_PREFIX;
    wxStandardPaths::Get().SetInstallPrefix(installPrefix);
    EditorConfigST::Get()->SetInstallDir(wxString() << INSTALL_PREFIX << "/share/wxcrafter");

#elif defined(__WXMSW__)
    HINSTANCE m_user32Dll = LoadLibrary(L"User32.dll");
    if (m_user32Dll) {
        SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(m_user32Dll, "SetProcessDPIAware");
        if (pFunc) {
            pFunc();
        }
        FreeLibrary(m_user32Dll);
        m_user32Dll = nullptr;
    }

    EditorConfigST::Get()->SetInstallDir(wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());
#endif
    EditorConfigST::Get()->Init("", "2.0.2");
    EditorConfigST::Get()->Load();

    for (size_t i = 0; i < parser.GetParamCount(); i++) {
        wxString argument = parser.GetParam(i);

        // convert to full path and open it
        wxFileName fn(argument);
        fn.MakeAbsolute(wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());

        if (fn.GetExt() == "wxcp") {
            wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
            evtOpen.SetString(fn.GetFullPath());
            EventNotifier::Get()->AddPendingEvent(evtOpen);
        }
    }

    // initialize the socket library
    wxSocketBase::Initialize();

    // Initialize the edit manager
    wxcEditManager::Get();

    // Open log file
    FileLogger::OpenLog("wxcrafter.log", FileLogger::System);
    clDEBUG() << "wxCrafter started" << endl;

    // Initialize the colours and font manager
    ColoursAndFontsManager::Get().Load();
    ColoursAndFontsManager::Get().RestoreDefaults();

    m_wxcPlugin = new wxCrafterPlugin(nullptr, false);
    SetTopWindow(m_wxcPlugin->GetMainFrame());

    if (parser.FoundSwitch("g") == wxCMD_SWITCH_ON) {
        wxString outputDirStr;
        if (parser.Found("o", &outputDirStr)) {
            wxFileName outputDir(outputDirStr);
            outputDir.MakeAbsolute();
            outputDirStr = outputDir.GetFullPath();
            wxMkdir(outputDirStr);
            clDEBUG() << "Set output directory to" << outputDirStr << endl;
        }

        auto wxcView = m_wxcPlugin->GetMainFrame()->GetWxcView();
        for (size_t i = 0; i != parser.GetParamCount(); ++i) {
            wxString filename = parser.GetParam(i);
            clDEBUG() << "Generate from file" << filename << endl;
            wxString fileContent;
            if (FileUtils::ReadFileContent(filename, fileContent)) {
                wxcView->LoadProject(filename, fileContent);
                if (!outputDirStr.empty()) {
                    wxcProjectMetadata::Get().SetGeneratedFilesDir(outputDirStr);
                }
                wxcView->DoGenerateCode(InteractionMode::HideDialogs, SaveMode::DontSave);
            } else {
                return false;
            }
        }
        exit(0);
    }

    return GetTopWindow()->Show();
}

int wxcApp::OnExit()
{
    wxDELETE(m_wxcPlugin);
    return TRUE;
}

bool wxcApp::OnCmdLineParsed(wxCmdLineParser& parser) { return true; }
