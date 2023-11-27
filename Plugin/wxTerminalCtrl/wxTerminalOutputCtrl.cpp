#include "wxTerminalOutputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "FontUtils.hpp"
#include "Platform.hpp"
#include "StringUtils.h"
#include "clModuleLogger.hpp"
#include "clSystemSettings.h"
#include "clWorkspaceManager.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "procutils.h"
#include "wxTerminalCtrl.h"
#include "wxTerminalInputCtrl.hpp"

#include <wx/uiaction.h>

INITIALISE_MODULE_LOG(LOG, "AnsiEscapeHandler", "ansi_escape_parser.log");

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/wupdlock.h>

namespace
{
class MyEventsHandler : public clEditEventsHandler
{
    wxTerminalInputCtrl* m_input_ctrl = nullptr;

public:
    MyEventsHandler(wxTerminalInputCtrl* input_ctrl, wxStyledTextCtrl* ctrl)
        : clEditEventsHandler(ctrl)
        , m_input_ctrl(input_ctrl)
    {
    }

    void OnPaste(wxCommandEvent& event) override
    {
        CHECK_FOCUS_WINDOW();
        CHECK_PTR_RET(m_input_ctrl);
        m_input_ctrl->Paste();
    }
};

struct EditorEnabler {
    wxStyledTextCtrl* m_ctrl = nullptr;
    EditorEnabler(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
        m_ctrl->SetEditable(true);
    }
    ~EditorEnabler() { m_ctrl->SetEditable(false); }
};
} // namespace

wxTerminalOutputCtrl::wxTerminalOutputCtrl(wxWindow* parent, wxWindowID winid)
    : wxWindow(parent, winid)
    , m_terminal(nullptr)
{
    Initialise();
    m_editEvents.Reset(new MyEventsHandler(nullptr, m_ctrl));
}

wxTerminalOutputCtrl::wxTerminalOutputCtrl(wxTerminalCtrl* parent, wxWindowID winid, const wxFont& font,
                                           const wxColour& bg_colour, const wxColour& text_colour)
    : wxWindow(parent, winid)
    , m_terminal(parent)
{
    Initialise(font, bg_colour, text_colour);
    m_editEvents.Reset(new MyEventsHandler(nullptr, m_ctrl));
}

void wxTerminalOutputCtrl::SetInputCtrl(wxTerminalInputCtrl* input_ctrl)
{
    m_editEvents.Reset(new MyEventsHandler(input_ctrl, m_ctrl));
}

void wxTerminalOutputCtrl::Initialise(const wxFont& font, const wxColour& bg_colour, const wxColour& text_colour)
{
    m_textFont = font.IsOk() ? font : FontUtils::GetDefaultMonospacedFont();
    m_textColour = text_colour;
    m_bgColour = bg_colour;
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    for(int i = 0; i < wxSTC_MAX_MARGIN; ++i) {
        m_ctrl->SetMarginWidth(i, 0);
    }

    m_ctrl->UsePopUp(1);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->StartStyling(0);
    m_ctrl->SetWrapMode(wxSTC_WRAP_CHAR);
    m_ctrl->SetEditable(false);
    m_ctrl->SetWordChars(R"#(\:~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$/.-)#");
    m_ctrl->IndicatorSetStyle(INDICATOR_HYPERLINK, wxSTC_INDIC_PLAIN);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    CallAfter(&wxTerminalOutputCtrl::ReloadSettings);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalOutputCtrl::OnThemeChanged, this);
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &wxTerminalOutputCtrl::OnKeyDown, this);
    m_ctrl->Bind(wxEVT_IDLE, &wxTerminalOutputCtrl::OnIdle, this);
    m_ctrl->Bind(wxEVT_LEFT_UP, &wxTerminalOutputCtrl::OnLeftUp, this);
    m_stcRenderer = new wxTerminalAnsiRendererSTC(m_ctrl);
}

wxTerminalOutputCtrl::~wxTerminalOutputCtrl()
{
    wxDELETE(m_stcRenderer);
    m_ctrl->Unbind(wxEVT_CHAR_HOOK, &wxTerminalOutputCtrl::OnKeyDown, this);
    m_ctrl->Unbind(wxEVT_IDLE, &wxTerminalOutputCtrl::OnIdle, this);
    m_ctrl->Unbind(wxEVT_LEFT_UP, &wxTerminalOutputCtrl::OnLeftUp, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalOutputCtrl::OnThemeChanged, this);
}

void wxTerminalOutputCtrl::AppendText(const wxString& buffer)
{
    EditorEnabler d{ m_ctrl };
    m_ctrl->AppendText(buffer);
    RequestScrollToEnd();
}

long wxTerminalOutputCtrl::GetLastPosition() const { return m_ctrl->GetLastPosition(); }

wxString wxTerminalOutputCtrl::GetRange(int from, int to) const { return m_ctrl->GetRange(from, to); }

bool wxTerminalOutputCtrl::PositionToXY(long pos, long* x, long* y) const { return m_ctrl->PositionToXY(pos, x, y); }

