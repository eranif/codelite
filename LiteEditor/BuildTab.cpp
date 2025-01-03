#include "BuildTab.hpp"

#include "StringUtils.h"
#include "build_settings_config.h"
#include "clStrings.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "mainbook.h"
#include "manager.h"
#include "shell_command.h"

#include <wx/app.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/tokenzr.h>

BuildTab::BuildTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    GetSizer()->Fit(this);

    m_viewStc = new BuildTabView(this);
    GetSizer()->Add(m_viewStc, 1, wxEXPAND);

    // Event handling
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_STARTED, &BuildTab::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_ADDLINE, &BuildTab::OnBuildAddLine, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_ENDED, &BuildTab::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, [this](clWorkspaceEvent& e) {
        e.Skip();
        Cleanup();
    });
    m_buffer_sw.Start();
}

BuildTab::~BuildTab() {}

void BuildTab::OnBuildStarted(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
    m_currentRootDir.clear();
    m_currentProjectName.clear();

    // clear all build markers
    IEditor::List_t all_editors;
    clGetManager()->GetAllEditors(all_editors);
    for (auto editor : all_editors) {
        editor->DelAllCompilerMarkers();
    }

    // ensure that the BUILD_IN is visible
    ManagerST::Get()->ShowOutputPane(BUILD_WIN, true, false);

    m_buffer.clear();
    if (e.IsCleanLog()) {
        ClearView();
    }

    // read the build tab settings
    EditorConfigST::Get()->ReadObject(wxT("BuildTabSettings"), &m_buildTabSettings);

    // clean the last used compiler
    m_activeCompiler = nullptr;

    // get the toolchain from the event and attempt to load the compiler
    const wxString& toolchain = e.GetToolchain();
    if (!toolchain.empty() && BuildSettingsConfigST::Get()->IsCompilerExist(toolchain)) {
        m_activeCompiler = BuildSettingsConfigST::Get()->GetCompiler(toolchain);
        clDEBUG() << "Active compiler is set to:" << m_activeCompiler->GetName() << endl;
    }

    m_viewStc->Initialise(m_activeCompiler, m_buildTabSettings.IsSkipWarnings(), e.GetProjectName());
    if (!m_activeCompiler) {
        clDEBUG() << "Compiler not selected in the workspace build settings or not available" << endl;

        // toolchain not selected in build configuration or unavailable
        m_viewStc->Add(_("\n"));
        m_viewStc->Add(
            WrapLineInColour(_("> WARNING: No toolchain selected. Build log highlighting will not be available!\n"),
                             AnsiColours::Yellow()));
        m_viewStc->Add(
            WrapLineInColour(_("           Check toolchain properly selected in the workspace build settings.\n"),
                             AnsiColours::Yellow()));
        m_viewStc->Add(_("\n"));
        m_viewStc->ScrollToEnd();
    }

    // notify the plugins that the build had started
    clBuildEvent build_started_event(wxEVT_BUILD_STARTED);
    build_started_event.SetProjectName(e.GetProjectName());
    build_started_event.SetConfigurationName(e.GetConfigurationName());
    EventNotifier::Get()->AddPendingEvent(build_started_event);

    // start stop watch
    m_sw.Start();
}

void BuildTab::OnBuildAddLine(clBuildEvent& e)
{
    e.Skip();
    m_buffer << e.GetString();
    ProcessBuffer();
}

void BuildTab::OnBuildEnded(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = false;
    ProcessBuffer(true);

    wxString text = CreateSummaryLine();
    m_buffer.swap(text);
    ProcessBuffer(true);

    if (m_buildTabSettings.GetScrollTo() == BuildTabSettingsData::SCROLL_TO_FIRST_ERROR) {
        m_viewStc->SelectFirstErrorOrWarning(0, m_buildTabSettings.IsSkipWarnings(), true);
    }

    // notify the plugins that the build has ended
    clBuildEvent build_ended_event(wxEVT_BUILD_ENDED);
    build_ended_event.SetErrorCount(m_viewStc->GetErrorCount());
    build_ended_event.SetWarningCount(m_viewStc->GetWarnCount());
    EventNotifier::Get()->AddPendingEvent(build_ended_event);

    m_currentProjectName.clear();
    m_currentRootDir.clear();
}

#define PROCESSBUFFER_FMT_LINES_MAX 8192
#define PROCESSBUFFER_FLUSH_TIME 200 // ms

