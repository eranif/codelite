#include "clToolBarButtonBase.h"

#include "clToolBar.h"
#include "drawingutils.h"

// -----------------------------------------------
// Button base
// -----------------------------------------------
clToolBarButtonBase::clToolBarButtonBase(clToolBar* parent, wxWindowID id, int bmpId, const wxString& label,
                                         size_t flags)
    : m_toolbar(parent)
    , m_id(id)
    , m_bmpId(bmpId)
    , m_label(label)
    , m_flags(flags)
    , m_renderFlags(0)
    , m_menu(nullptr)
{
}

clToolBarButtonBase::~clToolBarButtonBase()
{
    wxDELETE(m_menu);
    if(m_toolbar && m_toolbar->GetBitmaps()) {
        m_toolbar->GetBitmaps()->Delete(m_bmpId);
    }
}

void clToolBarButtonBase::Render(wxDC& dc, const wxRect& rect)
{
    m_dropDownArrowRect = wxRect();
    m_buttonRect = rect;
    const clColours& colours = DrawingUtils::GetColours();
    wxColour textColour = colours.GetItemTextColour();
    wxColour penColour;
    wxColour buttonColour;

    const wxColour bgColour = DrawingUtils::GetMenuBarBgColour(m_toolbar->HasFlag(clToolBar::kMiniToolBar));
    bool isdark = DrawingUtils::IsDark(bgColour);
    if(IsEnabled() && (IsPressed() || IsChecked())) {
        wxColour pressBgColour = isdark ? bgColour.ChangeLightness(110) : bgColour.ChangeLightness(80);
        wxRect highlightRect = m_buttonRect;
        highlightRect.Inflate(1);

        penColour = isdark ? pressBgColour.ChangeLightness(120) : pressBgColour.ChangeLightness(80);
        dc.SetBrush(pressBgColour);
        dc.SetPen(penColour);
        dc.DrawRectangle(highlightRect);
        textColour = colours.GetSelItemTextColour();
        buttonColour = colours.GetSelbuttonColour();

    } else if(IsEnabled() && IsHover()) {
        // wxColour hoverColour = bgColour;
        // penColour = bgColour;
        // wxRect highlightRect = m_buttonRect;
        // dc.SetBrush(hoverColour);
        // dc.SetPen(penColour);
        // dc.DrawRoundedRectangle(highlightRect, 0);
        // textColour = colours.GetSelItemTextColour();
        // buttonColour = colours.GetSelbuttonColour();

    } else if(!IsEnabled()) {
        // A disabled button
        textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        buttonColour = textColour;
    } else {
        // Default
        if(DrawingUtils::IsDark(bgColour)) {
            buttonColour = colours.GetSelbuttonColour();
        } else {
            buttonColour = colours.GetButtonColour();
        }
        textColour = colours.GetItemTextColour();
    }
    wxCoord xx = m_buttonRect.GetX();
    wxCoord yy = 0;
    xx += m_toolbar->GetXSpacer();

    const wxBitmap& m_bmp = m_toolbar->GetBitmap(m_bmpId);
    if(m_bmp.IsOk()) {
        wxBitmap bmp(m_bmp);
        if(!IsEnabled()) {
            bmp = DrawingUtils::CreateDisabledBitmap(m_bmp);
        }
        yy = (m_buttonRect.GetHeight() - bmp.GetScaledHeight()) / 2 + m_buttonRect.GetY();
        dc.DrawBitmap(bmp, wxPoint(xx, yy));
        xx += bmp.GetScaledWidth();
        xx += m_toolbar->GetXSpacer();
    }

    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        dc.SetTextForeground(textColour);
        wxSize sz = dc.GetTextExtent(m_label);
        yy = (m_buttonRect.GetHeight() - sz.GetHeight()) / 2 + m_buttonRect.GetY();
        dc.DrawText(m_label, wxPoint(xx, yy));
        xx += sz.GetWidth();
        xx += m_toolbar->GetXSpacer();
    }

    // Do we need to draw a drop down arrow?
    if(IsMenuButton()) {
        // draw a drop down menu
        m_dropDownArrowRect =
            wxRect(xx, m_buttonRect.GetY(), (2 * m_toolbar->GetXSpacer()) + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE,
                   m_buttonRect.GetHeight());
        if(IsPressed() && IsEnabled()) {
            dc.DrawLine(wxPoint(xx, m_buttonRect.GetY() + 2),
                        wxPoint(xx, m_buttonRect.GetY() + m_buttonRect.GetHeight() - 2));
        }
        xx += m_toolbar->GetXSpacer();
        DrawingUtils::DrawDropDownArrow(m_toolbar, dc, m_dropDownArrowRect);
        xx += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        xx += m_toolbar->GetXSpacer();
    }
}

void clToolBarButtonBase::SetMenu(wxMenu* menu)
{
    wxDELETE(m_menu);
    m_menu = menu;
}

const wxBitmap& clToolBarButtonBase::GetBitmap() const { return m_toolbar->GetBitmap(m_bmpId); }
