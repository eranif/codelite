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

public:
    wxFlatButtonBar(wxWindow* parent, const wxFlatButton::eTheme theme);
    virtual ~wxFlatButtonBar();

    wxFlatButton*
    /**
     * @brief add a button to the bar
     * @param label
     * @param bmp
     * @param size
     */
    AddButton(const wxString& label, const wxBitmap& bmp = wxNullBitmap, const wxSize& size = wxDefaultSize);

    /**
     * @brief add control to the bar
     * @return
     */
    wxWindow* AddControl(wxWindow* window, int proportion = 0, int flags = wxEXPAND|wxALL);
    
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
