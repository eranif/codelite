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
    ~CaptionBar() override = default;

protected:
    void OnLeftDown(wxMouseEvent& event) override;
    void OnEraseBG(wxEraseEvent& event) override;
    void OnPaint(wxPaintEvent& event) override;
};
#endif // CAPTIONBAR_H
