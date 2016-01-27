#include "clTabRenderer.h"
#include <wx/xrc/xmlres.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include "Notebook.h"

#if CL_BUILD
#include "drawingutils.h"
#endif

clTabColours::clTabColours() { InitDarkColours(); }

void clTabColours::InitFromColours(const wxColour& baseColour, const wxColour& textColour)
{
#if CL_BUILD
    if(DrawingUtils::IsDark(baseColour)) {
        activeTabTextColour = "WHITE";
        activeTabBgColour = baseColour;
        activeTabPenColour = baseColour.ChangeLightness(80);
        activeTabInnerPenColour = baseColour.ChangeLightness(120);

        inactiveTabTextColour = "WHITE";
        inactiveTabBgColour = baseColour.ChangeLightness(110);
        inactiveTabPenColour = inactiveTabBgColour.ChangeLightness(80);
        inactiveTabInnerPenColour = inactiveTabBgColour.ChangeLightness(120);

        tabAreaColour = baseColour.ChangeLightness(130);
        // 12x12 bitmap
        closeButton = wxXmlResource::Get()->LoadBitmap("notebook-dark-x");
        chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-grey");
    } else {
        activeTabTextColour = "BLACK";
        activeTabBgColour = baseColour;
        activeTabPenColour = baseColour.ChangeLightness(80);
        activeTabInnerPenColour = "WHITE";

        inactiveTabTextColour = "BLACK";
        inactiveTabBgColour = baseColour.ChangeLightness(90);
        inactiveTabPenColour = inactiveTabBgColour.ChangeLightness(80);
        inactiveTabInnerPenColour = baseColour;

        tabAreaColour = baseColour.ChangeLightness(130);
        // 12x12 bitmap
        closeButton = wxXmlResource::Get()->LoadBitmap("notebook-light-x");
        chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-black");
    }
#else
    wxUnusedVar(baseColour);
    wxUnusedVar(textColour);
#endif
}

void clTabColours::InitDarkColours()
{
    activeTabTextColour = "WHITE";
    activeTabBgColour = wxColour("#211e1e");
    activeTabPenColour = wxColour("#0e0d0d");
    activeTabInnerPenColour = wxColour("#343131");

    inactiveTabTextColour = wxColour("rgb(200, 200, 200)");
    inactiveTabBgColour = wxColour("#393838");
    inactiveTabPenColour = wxColour("#100f0f");
    inactiveTabInnerPenColour = wxColour("#535252");

    tabAreaColour = wxColour("#131111").ChangeLightness(115);
    // markerColour = wxColour("rgb(255, 128, 0)");
    markerColour = wxColour("rgb(105, 193, 240)");
    
    // 12x12 bitmap
    closeButton = wxXmlResource::Get()->LoadBitmap("notebook-dark-x");
    chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-grey");
}

void clTabColours::InitLightColours()
{
    activeTabTextColour = "#444444";
    activeTabPenColour = "#b9b9b9";
    activeTabInnerPenColour = "#ffffff";


    //    dc.GradientFillLinear( rect , wxColour( 0xCC,0xCC,0xCC ), wxColour( 0xA8,0xA8,0xA8 ) , wxSOUTH );
    //    dc.SetPen( wxPen( wxColour( 0x51,0x51,0x51 ) ) );
    inactiveTabTextColour = "#444444";
    //#ifdef __WXMSW__
    //    activeTabBgColour = "#FBFBFB";
    //#else
    activeTabBgColour = "#f0f0f0";
    //#endif
    inactiveTabBgColour = "#e5e5e5";
    inactiveTabPenColour = "#b9b9b9";
    inactiveTabInnerPenColour = "#ffffff";

    tabAreaColour = "#dcdcdc"; // wxColour("rgb(64, 64, 64)");
    markerColour = wxColour("rgb(105, 193, 240)");
    
    inactiveTabBgColour = tabAreaColour;
    
    // 12x12 bitmap
    closeButton = wxXmlResource::Get()->LoadBitmap("notebook-light-x");
    chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-black");
}

