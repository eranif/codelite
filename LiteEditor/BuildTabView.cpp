#include "BuildTabView.hpp"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clColours.h"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "dirsaver.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"

#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

namespace
{
wxString WrapLineInColour(const wxString& line, int colour, bool fold_font, bool is_dark_theme)
{
    wxString text;
    clAnsiEscapeCodeColourBuilder text_builder(&text);

    text_builder.SetTheme(is_dark_theme ? eColourTheme::DARK : eColourTheme::LIGHT).Add(line, colour, fold_font);
    return text;
}

wxString ProcessBuildingProjectLine(const wxString& line)
{
    // extract the project name from the line
    // an example line:
    // ----------Building project:[ CodeLiteIDE - Win_x64_Release ] (Single File Build)----------
    wxString s = line.AfterFirst('[');
    s = s.BeforeLast(']');
    s = s.BeforeLast('-');
    s.Trim().Trim(false);
    return s;
}

/// given range, [start, end), return the string in this range without any ANSI escape codes
wxString GetSelectedRange(wxStyledTextCtrl* ctrl, int start_pos, int end_pos)
{
    if (start_pos >= end_pos) {
        return wxEmptyString;
    }

    // Make sure we only pick visible chars (embedded ANSI colour can break the selected word)
    wxString res;
    res.reserve(end_pos - start_pos + 1);
    for (; start_pos < end_pos; start_pos++) {
        if (ctrl->StyleGetVisible(ctrl->GetStyleAt(start_pos))) {
            res << (wxChar)ctrl->GetCharAt(start_pos);
        }
    }
    return res;
}

constexpr int LINE_MARKER = 7;
constexpr int NUMBER_MARGIN_ID = 1;
constexpr int SYMBOLS_MARGIN_SEP_ID = 4;

} // namespace

BuildTabView::BuildTabView(wxWindow* parent)
    : wxStyledTextCtrl(parent)
{
    InitialiseView();
    Bind(wxEVT_LEFT_DOWN, &BuildTabView::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &BuildTabView::OnLeftUp, this);

    wxTheApp->Bind(wxEVT_MENU, &BuildTabView::OnNextBuildError, this, XRCID("next_build_error"));
    wxTheApp->Bind(wxEVT_UPDATE_UI, &BuildTabView::OnNextBuildErrorUI, this, XRCID("next_build_error"));
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &BuildTabView::OnThemeChanged, this);

    Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& e) {
        e.Skip();
        clCommandEvent focus_gained{ wxEVT_STC_GOT_FOCUS };
        EventNotifier::Get()->AddPendingEvent(focus_gained);
    });
}

BuildTabView::~BuildTabView()
{
    Unbind(wxEVT_LEFT_DOWN, &BuildTabView::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &BuildTabView::OnLeftUp, this);

    wxTheApp->Unbind(wxEVT_MENU, &BuildTabView::OnNextBuildError, this, XRCID("next_build_error"));
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &BuildTabView::OnNextBuildErrorUI, this, XRCID("next_build_error"));
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &BuildTabView::OnThemeChanged, this);
}

void BuildTabView::InitialiseView()
{
    SetSTCCursor(8);
    auto lexer = ColoursAndFontsManager::Get().GetLexer("terminal");
    if (lexer) {
        lexer->Apply(this);
        IndicatorSetStyle(INDICATOR_HYPERLINK, wxSTC_INDIC_COMPOSITIONTHICK);
        IndicatorSetForeground(INDICATOR_HYPERLINK, clColours::Blue(lexer->IsDark()));
        // Mark current line
        MarkerDefine(LINE_MARKER, wxSTC_MARK_BACKGROUND);
        MarkerSetBackground(LINE_MARKER, lexer->IsDark() ? *wxWHITE : wxColour("GRAY"));
        MarkerSetForeground(LINE_MARKER, lexer->IsDark() ? *wxWHITE : wxColour("GRAY"));
        MarkerSetAlpha(LINE_MARKER, 40);
    }

    SetScrollWidthTracking(true);
    SetWordChars(R"#(\:~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$/.-+@)#");
    SetEditable(false);
}
#define PROCESSBUFFER_FMT_LINES_MAX 8192
#define PROCESSBUFFER_FLUSH_TIME 200 // ms

