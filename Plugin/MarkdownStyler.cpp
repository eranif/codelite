#include "MarkdownStyler.hpp"

#include "ColoursAndFontsManager.h"

namespace
{
enum class State {
    kDefault,
    kCodeBlock,
    kCodeBlockTag,
    kCodeWord,
    kStrongText,
    kStrong2Text,
    kEmphasisText,
    kEmphasis2Text,
    kHeaderText,
};

/// Check if `sv` is a number
bool IsNumber(wxStringView sv)
{
    wxString s{sv.data(), sv.length()};
    long v{wxNOT_FOUND};
    return s.ToCLong(&v);
}

} // namespace

MarkdownStyler::MarkdownStyler(wxStyledTextCtrl* ctrl)
    : clSTCContainerStylerBase(ctrl)
{
    InitInternal(); // Will call the pure virtual method "InitStyles"
}

void MarkdownStyler::Reset() {}

void MarkdownStyler::InitStyles()
{
    wxColour default_bg, default_fg, code_bg;
    auto markdown_lexer = ColoursAndFontsManager::Get().GetLexer("markdown");
    CHECK_PTR_RET(markdown_lexer);

    auto rust_lexer = ColoursAndFontsManager::Get().GetLexer("rust");
    CHECK_PTR_RET(rust_lexer);

    bool is_dark = markdown_lexer->IsDark();
    auto default_prop = markdown_lexer->GetProperty(wxSTC_MARKDOWN_DEFAULT);
    default_bg = default_prop.GetBgColour();
    default_fg = default_prop.GetFgColour();
    code_bg = default_bg.ChangeLightness(is_dark ? 110 : 90);

    for (int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_ctrl->StyleSetBackground(i, default_bg);
    }

    auto keyword = rust_lexer->GetProperty(wxSTC_RUST_WORD2);
    auto variable = rust_lexer->GetProperty(wxSTC_RUST_WORD);
    auto string = rust_lexer->GetProperty(wxSTC_RUST_STRING);
    auto block_comment = rust_lexer->GetProperty(wxSTC_RUST_COMMENTBLOCK);

    auto header_1 = markdown_lexer->GetProperty(wxSTC_MARKDOWN_HEADER1);
    auto header_2 = markdown_lexer->GetProperty(wxSTC_MARKDOWN_HEADER2);
    auto other_headers = markdown_lexer->GetProperty(wxSTC_MARKDOWN_HEADER3);
    auto function = markdown_lexer->GetProperty(wxSTC_MARKDOWN_CODE);
    auto link = markdown_lexer->GetProperty(wxSTC_MARKDOWN_LINK);

    m_ctrl->StyleSetForeground(MarkdownStyles::kDefault, default_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDefault, default_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHorizontalLine, block_comment.GetFgColour());

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockTag, keyword.GetFgColour());
    m_ctrl->StyleSetForeground(MarkdownStyles::kBacktick, string.GetFgColour());

    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kCodeBlockText, true);
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockText, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeWord, function.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeWord, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader1, header_1.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader2, header_2.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeaderText, default_fg);
    m_ctrl->StyleSetBold(MarkdownStyles::kHeaderText, true);
    m_ctrl->StyleSetItalic(MarkdownStyles::kHeaderText, true);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kHeaderText, true);

    const std::vector<int> header_styles = {kHeader2, kHeader3, kHeader4, kHeader5, kHeader6};
    for (int style : header_styles) {
        m_ctrl->StyleSetForeground(style, other_headers.GetFgColour());
    }

    // Strong style
    m_ctrl->StyleSetForeground(MarkdownStyles::kStrongText, default_fg);
    m_ctrl->StyleSetItalic(MarkdownStyles::kStrongText, true);
    m_ctrl->StyleSetForeground(MarkdownStyles::kStrongTag, variable.GetFgColour());

    m_ctrl->StyleSetForeground(MarkdownStyles::kStrong2Text, default_fg);
    m_ctrl->StyleSetBold(MarkdownStyles::kStrong2Text, true);
    m_ctrl->StyleSetForeground(MarkdownStyles::kStrong2Tag, variable.GetFgColour());

    // Emphasis style
    m_ctrl->StyleSetForeground(MarkdownStyles::kEmphasisText, default_fg);
    m_ctrl->StyleSetItalic(MarkdownStyles::kEmphasisText, true);
    m_ctrl->StyleSetForeground(MarkdownStyles::kEmphasisTag, variable.GetFgColour());

    m_ctrl->StyleSetForeground(MarkdownStyles::kEmphasis2Text, default_fg);
    m_ctrl->StyleSetItalic(MarkdownStyles::kEmphasis2Text, true);
    m_ctrl->StyleSetForeground(MarkdownStyles::kEmphasis2Tag, variable.GetFgColour());

    // List items
    m_ctrl->StyleSetForeground(MarkdownStyles::kNumberedListItem, string.GetFgColour());
    m_ctrl->StyleSetForeground(MarkdownStyles::kNumberedListItemDot, header_1.GetFgColour());
    m_ctrl->StyleSetForeground(MarkdownStyles::kListItem, header_1.GetFgColour());

    SetStyleCallback([this](clSTCAccessor& accessor) { this->OnStyle(accessor); });
}