clTabInfo::clTabInfo(clTabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp)
    : m_tabCtrl(tabCtrl)
    , m_label(text)
    , m_bitmap(bmp)
    , m_window(page)
    , m_active(false)
{
    CalculateOffsets(style);
}

clTabInfo::clTabInfo(clTabCtrl* tabCtrl)
    : m_tabCtrl(tabCtrl)
    , m_window(NULL)
    , m_active(false)
    , m_textX(wxNOT_FOUND)
    , m_textY(wxNOT_FOUND)
    , m_bmpX(wxNOT_FOUND)
    , m_bmpY(wxNOT_FOUND)
    , m_bmpCloseX(wxNOT_FOUND)
    , m_bmpCloseY(wxNOT_FOUND)
{

    CalculateOffsets(0);
}

void clTabInfo::CalculateOffsets(size_t style)
{
    wxBitmap b(1, 1);
    wxMemoryDC memDC(b);
    m_bmpCloseX = wxNOT_FOUND;
    m_bmpCloseY = wxNOT_FOUND;

    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    memDC.SetFont(font);

    wxSize sz = memDC.GetTextExtent(m_label);
    wxSize fixedHeight = memDC.GetTextExtent("Tp");
    if(IS_VERTICAL_TABS(style)) {
        m_height = fixedHeight.GetHeight() + (5 * m_tabCtrl->GetArt()->ySpacer);
    } else {
        m_height = fixedHeight.GetHeight() + (4 * m_tabCtrl->GetArt()->ySpacer);
    }

#ifdef __WXGTK__
    // On GTK, limit the tab height
    if(m_height >= 30) {
        m_height = 30;
    }
#endif

    m_width = 0;
    if(!IS_VERTICAL_TABS(style) || true) {
        m_width += m_tabCtrl->GetArt()->majorCurveWidth;
        m_width += m_tabCtrl->GetArt()->smallCurveWidth;
    }
    m_width += m_tabCtrl->GetArt()->xSpacer;

    // bitmap
    m_bmpX = wxNOT_FOUND;
    m_bmpY = wxNOT_FOUND;

    if(m_bitmap.IsOk()) {
        m_bmpX = m_width;
        m_width += m_tabCtrl->GetArt()->xSpacer;
        m_width += m_bitmap.GetScaledWidth();
        m_bmpY = ((m_height - m_bitmap.GetScaledHeight()) / 2);
    }

    // Text
    m_textX = m_width;
    m_textY = ((m_height - sz.y) / 2);
    m_width += sz.x;

    // x button
    if((style & kNotebook_CloseButtonOnActiveTab)) {
        m_width += m_tabCtrl->GetArt()->xSpacer;
        m_bmpCloseX = m_width;
        m_bmpCloseY = ((m_height - 12) / 2);
        m_width += 12; // X button is 10 pixels in size
    }

    m_width += m_tabCtrl->GetArt()->xSpacer;
    if(!IS_VERTICAL_TABS(style) || true) {
        m_width += m_tabCtrl->GetArt()->majorCurveWidth;
        m_width += m_tabCtrl->GetArt()->smallCurveWidth;
    }

    // Update the rect width
    m_rect.SetWidth(m_width);

    if((style & kNotebook_RightTabs) || (style & kNotebook_LeftTabs)) {

        // swap the x and y coordinates
        wxSwap(m_height, m_width);
        wxSwap(m_bmpCloseY, m_bmpCloseX);
        wxSwap(m_bmpY, m_bmpX);
        wxSwap(m_textX, m_textY);
        
        m_width = m_tabCtrl->GetSize().GetWidth();
        m_textX += 1;
        
        m_rect.SetWidth(m_width);
        m_rect.SetHeight(m_height);
        m_rect.SetX(0);
        m_rect.SetY(0);
    }
}

void clTabInfo::SetBitmap(const wxBitmap& bitmap, size_t style)
{
    this->m_bitmap = bitmap;
    CalculateOffsets(style);
}

void clTabInfo::SetLabel(const wxString& label, size_t style)
{
    this->m_label = label;
    CalculateOffsets(style);
}

void clTabInfo::SetActive(bool active, size_t style)
{
    this->m_active = active;
    CalculateOffsets(style);
}
