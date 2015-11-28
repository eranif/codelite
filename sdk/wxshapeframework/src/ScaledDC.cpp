/***************************************************************
 * Name:      ScaledDC.cpp
 * Purpose:   Implements scaled DC class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-11-7
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/
 
#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/ScaledDC.h"

#if wxVERSION_NUMBER >= 2900

wxSFScaledDC::wxSFScaledDC( wxWindowDC *target, double scale) : wxDC( new wxSFDCImplWrapper( target->GetImpl(), scale ) )
{
}

wxSFScaledDC::~wxSFScaledDC()
{
}

#else // ! wxVERSION_NUMBER >= 2900

#include "wx/wxsf/CommonFcn.h"

bool wxSFScaledDC::m_fEnableGC = false;

//----------------------------------------------------------------------------------//
// Constructor and destructor
//----------------------------------------------------------------------------------//

wxSFScaledDC::wxSFScaledDC( wxWindowDC *target, double scale)
{
	m_nScale = scale;
	m_pTargetDC = target;
	
#if wxUSE_GRAPHICS_CONTEXT
    m_pGC = wxGraphicsContext::Create( *m_pTargetDC );
	//m_pGC->Scale( scale, scale );
#endif
}

wxSFScaledDC::~wxSFScaledDC()
{
#if wxUSE_GRAPHICS_CONTEXT
    if( m_pGC ) delete m_pGC;
#endif
}

//----------------------------------------------------------------------------------//
// wxGraphicContext related functions
//----------------------------------------------------------------------------------//

void wxSFScaledDC::InitGC()
{
#if wxUSE_GRAPHICS_CONTEXT
	m_pGC->PushState();
	
	m_pGC->Scale( m_nScale, m_nScale );
	
    /*m_pGC->SetPen( this->GetPen() );
    m_pGC->SetBrush( this->GetBrush() );
	
    m_pGC->SetFont( this->GetFont(), this->GetTextForeground() );*/
#endif
}

void wxSFScaledDC::UninitGC()
{
#if wxUSE_GRAPHICS_CONTEXT
    /*m_pGC->SetPen( wxNullPen );
    m_pGC->SetBrush( wxNullBrush );
    m_pGC->SetFont( wxNullFont, *wxBLACK );*/
	
	m_pGC->PopState();
#endif
}

void wxSFScaledDC::PrepareGC()
{
#if wxUSE_GRAPHICS_CONTEXT && !defined(__WXMAC__)
    int x, y;
    GetDeviceOrigin(&x, &y);

    //m_pGC->Translate( x / m_nScale, y / m_nScale );
    m_pGC->Translate( x, y );
#endif
}

//----------------------------------------------------------------------------------//
// Wrapped wxDC functions
//----------------------------------------------------------------------------------//

void wxSFScaledDC::CalcBoundingBox(wxCoord x, wxCoord y)
{
	m_pTargetDC->CalcBoundingBox(x, y);
}
bool wxSFScaledDC::CanDrawBitmap() const
{
	return m_pTargetDC->CanDrawBitmap();
}
bool wxSFScaledDC::CanGetTextExtent() const
{
	return m_pTargetDC->CanGetTextExtent();
}
void wxSFScaledDC::Clear() 
{
	m_pTargetDC->Clear();
}
void wxSFScaledDC::ComputeScaleAndOrigin()
{
	m_pTargetDC->ComputeScaleAndOrigin();
}
	bool wxSFScaledDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC* source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
	{
		return m_pTargetDC->Blit( Scale(xdest), Scale(ydest), width, height, source, xsrc, ysrc, rop, useMask, xsrcMask, ysrcMask);
	}
