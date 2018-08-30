#ifndef CLSTCTERMINALSTYLER_H
#define CLSTCTERMINALSTYLER_H

#include <unordered_map>
#include <vector>
#include <wx/colour.h>
#include <wx/textctrl.h>

class clTerminalTextCtrl : public wxTextCtrl
{
    wxTextAttr m_curTextAtrr;
    wxTextAttr m_defaultTextAttr;
    int m_offset = 0;

    enum eState {
        kNormal,
        kEscape,
        kParams,
    };
    std::unordered_map<int, wxColour> m_colours;

protected:
    void ParseInput(const wxString& text, std::vector<std::pair<wxString, wxTextAttr> >& styledText);
    void UpdateCurrentTextAttr(const wxString& params);
    wxColour GetTextColour(int colourNumber);
    wxColour GetTextBgColour(int colourNumber);

public:
    clTerminalTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
                       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                       const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
    virtual ~clTerminalTextCtrl();

    void AppendText(const wxString& text);
    void SetDefaults(const wxFont& font, const wxColour& textColour, const wxColour& bgColour);
    void Clear();
};

#endif // CLSTCTERMINALSTYLER_H
