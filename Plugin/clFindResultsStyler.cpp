#include "ColoursAndFontsManager.h"
#include "clFindResultsStyler.h"
#include "editor_config.h"
#include "lexer_configuration.h"
#include "optionsconfig.h"

clFindResultsStyler::clFindResultsStyler()
    : m_stc(NULL)
    , m_curstate(kStartOfLine)
{
}

clFindResultsStyler::clFindResultsStyler(wxStyledTextCtrl* stc)
    : m_stc(stc)
    , m_curstate(kStartOfLine)
{
    SetStyles(m_stc);
    m_stc->SetLexer(wxSTC_LEX_CONTAINER);
    m_stc->Bind(wxEVT_STC_STYLENEEDED, &clFindResultsStyler::OnStyleNeeded, this);
}

clFindResultsStyler::~clFindResultsStyler()
{
    if(m_stc) { m_stc->Unbind(wxEVT_STC_STYLENEEDED, &clFindResultsStyler::OnStyleNeeded, this); }
}

void clFindResultsStyler::SetStyles(wxStyledTextCtrl* sci)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) { lexer = ColoursAndFontsManager::Get().GetLexer("text"); }

    const StyleProperty& defaultStyle = lexer->GetProperty(0);
    wxFont defaultFont = lexer->GetFontForSyle(0);

    for(size_t i = 0; i < wxSTC_STYLE_MAX; ++i) {
        sci->StyleSetForeground(i, defaultStyle.GetFgColour());
        sci->StyleSetBackground(i, defaultStyle.GetBgColour());
        sci->StyleSetFont(i, defaultFont);
    }

    // Show/hide whitespace
    sci->SetViewWhiteSpace(EditorConfigST::Get()->GetOptions()->GetShowWhitspaces());
    StyleProperty::Map_t& props = lexer->GetLexerProperties();
    // Set the whitespace colours
    sci->SetWhitespaceForeground(true, props[WHITE_SPACE_ATTR_ID].GetFgColour());

    sci->StyleSetForeground(LEX_FIF_HEADER, props[11].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_HEADER, props[11].GetBgColour());

    // 33 is the style for line numbers
    sci->StyleSetForeground(LEX_FIF_LINE_NUMBER, props[33].GetFgColour());

    // 11 is the style number for "identifier"
    sci->StyleSetForeground(LEX_FIF_MATCH, props[11].GetFgColour());

    // 16 is the stule for colouring classes
    sci->StyleSetForeground(LEX_FIF_SCOPE, props[16].GetFgColour());

    sci->StyleSetForeground(LEX_FIF_MATCH_COMMENT, props[wxSTC_C_COMMENTLINE].GetFgColour());

    sci->StyleSetForeground(LEX_FIF_FILE, props[wxSTC_C_WORD].GetFgColour());
    sci->StyleSetEOLFilled(LEX_FIF_FILE, true);

    sci->StyleSetForeground(LEX_FIF_DEFAULT, props[11].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_DEFAULT, props[11].GetBgColour());

    sci->StyleSetHotSpot(LEX_FIF_MATCH, true);
    sci->StyleSetHotSpot(LEX_FIF_FILE, true);
    sci->StyleSetHotSpot(LEX_FIF_MATCH_COMMENT, true);

    sci->SetHotspotActiveForeground(true, lexer->IsDark() ? "WHITE" : "BLACK");
    sci->SetHotspotActiveUnderline(false);
    sci->MarkerDefine(7, wxSTC_MARK_ARROW);

#if wxVERSION_NUMBER < 3100
    // On GTK we dont have the wxSTC_INDIC_TEXTFORE symbol yet (old wx version)
    sci->MarkerDefine(7, wxSTC_MARK_ARROW);
    sci->MarkerSetBackground(7, lexer->IsDark() ? "CYAN" : "ORANGE");
    sci->MarkerSetForeground(7, lexer->IsDark() ? "CYAN" : "ORANGE");

    sci->IndicatorSetForeground(1, lexer->IsDark() ? "CYAN" : "ORANGE");
    sci->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
#else
    sci->MarkerDefine(7, wxSTC_MARK_BACKGROUND);
    sci->MarkerSetBackground(7, lexer->IsDark() ? *wxWHITE : *wxGREEN);
    sci->MarkerSetForeground(7, lexer->IsDark() ? *wxWHITE : *wxGREEN);
    sci->MarkerSetAlpha(7, 40);

    sci->IndicatorSetForeground(1, lexer->IsDark() ? "#FFD700" : "#FF4500");
    sci->IndicatorSetStyle(1, wxSTC_INDIC_TEXTFORE);