void BuildTab::ProcessBuffer(bool last_line)
{
    auto remainder = m_viewStc->Add(m_buffer, last_line);
    m_viewStc->ScrollToEnd();

    if (last_line) {
        m_buffer.clear();
    } else {
        m_buffer.swap(remainder);
    }
}

void BuildTab::Cleanup()
{
    m_buildInProgress = false;
    m_buffer.clear();
    ClearView();
    m_activeCompiler = nullptr;
    m_buildInterrupted = false;
    m_currentProjectName.clear();
}

void BuildTab::AppendLine(const wxString& text)
{
    m_buffer << text;
    ProcessBuffer();
}

void BuildTab::ClearView()
{
    m_viewStc->Clear();
    m_buffer.clear();
}

wxString BuildTab::WrapLineInColour(const wxString& line, int colour, bool fold_font) const
{
    wxString text;
    clAnsiEscapeCodeColourBuilder text_builder(&text);

    bool is_light = !DrawingUtils::IsDark(m_viewStc->StyleGetBackground(0));
    text_builder.SetTheme(is_light ? eColourTheme::LIGHT : eColourTheme::DARK).Add(line, colour, fold_font);
    return text;
}

void BuildTab::SaveBuildLog()
{
    // get the content
    wxString content;
    content.reserve(16 * 1024); // 16K
    wxString path = ::wxFileSelector();
    if (path.empty()) {
        return;
    }

    if (wxFileName::FileExists(path)) {
        if (::wxMessageBox(_("A file with the same name already exists, continue?"),
                           "CodeLite",
                           wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
            return;
        }
    }
    FileUtils::WriteFileContent(path, m_viewStc->GetText());
}

wxString BuildTab::CreateSummaryLine()
{
    long elapsed = m_sw.Time() / 1000;
    wxString total_time;
    if (elapsed > 5) {
        long sec = elapsed % 60;
        long hours = elapsed / 3600;
        long minutes = (elapsed % 3600) / 60;
        total_time << wxString::Format(
            wxT(", %s: %02ld:%02ld:%02ld %s"), _("total time"), hours, minutes, sec, _("seconds"));
    }

    wxString text;
    if (m_buildInterrupted) {
        // build was cancelled by the user
        text << _("(Build cancelled by the user)\n");
        text = WrapLineInColour(text, AnsiColours::Yellow());
        m_buildInterrupted = false;
    } else {

        // at this point, m_buffer is empty
        text << "\n";
        if (m_viewStc->GetErrorCount()) {
            text = _("==== build ended with ");
            text << WrapLineInColour(_("errors"), AnsiColours::Red(), true);
        } else if (m_viewStc->GetWarnCount()) {
            text = _("=== build ended with ");
            text << WrapLineInColour(_("warnings"), AnsiColours::Yellow(), true);
        } else {
            text = _("=== build completed ");
            text << WrapLineInColour(_("successfully"), AnsiColours::Green(), true);
        }
        text << " (" << m_viewStc->GetErrorCount() << _(" errors, ") << m_viewStc->GetWarnCount() << _(" warnings)");
        if (!total_time.empty()) {
            text << total_time;
        }
        text << " ===";
    }
    return text;
}

bool BuildTab::ProcessCargoBuildLine(const wxString& line)
{
    // An example for such a line:
    //  Compiling hello_rust v0.1.0 (C:\Users\eran\Documents\HelloRust\HelloRust\cargo-project)
    static wxRegEx re_compiling{ R"#(Compiling[ \t]+.*?\((.*?)\))#" };
    wxString strippedLine;
    StringUtils::StripTerminalColouring(line, strippedLine);

    if (re_compiling.IsValid() && re_compiling.Matches(strippedLine)) {
        m_currentRootDir = re_compiling.GetMatch(strippedLine, 1); // get the path
        m_currentRootDir.Trim().Trim(false);
        LOG_IF_DEBUG { clDEBUG() << "Rustc: current root dir is:" << m_currentRootDir << endl; }
        return true;
    }
    return false;
}

void BuildTab::ProcessBuildingProjectLine(const wxString& line)
{
    // extract the project name from the line
    // an example line:
    // ----------Building project:[ CodeLiteIDE - Win_x64_Release ] (Single File Build)----------
    wxString s = line.AfterFirst('[');
    s = s.BeforeLast(']');
    s = s.BeforeLast('-');
    s.Trim().Trim(false);

    // keep the current project name
    m_currentProjectName.swap(s);
}
