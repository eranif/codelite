#include "clSTCContainerStylerBase.hpp"

#include "ColoursAndFontsManager.h"
#include "codelite_events.h"
#include "event_notifier.h"

clSTCContainerStylerBase::clSTCContainerStylerBase(wxStyledTextCtrl* ctrl)
    : m_ctrl(ctrl)
{
    if (m_ctrl) {
        m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
        m_ctrl->SetIdleStyling(wxSTC_IDLESTYLING_AFTERVISIBLE); // style all the visible text at once
    }
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clSTCContainerStylerBase::OnThemChanged, this);
}

clSTCContainerStylerBase::~clSTCContainerStylerBase()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clSTCContainerStylerBase::OnThemChanged, this);
    if (m_ctrl && m_lineStylingEnabled) {
        m_ctrl->Unbind(wxEVT_STC_STYLENEEDED, &clSTCContainerStylerBase::OnStyleNeeded, this);
    }
}

void clSTCContainerStylerBase::EnableLineStyling()
{
    CHECK_PTR_RET(m_ctrl);
    CHECK_COND_RET(!m_lineStylingEnabled);
    m_lineStylingEnabled = true;
    m_ctrl->Bind(wxEVT_STC_STYLENEEDED, &clSTCContainerStylerBase::OnStyleNeeded, this);

    // The control may hold the styles and the line states of another styler: style it again from the start.
    // Styling is done in the order of the lines, so a line always starts with a state that this styler saved.
    m_ctrl->ClearDocumentStyle();
}

std::vector<char> clSTCContainerStylerBase::StyleLines(std::string_view text,
                                                       int doc_offset,
                                                       int start_state,
                                                       const std::function<void(size_t, int)>& on_line_end)
{
    std::vector<char> styles;
    styles.reserve(text.size());
    RestoreLineState(start_state);

    size_t line_start = 0;
    size_t line_index = 0;
    while (line_start < text.size()) {
        // The line with its EOL. Scintilla accepts "\n", "\r\n" and "\r" as the end of a line
        size_t line_end = line_start;
        while (line_end < text.size() && text[line_end] != '\n' && text[line_end] != '\r') {
            ++line_end;
        }
        if (line_end < text.size()) {
            const bool is_crlf = text[line_end] == '\r' && line_end + 1 < text.size() && text[line_end + 1] == '\n';
            line_end += is_crlf ? 2 : 1;
        }

        clSTCLineAccessor accessor{
            text.substr(line_start, line_end - line_start), doc_offset + static_cast<int>(line_start), styles};
        StyleLine(accessor);

        // Each byte must have a style: if the styler stopped early, use the default style for the rest of the line
        if (styles.size() < line_end) {
            styles.resize(line_end, 0);
        }

        const int state = SaveLineState();
        if (on_line_end) {
            on_line_end(line_index, state);
        }
        line_start = line_end;
        ++line_index;
    }
    return styles;
}

void clSTCContainerStylerBase::OnStyleNeeded(wxStyledTextEvent& event)
{
    CHECK_PTR_RET(m_ctrl);

    // Restart at the start of the first line that is not styled, and style up to the end of the line that holds the
    // requested position. Scintilla treats the text after GetEndStyled() as not styled, so an edit restyles from
    // the line of the edit, and only up to what is visible.
    const int first_line = m_ctrl->LineFromPosition(m_ctrl->GetEndStyled());
    const int last_line = m_ctrl->LineFromPosition(event.GetPosition());
    CHECK_COND_RET(last_line >= first_line);

    const int start = m_ctrl->PositionFromLine(first_line);
    const int end =
        (last_line + 1 < m_ctrl->GetLineCount()) ? m_ctrl->PositionFromLine(last_line + 1) : m_ctrl->GetLength();
    CHECK_COND_RET(end > start);

    // Read the text once, and write the styles once. GetRangePointer() gives the text in the buffer of Scintilla,
    // without a copy. The pointer stays valid while the text does not change: styling changes only the styles.
    const char* range = m_ctrl->GetRangePointer(start, end - start);
    CHECK_PTR_RET(range);
    const std::string_view text{range, static_cast<size_t>(end - start)};
    const int start_state = first_line > 0 ? m_ctrl->GetLineState(first_line - 1) : 0;

    std::vector<char> styles = StyleLines(text, start, start_state, [this, first_line](size_t line_index, int state) {
        m_ctrl->SetLineState(first_line + static_cast<int>(line_index), state);
    });

    m_ctrl->StartStyling(start);
    m_ctrl->SetStyleBytes(static_cast<int>(styles.size()), styles.data());
}

void clSTCContainerStylerBase::OnThemChanged(wxCommandEvent& event)
{
    event.Skip();
    InitInternal();
}

void clSTCContainerStylerBase::StyleText(bool full_document)
{
    CHECK_PTR_RET(m_ctrl);

    if (m_lineStylingEnabled) {
        // With a container lexer, Colourise() marks the text as not styled and sends wxEVT_STC_STYLENEEDED
        m_ctrl->Colourise(full_document ? 0 : m_ctrl->GetEndStyled(), wxNOT_FOUND);
        return;
    }

    CHECK_COND_RET(m_on_style_callback != nullptr);

    m_ctrl->StartStyling(full_document ? 0 : m_ctrl->GetEndStyled());
    clSTCAccessor accessor{m_ctrl};
    m_on_style_callback(accessor);
}

void clSTCContainerStylerBase::InitInternal()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_PTR_RET(lexer);

    lexer->Apply(m_ctrl);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    InitStyles();
}
