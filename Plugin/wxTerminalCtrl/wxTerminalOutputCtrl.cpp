#include "wxTerminalOutputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "FontUtils.hpp"
#include "clModuleLogger.hpp"
#include "clSystemSettings.h"
#include "clWorkspaceManager.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
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

TextView::TextView(wxWindow* parent, wxTerminalInputCtrl* inputCtrl, wxWindowID winid)
    : wxWindow(parent, winid)
    , m_terminal(nullptr)
{
    Initialise();
    m_editEvents.Reset(new MyEventsHandler(inputCtrl, m_ctrl));
}

TextView::TextView(wxTerminalCtrl* parent, wxTerminalInputCtrl* inputCtrl, wxWindowID winid, const wxFont& font,
                   const wxColour& bg_colour, const wxColour& text_colour)
    : wxWindow(parent, winid)
    , m_terminal(parent)
{
    Initialise(font, bg_colour, text_colour);
    m_editEvents.Reset(new MyEventsHandler(inputCtrl, m_ctrl));
}

void TextView::Initialise(const wxFont& font, const wxColour& bg_colour, const wxColour& text_colour)
{
    m_textFont = font.IsOk() ? font : FontUtils::GetDefaultMonospacedFont();
    m_textColour = text_colour;
    m_bgColour = bg_colour;
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_ctrl->UsePopUp(1);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->StartStyling(0);
    m_ctrl->SetWrapMode(wxSTC_WRAP_CHAR);
    m_ctrl->SetEditable(false);
    m_ctrl->SetWordChars(R"#(\:~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$/.-)#");
    m_ctrl->IndicatorSetStyle(INDICATOR_HYPERLINK, wxSTC_INDIC_PLAIN);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    CallAfter(&TextView::ReloadSettings);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &TextView::OnThemeChanged, this);
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &TextView::OnKeyDown, this);
    m_ctrl->Bind(wxEVT_IDLE, &TextView::OnIdle, this);
    m_ctrl->Bind(wxEVT_LEFT_UP, &TextView::OnLeftUp, this);
    m_stcRenderer = new wxTerminalAnsiRendererSTC(m_ctrl);
}

TextView::~TextView()
{
    wxDELETE(m_stcRenderer);
    m_ctrl->Unbind(wxEVT_CHAR_HOOK, &TextView::OnKeyDown, this);
    m_ctrl->Unbind(wxEVT_IDLE, &TextView::OnIdle, this);
    m_ctrl->Unbind(wxEVT_LEFT_UP, &TextView::OnLeftUp, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &TextView::OnThemeChanged, this);
}

void TextView::AppendText(const wxString& buffer)
{
    EditorEnabler d{ m_ctrl };
    m_ctrl->AppendText(buffer);
    RequestScrollToEnd();
}

long TextView::GetLastPosition() const { return m_ctrl->GetLastPosition(); }

wxString TextView::GetRange(int from, int to) const { return m_ctrl->GetRange(from, to); }

bool TextView::PositionToXY(long pos, long* x, long* y) const { return m_ctrl->PositionToXY(pos, x, y); }

long TextView::XYToPosition(long x, long y) const { return m_ctrl->XYToPosition(x, y); }

void TextView::Remove(long from, long to) { return m_ctrl->Remove(from, to); }

void TextView::SetInsertionPoint(long pos) { m_ctrl->SetInsertionPoint(pos); }

void TextView::SelectNone() { m_ctrl->SelectNone(); }

void TextView::SetInsertionPointEnd() { m_ctrl->SetInsertionPointEnd(); }

int TextView::GetNumberOfLines() const { return m_ctrl->GetNumberOfLines(); }

void TextView::SetDefaultStyle(const wxTextAttr& attr) { m_defaultAttr = attr; }

wxTextAttr TextView::GetDefaultStyle() const { return m_defaultAttr; }

long TextView::GetInsertionPoint() const { return m_ctrl->GetInsertionPoint(); }

void TextView::Replace(long from, long to, const wxString& replaceWith) { m_ctrl->Replace(from, to, replaceWith); }

wxString TextView::GetLineText(int lineNumber) const { return m_ctrl->GetLineText(lineNumber); }

void TextView::ReloadSettings() { ApplyTheme(); }

void TextView::StyleAndAppend(wxStringView buffer, wxString* window_title)
{
    size_t consumed = m_outputHandler.ProcessBuffer(buffer, m_stcRenderer);
    if(window_title) {
        *window_title = m_stcRenderer->GetWindowTitle();
    }
}

void TextView::ShowCommandLine()
{
    m_ctrl->SetSelection(m_ctrl->GetLastPosition(), m_ctrl->GetLastPosition());
    m_ctrl->EnsureCaretVisible();
    RequestScrollToEnd();
}

void TextView::SetCaretEnd()
{
    m_ctrl->SelectNone();
    m_ctrl->SetSelection(GetLastPosition(), GetLastPosition());
    m_ctrl->SetCurrentPos(GetLastPosition());
}

int TextView::Truncate()
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

wxChar TextView::GetLastChar() const { return m_ctrl->GetCharAt(m_ctrl->GetLastPosition() - 1); }

int TextView::GetCurrentStyle() { return 0; }

void TextView::Clear()
{
    m_stcRenderer->Clear();
    EditorEnabler d{ m_ctrl };
    m_ctrl->ClearAll();
}

void TextView::DoScrollToEnd()
{
    m_scrollToEndQueued = false;
    m_ctrl->ScrollToEnd();
}

void TextView::RequestScrollToEnd()
{
    if(m_scrollToEndQueued) {
        return;
    }
    m_scrollToEndQueued = true;
    CallAfter(&TextView::DoScrollToEnd);
}

void TextView::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void TextView::ApplyTheme()
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

void TextView::OnKeyDown(wxKeyEvent& event)
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

void TextView::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(!m_ctrl->IsShownOnScreen() || !m_ctrl->IsShown()) {
        ClearIndicators();
        return;
    }

    if(!::wxGetKeyState(WXK_CONTROL)) {
        ClearIndicators();
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
    if(m_indicatorHyperlink.is_ok() && m_indicatorHyperlink == range) {
        // already marked
        return;
    }

    // clear the current
    ClearIndicators();

    // set new
    m_ctrl->SetIndicatorCurrent(INDICATOR_HYPERLINK);
    m_ctrl->IndicatorFillRange(range.get_start(), range.length());
    m_indicatorHyperlink = range;
}

void TextView::ClearIndicators()
{
    if(m_indicatorHyperlink.is_ok()) {
        m_ctrl->SetIndicatorCurrent(INDICATOR_HYPERLINK);
        m_ctrl->IndicatorClearRange(m_indicatorHyperlink.get_start(), m_indicatorHyperlink.length());
        m_indicatorHyperlink.reset();
    }
}

void TextView::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();

    if(!m_indicatorHyperlink.is_ok()) {
        return;
    }

    // fire an event
    wxString pattern = m_ctrl->GetTextRange(m_indicatorHyperlink.get_start(), m_indicatorHyperlink.get_end());
    CallAfter(&TextView::DoPatternClicked, pattern);
}

void TextView::OnEnterWindow(wxMouseEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_ctrl);
}

void TextView::OnLeaveWindow(wxMouseEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_ctrl);
}

void TextView::DoPatternClicked(const wxString& pattern)
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