void wxSFScaledDC::DoCrossHair(wxCoord x, wxCoord y)
{
	m_pTargetDC->CrossHair( Scale(x), Scale(y) );
}
void wxSFScaledDC::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
	if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
		wxGraphicsPath path = m_pGC->CreatePath();
		
		double dist, sang, eang;
		
		dist = wxSFCommonFcn::Distance( wxRealPoint(x2, y2), wxRealPoint(xc, yc) );
		sang = acos( (x2 - xc) / dist ) + ( yc > y2 ? wxSF::PI : 0 );
		
		dist = wxSFCommonFcn::Distance( wxRealPoint(x1, y1), wxRealPoint(xc, yc) );
		eang = acos( (x1 - xc) / dist ) + ( yc > y1 ? wxSF::PI : 0 );
		
		path.AddArc( xc, yc, dist, sang, eang, true );
		
		m_pGC->StrokePath( path );
		
        UninitGC();
        #endif
    }
    else
		m_pTargetDC->DrawArc(Scale(x1), Scale(y1), Scale(x2), Scale(y2), Scale(xc), Scale(yc));
}
void wxSFScaledDC::DoDrawBitmap(const wxBitmap& bmp, wxCoord x, wxCoord y, bool useMask)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->DrawBitmap( bmp, x, y, bmp.GetWidth(), bmp.GetHeight() );
        UninitGC();
        #endif
    }
    else
		m_pTargetDC->DrawBitmap( bmp, Scale(x), Scale(y), useMask );
}
void wxSFScaledDC::DoDrawCheckMark(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	m_pTargetDC->DrawCheckMark(Scale(x), Scale(y), Scale(width), Scale(height));
}
void wxSFScaledDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->DrawEllipse(x, y, width, height );
        UninitGC();
        #endif
    }
    else
		m_pTargetDC->DrawEllipse( Scale(x), Scale(y), Scale(width), Scale(height) );
}
void wxSFScaledDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{
	m_pTargetDC->DrawEllipticArc(Scale(x), Scale(y), Scale(w), Scale(h), sa, ea);
}
void wxSFScaledDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
	m_pTargetDC->DrawIcon( icon, Scale(x), Scale(y) );
}
void wxSFScaledDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->StrokeLine(x1, y1, x2, y2);
        UninitGC();
        #endif
    }
    else
         m_pTargetDC->DrawLine(Scale(x1), Scale(y1), Scale(x2), Scale(y2));
}
void wxSFScaledDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
   if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();

        wxPoint2DDouble *pts = new wxPoint2DDouble[n];

        for(int i = 0; i < n; i++)
        {
            pts[0].m_x = points[0].x;
            pts[0].m_y = points[0].y;
        }

        m_pGC->StrokeLines(n, pts);

        delete [] pts;

        UninitGC();
        #endif
    }
    else
    {
        wxPoint *updPoints = new wxPoint[n];

        for(int i = 0; i < n; i++)
        {
            updPoints[i].x = (int)((double)points[i].x*m_nScale);
            updPoints[i].y = (int)((double)points[i].y*m_nScale);
        }

        m_pTargetDC->DrawLines(n, updPoints, Scale(xoffset), Scale(yoffset));

        delete [] updPoints;
    }
}
void wxSFScaledDC::DoDrawPoint(wxCoord x, wxCoord y)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->StrokeLine(x, y, x+1, y);
        UninitGC();
        #endif
    }
    else
        m_pTargetDC->DrawPoint(Scale(x), Scale(y));
}
void wxSFScaledDC::DoDrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        int nIndex = 0;

        InitGC();

        wxGraphicsPath gcPath = m_pGC->CreatePath();

        for(int i = 0; i < n; i++)
        {
            gcPath.MoveToPoint( points[nIndex].x, points[nIndex].y );
            for(int j = 0; j < count[i]; j++)
            {
                nIndex++;
                gcPath.AddLineToPoint( points[nIndex].x, points[nIndex].y );
            }
            gcPath.CloseSubpath();
        }

        m_pGC->Translate( xoffset, yoffset );
        m_pGC->DrawPath( gcPath );

        UninitGC();
        #endif
    }
    else
    {
        int nTotalPoints = 0;
		int i = 0;

        for(i = 0; i < n; i++) nTotalPoints += count[i];

        wxPoint *updPoints = new wxPoint[nTotalPoints];

        for(i = 0; i < nTotalPoints; i++)
        {
            updPoints[i].x = (int)((double)points[i].x*m_nScale);
            updPoints[i].y = (int)((double)points[i].y*m_nScale);
        }

        m_pTargetDC->DrawPolyPolygon(n, count, updPoints, Scale(xoffset), Scale(yoffset), fillStyle);

        delete [] updPoints;
    }
}
void wxSFScaledDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();

        wxGraphicsPath gcPath = m_pGC->CreatePath();
        gcPath.MoveToPoint( points[0].x, points[0].y );
        for(int i = 1; i < n; i++)
        {
            gcPath.AddLineToPoint( points[i].x, points[i].y );
        }
        gcPath.CloseSubpath();

        m_pGC->DrawPath( gcPath );

        UninitGC();
        #endif
    }
    else
    {
        wxPoint *updPoints = new wxPoint[n];

        for(int i = 0; i < n; i++)
        {
            updPoints[i].x = (int)((double)points[i].x*m_nScale);
            updPoints[i].y = (int)((double)points[i].y*m_nScale);
        }

        m_pTargetDC->DrawPolygon(n, updPoints, Scale(xoffset), Scale(yoffset), fillStyle);

        delete [] updPoints;
    }
}
void wxSFScaledDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->DrawRectangle(x, y, width, height);
        UninitGC();
        #endif
    }
    else
		m_pTargetDC->DrawRectangle(Scale(x), Scale(y), Scale(width), Scale(height));
}
void wxSFScaledDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->DrawText( text, x, y, angle );
        UninitGC();
        #endif
    }
    else
    {
        wxFont font = GetFont();
        wxFont prevfont = font;

        if(font != wxNullFont)
        {
            font.SetPointSize(int(font.GetPointSize()*m_nScale));
            SetFont(font);
        }
        m_pTargetDC->DrawRotatedText(text, Scale(x), Scale(y), angle);

        SetFont(prevfont);
    }
}
void wxSFScaledDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->DrawRoundedRectangle(x, y, width, height, radius);
        UninitGC();
        #endif
    }
    else
		m_pTargetDC->DrawRoundedRectangle(Scale(x), Scale(y), Scale(width), Scale(height), radius*m_nScale);
}
void wxSFScaledDC::DoDrawSpline(wxList* points)
{
	m_pTargetDC->DrawSpline( points );
}
void wxSFScaledDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
        InitGC();
        m_pGC->DrawText( text, x, y );
        UninitGC();
        #endif
    }
    else
    {
        wxFont font = GetFont();
        wxFont prevfont = font;

        if(font != wxNullFont)
        {
			int scaledSize = int(font.GetPointSize()*m_nScale);			
            font.SetPointSize( scaledSize ? scaledSize : 1 );
            SetFont(font);
        }
        m_pTargetDC->DrawText(text, Scale(x), Scale(y));

        SetFont(prevfont);
    }
}
bool wxSFScaledDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style)
{
	return m_pTargetDC->FloodFill( Scale(x), Scale(y), col, style );
}
wxBitmap wxSFScaledDC::DoGetAsBitmap(const wxRect *subrect) const
{
	return m_pTargetDC->GetAsBitmap( subrect );
}
void wxSFScaledDC::DoGetClippingBox(wxCoord* x, wxCoord* y, wxCoord* w, wxCoord* h) const
{
	m_pTargetDC->GetClippingBox( x, y, w, h );
}
void wxSFScaledDC::DoGetClippingRegion(wxCoord* x, wxCoord* y, wxCoord* w, wxCoord* h)
{
	wxUnusedVar( x );
	wxUnusedVar( y );
	wxUnusedVar( w );
	wxUnusedVar( h );

	// not implemented...
}
void wxSFScaledDC::DoGetDeviceOrigin(wxCoord* x, wxCoord* y) const
{
	m_pTargetDC->GetDeviceOrigin( x, y );
}
void wxSFScaledDC::DoGetLogicalOrigin(wxCoord* x, wxCoord* y) const
{
	m_pTargetDC->GetLogicalOrigin( x, y );
}
bool wxSFScaledDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
	return m_pTargetDC->GetPartialTextExtents( text, widths );
}
bool wxSFScaledDC::DoGetPixel(wxCoord x, wxCoord y, wxColour* col) const
{
	return m_pTargetDC->GetPixel( x, y, col );
}
void wxSFScaledDC::DoGetSize(int* width, int* height) const
{
	m_pTargetDC->GetSize( width, height );
}
void wxSFScaledDC::DoGetSizeMM(int* width, int* height) const
{
	m_pTargetDC->GetSizeMM( width, height );
}
void wxSFScaledDC::DoGetTextExtent(const wxString& string, wxCoord* x, wxCoord* y, wxCoord* descent, wxCoord* externalLeading, wxFont* theFont) const
{
	m_pTargetDC->GetTextExtent( string, x, y, descent, externalLeading, theFont );
}
void wxSFScaledDC::DoGradientFillConcentric(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, const wxPoint& circleCenter)
{
	wxRect updRct( rect.x * m_nScale, rect.y * m_nScale, rect.width * m_nScale, rect.height * m_nScale ); 
	
	m_pTargetDC->GradientFillConcentric( updRct, initialColour, destColour, circleCenter);
}
void wxSFScaledDC::DoGradientFillLinear(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, wxDirection nDirection)
{
	wxRect updRct( rect.x * m_nScale, rect.y * m_nScale, rect.width * m_nScale, rect.height * m_nScale );
	
	m_pTargetDC->GradientFillLinear( updRct, initialColour, destColour, nDirection );
}
void wxSFScaledDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	m_pTargetDC->SetClippingRegion( x, y, width, height );
}
void wxSFScaledDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
	wxUnusedVar( region );

	// not implemented...
}
void wxSFScaledDC::DrawObject(wxDrawObject* drawobject)
{
	m_pTargetDC->DrawObject( drawobject );
}
void wxSFScaledDC::EndDoc()
{
	m_pTargetDC->EndDoc();
}
void wxSFScaledDC::EndPage()
{
	m_pTargetDC->EndPage();
}
const wxBrush& wxSFScaledDC::GetBackground() const
{
	return m_pTargetDC->GetBackground();
}
int wxSFScaledDC::GetBackgroundMode() const
{
	return m_pTargetDC->GetBackgroundMode();
}
const wxBrush& wxSFScaledDC::GetBrush() const
{
	return m_pTargetDC->GetBrush();
}
wxCoord wxSFScaledDC::GetCharHeight() const
{
	return m_pTargetDC->GetCharHeight();
}
wxCoord wxSFScaledDC::GetCharWidth() const
{
	return m_pTargetDC->GetCharWidth();
}
int wxSFScaledDC::GetDepth() const
{
	return m_pTargetDC->GetDepth();
}
const wxFont& wxSFScaledDC::GetFont() const
{
	return m_pTargetDC->GetFont();
}
#ifdef __WXGTK__
GdkWindow* wxSFScaledDC::GetGDKWindow() const
{
	return m_pTargetDC->GetGDKWindow();
}
#endif
wxLayoutDirection wxSFScaledDC::GetLayoutDirection() const
{
	return m_pTargetDC->GetLayoutDirection();
}
int wxSFScaledDC::GetLogicalFunction() const
{
	return m_pTargetDC->GetLogicalFunction();
}
void wxSFScaledDC::GetLogicalScale(double* x, double* y)
{
	m_pTargetDC->GetLogicalScale( x, y );
}
int wxSFScaledDC::GetMapMode() const
{
	return m_pTargetDC->GetMapMode();
}
void wxSFScaledDC::GetMultiLineTextExtent(const wxString& string, wxCoord* width, wxCoord* height, wxCoord* heightLine, wxFont* font) const
{
	m_pTargetDC->GetMultiLineTextExtent( string, width, height, heightLine, font );
}
wxSize wxSFScaledDC::GetPPI() const
{
	return m_pTargetDC->GetPPI();
}
const wxPen& wxSFScaledDC::GetPen() const
{
	return m_pTargetDC->GetPen();
}
wxBitmap wxSFScaledDC::GetSelectedBitmap() const
{
	#ifndef __WXMAC__ 
	return m_pTargetDC->GetSelectedBitmap(); 
	#else 
	return wxNullBitmap; 
	#endif 
}
const wxColour& wxSFScaledDC::GetTextBackground() const
{
	return m_pTargetDC->GetTextBackground();
}
const wxColour& wxSFScaledDC::GetTextForeground() const
{
	return m_pTargetDC->GetTextForeground();
}
void wxSFScaledDC::GetUserScale(double* x, double* y) const
{
	m_pTargetDC->GetUserScale( x, y );
}
bool wxSFScaledDC::IsOk() const
{
	return m_pTargetDC->IsOk();
}
bool wxSFScaledDC::Ok() const
{
	return m_pTargetDC->Ok();
}
void wxSFScaledDC::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
	m_pTargetDC->SetAxisOrientation( xLeftRight, yBottomUp );
}
void wxSFScaledDC::SetBackground(const wxBrush& brush)
{
	m_pTargetDC->SetBackground( brush );
}
void wxSFScaledDC::SetBackgroundMode(int mode)
{
	m_pTargetDC->SetBackgroundMode( mode );
}
void wxSFScaledDC::SetBrush(const wxBrush& brush)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
		m_pGC->SetBrush( brush );
		#endif
	}
	m_pTargetDC->SetBrush( brush );
}
void wxSFScaledDC::SetDeviceOrigin(wxCoord x, wxCoord y)
{
	m_pTargetDC->SetDeviceOrigin( x, y );
}
void wxSFScaledDC::SetFont(const wxFont& font)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
		m_pGC->SetFont( font, this->GetTextForeground() );
		#endif
	}
	m_pTargetDC->SetFont( font );
}
void wxSFScaledDC::SetLayoutDirection(wxLayoutDirection dir)
{
	m_pTargetDC->SetLayoutDirection( dir );
}
void wxSFScaledDC::SetLogicalFunction(int function)
{
	m_pTargetDC->SetLogicalFunction( function );
}
void wxSFScaledDC::SetLogicalOrigin(wxCoord x, wxCoord y)
{
	m_pTargetDC->SetLogicalOrigin( x, y );
}
void wxSFScaledDC::SetLogicalScale(double x, double y)
{
	m_pTargetDC->SetLogicalScale( x, y );
}
void wxSFScaledDC::SetMapMode(int mode)
{
	m_pTargetDC->SetMapMode( mode );
}
void wxSFScaledDC::SetPalette(const wxPalette& palette)
{
	m_pTargetDC->SetPalette( palette );
}
void wxSFScaledDC::SetPen(const wxPen& pen)
{
    if( m_fEnableGC )
    {
        #if wxUSE_GRAPHICS_CONTEXT
		m_pGC->SetPen( pen );
		#endif
	}
	m_pTargetDC->SetPen( pen );
}
void wxSFScaledDC::SetTextBackground(const wxColour& colour)
{
	m_pTargetDC->SetTextBackground( colour );
}
void wxSFScaledDC::SetTextForeground(const wxColour& colour)
{
	m_pTargetDC->SetTextForeground( colour );
}
void wxSFScaledDC::SetUserScale(double x, double y)
{
	m_pTargetDC->SetUserScale( x, y );
}
bool wxSFScaledDC::StartDoc( const wxString& message )
{
	return m_pTargetDC->StartDoc( message );
}
void wxSFScaledDC::StartPage()
{
	m_pTargetDC->StartPage();
}

#endif // wxVERSION_NUMBER >= 2900
