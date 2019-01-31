#ifndef CAPTIONBAR_H
#define CAPTIONBAR_H

#include "gui.h"
#include <wx/bitmap.h>

class CaptionBar : public CaptionBarBase
{
    wxString m_caption;
    wxString m_style;
    wxBitmap m_icon;

public:
    CaptionBar(wxWindow* parent, const wxString& caption, const wxString& style, const wxBitmap& icon);
    virtual ~CaptionBar();

protected:
    virtual void OnLeftDown(wxMouseEvent& event);
    virtual void OnEraseBG(wxEraseEvent& event);
    virtual void OnPaint(wxPaintEvent& event);
};
#endif // CAPTIONBAR_H