long wxTerminalOutputCtrl::XYToPosition(long x, long y) const { return m_ctrl->XYToPosition(x, y); }

void wxTerminalOutputCtrl::Remove(long from, long to) { return m_ctrl->Remove(from, to); }

void wxTerminalOutputCtrl::SetInsertionPoint(long pos) { m_ctrl->SetInsertionPoint(pos); }

void wxTerminalOutputCtrl::SelectNone() { m_ctrl->SelectNone(); }

void wxTerminalOutputCtrl::SetInsertionPointEnd() { m_ctrl->SetInsertionPointEnd(); }

int wxTerminalOutputCtrl::GetNumberOfLines() const { return m_ctrl->GetNumberOfLines(); }

void wxTerminalOutputCtrl::SetDefaultStyle(const wxTextAttr& attr) { m_defaultAttr = attr; }

wxTextAttr wxTerminalOutputCtrl::GetDefaultStyle() const { return m_defaultAttr; }

long wxTerminalOutputCtrl::GetInsertionPoint() const { return m_ctrl->GetInsertionPoint(); }

void wxTerminalOutputCtrl::Replace(long from, long to, const wxString& replaceWith)
{
    m_ctrl->Replace(from, to, replaceWith);
}

wxString wxTerminalOutputCtrl::GetLineText(int lineNumber) const { return m_ctrl->GetLineText(lineNumber); }

void wxTerminalOutputCtrl::ReloadSettings() { ApplyTheme(); }

void wxTerminalOutputCtrl::StyleAndAppend(wxStringView buffer, wxString* window_title)
{
    size_t consumed = m_outputHandler.ProcessBuffer(buffer, m_stcRenderer);
    if(window_title) {
        *window_title = m_stcRenderer->GetWindowTitle();
    }
}

void wxTerminalOutputCtrl::ShowCommandLine()
{
    m_ctrl->SetSelection(m_ctrl->GetLastPosition(), m_ctrl->GetLastPosition());
    m_ctrl->EnsureCaretVisible();
    RequestScrollToEnd();
}

void wxTerminalOutputCtrl::SetCaretEnd()
{
    m_ctrl->SelectNone();
    m_ctrl->SetSelection(GetLastPosition(), GetLastPosition());
    m_ctrl->SetCurrentPos(GetLastPosition());
}

int wxTerminalOutputCtrl::Truncate()
{
    if(GetNumberOfLines() > 1000) {
        // Start removing lines from the top
        long linesToRemove = (GetNumberOfLines() - 1000);
        long startPos = 0;
        long endPos = XYToPosition(0, linesToRemove);
        this->Remove(startPos, endPos);
        return endPos - startPos;
    }
    return 0;
}

wxChar wxTerminalOutputCtrl::GetLastChar() const { return m_ctrl->GetCharAt(m_ctrl->GetLastPosition() - 1); }

int wxTerminalOutputCtrl::GetCurrentStyle() { return 0; }

void wxTerminalOutputCtrl::Clear()
{
    m_stcRenderer->Clear();
    EditorEnabler d{ m_ctrl };
    m_ctrl->ClearAll();
}

void wxTerminalOutputCtrl::DoScrollToEnd()
{
    m_scrollToEndQueued = false;
    m_ctrl->ScrollToEnd();
}

void wxTerminalOutputCtrl::RequestScrollToEnd()
{
    if(m_scrollToEndQueued) {
        return;
    }
    m_scrollToEndQueued = true;
    CallAfter(&wxTerminalOutputCtrl::DoScrollToEnd);
}

void wxTerminalOutputCtrl::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void wxTerminalOutputCtrl::ApplyTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_ctrl);

    auto style = lexer->GetProperty(0);
    wxTextAttr defaultAttr = wxTextAttr(style.GetFgColour(), style.GetBgColour(), lexer->GetFontForStyle(0, m_ctrl));
    SetDefaultStyle(defaultAttr);
    m_stcRenderer->SetDefaultAttributes(defaultAttr);
    m_stcRenderer->SetUseDarkThemeColours(lexer->IsDark());
    m_ctrl->IndicatorSetForeground(INDICATOR_HYPERLINK, lexer->IsDark() ? wxColour("WHITE") : wxColour("BLUE"));
    m_ctrl->Refresh();
}

void wxTerminalOutputCtrl::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.ControlDown() || event.AltDown() || event.RawControlDown()) {
        return;
    }

    // pass the focus
    if(m_terminal) {
        m_terminal->GetInputCtrl()->SimulateKeyEvent(event);
    }
}