wxString BuildTabView::Add(const wxString& output, bool process_last_line)
{
    SetEditable(true);
    auto lines = ::wxStringTokenize(output, "\n", wxTOKEN_RET_DELIMS);
    wxString remainder;
    const size_t line_count = lines.Count();
    bool is_dark_theme = DrawingUtils::IsDark(StyleGetBackground(0));
    size_t cur_line_number = GetLineCount() - 1;

    wxString textToAppend;
    for (size_t i = 0; i < line_count; i++, cur_line_number++) {
        auto& line = lines[i];
        if (!process_last_line && !line.EndsWith("\n")) {
            // not a complete line
            remainder.swap(line);
            break;
        }
        line.Trim();

        // easy path: check for common makefile messages
        wxString lcLine = line.Lower();
        if (lcLine.Contains("entering directory") || lcLine.Contains("leaving directory") ||
            lcLine.Contains(CLEAN_PROJECT_PREFIX)) {
            StringUtils::StripTerminalColouring(line, line);
            line = WrapLineInColour(line, AnsiColours::Gray(), false, is_dark_theme);
            textToAppend << line << "\n";

        } else if (lcLine.Contains(BUILD_END_MSG) || lcLine.Contains("=== build completed") ||
                   lcLine.Contains("=== build ended")) {
            StringUtils::StripTerminalColouring(line, line);
            if (m_errorCount > 0) {
                // build ended with error
                line = WrapLineInColour(line, AnsiColours::Red(), false, is_dark_theme);
            } else if (m_warnCount > 0) {
                // build ended with warnings only
                line = WrapLineInColour(line, AnsiColours::Yellow(), false, is_dark_theme);
            } else {
                // clean build
                line = WrapLineInColour(line, AnsiColours::Green(), false, is_dark_theme);
            }
            textToAppend << line << "\n";

        } else if (lcLine.Contains(BUILD_PROJECT_PREFIX)) {
            m_currentProject = ProcessBuildingProjectLine(line);
            line = WrapLineInColour(line, AnsiColours::Gray(), false, is_dark_theme);
            textToAppend << line << "\n";

        } else if (line_count > PROCESSBUFFER_FMT_LINES_MAX) {
            // Do not heavy process big lines count, no one will read results.
            textToAppend << line;

        } else {
            std::shared_ptr<LineClientData> line_data(new LineClientData);
            line_data->message = line;
            line_data->root_dir = wxEmptyString; // maybe empty string

            // remove the terminal ANSI colouring escape code
            wxString modified_line;
            StringUtils::StripTerminalColouring(line, modified_line);
            bool lineHasColours = (line.length() != modified_line.length());

            if (!m_activeCompiler || !m_activeCompiler->Matches(line, &line_data->match_pattern)) {
                line_data.reset();
            } else {
                switch (line_data->match_pattern.sev) {
                case Compiler::kSevError:
                    m_errorCount++;
                    break;
                case Compiler::kSevWarning:
                    m_warnCount++;
                    break;
                default:
                    break;
                }
            }

            // if this line matches a pattern (error or warning) AND
            // this colour has no colour associated with it (using ANSI escape)
            // add some
            if (!lineHasColours && line_data != nullptr) {
                line = WrapLineInColour(line,
                                        line_data->match_pattern.sev == Compiler::kSevError ? AnsiColours::Red()
                                                                                            : AnsiColours::Yellow(),
                                        false,
                                        is_dark_theme);
            }
            // Associate the match info with the line in the view
            // this will be used later when selecting lines
            // Note: its OK to pass null here
            if (line_data) {
                // set the line project name
                line_data->toolchain = m_activeCompiler ? m_activeCompiler->GetName() : wxString();
                line_data->project_name = m_currentProject;
                clDEBUG() << "Storing line info for line:" << cur_line_number << endl;
                m_lineInfo.insert({ cur_line_number, line_data });
            }
            textToAppend << line << "\n";
        }
    }

    if (!textToAppend.empty()) {
        AppendText(textToAppend);
    }

    SetEditable(false);
    return remainder;
}

