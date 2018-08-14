#include "ColoursAndFontsManager.h"
#include "clGenericSTCStyler.h"
#include "codelite_events.h"
#include "event_notifier.h"

clGenericSTCStyler::clGenericSTCStyler(wxStyledTextCtrl* stc)
    : m_ctrl(stc)
    , m_nextAvailStyle(kLastStyle)
{
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->Bind(wxEVT_STC_STYLENEEDED, &clGenericSTCStyler::OnStyleNeeded, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clGenericSTCStyler::OnThemChanged, this);
    ResetStyles();
}

clGenericSTCStyler::~clGenericSTCStyler()
{
    m_ctrl->Unbind(wxEVT_STC_STYLENEEDED, &clGenericSTCStyler::OnStyleNeeded, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clGenericSTCStyler::OnThemChanged, this);
}

void clGenericSTCStyler::OnStyleNeeded(wxStyledTextEvent& event)
{
    int startPos = m_ctrl->GetEndStyled();
    int endPos = event.GetPosition();
    wxString text = m_ctrl->GetTextRange(startPos, endPos);

#if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
    // The scintilla syntax in wx3.1.1 changed
    m_ctrl->StartStyling(startPos);
#else
    m_ctrl->StartStyling(startPos, 0x1f);
#endif
    wxString lineText;
    while(GetNextLine(text, lineText)) {
        text = text.Mid(lineText.length());
        int style = GetStyleForLine(lineText);
        m_ctrl->SetStyling(lineText.length(), style);
    }
}

bool clGenericSTCStyler::GetNextLine(const wxString& inText, wxString& lineText) const
{
    lineText.Clear();
    wxString::const_iterator iter = inText.begin();
    while(iter != inText.end()) {
        const wxUniChar& ch = *iter;
        lineText << ch;
        ++iter;
        if(ch == '\n') {
            // found EOL
            return true;
        }
    }
    return false;
}

int clGenericSTCStyler::GetStyleForLine(const wxString& lineText) const
{
    wxString lcLine = lineText.Lower();
    for(size_t i = 0; i < m_words.size(); ++i) {
        if(lcLine.Contains(m_words[i].first)) { return m_words[i].second; }
    }

    // Return the default style, which is 0
    return 0;
}

void clGenericSTCStyler::ResetStyles()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(!lexer) return;
    lexer->Apply(m_ctrl);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    InitDefaultStyles();
}

void clGenericSTCStyler::AddStyle(const wxArrayString& words, clGenericSTCStyler::eStyles style)
{
    if(words.IsEmpty()) { return; }
    for(size_t i = 0; i < words.size(); ++i) {
        m_words.push_back({ words.Item(i).Lower(), (int)style });
    }
}

void clGenericSTCStyler::AddUserStyle(const wxArrayString& words, const wxColour& fgColour, const wxColour& bgColour)
{
    if(words.IsEmpty()) { return; }
    m_styleInfo.push_back(std::make_tuple(m_nextAvailStyle, fgColour, bgColour));
    for(size_t i = 0; i < words.size(); ++i) {
        m_words.push_back({ words.Item(i).Lower(), m_nextAvailStyle });
    }
    ++m_nextAvailStyle;
}

void clGenericSTCStyler::ApplyStyles()
{
    std::for_each(m_styleInfo.begin(), m_styleInfo.end(), [&](const std::tuple<int, wxColour, wxColour>& t) {
        int style = std::get<0>(t);
        const wxColour& fgColour = std::get<1>(t);
        const wxColour& bgColour = std::get<2>(t);
        m_ctrl->StyleSetForeground(style, fgColour);
        if(bgColour.IsOk()) { m_ctrl->StyleSetBackground(style, bgColour); }
    });
}

void clGenericSTCStyler::OnThemChanged(wxCommandEvent& event)
{
    event.Skip();
    ResetStyles();
    ApplyStyles();
}

void clGenericSTCStyler::ClearAllStyles()
{
    m_styleInfo.clear();
    m_words.clear();
    ResetStyles();
}

void clGenericSTCStyler::InitDefaultStyles()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(!lexer) return;

    const StyleProperty& defaultStyle = lexer->GetProperty(0);
    bool isDark = lexer->IsDark();
    m_styleInfo.resize(kLastStyle);
    m_styleInfo[kDefault] = std::make_tuple(kDefault, defaultStyle.GetFgColour(), defaultStyle.GetBgColour());
    m_styleInfo[kInfo] = std::make_tuple(kInfo, isDark ? wxColour("rgb(167, 226, 46)") : wxColour("rgb(80, 161, 79)"),
                                         defaultStyle.GetBgColour());
    m_styleInfo[kWarning] = std::make_tuple(
        kWarning, isDark ? wxColour("rgb(150,155,73)") : wxColour("rgb(255,201,14)"), defaultStyle.GetBgColour());
    m_styleInfo[kError] =
        std::make_tuple(kError, isDark ? wxColour("rgb(255,128,128)") : *wxRED, defaultStyle.GetBgColour());
}
