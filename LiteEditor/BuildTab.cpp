#include "BuildTab.hpp"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "build_settings_config.h"
#include "clAnsiEscapeCodeHandler.hpp"
#include "clStrings.h"
#include "clTerminalViewCtrl.hpp"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "mainbook.h"
#include "manager.h"
#include "shell_command.h"

#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/tokenzr.h>

namespace
{
struct LineClientData {
    wxString project_name;
    // use this as the root folder for changing relative paths to abs. If empty, use the workspace path
    wxString root_dir;
    Compiler::PatternMatch match_pattern;
    wxString message;
    wxString toolchain;
};

} // namespace

BuildTab::BuildTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    GetSizer()->Fit(this);

    m_view = new clTerminalViewCtrl(this);
    GetSizer()->Add(m_view, 1, wxEXPAND);

    // Event handling
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_STARTED, &BuildTab::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_ADDLINE, &BuildTab::OnBuildAddLine, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_ENDED, &BuildTab::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, [this](clWorkspaceEvent& e) {
        e.Skip();
        Cleanup();
    });

    m_view->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &BuildTab::OnLineActivated, this);
    m_view->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &BuildTab::OnLineActivated, this);
    m_view->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &BuildTab::OnContextMenu, this);

    wxTheApp->Bind(wxEVT_MENU, &BuildTab::OnNextBuildError, this, XRCID("next_build_error"));
    wxTheApp->Bind(wxEVT_UPDATE_UI, &BuildTab::OnNextBuildErrorUI, this, XRCID("next_build_error"));

    m_buffer_sw.Start();
}

BuildTab::~BuildTab()
{
    wxTheApp->Unbind(wxEVT_MENU, &BuildTab::OnNextBuildError, this, XRCID("next_build_error"));
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &BuildTab::OnNextBuildErrorUI, this, XRCID("next_build_error"));
}