void BuildTabView::Clear()
{
    SetEditable(true);
    ClearAll();
    SetEditable(false);
    m_lineInfo.clear();
    m_errorCount = 0;
    m_warnCount = 0;
    m_currentProject = wxEmptyString;
    m_activeCompiler = nullptr;
    ClearLineMarker();
}

void BuildTabView::OnLeftDown(wxMouseEvent& e)
{
    e.Skip();

    int pos = PositionFromPoint(e.GetPosition());
    SetLineMarker(LineFromPosition(pos));

    int start_pos = WordStartPosition(pos, true);
    int end_pos = WordEndPosition(pos, true);
    if (start_pos >= end_pos) {
        return;
    }

    if (m_indicatorStartPos != wxNOT_FOUND && m_indicatorEndPos != wxNOT_FOUND) {
        SetIndicatorCurrent(INDICATOR_HYPERLINK);
        IndicatorClearRange(m_indicatorStartPos, m_indicatorEndPos - m_indicatorStartPos);
    }

    // set new indicator
    SetIndicatorCurrent(INDICATOR_HYPERLINK);
    IndicatorFillRange(start_pos, end_pos - start_pos);
    m_indicatorStartPos = start_pos;
    m_indicatorEndPos = end_pos;
}

void BuildTabView::OnLeftUp(wxMouseEvent& e)
{
    e.Skip();
    if (m_indicatorStartPos != wxNOT_FOUND && m_indicatorEndPos != wxNOT_FOUND) {
        // Open the highlighted text
        auto pattern = GetSelectedRange(this, m_indicatorStartPos, m_indicatorEndPos);
        CallAfter(&BuildTabView::DoPatternClicked, pattern, LineFromPosition(m_indicatorStartPos));

        SetIndicatorCurrent(INDICATOR_HYPERLINK);
        IndicatorClearRange(m_indicatorStartPos, m_indicatorEndPos - m_indicatorStartPos);
        m_indicatorStartPos = m_indicatorEndPos = wxNOT_FOUND;
    }
}

void BuildTabView::DoPatternClicked(const wxString& pattern, int pattern_line)
{
    clDEBUG() << "Searching for line info for view line:" << pattern_line << endl;
    if (m_lineInfo.count(pattern_line)) {
        clDEBUG() << "Using parsed data for view line:" << pattern_line << endl;
        const auto& line_info = m_lineInfo[pattern_line];
        OpenEditor(line_info);
    } else {
        clDEBUG() << "No line info for view line:" << pattern_line << endl;
        // if the pattern matches a URL, open it
        if (pattern.StartsWith("https://") || pattern.StartsWith("http://")) {
            ::wxLaunchDefaultBrowser(pattern);
            return;
        }

        wxString file;
        long line = wxNOT_FOUND;
        long col = wxNOT_FOUND;
        auto parts = ::wxStringTokenize(pattern, ":", wxTOKEN_STRTOK);
        if (parts.size() > 1) {
            if (parts[0].length() == 1) {
                // single char -> volume
                // assume windows full path
                file = parts[0] + ":" + parts[1];
                parts.RemoveAt(0, 2);
            } else {
                file = parts[0];
                parts.RemoveAt(0, 1);
            }
            if (!parts.empty()) {
                // line number
                parts[0].ToCLong(&line);
                parts.RemoveAt(0, 1);
            }
            if (!parts.empty()) {
                // column
                parts[0].ToCLong(&col);
                parts.RemoveAt(0, 1);
            }
        } else {
            file = pattern;
        }

        clDEBUG() << "Build tab view: firing hostspot event for:" << endl;
        clDEBUG() << "Build tab view: file:" << file << endl;
        clDEBUG() << "Build tab view: line:" << line << endl;
        clDEBUG() << "Build tab view: column:" << col << endl;

        clBuildEvent event_clicked(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED);
        event_clicked.SetBuildDir(wxEmptyString); // can be empty
        event_clicked.SetFileName(file);
        if (line != wxNOT_FOUND) {
            event_clicked.SetLineNumber(line);
        }
        event_clicked.SetProjectName(wxEmptyString);
        if (EventNotifier::Get()->ProcessEvent(event_clicked)) {
            return;
        }

        if (FileUtils::IsBinaryExecutable(file)) {
            return;
        }

        // Default handling for opening files
        // change dir to the active workspace directory
        DirSaver ds;
        clDEBUG() << "Build tab view: using default open file handler" << endl;
        auto workspace = clWorkspaceManager::Get().GetWorkspace();
        if (workspace && !workspace->IsRemote()) {
            ::wxSetWorkingDirectory(wxFileName(workspace->GetFileName()).GetPath());
        }

        OpenEditor(file, line, col);
    }
}

