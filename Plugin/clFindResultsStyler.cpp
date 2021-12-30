#include "clFindResultsStyler.h"

#include "ColoursAndFontsManager.h"
#include "editor_config.h"
#include "globals.h"
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
    if(m_stc) {
        m_stc->Unbind(wxEVT_STC_STYLENEEDED, &clFindResultsStyler::OnStyleNeeded, this);
    }
}

void clFindResultsStyler::SetStyles(wxStyledTextCtrl* sci)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }

    const StyleProperty& defaultStyle = lexer->GetProperty(0);
    wxFont defaultFont = lexer->GetFontForSyle(0, sci);

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

    sci->StyleSetForeground(LEX_FIF_HEADER, props[0].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_HEADER, props[0].GetBgColour());

    // 33 is the style for line numbers
    sci->StyleSetForeground(LEX_FIF_LINE_NUMBER, props[33].GetFgColour());

    sci->StyleSetForeground(LEX_FIF_MATCH, props[wxSTC_C_IDENTIFIER].GetFgColour());
    sci->StyleSetForeground(LEX_FIF_SCOPE, props[wxSTC_C_GLOBALCLASS].GetFgColour());

    sci->StyleSetForeground(LEX_FIF_MATCH_COMMENT, props[wxSTC_C_COMMENTLINE].GetFgColour());

    sci->StyleSetForeground(LEX_FIF_FILE, props[wxSTC_C_WORD].GetFgColour());
    sci->StyleSetEOLFilled(LEX_FIF_FILE, true);

    sci->StyleSetForeground(LEX_FIF_DEFAULT, props[0].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_DEFAULT, props[0].GetBgColour());

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

    sci->SetMarginWidth(0, 0);                    // line numbers
    sci->SetMarginWidth(1, ::clGetSize(16, sci)); // symbols margin
    sci->SetMarginWidth(2, 0);                    // folding margin
    sci->SetMarginWidth(3, 0);                    // separator margin
    sci->SetMarginType(3, wxSTC_MARGIN_FORE);
    sci->SetMarginMask(3, 0);

    sci->SetMarginWidth(4, 0);
    sci->SetMarginSensitive(1, true);
    sci->HideSelection(true);
#if wxCHECK_VERSION(3, 1, 0)
    sci->SetMarginBackground(3, *wxBLACK);
#endif
    // Indentation
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    sci->SetUseTabs(options->GetIndentUsesTabs());
    sci->SetTabWidth(options->GetIndentWidth());
    sci->SetIndent(options->GetIndentWidth());
    for(int i = 0; i <= 4; ++i) { // there are 5 margins defined from 0 -> 4 (including)
        sci->SetMarginCursor(i, wxSTC_CURSORARROW);
    }
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
        const wxUniChar& ch = *iter;
        size_t chWidth = 1;
        if(!ch.IsAscii()) {
            chWidth = wxString(ch).mb_str(wxConvUTF8).length();
        }

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
                filenameStyleLen = chWidth;
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
            scopeStyleLen += chWidth;
            if(ch == ']') {
                // end of scope
                ctrl->SetStyling(scopeStyleLen, LEX_FIF_SCOPE);
                scopeStyleLen = 0;
                m_curstate = kMatch;
            }
            break;
        case kMatch:
            matchStyleLen += chWidth;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
                matchStyleLen = 0;
            }
            break;
        case kFile:
            filenameStyleLen += chWidth;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 2 | wxSTC_FOLDLEVELHEADERFLAG);
                ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
                filenameStyleLen = 0;
            }
            break;
        case kHeader:
            headerStyleLen += chWidth;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos + i), 1 | wxSTC_FOLDLEVELHEADERFLAG);
                ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
                headerStyleLen = 0;
            }
            break;
        }
        i += chWidth;
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
