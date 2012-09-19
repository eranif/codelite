#ifndef CCBOXTIPWINDOW_H
#define CCBOXTIPWINDOW_H

#include <wx/popupwin.h> // Base class: wxPopupTransientWindow
#include <wx/panel.h>

class LEditor;
class CCBox;

extern const wxEventType wxEVT_TIP_BTN_CLICKED_UP;
extern const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN;

class CCBoxTipWindow : public wxPopupWindow
{
    wxString m_tip;
    int      m_lineHeight;
    wxFont   m_codeFont;
    wxFont   m_commentFont;
    size_t   m_numOfTips;
    wxRect   m_leftTipRect;
    wxRect   m_rightTipRect;
    bool     m_positionedToRight;

    static wxBitmap m_leftbmp;
    static wxBitmap m_rightbmp;

protected:
    void OnPaint(wxPaintEvent &e);
    void OnEraseBG(wxEraseEvent &e);
    void OnMouseLeft(wxMouseEvent &e);
    void DoPrintText(wxDC& dc, wxString &text, wxPoint& pt);
    wxString DoStripMarkups();

public:
    CCBoxTipWindow(wxWindow* parent, const wxString &tip, size_t numOfTips);
    virtual ~CCBoxTipWindow();

    /**
     * @brief position the tip next to 'win' and show it
     * if focusEditor is NOT null, the editor will gain the focus once
     * the tip is shown
     */
    void PositionRelativeTo(wxWindow* win, LEditor* focusEdior = NULL);
    /**
     * @brief position and show the tip at a given location
     */
    void PositionAt(const wxPoint& pt, LEditor* focusEdior = NULL);
};

#endif // CCBOXTIPWINDOW_H
