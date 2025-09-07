#include "MarkdownStyler.hpp"

#include "ColoursAndFontsManager.h"

namespace
{
enum MarkdownStyles {
    kDefault,
    kHeader1,
    kHeader2,
    kHeader3,
    kHeader4,
    kHeader5,
    kHeader6,
    kHeaderText,
    kCodeBlock,
    kCodeBlockTag,
    kCodeWord,
};

enum class State {
    kDefault,
    kCodeBlock,
    kCodeWord,
    kHeaderText,
};

} // namespace

MarkdownStyler::MarkdownStyler(wxStyledTextCtrl* ctrl)
    : clSTCContainerStylerBase(ctrl)
{
    InitInternal(); // Will call the pure virtual method "InitStyles"
}

void MarkdownStyler::Reset() {}

void MarkdownStyler::InitStyles()
{
    wxColour default_bg, default_fg;
    auto lexer = ColoursAndFontsManager::Get().GetLexer("rust");
    CHECK_PTR_RET(lexer);

    auto default_prop = lexer->GetProperty(wxSTC_RUST_DEFAULT);
    default_bg = default_prop.GetBgColour();
    default_fg = default_prop.GetFgColour();

    for (int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_ctrl->StyleSetBackground(i, default_bg);
    }

    auto macro = lexer->GetProperty(wxSTC_RUST_MACRO);
    auto string = lexer->GetProperty(wxSTC_RUST_STRING);
    auto number = lexer->GetProperty(wxSTC_RUST_NUMBER);
    auto strukt = lexer->GetProperty(wxSTC_RUST_WORD4);
    auto variable = lexer->GetProperty(wxSTC_RUST_WORD5);
    auto function = lexer->GetProperty(wxSTC_RUST_WORD6);
    auto keyword = lexer->GetProperty(wxSTC_RUST_WORD);

    m_ctrl->StyleSetForeground(MarkdownStyles::kDefault, default_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDefault, default_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlock, default_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlock, default_bg.ChangeLightness(80));
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kCodeBlock, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockTag, keyword.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockTag, default_bg.ChangeLightness(80));

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeWord, function.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeWord, default_bg.ChangeLightness(80));

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader1, strukt.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader2, macro.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeaderText, default_fg);
    m_ctrl->StyleSetBold(MarkdownStyles::kHeaderText, true);
    m_ctrl->StyleSetItalic(MarkdownStyles::kHeaderText, true);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kHeaderText, true);

    const std::vector<int> header_styles = { kHeader2, kHeader3, kHeader4, kHeader5, kHeader6 };
    for (int style : header_styles) {
        m_ctrl->StyleSetForeground(style, variable.GetFgColour());
    }

    SetStyleCallback([this](clSTCAccessor& accessor) { this->OnStyle(accessor); });
}

void MarkdownStyler::OnStyle(clSTCAccessor& accessor)
{
    State state = State::kDefault;
    while (accessor.CanNext()) {
        wxChar ch = accessor.Get();
        wxUniChar uni_char(ch);
        int default_step{ 1 };
        if (!uni_char.IsAscii()) {
            clSYSTEM() << "Non ASCII char found!:" << uni_char << endl;
            wxString as_str(uni_char);
            default_step = StringUtils::UTF8Length(as_str.wc_str(), as_str.length());
            clSYSTEM() << "Step is set to:" << default_step << endl;
        }

        switch (state) {
        case State::kDefault:
            switch (ch) {
            case '#':
                if (accessor.IsAtStartOfLine()) {
                    if (accessor.Get(6) == "######") {
                        accessor.SetStyle(MarkdownStyles::kHeader6, 6);
                    } else if (accessor.Get(5) == "#####") {
                        accessor.SetStyle(MarkdownStyles::kHeader5, 5);
                    } else if (accessor.Get(4) == "####") {
                        accessor.SetStyle(MarkdownStyles::kHeader4, 4);
                    } else if (accessor.Get(3) == "###") {
                        accessor.SetStyle(MarkdownStyles::kHeader3, 3);
                    } else if (accessor.Get(2) == "##") {
                        accessor.SetStyle(MarkdownStyles::kHeader2, 2);
                    } else {
                        accessor.SetStyle(MarkdownStyles::kHeader1, 1);
                    }
                    state = State::kHeaderText;
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '`':
                if (accessor.Get(3) == "```") {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kCodeBlock, 3);
                    state = State::kCodeBlock;
                } else {
                    accessor.SetStyle(MarkdownStyles::kCodeWord, 1);
                    state = State::kCodeWord;
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kDefault, 1, default_step);
                break;
            }
            break;
        case State::kCodeBlock:
            switch (ch) {
            case '`':
                if (accessor.Get(3) == "```") {
                    accessor.SetStyle(MarkdownStyles::kCodeBlock, 3);
                    state = State::kDefault;
                } else {
                    accessor.SetStyle(MarkdownStyles::kCodeBlock, 1);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kCodeBlock, 1, default_step);
                break;
            }
            break;
        case State::kCodeWord:
            switch (ch) {
            case '`':
                accessor.SetStyle(MarkdownStyles::kCodeWord, 1);
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
        }
    }
}