#endif
    sci->IndicatorSetUnder(1, true);

    sci->SetMarginWidth(0, 0);
    sci->SetMarginWidth(1, 16);
    sci->SetMarginWidth(2, 0);
    sci->SetMarginWidth(3, 0);
    sci->SetMarginWidth(4, 0);
    sci->SetMarginSensitive(1, true);
    sci->HideSelection(true);

    // Indentation
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    sci->SetUseTabs(options->GetIndentUsesTabs());
    sci->SetTabWidth(options->GetIndentWidth());
    sci->SetIndent(options->GetIndentWidth());
    sci->Refresh();
}

void clFindResultsStyler::StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope)
{
    int startPos = ctrl->GetEndStyled();
    int endPos = e.GetPosition();
    wxString text = ctrl->GetTextRange(startPos, endPos);
#if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
    // The scintilla syntax in wx3.1.1 changed
    ctrl->StartStyling(startPos);
#else
    ctrl->StartStyling(startPos, 0x1f);
#endif

    wxString::const_iterator iter = text.begin();
    size_t headerStyleLen = 0;
    size_t filenameStyleLen = 0;
    size_t lineNumberStyleLen = 0;
    size_t scopeStyleLen = 0;
    size_t matchStyleLen = 0;
    size_t i = 0;
    for(; iter != text.end(); ++iter) {
        bool advance2Pos = false;
        const wxUniChar& ch = *iter;
        if((long)ch >= 128) { advance2Pos = true; }

        switch(m_curstate) {
        default:
            break;
        case kStartOfLine:
            if(ch == '=') {
                m_curstate = kHeader;
                headerStyleLen = 1;
            } else if(ch == ' ') {
                // start of a line number
                lineNumberStyleLen = 1;
                m_curstate = kLineNumber;
            } else if(ch == '\n') {
                ctrl->SetStyling(1, LEX_FIF_DEFAULT);
            } else {
                // File name
                filenameStyleLen = 1;
                m_curstate = kFile;
            }
            break;
        case kLineNumber:
            ++lineNumberStyleLen;
            if(ch == ':') {
                ctrl->SetStyling(lineNumberStyleLen, LEX_FIF_LINE_NUMBER);
                lineNumberStyleLen = 0;
                if(hasSope) {
                    // the scope showed by displayed after the line number
                    m_curstate = kScope;
                } else {
                    // No scope, from hereon, match until EOF
                    m_curstate = kMatch;
                }
            }
            break;
        case kScope:
            ++scopeStyleLen;

            if(ch == ']') {
                // end of scope
                ctrl->SetStyling(scopeStyleLen, LEX_FIF_SCOPE);
                scopeStyleLen = 0;
                m_curstate = kMatch;
            }
            break;
        case kMatch:
            ++matchStyleLen;
            if(advance2Pos) { ++matchStyleLen; }
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
                matchStyleLen = 0;
            }
            break;
        case kFile:
            ++filenameStyleLen;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 2 | wxSTC_FOLDLEVELHEADERFLAG);
                ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
                filenameStyleLen = 0;
            }
            break;
        case kHeader:
            ++headerStyleLen;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 1 | wxSTC_FOLDLEVELHEADERFLAG);
                ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
                headerStyleLen = 0;
            }
            break;
        }
        if(advance2Pos) {
            i += 2;
        } else {
            ++i;
        }
    }

    // Left overs...
    if(headerStyleLen) {
        ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 1 | wxSTC_FOLDLEVELHEADERFLAG);
        ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
        headerStyleLen = 0;
    }

    if(filenameStyleLen) {
        ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 2 | wxSTC_FOLDLEVELHEADERFLAG);
        ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
        filenameStyleLen = 0;
    }

    if(matchStyleLen) {
        ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
        matchStyleLen = 0;
    }

    if(lineNumberStyleLen) {
        ctrl->SetStyling(lineNumberStyleLen, LEX_FIF_LINE_NUMBER);
        lineNumberStyleLen = 0;
    }
}

void clFindResultsStyler::Reset() { m_curstate = kStartOfLine; }

int clFindResultsStyler::HitTest(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, int& line)
{
    long pos = e.GetPosition();
    line = ctrl->LineFromPosition(pos);
    return ctrl->GetStyleAt(pos);
}

int clFindResultsStyler::TestToggle(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e)
{
    int line = wxNOT_FOUND;
    int style = HitTest(ctrl, e, line);
    if(style == LEX_FIF_FILE || style == LEX_FIF_HEADER) {
        return line;
    } else {
        return wxNOT_FOUND;
    }
}

void clFindResultsStyler::OnStyleNeeded(wxStyledTextEvent& e)
{
    e.Skip();
    StyleText(m_stc, e, false);
}

int clFindResultsStyler::HitTest(wxStyledTextEvent& e, int& line) { return HitTest(m_stc, e, line); }
