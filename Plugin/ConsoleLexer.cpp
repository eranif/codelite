#include "ConsoleLexer.hpp"

#include "ExtraLexers.h"
#include "file_logger.h"

namespace
{
class StcAccessor : public AccessorInterface
{
public:
    StcAccessor(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
    }
    const char operator[](size_t index) const override { return SafeGetCharAt(index); }
    char SafeGetCharAt(size_t index, char chDefault = ' ') const override
    {
        if (index >= m_ctrl->GetLength()) {
            return chDefault;
        }
        return m_ctrl->GetCharAt(index);
    }

    /// Style from the current position to the `pos`
    void ColourTo(size_t pos, int style) override
    {
        pos += 1;
        size_t last_styled_pos = m_ctrl->GetEndStyled();
        CHECK_COND_RET(pos > last_styled_pos);
        m_ctrl->SetStyling(pos - last_styled_pos, style);
    }

    void StartAt(size_t start) override { m_ctrl->StartStyling(start); }
    void StartSegment(size_t pos) override { m_ctrl->StartStyling(pos); }
    int GetPropertyInt(const std::string& name, int defaultVal = 0) const override
    {
        wxUnusedVar(name);
        wxUnusedVar(defaultVal);
        return 1;
    };

private:
    wxStyledTextCtrl* m_ctrl = nullptr;
};
} // namespace

ConsoleLexer::ConsoleLexer(wxStyledTextCtrl* stc)
    : m_stc(stc)
{
    m_stc->Bind(wxEVT_STC_STYLENEEDED, &ConsoleLexer::OnStyleNeeded, this);
    CallAfter(&ConsoleLexer::SetStyles);
}

/// Apply the lexer's "terminal" styles for this instance managed wxSTC
void ConsoleLexer::SetStyles()
{
    m_stc->SetProperty("lexer.errorlist.escape.sequences", "1");
    m_stc->SetProperty("lexer.errorlist.value.separate", "1");
    m_stc->SetProperty("lexer.terminal.escape.sequences", "1");
    m_stc->SetProperty("lexer.terminal.value.separate", "1");

    // Hide escape sequence styles
    m_stc->StyleSetVisible(wxSTC_TERMINAL_ESCSEQ, false);
    m_stc->StyleSetVisible(wxSTC_TERMINAL_ESCSEQ_UNKNOWN, false);
}

void ConsoleLexer::OnStyleNeeded(wxStyledTextEvent& e)
{
    e.Skip();
    int startPos = m_stc->GetEndStyled();
    int endPos = e.GetPosition();

    StcAccessor styler(m_stc);
    ::LexerTerminalStyle(startPos, (endPos - startPos), styler);
}
