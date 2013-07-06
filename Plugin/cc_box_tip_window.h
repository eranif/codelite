#ifndef CCBOXTIPWINDOW_H
#define CCBOXTIPWINDOW_H

#include <wx/popupwin.h> // Base class: wxPopupTransientWindow
#include <wx/panel.h>
#include <vector>
#include "codelite_exports.h"

class IEditor;
class CCBox;

WXDLLIMPEXP_SDK extern const wxEventType wxEVT_TIP_BTN_CLICKED_UP;
WXDLLIMPEXP_SDK extern const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN;

class WXDLLIMPEXP_SDK CCBoxTipWindow : public wxPopupWindow
{
protected:
    class Links {
    public:
        wxString m_url;
        wxRect   m_rect;
    };
    
    wxString m_tip;
    int      m_lineHeight;
    wxFont   m_codeFont;
    wxFont   m_commentFont;
    size_t   m_numOfTips;
    wxRect   m_leftTipRect;
    wxRect   m_rightTipRect;
    bool     m_positionedToRight;
    std::vector<Links> m_links;
    
protected:
    void OnPaint(wxPaintEvent &e);
    void OnEraseBG(wxEraseEvent &e);
    void OnMouseLeft(wxMouseEvent &e);
    wxRect DoPrintText(wxDC& dc, wxString &text, wxPoint& pt);
    wxString DoStripMarkups();
    void DoInitialize( const wxString &tip, size_t numOfTips, bool simpleTip);
    
public:
    CCBoxTipWindow(wxWindow* parent, const wxString &tip, size_t numOfTips, bool simpleTip = false);
    CCBoxTipWindow(wxWindow* parent, const wxString &tip); 
    virtual ~CCBoxTipWindow();

    /**
     * @brief position the tip next to 'win' and show it
     * if focusEditor is NOT null, the editor will gain the focus once
     * the tip is shown
     */
    void PositionRelativeTo(wxWindow* win, IEditor* focusEdior = NULL);
    /**
     * @brief position and show the tip at a given location
     */
    void PositionAt(const wxPoint& pt, IEditor* focusEdior = NULL);
};

#endif // CCBOXTIPWINDOW_H
