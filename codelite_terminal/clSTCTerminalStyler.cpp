#include "clSTCTerminalStyler.h"
#include <wx/tokenzr.h>

clTerminalTextCtrl::clTerminalTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                                       const wxSize& size, long style, const wxValidator& validator,
                                       const wxString& name)
    : wxTextCtrl(parent, id, value, pos, size, style | wxTE_RICH | wxTE_MULTILINE, validator, name)
{
    SetDefaults(wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT), wxColour("#fdfefe"), wxColour("#1c2833"));
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
}

clTerminalTextCtrl::~clTerminalTextCtrl() {}

void clTerminalTextCtrl::AppendText(const wxString& text)
{
    std::vector<std::pair<wxString, wxTextAttr> > styles;
    ParseInput(text, styles);

    for(size_t i = 0; styles.size(); ++i) {
        wxString& text = styles[i].first;
        wxTextAttr& textAttr = styles[i].second;
        SetDefaultStyle(textAttr);
        wxTextCtrl::AppendText(text);
    }
}

void clTerminalTextCtrl::SetDefaults(const wxFont& font, const wxColour& textColour, const wxColour& bgColour)
{
    m_defaultTextAttr = wxTextAttr(textColour, bgColour, font);
    m_curTextAtrr = m_defaultTextAttr;
    m_colours[39] = textColour;
    m_colours[49] = bgColour;
    SetDefaultStyle(m_curTextAtrr);
    SetBackgroundColour(bgColour);
}

void clTerminalTextCtrl::ParseInput(const wxString& text, std::vector<std::pair<wxString, wxTextAttr> >& styledText)
{
    wxString tmpbuffer;
    wxString paramsBuffer;
    eState state = kNormal;
    wxString::const_iterator iter = text.begin();
    while(iter != text.end()) {
        wxChar ch = *iter;
        switch(state) {
        case kNormal:
            switch(ch) {
            case '\033':
                // Before switching state, push the current buffer with its style
                if(!tmpbuffer.IsEmpty()) {
                    styledText.push_back({ tmpbuffer, m_curTextAtrr });
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
                tmpbuffer << "\033" << ch;
                state = kNormal;
                break;
            }
        case kParams:
            switch(ch) {
            case 'm':
                state = kNormal;
                if(!paramsBuffer.IsEmpty()) {
                    // Update the current text style
                    paramsBuffer.Clear();
                }
                break;
            default:
                // No [ ? restore the "\033" back to the buffer and put us back in the "Nomral" state
                paramsBuffer << ch;
                break;
            }
        }
    }

    if(!tmpbuffer.IsEmpty()) {
        styledText.push_back({ tmpbuffer, m_curTextAtrr });
        tmpbuffer.Clear();
    }
}

void clTerminalTextCtrl::Clear()
{
    m_curTextAtrr = m_defaultTextAttr;
    wxTextCtrl::Clear();
}

void clTerminalTextCtrl::UpdateCurrentTextAttr(const wxString& params)
{
    wxArrayString numbers = ::wxStringTokenize(params, ";", wxTOKEN_STRTOK);
    for(size_t i = 0; i < numbers.size(); ++i) {
        long nParam = 0;
        if(!numbers.Item(i).ToCLong(&nParam)) { return; }
        switch(nParam) {
        case 0:
            // reset the style
            m_curTextAtrr = m_defaultTextAttr;
            break;
        case 1: {
            // Bold font
            wxFont f = m_curTextAtrr.GetFont();
            f.SetWeight(wxFONTWEIGHT_BOLD);
            m_curTextAtrr.SetFont(f);
            break;
        }
        case 3: {
            // Bold font
            wxFont f = m_curTextAtrr.GetFont();
            f.SetStyle(wxFONTSTYLE_ITALIC);
            m_curTextAtrr.SetFont(f);
            break;
        }
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 39:
            m_curTextAtrr.SetTextColour(GetTextColour(nParam));
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
            m_curTextAtrr.SetBackgroundColour(GetTextBgColour(nParam));
            break;
        }
    }
}

wxColour clTerminalTextCtrl::GetTextColour(int colourNumber)
{
    if(m_colours.count(colourNumber) == 0) { return m_defaultTextAttr.GetTextColour(); }
    return m_colours[colourNumber];
}

wxColour clTerminalTextCtrl::GetTextBgColour(int colourNumber)
{
    if(m_colours.count(colourNumber) == 0) { return m_defaultTextAttr.GetBackgroundColour(); }
    return m_colours[colourNumber];
}
