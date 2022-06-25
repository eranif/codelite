#include "DAPTextView.h"

#include "ColoursAndFontsManager.h"
#include "bookmark_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

namespace
{
constexpr int NUMBER_MARGIN_ID = 0;
constexpr int EDIT_TRACKER_MARGIN_ID = 1;
constexpr int SYMBOLS_MARGIN_ID = 2;
constexpr int SYMBOLS_MARGIN_SEP_ID = 3;
constexpr int FOLD_MARGIN_ID = 4;
} // namespace

DAPTextView::DAPTextView(wxWindow* parent)
    : DAPTextViewBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &DAPTextView::OnColourChanged, this);
    m_stcTextView->SetEditable(false);
    ApplyTheme();

    // define the debugger marker
    auto options = EditorConfigST::Get()->GetOptions();
    if(options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line)) {
        m_stcTextView->MarkerDefine(smt_indicator, wxSTC_MARK_BACKGROUND, wxNullColour,
                                    options->GetDebuggerMarkerLine());
        m_stcTextView->MarkerSetAlpha(smt_indicator, 50);

    } else {
        m_stcTextView->MarkerDefine(smt_indicator, wxSTC_MARK_SHORTARROW);
        wxColour debuggerMarkerColour(136, 170, 0);
        m_stcTextView->MarkerSetBackground(smt_indicator, debuggerMarkerColour);
        m_stcTextView->MarkerSetForeground(smt_indicator, debuggerMarkerColour.ChangeLightness(50));
    }

    // margins
    // line numbers
    m_stcTextView->SetMarginType(NUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);

    // symbol margin
    m_stcTextView->SetMarginType(SYMBOLS_MARGIN_ID, wxSTC_MARGIN_SYMBOL);
    m_stcTextView->SetMarginWidth(SYMBOLS_MARGIN_ID, FromDIP(16));
}

DAPTextView::~DAPTextView()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &DAPTextView::OnColourChanged, this);
}

void DAPTextView::OnColourChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void DAPTextView::ApplyTheme()
{
    wxString file_name = "file.text";
    if(!m_current_source.path.empty()) {
        file_name = wxFileName(m_current_source.path).GetFullName();
    } else if(!m_mimeType.empty()) {
        // try the mime type
        if(m_mimeType == "text/x-lldb.disassembly") {
            file_name = "file.asm"; // assembly
        }
    }

    auto lexer = ColoursAndFontsManager::Get().GetLexerForFile(file_name);
    lexer->Apply(m_stcTextView);
}

void DAPTextView::ClearMarker() { m_stcTextView->MarkerDeleteAll(smt_indicator); }

void DAPTextView::SetMarker(int line_number)
{
    m_stcTextView->MarkerDeleteAll(smt_indicator);
    m_stcTextView->MarkerAdd(line_number, smt_indicator);
    int caretPos = m_stcTextView->PositionFromLine(line_number);
    m_stcTextView->SetSelection(caretPos, caretPos);
    m_stcTextView->SetCurrentPos(caretPos);
    m_stcTextView->EnsureCaretVisible();
}

void DAPTextView::Clear()
{
    m_stcTextView->MarkerDeleteAll(smt_indicator);
    m_stcTextView->ClearAll();
    m_filepath.clear();
}

void DAPTextView::SetText(const dap::Source& source, const wxString& text, const wxString& path,
                          const wxString& mimeType)
{
    m_stcTextView->SetEditable(true);
    m_stcTextView->SetText(text);
    m_stcTextView->SetEditable(false);
    m_current_source = source;
    m_mimeType = mimeType;
    SetFilePath(path);
    UpdateLineNumbersMargin();
    ApplyTheme();
}

void DAPTextView::LoadFile(const dap::Source& source, const wxString& filepath)
{
    m_stcTextView->SetEditable(true);
    bool ok = m_stcTextView->LoadFile(filepath);
    m_stcTextView->SetEditable(false);
    m_mimeType.clear();

    if(ok) {
        SetFilePath(filepath);
        m_current_source = source;
    } else {
        Clear();
    }

    UpdateLineNumbersMargin();
    ApplyTheme();
}

void DAPTextView::SetFilePath(const wxString& filepath)
{
    m_filepath = filepath;
    auto book = clGetManager()->GetMainNotebook();
    int where = book->FindPage(this);
    if(where == wxNOT_FOUND) {
        return;
    }
    book->SetPageText(where, "[dap]: " + m_filepath);
}

void DAPTextView::UpdateLineNumbersMargin()
{
    int newLineCount = m_stcTextView->GetLineCount();
    int newWidthCount = log10(newLineCount) + 2;
    int size = FromDIP(newWidthCount * m_stcTextView->TextWidth(wxSTC_STYLE_LINENUMBER, "X"));
    m_stcTextView->SetMarginWidth(NUMBER_MARGIN_ID, size);
}
