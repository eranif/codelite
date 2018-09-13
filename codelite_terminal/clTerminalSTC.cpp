#include "clTerminalSTC.h"
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#define MARKER_ID 1

clTerminalSTC::clTerminalSTC(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
    : wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    SetLexer(wxSTC_LEX_CONTAINER);

    m_colours.insert({ 30, *wxBLACK });
    m_colours.insert({ 31, *wxRED });
    m_colours.insert({ 32, *wxGREEN });
    m_colours.insert({ 33, *wxYELLOW });
    m_colours.insert({ 34, *wxBLUE });
    m_colours.insert({ 35, wxColour("MAGENTA") });
    m_colours.insert({ 36, wxColour("CYAN") });
    m_colours.insert({ 37, wxColour("WHITE") });
    m_colours.insert({ 39, wxColour("#fdfefe") }); // default text colour
    m_colours.insert({ 40, *wxBLACK });
    m_colours.insert({ 41, *wxRED });
    m_colours.insert({ 42, *wxGREEN });
    m_colours.insert({ 43, *wxYELLOW });
    m_colours.insert({ 44, *wxBLUE });
    m_colours.insert({ 45, wxColour("MAGENTA") });
    m_colours.insert({ 46, wxColour("CYAN") });
    m_colours.insert({ 47, wxColour("WHITE") });
    m_colours.insert({ 49, wxColour("#1c2833") }); // Default bg colour

    // Define some basic styles
    wxColour defaultTextBgColour = GetTextBgColour(49);
    wxColour defaultTextColour = GetTextBgColour(39);
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        StyleSetForeground(i, defaultTextColour);
        StyleSetBackground(i, defaultTextBgColour);
    }
    Bind(wxEVT_STC_STYLENEEDED, &clTerminalSTC::OnStyleNeeded, this);

    MarkerDefine(MARKER_ID, wxSTC_MARK_ARROWS);
    MarkerSetBackground(MARKER_ID, *wxBLACK);
    SetWrapMode(wxSTC_WRAP_CHAR);

#if defined(__WXMSW__)
    SetTechnology(wxSTC_TECHNOLOGY_DIRECTWRITE);
#elif defined(__WXGTK__)
    SetTechnology(wxSTC_TECHNOLOGY_DIRECTWRITE);
    SetDoubleBuffered(false);
#if wxCHECK_VERSION(3, 1, 1)
    SetFontQuality(wxSTC_EFF_QUALITY_ANTIALIASED);
#endif
#endif
}

clTerminalSTC::~clTerminalSTC() { Unbind(wxEVT_STC_STYLENEEDED, &clTerminalSTC::OnStyleNeeded, this); }

void clTerminalSTC::AppendText(const wxString& text)
{
    m_styles.clear();
    ParseInput(text, m_styles);

    wxWindowUpdateLocker locker(this);
    for(size_t i = 0; i < m_styles.size(); ++i) {
        TextStyle& style = m_styles[i];
        const wxString& text = style.buffer;
        style.pos_start = GetLastPosition();
        style.style_len = text.length();
        wxStyledTextCtrl::AppendText(text);
    }
}

void clTerminalSTC::ParseInput(const wxString& text, std::vector<TextStyle>& styledText)
{
    char* p = const_cast<char*>(text.mb_str(wxConvUTF8).data());
    wxUnusedVar(p);

    wxString tmpbuffer;
    wxString paramsBuffer;
    eState state = kNormal;
    wxString::const_iterator iter = text.begin();
    int style_number = 0;
    while(iter != text.end()) {
        wxChar ch = *iter;
        ++iter;
        switch(state) {
        case kNormal:
            switch(ch) {
            case 27: // \033
                // Before switching state, push the current buffer with its style
                if(!tmpbuffer.IsEmpty()) {
                    styledText.push_back({ style_number, tmpbuffer });
                    tmpbuffer.Clear();
                }
                state = kEscape;
                break;
            default:
                tmpbuffer << ch;
                break;
            }
            break;
        // We found '\033' we are now expecting to see '['
        case kEscape:
            switch(ch) {
            case '[':
                state = kParams;
                break;
            default:
                // No [ ? restore the "\033" back to the buffer and put us back in the "Nomral" state
                tmpbuffer << ((char)27) << ch;
                state = kNormal;
                break;
            }
            break;
        case kParams:
            switch(ch) {
            case 'm':
                state = kNormal;
                if(!paramsBuffer.IsEmpty()) {
                    // Update the current text style
                    style_number = GetStyleFromParams(paramsBuffer);
                    paramsBuffer.Clear();
                }
                break;
            default:
                // No [ ? restore the "\033" back to the buffer and put us back in the "Nomral" state
                paramsBuffer << ch;
                break;
            }
            break;
        }
    }

    if(!tmpbuffer.IsEmpty()) {
        styledText.push_back({ style_number, tmpbuffer });
        tmpbuffer.Clear();
    }
}

void clTerminalSTC::Clear()
{
    m_styles.clear();
    ClearAll();
}

int clTerminalSTC::GetStyleFromParams(const wxString& params)
{
    wxArrayString numbers = ::wxStringTokenize(params, ";", wxTOKEN_STRTOK);
    int text_colour = 39;
    int text_bg_colour = 49;
    for(size_t i = 0; i < numbers.size(); ++i) {
        long nParam = 0;
        if(!numbers.Item(i).ToCLong(&nParam)) { return GetStcStyle(39, 49); }
        switch(nParam) {
        case 0:
            // reset the style
            text_colour = 39;
            text_bg_colour = 49;
            break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 39:
            text_colour = nParam;
            break;
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 49:
            text_bg_colour = nParam;
            break;
        }
    }
    return GetStcStyle(text_colour, text_bg_colour);
}

wxColour clTerminalSTC::GetTextColour(int colourNumber)
{
    if(m_colours.count(colourNumber) == 0) { colourNumber = 39; }
    return m_colours[colourNumber];
}

wxColour clTerminalSTC::GetTextBgColour(int colourNumber)
{
    if(m_colours.count(colourNumber) == 0) { colourNumber = 49; }
    return m_colours[colourNumber];
}

void clTerminalSTC::OnStyleNeeded(wxStyledTextEvent& event)
{
    if(m_styles.empty()) return;

    int startPos = m_styles[0].pos_start;
#if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
    // The scintilla syntax in wx3.1.1 changed
    StartStyling(startPos);
#else
    StartStyling(startPos, 0x1f);
#endif
    for(size_t i = 0; i < m_styles.size(); ++i) {
        SetStyling(m_styles[i].style_len, m_styles[i].style_id);
    }
}

int clTerminalSTC::GetStcStyle(int textColour, int bgColour)
{
    wxString k;
    k << textColour << "-" << bgColour;
    if(m_editorStyles.count(k) == 0) {
        // No such style, create it
        int new_style = m_nextAvailableStyle;
        ++m_nextAvailableStyle;
        wxFont font = StyleGetFont(0);
        StyleSetBackground(new_style, GetTextBgColour(bgColour));
        StyleSetForeground(new_style, GetTextColour(textColour));
        m_editorStyles.insert({ k, new_style });
    }
    return m_editorStyles[k];
}

void clTerminalSTC::SetPreferences(const wxFont& font, const wxColour& textColour, const wxColour& textBgColour)
{
    wxFont f = font;
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        StyleSetFont(i, f);
        StyleSetForeground(i, textColour);
        StyleSetBackground(i, textBgColour);
    }
    
    SetCaretForeground(textColour);
    MarkerSetForeground(MARKER_ID, textColour);
}
