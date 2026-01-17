#include "wxTerminalOutputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "Platform/Platform.hpp"
#include "StringUtils.h"
#include "clIdleEventThrottler.hpp"
#include "clSystemSettings.h"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "procutils.h"
#include "wxTerminalCtrl.h"
#include "wxTerminalInputCtrl.hpp"

#include <wx/menu.h>
#include <wx/sizer.h>

namespace
{
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

    void OnCopy(wxCommandEvent& event) override
    {
        CHECK_FOCUS_WINDOW();
        CHECK_PTR_RET(m_stc);
        if (!m_stc->CanCopy()) {
            return;
        }

        auto text = GetSelectedRange(m_stc, m_stc->GetSelectionStart(), m_stc->GetSelectionEnd());
        if (text.empty()) {
            return;
        }
        ::CopyToClipboard(text);
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
    m_editEvents = std::make_unique<MyEventsHandler>(nullptr, m_ctrl);
}

wxTerminalOutputCtrl::wxTerminalOutputCtrl(wxTerminalCtrl* parent,
                                           wxWindowID winid,
                                           const wxFont& font,
                                           const wxColour& bg_colour,
                                           const wxColour& text_colour)
    : wxWindow(parent, winid)
    , m_terminal(parent)
{
    Initialise(font, bg_colour, text_colour);
    m_editEvents = std::make_unique<MyEventsHandler>(nullptr, m_ctrl);
}

void wxTerminalOutputCtrl::SetInputCtrl(wxTerminalInputCtrl* input_ctrl)
{
    m_editEvents = std::make_unique<MyEventsHandler>(input_ctrl, m_ctrl);
}

void wxTerminalOutputCtrl::Initialise(const wxFont& font, const wxColour& bg_colour, const wxColour& text_colour)
{
    m_textFont = wxNullFont;
    m_textColour = text_colour;
    m_bgColour = bg_colour;
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    for (int i = 0; i < wxSTC_MAX_MARGIN; ++i) {
        m_ctrl->SetMarginWidth(i, 0);
    }

    m_ctrl->UsePopUp(0);
    m_ctrl->Bind(wxEVT_CONTEXT_MENU, &wxTerminalOutputCtrl::OnMenu, this);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->SetWrapMode(wxSTC_WRAP_CHAR);
    m_ctrl->SetEditable(false);
    m_ctrl->SetWordChars(R"#(\:~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$/.-+@)#");
    m_ctrl->IndicatorSetStyle(INDICATOR_HYPERLINK, wxSTC_INDIC_COMPOSITIONTHICK);
    auto lexer = ColoursAndFontsManager::Get().GetLexer("terminal");
    if (lexer) {
        lexer->Apply(m_ctrl);
        m_ctrl->IndicatorSetForeground(INDICATOR_HYPERLINK, clColours::Blue(lexer->IsDark()));
    }

    // If font was provided, use it.
    if (font.IsOk()) {
        m_textFont = font;
        for (int style = 0; style < wxSTC_STYLE_MAX; ++style) {
            m_ctrl->StyleSetFont(style, m_textFont);
        }
    }

    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    CallAfter(&wxTerminalOutputCtrl::ReloadSettings);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalOutputCtrl::OnThemeChanged, this);
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &wxTerminalOutputCtrl::OnKeyDown, this);
    m_ctrl->Bind(wxEVT_LEFT_UP, &wxTerminalOutputCtrl::OnLeftUp, this);

    m_ctrl->Bind(wxEVT_KILL_FOCUS, &wxTerminalOutputCtrl::OnFocusLost, this);
    m_ctrl->Bind(wxEVT_SET_FOCUS, &wxTerminalOutputCtrl::OnFocus, this);
}

wxTerminalOutputCtrl::~wxTerminalOutputCtrl()
{
    wxDELETE(m_stcRenderer);
    m_ctrl->Unbind(wxEVT_CHAR_HOOK, &wxTerminalOutputCtrl::OnKeyDown, this);
    m_ctrl->Unbind(wxEVT_LEFT_UP, &wxTerminalOutputCtrl::OnLeftUp, this);

    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalOutputCtrl::OnThemeChanged, this);
    m_ctrl->Unbind(wxEVT_KILL_FOCUS, &wxTerminalOutputCtrl::OnFocusLost, this);
    m_ctrl->Unbind(wxEVT_SET_FOCUS, &wxTerminalOutputCtrl::OnFocus, this);
}

