#include "clToolBarButtonBase.h"
#include <wx/settings.h>

#ifndef CL_BUILD
#if defined(WXUSINGDLL_CL) || defined(USE_SFTP) || defined(PLUGINS_DIR)
#define CL_BUILD
#endif
#endif
#ifdef CL_BUILD
#include "drawingutils.h"
#endif

static wxBitmap MakeDisabledBitmap(const wxBitmap& bmp)
{
#ifdef CL_BUILD
#ifdef __WXOSX__
    return DrawingUtils::CreateGrayBitmap(bmp);
#else
    return DrawingUtils::CreateDisabledBitmap(bmp);
#endif
#else
    return bmp.ConvertToDisabled(255);
#endif
}

clToolBarButtonBase::clToolBarButtonBase(clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label,
                                         size_t flags)
    : m_toolbar(parent)
    , m_id(id)
    , m_bmp(bmp)
    , m_label(label)
    , m_flags(flags)
    , m_renderFlags(0)
{
}

clToolBarButtonBase::~clToolBarButtonBase() {}

void clToolBarButtonBase::Render(wxDC& dc, const wxRect& rect)
{
    m_dropDownArrowRect = wxRect();
    m_buttonRect = rect;

    wxColour penColour;
    wxColour bgColour;
    wxColour textColour;

#ifdef __WXMSW__
    wxColour bgHighlightColour("rgb(153, 209, 255)");
    penColour = bgHighlightColour;
#else
    wxColour bgHighlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
#endif
    if(IsEnabled()) {
        penColour = (IsHover() || IsPressed() || IsChecked()) ? bgHighlightColour
                                                              : wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);

        bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
        if(IsHover() || IsChecked()) {
            bgColour = bgHighlightColour;
        } else if(IsPressed()) {
            bgColour = penColour;
        }

        textColour = (IsHover() || IsPressed() || IsChecked()) ? wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT)
                                                               : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    } else {
        // A disabled button
        penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
        bgColour = penColour;
        textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    }

    dc.SetBrush(bgColour);
    dc.SetPen(penColour);
    dc.DrawRectangle(rect);

    wxCoord xx = rect.GetX();
    wxCoord yy = 0;
    xx += CL_TOOL_BAR_X_MARGIN;

    if(m_bmp.IsOk()) {
        wxBitmap bmp(m_bmp);
        if(!IsEnabled()) { bmp = MakeDisabledBitmap(m_bmp); }
        yy = (rect.GetHeight() - bmp.GetScaledHeight()) / 2 + rect.GetY();
        dc.DrawBitmap(bmp, wxPoint(xx, yy));
        xx += bmp.GetScaledWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        dc.SetTextForeground(textColour);
        wxSize sz = dc.GetTextExtent(m_label);
        yy = (rect.GetHeight() - sz.GetHeight()) / 2 + rect.GetY();
        dc.DrawText(m_label, wxPoint(xx, yy));
        xx += sz.GetWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    // Do we need to draw a drop down arrow?
    if(HasMenu()) {
        // draw a drop down menu
        m_dropDownArrowRect =
            wxRect(xx, rect.GetY(), (2 * CL_TOOL_BAR_X_MARGIN) + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE, rect.GetHeight());
        dc.DrawLine(wxPoint(xx, rect.GetY()), wxPoint(xx, rect.GetY() + rect.GetHeight()));
        xx += CL_TOOL_BAR_X_MARGIN;

        wxPoint points[3];
        points[0].x = xx;
        points[0].y = (rect.GetHeight() - CL_TOOL_BAR_DROPDOWN_ARROW_SIZE) / 2 + rect.GetY();

        points[1].x = xx + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        points[1].y = points[0].y;

        points[2].x = xx + (CL_TOOL_BAR_DROPDOWN_ARROW_SIZE / 2);
        points[2].y = points[0].y + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        dc.SetPen(textColour);
        dc.SetBrush(textColour);
        dc.DrawPolygon(3, points);

        xx += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        xx += CL_TOOL_BAR_X_MARGIN;
    }
}