void BuildTab::OnBuildStarted(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
    m_currentRootDir.clear();
    m_currentProjectName.clear();

    // clear all build markers
    IEditor::List_t all_editors;
    clGetManager()->GetAllEditors(all_editors);
    for(auto editor : all_editors) {
        editor->DelAllCompilerMarkers();
    }

    // ensure that the BUILD_IN is visible
    ManagerST::Get()->ShowOutputPane(BUILD_WIN, true, false);

    m_buffer.clear();
    if(e.IsCleanLog()) {
        ClearView();
    }

    // read the build tab settings
    EditorConfigST::Get()->ReadObject(wxT("BuildTabSettings"), &m_buildTabSettings);

    // clean the last used compiler
    m_activeCompiler.Reset(nullptr);
    m_error_count = m_warn_count = 0;

    // get the toolchain from the event and attempt to load the compiler
    const wxString& toolchain = e.GetToolchain();
    if(!toolchain.empty() && BuildSettingsConfigST::Get()->IsCompilerExist(toolchain)) {
        m_activeCompiler = BuildSettingsConfigST::Get()->GetCompiler(toolchain);
        clDEBUG() << "Active compiler is set to:" << m_activeCompiler->GetName() << endl;
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

    if(m_buildTabSettings.GetScrollTo() == BuildTabSettingsData::SCROLL_TO_FIRST_ERROR) {
        SelectFirstErrorOrWarning(0);
    }

    // notify the plugins that the build has ended
    clBuildEvent build_ended_event(wxEVT_BUILD_ENDED);
    build_ended_event.SetErrorCount(m_error_count);
    build_ended_event.SetWarningCount(m_warn_count);
    EventNotifier::Get()->AddPendingEvent(build_ended_event);

    m_currentProjectName.clear();
    m_currentRootDir.clear();
}


#define PROCESSBUFFER_FMT_LINES_MAX     8192
#define PROCESSBUFFER_FLUSH_TIME        200 // ms

void BuildTab::ProcessBuffer(bool last_line)
{
    const long cur_time = m_buffer_sw.Time();

    // Limit data process frequency.
    if(!last_line) {
        if ((cur_time - m_buffer_time) < PROCESSBUFFER_FLUSH_TIME)
            return;
        m_buffer_time = cur_time;
    }

    // process completed lines
    m_view->Begin();
    auto lines = ::wxStringTokenize(m_buffer, "\n", wxTOKEN_RET_DELIMS);
    wxString remainder;
    const size_t cnt = lines.Count();
    for (size_t i = 0; i < cnt; i ++) {
        auto& line = lines[i];
        if(!last_line && !line.EndsWith("\n")) {
            // not a complete line
            remainder.swap(line);
            break;
        }
        line.Trim();

        // easy path: check for common makefile messages
        if(line.Lower().Contains("entering directory") || line.Lower().Contains("leaving directory")) {
            line = WrapLineInColour(line, AnsiColours::Gray());
            m_view->AppendItem(line);

        } else if(line.Lower().Contains("building project")) {
            ProcessBuildingProjectLine(line);
            line = WrapLineInColour(line, AnsiColours::NormalText(), true);
            m_view->AppendItem(line);

        } else if(false && m_activeCompiler && (m_activeCompiler->GetName() == "rustc") &&
                  line.Lower().Contains("compiling") && ProcessCargoBuildLine(line)) {
            // for now, I have disabled ("false") this check since in Cargo workspace, the path
            // reported by the compiler is relative to the root workspace and not to
            // the internal project
            m_view->AppendItem(line);

         } else if(cnt > PROCESSBUFFER_FMT_LINES_MAX) {
            // Do not heavy process big lines count, no one will read results.
            m_view->AppendItem(line);

        } else {
            std::unique_ptr<LineClientData> m(new LineClientData);
            m->message = line;
            m->root_dir = m_currentRootDir; // maybe empty string

            // remove the terminal ascii colouring escape code
            wxString modified_line;
            StringUtils::StripTerminalColouring(line, modified_line);
            bool lineHasColours = (line.length() != modified_line.length());
            if(!m_activeCompiler || !m_activeCompiler->Matches(modified_line, &m->match_pattern)) {
                m.reset();
            } else {
                switch(m->match_pattern.sev) {
                case Compiler::kSevError:
                    m_error_count++;
                    break;
                case Compiler::kSevWarning:
                    m_warn_count++;
                    break;
                default:
                    break;
                }
            }

            // if this line matches a pattern (error or warning) AND
            // this colour has no colour associated with it (using ANSI escape)
            // add some
            if(!lineHasColours && m.get()) {
                line = WrapLineInColour(line, m->match_pattern.sev == Compiler::kSevError ? AnsiColours::Red()
                                                                                          : AnsiColours::Yellow());
            }
            // Associate the match info with the line in the view
            // this will be used later when selecting lines
            // Note: its OK to pass null here
            if(m) {
                // set the line project name
                m->toolchain = m_activeCompiler->GetName();
                m->project_name = GetCurrentProjectName();
            }
            m_view->AppendItem(line, wxNOT_FOUND, wxNOT_FOUND, (wxUIntPtr)m.release());
        }
    }

    if(last_line) {
        m_buffer.clear();
    } else {
        m_buffer.swap(remainder);
    }
    m_view->Commit();
    m_view->ScrollToBottom();
}

void BuildTab::Cleanup()
{
    m_buildInProgress = false;
    m_buffer.clear();
    ClearView();
    m_activeCompiler.Reset(nullptr);
    m_error_count = 0;
    m_warn_count = 0;
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
    m_view->DeleteAllItems([](wxUIntPtr d) {
        if(d) {
            LineClientData* p = reinterpret_cast<LineClientData*>(d);
            wxDELETE(p);
        }
    });
    m_buffer.clear();
}

wxString BuildTab::WrapLineInColour(const wxString& line, int colour, bool fold_font) const
{
    wxString text;
    clAnsiEscapeCodeColourBuilder text_builder(&text);

    bool is_light = m_view->GetColours().IsLightTheme();
    text_builder.SetTheme(is_light ? eColourTheme::LIGHT : eColourTheme::DARK).Add(line, colour, fold_font);
    return text;
}

void BuildTab::OnLineActivated(wxDataViewEvent& e)
{
    LOG_IF_TRACE { clDEBUG1() << "Build line double clicked" << endl; }
    auto cd = reinterpret_cast<LineClientData*>(m_view->GetItemData(e.GetItem()));
    CHECK_PTR_RET(cd);

    // let the plugins a first chance in handling this line
    if(!cd->match_pattern.file_path.empty()) {
        clBuildEvent eventErrorClicked(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED);
        eventErrorClicked.SetBuildDir(cd->root_dir); // can be empty
        eventErrorClicked.SetFileName(cd->match_pattern.file_path);
        eventErrorClicked.SetLineNumber(cd->match_pattern.line_number);
        eventErrorClicked.SetProjectName(cd->project_name);
        clDEBUG1() << "Letting plugins process it first (" << cd->match_pattern.file_path << ":"
                   << cd->match_pattern.line_number << ")" << endl;
        if(EventNotifier::Get()->ProcessEvent(eventErrorClicked)) {
            return;
        }

        wxFileName fn(cd->match_pattern.file_path);
        if(fn.FileExists()) {
            // if we resolved it now, open the file there is no point in searching this file
            // in m_buildInfoPerFile since the key on this map is kept as full name
            int line_number = cd->match_pattern.line_number;
            line_number -= 1;

            int column = cd->match_pattern.column != wxNOT_FOUND ? cd->match_pattern.column - 1 : wxNOT_FOUND;
            auto cb = [=](IEditor* editor) {
                editor->GetCtrl()->ClearSelections();
                // compilers report line numbers starting from `1`
                // our editor sees line numbers starting from `0`
                editor->CenterLine(line_number, column);
                if(cd->match_pattern.sev == Compiler::kSevError) {
                    editor->SetErrorMarker(line_number, cd->message);
                } else {
                    editor->SetWarningMarker(line_number, cd->message);
                }
                editor->SetActive();
            };
            clGetManager()->OpenFileAndAsyncExecute(fn.GetFullPath(), std::move(cb));
        }
    }
}

void BuildTab::OnContextMenu(wxDataViewEvent& e)
{
    wxMenu menu;
    if(e.GetItem().IsOk()) {
        menu.Append(XRCID("copy-current-lines"), _("Copy"));
        menu.Enable(XRCID("copy-current-lines"), !m_buildInProgress);

        menu.Append(XRCID("copy-all-lines"), _("Copy all"));
        menu.Enable(XRCID("copy-all-lines"), !m_buildInProgress);

        menu.AppendSeparator();
    }

    menu.Append(wxID_SAVEAS);
    menu.Enable(wxID_SAVEAS, !m_buildInProgress);

    menu.AppendSeparator();
    menu.Append(wxID_CLEAR);
    menu.Enable(wxID_CLEAR, !m_buildInProgress);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            Cleanup();
        },
        wxID_CLEAR);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            CallAfter(&BuildTab::SaveBuildLog);
        },
        wxID_SAVEAS);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            CallAfter(&BuildTab::CopySelections);
        },
        XRCID("copy-current-lines"));

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            CallAfter(&BuildTab::CopyAll);
        },
        XRCID("copy-all-lines"));

    m_view->PopupMenu(&menu);
}