void wxTerminalOutputCtrl::AppendText(const wxString& buffer)
{
    EditorEnabler d{m_ctrl};

    // Remove unwanted ANSI OSC escape sequences
    m_ctrl->AppendText(StringUtils::StripTerminalOSC(buffer));
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

void wxTerminalOutputCtrl::StyleAndAppend(wxStringView buffer, [[maybe_unused]] wxString* window_title)
{
    EditorEnabler enabler{m_ctrl};
    m_ctrl->AppendText(StringUtils::StripTerminalOSC(buffer));
    RequestScrollToEnd();
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
    if (GetNumberOfLines() > 1000) {
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
    EditorEnabler d{m_ctrl};
    m_ctrl->ClearAll();
}

void wxTerminalOutputCtrl::DoScrollToEnd()
{
    m_scrollToEndQueued = false;
    m_ctrl->ScrollToEnd();
}

void wxTerminalOutputCtrl::RequestScrollToEnd()
{
    if (m_scrollToEndQueued) {
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
    auto lexer = ColoursAndFontsManager::Get().GetLexer("terminal");
    if (lexer) {
        lexer->Apply(m_ctrl);
    }

    // If font was provided, use it.
    if (m_textFont.IsOk()) {
        for (int style = 0; style < wxSTC_STYLE_MAX; ++style) {
            m_ctrl->StyleSetFont(style, m_textFont);
        }
    }

    m_ctrl->SetEOLMode(wxSTC_EOL_LF);
    m_ctrl->Refresh();
}

void wxTerminalOutputCtrl::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if (event.ControlDown() || event.AltDown() || event.RawControlDown()) {
        return;
    }

    // pass the focus
    if (m_terminal) {
        m_terminal->GetInputCtrl()->SimulateKeyEvent(event);
    }
}

void wxTerminalOutputCtrl::ProcessIdle()
{
    static clIdleEventThrottler event_throttler{200};
    if (!event_throttler.CanHandle()) {
        return;
    }

    if (!m_ctrl->IsShownOnScreen() || !m_ctrl->IsShown()) {
        ClearIndicators();
        return;
    }

    if (!::wxGetKeyState(WXK_CONTROL)) {
        ClearIndicators();
        m_ctrl->SetSTCCursor(wxSTC_CURSORNORMAL);
        return;
    }

    auto client_pt = m_ctrl->ScreenToClient(::wxGetMousePosition());
    if (!m_ctrl->GetRect().Contains(client_pt)) {
        return;
    }

    int pos = m_ctrl->PositionFromPoint(client_pt);
    int start_pos = m_ctrl->WordStartPosition(pos, true);
    int end_pos = m_ctrl->WordEndPosition(pos, true);
    if (start_pos == end_pos) {
        return;
    }

    IndicatorRange range{start_pos, end_pos};
    if (m_indicatorHyperlink.ok() && m_indicatorHyperlink == range) {
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
    if (m_indicatorHyperlink.ok()) {
        m_ctrl->SetIndicatorCurrent(INDICATOR_HYPERLINK);
        m_ctrl->IndicatorClearRange(m_indicatorHyperlink.start(), m_indicatorHyperlink.length());
        m_indicatorHyperlink.reset();
    }
}

void wxTerminalOutputCtrl::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();

    if (!m_indicatorHyperlink.ok()) {
        return;
    }

    // fire an event
    auto pattern = GetSelectedRange(m_ctrl, m_indicatorHyperlink.start(), m_indicatorHyperlink.end());
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
    if (pattern.StartsWith("https://") || pattern.StartsWith("http://")) {
        m_indicatorHyperlink.reset();
        ::wxLaunchDefaultBrowser(pattern);
        return;
    }

    wxString file;
    wxString line_str;
    wxString col_str;
    auto parts = ::wxStringTokenize(pattern, ":", wxTOKEN_STRTOK);
    if (parts.size() > 1) {
        if (parts[0].length() == 1) {
            // single char -> volume
            // assume windows fullpath
            file = parts[0] + ":" + parts[1];
            parts.RemoveAt(0, 2);
        } else {
            file = parts[0];
            parts.RemoveAt(0, 1);
        }
        if (!parts.empty()) {
            // line number
            line_str = parts[0];
            parts.RemoveAt(0, 1);
        }
        if (!parts.empty()) {
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
    if (!line_str.empty() && line_str.ToCLong(&nLine)) {
        event_clicked.SetLineNumber(nLine);
    }
    event_clicked.SetProjectName(wxEmptyString);
    if (EventNotifier::Get()->ProcessEvent(event_clicked)) {
        return;
    }

    if (FileUtils::IsBinaryExecutable(file)) {
#ifdef __WXMSW__
        // if we are running under Windows and the file path is POSIX (e.g. MSYS2)
        // convert it into Windows native path
        if (file.StartsWith("/")) {
            if (const auto cygpath = ThePlatform->Which("cygpath")) {
                wxString command;
                command << StringUtils::WrapWithDoubleQuotes(*cygpath) << " -w "
                        << StringUtils::WrapWithDoubleQuotes(file);
                wxString winpath = ProcUtils::SafeExecuteCommand(command);
                winpath.Trim().Trim(false);
                if (wxFileName::FileExists(winpath)) {
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
    if (workspace && !workspace->IsRemote()) {
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

void wxTerminalOutputCtrl::OnMenu(wxContextMenuEvent& event)
{
    wxUnusedVar(event);
    wxMenu menu;
    menu.Append(wxID_COPY);
    menu.Append(XRCID("copy-with-ansi-colors"), _("Copy with Terminal Colours"));
    menu.AppendSeparator();
    menu.Append(wxID_SELECTALL);
    menu.AppendSeparator();
    menu.Append(wxID_CLEAR);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            wxString text = GetSelectedRange(m_ctrl, m_ctrl->GetSelectionStart(), m_ctrl->GetSelectionEnd());
            if (text.empty()) {
                return;
            }

            wxString modbuffer;
            StringUtils::StripTerminalColouring(text, modbuffer);
            ::CopyToClipboard(modbuffer);
        },
        wxID_COPY);
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            wxString text = m_ctrl->GetSelectedText();
            if (text.empty()) {
                return;
            }
            ::CopyToClipboard(text);
        },
        XRCID("copy-with-ansi-colors"));
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            m_ctrl->SelectAll();
        },
        wxID_SELECTALL);
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            Clear();
        },
        wxID_CLEAR);
    PopupMenu(&menu);
}

void wxTerminalOutputCtrl::OnFocusLost(wxFocusEvent& event)
{
    clCommandEvent focus_event{wxEVT_STC_LOST_FOCUS};
    EventNotifier::Get()->AddPendingEvent(focus_event);
}

void wxTerminalOutputCtrl::OnFocus(wxFocusEvent& event)
{
    clCommandEvent focus_event{wxEVT_STC_GOT_FOCUS};
    EventNotifier::Get()->AddPendingEvent(focus_event);
}