void MarkdownStyler::OnStyle(clSTCAccessor& accessor)
{
    State state = State::kDefault;
    while (accessor.CanNext()) {
        wxChar ch = accessor.GetCurrentChar();
        wxUniChar uni_char{ch};
        int default_step{1};
        if (!uni_char.IsAscii()) {
            wxString as_str(uni_char);
            default_step = StringUtils::UTF8Length(as_str.wc_str(), as_str.length());
        }

        switch (state) {
        case State::kDefault:
            // we support up to 99 bullets.
            if (::wxIsdigit(ch) && accessor.IsAtStartOfLine()) {
                if (::wxIsdigit(accessor.GetCharAt(1)) && accessor.GetCharAt(2) == '.') {
                    accessor.SetStyle(MarkdownStyles::kNumberedListItem, 2);
                    accessor.SetStyle(MarkdownStyles::kNumberedListItemDot, 1);
                    break;
                } else if (accessor.GetCharAt(1) == '.') {
                    accessor.SetStyle(MarkdownStyles::kNumberedListItem, 1);
                    accessor.SetStyle(MarkdownStyles::kNumberedListItemDot, 1);
                    break;
                }
            }

            // not a number, run the switch statement.
            switch (ch) {
            case '#':
                if (accessor.IsAtStartOfLine()) {
                    if (accessor.GetSubstr(6) == "######") {
                        accessor.SetStyle(MarkdownStyles::kHeader6, 6);
                    } else if (accessor.GetSubstr(5) == "#####") {
                        accessor.SetStyle(MarkdownStyles::kHeader5, 5);
                    } else if (accessor.GetSubstr(4) == "####") {
                        accessor.SetStyle(MarkdownStyles::kHeader4, 4);
                    } else if (accessor.GetSubstr(3) == "###") {
                        accessor.SetStyle(MarkdownStyles::kHeader3, 3);
                    } else if (accessor.GetSubstr(2) == "##") {
                        accessor.SetStyle(MarkdownStyles::kHeader2, 2);
                    } else {
                        accessor.SetStyle(MarkdownStyles::kHeader1, 1);
                    }
                    state = State::kHeaderText;
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '-':
                if (accessor.IsAtStartOfLine() && accessor.GetCharAt(1) == ' ') {
                    accessor.SetStyle(MarkdownStyles::kListItem, 2);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '*':
                if (accessor.GetSubstr(2) == "**" && accessor.CurrentLineContains(2, "**")) {
                    accessor.SetStyle(MarkdownStyles::kStrong2Tag, 2);
                    state = State::kStrong2Text;
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '_':
                if (accessor.GetSubstr(2) == "__" && accessor.CurrentLineContains(2, "__")) {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Tag, 2);
                    state = State::kEmphasis2Text;
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '`':
                if (accessor.GetSubstr(3) == "```") {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 3);
                    state = State::kCodeBlockTag;
                } else {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                    state = State::kCodeWord;
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kDefault, 1, default_step);
                break;
            }
            break;
        case State::kCodeBlockTag:
            switch (ch) {
            case '\n':
                accessor.SetStyle(MarkdownStyles::kCodeBlockTag, 1);
                state = State::kCodeBlock;
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kCodeBlockTag, 1, default_step);
                break;
            }
            break;
        case State::kCodeBlock:
            switch (ch) {
            case '`':
                if (accessor.GetSubstr(3) == "```") {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 3);
                    state = State::kDefault;
                } else {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kCodeBlockText, 1, default_step);
                break;
            }
            break;
        case State::kCodeWord:
            switch (ch) {
            case '`':
                accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                state = State::kDefault;
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kCodeWord, 1, default_step);
                break;
            }
            break;
        case State::kHeaderText:
            switch (ch) {
            case '\n':
                accessor.SetStyle(MarkdownStyles::kHeaderText, 1);
                state = State::kDefault;
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kHeaderText, 1, default_step);
                break;
            }
            break;
        case State::kStrong2Text:
            switch (ch) {
            case '*':
                if (accessor.GetSubstr(2) == "**") {
                    accessor.SetStyle(MarkdownStyles::kStrong2Tag, 2);
                    state = State::kDefault;
                } else {
                    accessor.SetStyle(MarkdownStyles::kStrong2Text, 1);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kStrong2Text, 1, default_step);
                break;
            }
            break;
        case State::kStrongText:
            switch (ch) {
            case '*':
                accessor.SetStyle(MarkdownStyles::kStrongTag, 1);
                state = State::kDefault;
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kStrongText, 1, default_step);
                break;
            }
            break;
        case State::kEmphasis2Text:
            switch (ch) {
            case '_':
                if (accessor.GetSubstr(2) == "__") {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Tag, 2);
                    state = State::kDefault;
                } else {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Text, 1);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kEmphasis2Text, 1, default_step);
                break;
            }
            break;
        case State::kEmphasisText:
            switch (ch) {
            case '_':
                accessor.SetStyle(MarkdownStyles::kEmphasisTag, 1);
                state = State::kDefault;
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kEmphasisText, 1, default_step);
                break;
            }
            break;
        }
    }
}
