/***************************************************************
 * Name:      Printout.cpp
 * Purpose:   Implements printout class for shape canvas
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-05-06
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/
#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/Printout.h"
#include "wx/wxsf/ScaledDC.h"
#include "wx/wxsf/ShapeCanvas.h"

wxSFPrintout::wxSFPrintout(const wxString& title, wxSFShapeCanvas *canvas)
: wxPrintout(title)
{
    m_pCanvas = canvas;
}

wxSFPrintout::~wxSFPrintout()
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

bool wxSFPrintout::HasPage(int page)
{
    return (page == 1);
}

bool wxSFPrintout::OnBeginDocument(int startPage, int endPage)
{
    // HINT: perform custom actions...

    return wxPrintout::OnBeginDocument(startPage, endPage);
}

void wxSFPrintout::OnEndDocument()
{
    // HINT: perform custom actions ...

    wxPrintout::OnEndDocument();
}

bool wxSFPrintout::OnPrintPage(int page)
{
	wxUnusedVar( page );

    wxASSERT_MSG(m_pCanvas, wxT("Shape canvas must be set in the wxSFPrintout class instance."));

    wxDC *dc = GetDC();
    if (dc && m_pCanvas)
    {
        // get grawing size
        wxRect fitRect, totalBB = m_pCanvas->GetTotalBoundingBox();
        wxCoord maxX = totalBB.GetRight();
        wxCoord maxY = totalBB.GetBottom();

        // set printing mode
        switch( m_pCanvas->GetPrintMode() )
        {
            case wxSFShapeCanvas::prnFIT_TO_PAGE:
                FitThisSizeToPage(wxSize(maxX, maxY));
                fitRect = GetLogicalPageRect();
                break;

            case wxSFShapeCanvas::prnFIT_TO_PAPER:
                FitThisSizeToPaper(wxSize(maxX, maxY));
                fitRect = GetLogicalPaperRect();
                break;

            case wxSFShapeCanvas::prnFIT_TO_MARGINS:
                FitThisSizeToPageMargins(wxSize(maxX, maxY), *g_pageSetupData);
                fitRect = GetLogicalPageMarginsRect(*g_pageSetupData);
                break;

            case wxSFShapeCanvas::prnMAP_TO_PAGE:
                MapScreenSizeToPage();
                fitRect = GetLogicalPageRect();
                break;

            case wxSFShapeCanvas::prnMAP_TO_PAPER:
                MapScreenSizeToPaper();
                fitRect = GetLogicalPaperRect();
                break;

            case wxSFShapeCanvas::prnMAP_TO_MARGINS:
                MapScreenSizeToPage();
                fitRect = GetLogicalPageMarginsRect(*g_pageSetupData);
                break;

            case wxSFShapeCanvas::prnMAP_TO_DEVICE:
                MapScreenSizeToDevice();
                fitRect = GetLogicalPageRect();
                break;
        }

        // This offsets the image so that it is centered within the reference
        // rectangle defined above.

        wxCoord xoff = (fitRect.width - maxX - totalBB.GetLeft()) / 2;
        wxCoord yoff = (fitRect.height - maxY - totalBB.GetTop()) / 2;

        switch( m_pCanvas->GetPrintHAlign() )
        {
            case wxSFShapeCanvas::halignLEFT:
                xoff = 0;
                break;

            case wxSFShapeCanvas::halignRIGHT:
                xoff = fitRect.width - totalBB.GetWidth();
                break;

            default:
                break;
        }

        switch( m_pCanvas->GetPrintVAlign() )
        {
            case wxSFShapeCanvas::valignTOP:
                yoff = 0;
                break;

            case wxSFShapeCanvas::valignBOTTOM:
                yoff = fitRect.height - totalBB.GetHeight();
                break;

            default:
                break;
        }

        OffsetLogicalOrigin(xoff, yoff);

        // store current canvas properties
        double prevScale = m_pCanvas->GetScale();
        long prevStyle = m_pCanvas->GetStyle();
        wxColour prevColour = m_pCanvas->GetCanvasColour();

        // disable canvas background drawing if required
        if( !m_pCanvas->ContainsStyle( wxSFShapeCanvas::sfsPRINT_BACKGROUND ) )
        {
            m_pCanvas->RemoveStyle( wxSFShapeCanvas::sfsGRADIENT_BACKGROUND );
            m_pCanvas->RemoveStyle( wxSFShapeCanvas::sfsGRID_SHOW );
            m_pCanvas->SetCanvasColour( *wxWHITE);
        }

        // draw the canvas content without any scale (dc is scaled by the printing framework)
		double nScale = 1;
		if( wxSFShapeCanvas::IsGCEnabled() ) dc->GetUserScale( &nScale, &nScale );
        m_pCanvas->SetScale(1);

		#ifdef __WXMSW__
		wxSFScaledDC sdc( (wxWindowDC*)dc, nScale );
		sdc.PrepareGC();
        m_pCanvas->DrawContent(sdc, sfNOT_FROM_PAINT);
        #else
		m_pCanvas->DrawContent(*dc, sfNOT_FROM_PAINT);
		#endif

        m_pCanvas->SetScale(prevScale);

        // restore previous canvas properties if needed
        if( !m_pCanvas->ContainsStyle( wxSFShapeCanvas::sfsPRINT_BACKGROUND ) )
        {
            m_pCanvas->SetStyle( prevStyle );
            m_pCanvas->SetCanvasColour( prevColour );
        }

        return true;
    }
    else
        return false;
}

void wxSFPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}
