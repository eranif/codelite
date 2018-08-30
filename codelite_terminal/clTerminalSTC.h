#ifndef CLSTCTERMINALSTYLER_H
#define CLSTCTERMINALSTYLER_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wx/colour.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/version.h>

#if wxVERSION_NUMBER < 3100
#include <functional>
#include <wx/string.h>

namespace std
{
template <> struct hash<wxString> {
    std::size_t operator()(const wxString& s) const { return hash<std::wstring>{}(s.ToStdWstring()); }
};
} // namespace std
#endif

struct TextStyle {
    int style_id = 0;
    wxString buffer;
    int pos_start = wxNOT_FOUND;
    int style_len = wxNOT_FOUND;
    TextStyle(int style_number, const wxString& text)
        : style_id(style_number)
        , buffer(text)
    {
    }
};

class clTerminalSTC : public wxStyledTextCtrl
{
    enum eState {
        kNormal,
        kEscape,
        kParams,
    };
    std::unordered_map<int, wxColour> m_colours;
    std::unordered_map<wxString, int> m_editorStyles;
    std::vector<TextStyle> m_styles;
    int m_nextAvailableStyle = 1;

protected:
    void ParseInput(const wxString& text, std::vector<TextStyle>& styledText);
    int GetStyleFromParams(const wxString& params);
    wxColour GetTextColour(int colourNumber);
    wxColour GetTextBgColour(int colourNumber);
    void OnStyleNeeded(wxStyledTextEvent& event);
    int GetStcStyle(int textColour, int bgColour);

public:
    clTerminalSTC(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxSTCNameStr);
    virtual ~clTerminalSTC();
    void AppendText(const wxString& text);
    void SetPreferences(const wxFont& font, const wxColour& textColour, const wxColour& textBgColour);
    void Clear();
};

#endif // CLSTCTERMINALSTYLER_H
