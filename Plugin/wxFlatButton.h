#ifndef WXFLATBUTTON_H
#define WXFLATBUTTON_H

#include "wxFlatButtonBase.h"
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/event.h>
#include <wx/graphics.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include <wx/menu.h>
#include <wx/popupwin.h>
#include <wx/vector.h>
#include "codelite_exports.h"
#include <wx/menu.h>

//++++++++---------------------------------
// wxFlatButton
//++++++++---------------------------------

class WXDLLIMPEXP_SDK wxFlatButton : public wxFlatButtonBase
{
protected:
    virtual void OnLeftDClick(wxMouseEvent& event);
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnLeftDown(wxMouseEvent& event);
    /**
     * button state:
     */
    enum eState { kStateHover, kStateNormal, kStatePressed };
    /**
     * button kind
     */
    enum eKind { kKindNormal, kKindChek };

public:
    enum eTheme { kThemeDark, kThemeNormal };
    static wxColour GetBarBgColour(wxFlatButton::eTheme theme)
    ;

    static wxColour GetBarTextColour(wxFlatButton::eTheme theme)
    ;
    typedef wxVector<wxFlatButton> Vec_t;

protected:
    wxFlatButton::eTheme m_theme;
    wxColour m_bgColour;
    wxColour m_bgHoverColour;
    wxColour m_bgPressedColour;
    wxColour m_penPressedColour;
    wxColour m_penNormalColour;
    wxColour m_penHoverColourInner;
    wxColour m_penHoverOuterColour;
    wxColour m_textColour;
    wxColour m_textColourDisabled;
    wxFont m_textFont;
    eState m_state;
    wxString m_text;
    wxBitmap m_bmp;
    wxBitmap m_bmpDisabled;

    int m_accelIndex;
    eKind m_kind;
    bool m_isChecked;
    wxMenu* m_contextMenu;
    bool m_isDisabled;
    int m_style;
    
    // Helpers
    void GetGCDC(wxAutoBufferedPaintDC& dc, wxGCDC& gdc);
    void DoActivate();
    wxSize GetBestSize();
    bool HasMenu() const;

    void DoShowContextMenu();

public:
    wxFlatButton(wxWindow* parent,
                 const wxString& label,
                 const wxFlatButton::eTheme theme = wxFlatButton::kThemeDark,
                 const wxBitmap& bmp = wxNullBitmap,
                 const wxSize& size = wxDefaultSize,
                 int flags = 0);
    virtual ~wxFlatButton();
    /**
     * @brief set this button a toggle button
     * @param b enable or disable toggle mode
     */
    void SetTogglable(bool b);

    /**
     * @brief check the button (only for toggle buttons)
     */
    void Check(bool check);

    bool IsChecked() const { return m_isChecked; }

    /**
     * @brief associate a menu with this button
     * The menu is allocated on the heap and will be released
     * by the button
     */
    void SetPopupWindow(wxMenu* menu);

    /**
     * @brief enable the button
     */
    bool Enable(bool enable);

    /**
     * @brief is this button enabled?
     */
    bool IsEnabled() const { return !m_isDisabled; }

    /**
     * @brief set a bitmap.
     * To remove a bitmap set a wxNullBitmap
     */
    void SetBmp(const wxBitmap& bmp) { this->m_bmp = bmp; }
    /**
     * @brief set the button label
     * To remove a label, set an empty string
     */
    void SetText(const wxString& text) { this->m_text = text; }
    /**
     * @brief set the label font
     */
    void SetTextFont(const wxFont& textFont) { this->m_textFont = textFont; }
    /**
     * @brief return the associated bitmap
     */
    const wxBitmap& GetBmp() const { return m_bmp; }
    /**
     * @brief return the button label
     */
    const wxString& GetText() const { return m_text; }
    /**
     * @brief return the font associated with the button
     */
    const wxFont& GetTextFont() const { return m_textFont; }

    // Colour management
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    void SetTextColour(const wxColour& textColour) { this->m_textColour = textColour; }
    void SetPenNormalColour(const wxColour& penNormalColour) { this->m_penNormalColour = penNormalColour; }
    void SetPenPressedColour(const wxColour& penPressedColour) { this->m_penPressedColour = penPressedColour; }
    const wxColour& GetPenNormalColour() const { return m_penNormalColour; }
    const wxColour& GetPenPressedColour() const { return m_penPressedColour; }
    const wxColour& GetTextColour() const { return m_textColour; }
    void SetBgPressedColour(const wxColour& bgPressedColour) { this->m_bgPressedColour = bgPressedColour; }
    const wxColour& GetBgPressedColour() const { return m_bgPressedColour; }

    void SetBmpDisabled(const wxBitmap& bmpDisabled) { this->m_bmpDisabled = bmpDisabled; }
    void SetTextColourDisabled(const wxColour& textColourDisabled) { this->m_textColourDisabled = textColourDisabled; }
    const wxBitmap& GetBmpDisabled() const { return m_bmpDisabled; }
    const wxColour& GetTextColourDisabled() const { return m_textColourDisabled; }

    void SetBgHoverColour(const wxColour& bgHoverColour) { this->m_bgHoverColour = bgHoverColour; }
    const wxColour& GetBgHoverColour() const { return m_bgHoverColour; }

protected:
    // Event handlers
    virtual void OnEraseBackground(wxEraseEvent& event);
    virtual void OnPaint(wxPaintEvent& event);
    virtual void OnEnterWindow(wxMouseEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnLeaveWindow(wxMouseEvent& event);
    virtual void OnLeftUp(wxMouseEvent& event);
};

//++++++++---------------------------------
// wxFlatButtonEvent
//++++++++---------------------------------

class wxFlatButtonEvent : public wxCommandEvent
{
    wxMenu* m_menu;

public:
    wxFlatButtonEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    wxFlatButtonEvent(const wxFlatButtonEvent& event) { *this = event; }
    virtual ~wxFlatButtonEvent() {}
    wxFlatButtonEvent& operator=(const wxFlatButtonEvent& src);
    virtual wxEvent* Clone() const { return new wxFlatButtonEvent(*this); }
    void SetMenu(wxMenu* menu) { this->m_menu = menu; }
    wxMenu* GetMenu() { return m_menu; }
};

typedef void (wxEvtHandler::*wxFlatButtonEventFunction)(wxFlatButtonEvent&);
#define wxFlatButtonEventHandler(func) wxEVENT_HANDLER_CAST(wxFlatButtonEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CMD_FLATBUTTON_CLICK, wxFlatButtonEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CMD_FLATBUTTON_MENU_SHOWING, wxFlatButtonEvent);
#endif // WXFLATBUTTON_H
