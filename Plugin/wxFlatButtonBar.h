#ifndef WXFLATBUTTONBAR_H
#define WXFLATBUTTONBAR_H

#include "wxFlatButtonBase.h"
#include "wxFlatButton.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK wxFlatButtonBar : public wxFlatButtonBarBase
{
    wxFlatButton::eTheme m_theme;
    wxColour m_penColour;
    wxColour m_bgColour;
    int m_style;
    
public:
    /**
     * @brief construct a button bar
     * @param parent the parent window
     * @param theme the theme. Can be Dark or Normal
     * @param flags bar style flags. Only supported flag is wxNO_BORDER or 0
     */
    wxFlatButtonBar(wxWindow* parent, const wxFlatButton::eTheme theme, int flags = 0);
    virtual ~wxFlatButtonBar();

    wxFlatButton*
    /**
     * @brief add a button to the bar
     * @param label
     * @param bmp
     * @param size
     */
    AddButton(const wxString& label, const wxBitmap& bmp = wxNullBitmap, const wxSize& size = wxDefaultSize, int style = 0);

    /**
     * @brief add control to the bar
     * @return
     */
    wxWindow* AddControl(wxWindow* window, int proportion = 0, int flags = wxEXPAND|wxALL);

    /**
     * @brief add a spacer
     */
    void AddSpacer(int size);
    
protected:
    virtual void OnIdle(wxIdleEvent& event);
    virtual void OnSize(wxSizeEvent& event);
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetPenColour(const wxColour& penColour) { this->m_penColour = penColour; }
    void SetTheme(const wxFlatButton::eTheme& theme) { this->m_theme = theme; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxColour& GetPenColour() const { return m_penColour; }
    const wxFlatButton::eTheme& GetTheme() const { return m_theme; }
    virtual void OnPaint(wxPaintEvent& event);
};
#endif // WXFLATBUTTONBAR_H