void wxTerminalOutputCtrl::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(!m_ctrl->IsShownOnScreen() || !m_ctrl->IsShown()) {
        ClearIndicators();
        return;
    }

    if(!::wxGetKeyState(WXK_CONTROL)) {
        ClearIndicators();
        m_ctrl->SetSTCCursor(wxSTC_CURSORNORMAL);
        return;
    }

    auto client_pt = m_ctrl->ScreenToClient(::wxGetMousePosition());
    if(!m_ctrl->GetRect().Contains(client_pt)) {
        return;
    }

    int pos = m_ctrl->PositionFromPoint(client_pt);
    int word_start_pos = m_ctrl->WordStartPosition(pos, true);
    int word_end_pos = m_ctrl->WordEndPosition(pos, true);
    IndicatorRange range{ word_start_pos, word_end_pos };
    if(m_indicatorHyperlink.ok() && m_indicatorHyperlink == range) {
        // already marked
        return;
    }

    // clear the current
    ClearIndicators();

    // set new
    m_ctrl->SetIndicatorCurrent(INDICATOR_HYPERLINK);
    m_ctrl->IndicatorFillRange(range.start(), range.length());
    m_ctrl->SetSTCCursor(8); // hand cursor
    m_indicatorHyperlink = range;
}

void wxTerminalOutputCtrl::ClearIndicators()
{
    if(m_indicatorHyperlink.ok()) {
        m_ctrl->SetIndicatorCurrent(INDICATOR_HYPERLINK);
        m_ctrl->IndicatorClearRange(m_indicatorHyperlink.start(), m_indicatorHyperlink.length());
        m_indicatorHyperlink.reset();
    }
}

void wxTerminalOutputCtrl::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();

    if(!m_indicatorHyperlink.ok()) {
        return;
    }

    // fire an event
    wxString pattern = m_ctrl->GetTextRange(m_indicatorHyperlink.start(), m_indicatorHyperlink.end());
    CallAfter(&wxTerminalOutputCtrl::DoPatternClicked, pattern);
}

void wxTerminalOutputCtrl::OnEnterWindow(wxMouseEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_ctrl);
}

void wxTerminalOutputCtrl::OnLeaveWindow(wxMouseEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_ctrl);
}

void wxTerminalOutputCtrl::DoPatternClicked(const wxString& pattern)
{
    // if the pattern matches a URL, open it
    if(pattern.StartsWith("https://") || pattern.StartsWith("http://")) {
        m_indicatorHyperlink.reset();
        ::wxLaunchDefaultBrowser(pattern);
        return;
    }

    wxString file;
    wxString line_str;
    wxString col_str;
    auto parts = ::wxStringTokenize(pattern, ":", wxTOKEN_STRTOK);
    if(parts.size() > 1) {
        if(parts[0].length() == 1) {
            // single char -> volume
            // assume windows fullpath
            file = parts[0] + ":" + parts[1];
            parts.RemoveAt(0, 2);
        } else {
            file = parts[0];
            parts.RemoveAt(0, 1);
        }
        if(!parts.empty()) {
            // line number
            line_str = parts[0];
            parts.RemoveAt(0, 1);
        }
        if(!parts.empty()) {
            // column
            col_str = parts[0];
            parts.RemoveAt(0, 1);
        }
    } else {
        file = pattern;
    }

    clDEBUG() << "Terminal: firing hostspot event for:" << endl;
    clDEBUG() << "Terminal: file:" << file << endl;
    clDEBUG() << "Terminal: line:" << line_str << endl;
    clDEBUG() << "Terminal: column:" << col_str << endl;

    clBuildEvent event_clicked(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED);
    event_clicked.SetBuildDir(wxEmptyString); // can be empty
    event_clicked.SetFileName(file);
    event_clicked.SetLineNumber(0);
    long nLine = 0;
    if(!line_str.empty() && line_str.ToCLong(&nLine)) {
        event_clicked.SetLineNumber(nLine);
    }
    event_clicked.SetProjectName(wxEmptyString);
    if(EventNotifier::Get()->ProcessEvent(event_clicked)) {
        return;
    }

    if(FileUtils::IsBinaryExecutable(file)) {
#ifdef __WXMSW__
        // if we are running under Windows and the file path is POSIX (e.g. MSYS2)
        // convert it into Windows native path
        if(file.StartsWith("/")) {
            wxString cygpath;
            if(ThePlatform->Which("cygpath", &cygpath)) {
                wxString command;
                command << StringUtils::WrapWithDoubleQuotes(cygpath) << " -w "
                        << StringUtils::WrapWithDoubleQuotes(file);
                wxString winpath = ProcUtils::SafeExecuteCommand(command);
                winpath.Trim().Trim(false);
                if(wxFileName::FileExists(winpath)) {
                    file = StringUtils::WrapWithDoubleQuotes(winpath);
                }
            }
        }
#endif
        // do not attempt to open a biary file
        ::wxLaunchDefaultApplication(file);
        return;
    }

    // Default handling for opening files
    // change dir to the active workspace directory
    DirSaver ds;
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if(workspace && !workspace->IsRemote()) {
        ::wxSetWorkingDirectory(wxFileName(workspace->GetFileName()).GetPath());
    }

    auto cb = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        // compilers report line numbers starting from `1`
        // our editor sees line numbers starting from `0`
        editor->CenterLine(nLine > 0 ? nLine - 1 : 0, wxNOT_FOUND);
        editor->SetActive();
    };
    clGetManager()->OpenFileAndAsyncExecute(file, std::move(cb));
}