void BuildTab::SaveBuildLog()
{
    // get the content
    wxString content;
    content.reserve(16 * 1024); // 16K

    for(size_t i = 0; i < m_view->GetItemCount(); ++i) {
        wxString str;
        StringUtils::StripTerminalColouring(m_view->GetItemText(m_view->RowToItem(i)), str);
        content << str << "\n";
    }

    wxString path = ::wxFileSelector();
    if(path.empty()) {
        return;
    }

    if(wxFileName::FileExists(path)) {
        if(::wxMessageBox(_("A file with the same name already exists, continue?"), "CodeLite",
                          wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
            return;
        }
    }
    FileUtils::WriteFileContent(path, content);
}

void BuildTab::CopySelections()
{
    wxDataViewItemArray items;
    if(m_view->GetSelections(items) == 0) {
        return;
    }

    wxString content;
    for(size_t i = 0; i < items.size(); ++i) {
        wxString str;
        StringUtils::StripTerminalColouring(m_view->GetItemText(items[i]), str);
        content << str << "\n";
    }
    ::CopyToClipboard(content);
}

void BuildTab::CopyAll()
{
    if(m_view->IsEmpty()) {
        return;
    }

    wxString content;
    content.reserve(16 * 1024); // reserve 16K
    for(size_t i = 0; i < m_view->GetItemCount(); ++i) {
        wxString str;
        StringUtils::StripTerminalColouring(m_view->GetItemText(m_view->RowToItem(i)), str);
        content << str << "\n";
    }
    ::CopyToClipboard(content);
}

wxString BuildTab::CreateSummaryLine()
{
    long elapsed = m_sw.Time() / 1000;
    wxString total_time;
    if(elapsed > 5) {
        long sec = elapsed % 60;
        long hours = elapsed / 3600;
        long minutes = (elapsed % 3600) / 60;
        total_time << wxString::Format(wxT(", %s: %02ld:%02ld:%02ld %s"), _("total time"), hours, minutes, sec,
                                       _("seconds"));
    }

    wxString text;
    if(m_buildInterrupted) {
        // build was cancelled by the user
        text << _("(Build cancelled by the user)\n");
        text = WrapLineInColour(text, AnsiColours::Yellow());
        m_buildInterrupted = false;
    } else {

        // at this point, m_buffer is empty
        text << "\n";
        if(m_error_count) {
            text = _("==== build ended with ");
            text << WrapLineInColour(_("errors"), AnsiColours::Red(), true);
        } else if(m_warn_count) {
            text = _("=== build ended with ");
            text << WrapLineInColour(_("warnings"), AnsiColours::Yellow(), true);
        } else {
            text = _("=== build completed ");
            text << WrapLineInColour(_("successfully"), AnsiColours::Green(), true);
        }
        text << " (" << m_error_count << _(" errors, ") << m_warn_count << _(" warnings)");
        if(!total_time.empty()) {
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

    if(re_compiling.IsValid() && re_compiling.Matches(strippedLine)) {
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

size_t BuildTab::GetNextLineWithErrorOrWarning(size_t from, bool errors_only) const
{
    if(m_error_count == 0 && (m_warn_count == 0 || errors_only)) {
        return wxString::npos;
    }

    if(from >= m_view->GetItemCount()) {
        return wxString::npos;
    }

    for(size_t i = from; i < m_view->GetItemCount(); ++i) {
        wxUIntPtr p = m_view->GetItemData(m_view->RowToItem(i));
        if(p) {
            LineClientData* cd = reinterpret_cast<LineClientData*>(p);
            if(errors_only && cd->match_pattern.sev == Compiler::kSevError) {
                return i;
            } else if(!errors_only && (cd->match_pattern.sev == Compiler::kSevWarning ||
                                       cd->match_pattern.sev == Compiler::kSevError)) {
                return i;
            }
        }
    }
    return wxString::npos;
}

void BuildTab::OnNextBuildError(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // get the next line to select
    size_t from = m_view->GetSelectedRow();
    if(from == wxString::npos) {
        from = 0;
    } else {
        ++from;
    }
    SelectFirstErrorOrWarning(from);
}

void BuildTab::OnNextBuildErrorUI(wxUpdateUIEvent& event) { event.Enable(m_warn_count || m_error_count); }

void BuildTab::SelectFirstErrorOrWarning(size_t from)
{
    size_t line_to_select = GetNextLineWithErrorOrWarning(from, m_buildTabSettings.IsSkipWarnings());
    if(line_to_select != wxString::npos) {
        m_view->UnselectAll();
        m_view->SetFirstVisibleRow(line_to_select);
        m_view->SelectRow(line_to_select);
    }
}