void BuildTabView::OnNextBuildError(wxCommandEvent& event)
{
    event.Skip();
    // get the next line to select
    int mask = (1 << LINE_MARKER);
    size_t from = MarkerNext(GetFirstVisibleLine(), mask);
    if (from == wxString::npos) {
        from = 0;
    } else {
        ++from;
    }
    clDEBUG() << "searching error from line:" << from << endl;
    SelectFirstErrorOrWarning(from, m_onlyErrors);
}

void BuildTabView::OnNextBuildErrorUI(wxUpdateUIEvent& event) { event.Enable(m_warnCount || m_errorCount); }

void BuildTabView::SelectFirstErrorOrWarning(size_t from, bool errors_only)
{
    auto line_info = GetNextLineWithErrorOrWarning(from, errors_only);
    if (line_info.has_value()) {
        SetLineMarker(line_info.value().first);
        OpenEditor(line_info.value().second);
    }
}

std::optional<std::pair<int, std::shared_ptr<LineClientData>>>
BuildTabView::GetNextLineWithErrorOrWarning(size_t from, bool errors_only) const
{
    if (m_errorCount == 0 && (m_warnCount == 0 || errors_only)) {
        return {};
    }

    size_t line_count = GetLineCount();
    if (from >= GetLineCount()) {
        return {};
    }

    if (m_lineInfo.empty()) {
        clDEBUG() << "Empty line info" << endl;
        return {};
    }

    auto iter = m_lineInfo.lower_bound(from);
    while (iter != m_lineInfo.end()) {
        switch (iter->second->match_pattern.sev) {
        case Compiler::kSevWarning:
        case Compiler::kSevNote:
            if (errors_only) {
                // items are sorted, move forward to the next match
                ++iter;
                continue;
            }
            // fall through
        case Compiler::kSevError:
            clDEBUG() << "Found:" << iter->second->message << endl;
            return *iter;
        }
    }
    return {};
}

void BuildTabView::ClearLineMarker() { MarkerDeleteAll(LINE_MARKER); }
void BuildTabView::SetLineMarker(size_t line)
{
    ClearSelections();
    MarkerDeleteAll(LINE_MARKER);
    MarkerAdd(line, LINE_MARKER);
}

void BuildTabView::OpenEditor(const wxString& filename, int line, int col, const wxString& wd)
{
    wxFileName fn(filename);
    if (!wd.empty() && fn.IsRelative()) {
        fn.MakeAbsolute(wd);
    }

    if (!fn.FileExists()) {
        return;
    }

    auto cb = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        // compilers report line numbers starting from `1`
        // our editor sees line numbers starting from `0`
        editor->CenterLine(line - 1, col - 1);
        editor->SetActive();
    };
    clGetManager()->OpenFileAndAsyncExecute(fn.GetFullPath(), std::move(cb));
}

void BuildTabView::OpenEditor(std::shared_ptr<LineClientData> line_info)
{
    clBuildEvent eventErrorClicked(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED);
    eventErrorClicked.SetBuildDir(line_info->root_dir); // can be empty
    eventErrorClicked.SetFileName(line_info->match_pattern.file_path);
    eventErrorClicked.SetLineNumber(line_info->match_pattern.line_number);
    eventErrorClicked.SetProjectName(line_info->project_name);
    clDEBUG1() << "Letting plugins process it first (" << line_info->match_pattern.file_path << ":"
               << line_info->match_pattern.line_number << ")" << endl;
    if (EventNotifier::Get()->ProcessEvent(eventErrorClicked)) {
        return;
    }

    OpenEditor(line_info->match_pattern.file_path,
               line_info->match_pattern.line_number,
               line_info->match_pattern.column,
               line_info->root_dir);
}

void BuildTabView::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    InitialiseView();
}
