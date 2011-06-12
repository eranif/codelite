/***************************************************************
 * Name:      ScaledDC.h
 * Purpose:   Defines scaled DC class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-11-7
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSCALEDDC_H
#define _WXSFSCALEDDC_H

#include <wx/graphics.h>
#include <wx/dc.h>
#include <math.h>
#include <wx/dcclient.h>
#include <wx/wxsf/Defs.h>

#if wxVERSION_NUMBER < 2900

/*! \brief Class acts as a wrapper for given DC class and provides modified
 * drawing functions cooperating with the shape canvas able to draw scaled graphics.
 * All drawing operations performed by the shapes should be done via this class otherwise
 * the global scalling capabilities provided by the shape canvas wont be available.
 * \sa wxSFShapeCanvas
 */


class WXDLLIMPEXP_SF wxSFScaledDC : public wxDC {

public:
	wxSFScaledDC( wxWindowDC* target, double scale );
	virtual ~wxSFScaledDC();
	
	// public member data accessors
	/*!
     * \brief Set the global graphics scale.
	 * \param scale Scale
	 */
	void SetScale(double scale){m_nScale = scale;}

	/**
	 * \brief Prepare wxGraphicsContext similiarly to PrepareDC() function.
	 */
    void PrepareGC();
	/**
	 * \brief Enable/Disable usage of wxGraphicsContext.
	 * \param enab Set to TRUE if the wxGraphicsContext should be used for drawing
	 */
    static void EnableGC(bool enab){m_fEnableGC = enab;}

public:
	virtual void CalcBoundingBox(wxCoord x, wxCoord y);
	virtual bool CanDrawBitmap() const;
	virtual bool CanGetTextExtent() const;
	virtual void Clear();
	virtual void ComputeScaleAndOrigin();
	virtual void DrawObject(wxDrawObject* drawobject);
	virtual void EndDoc();
	virtual void EndPage();
	virtual const wxBrush& GetBackground() const;
	virtual int GetBackgroundMode() const;
	virtual const wxBrush& GetBrush() const;
	virtual wxCoord GetCharHeight() const;
	virtual wxCoord GetCharWidth() const;
	virtual int GetDepth() const;
	virtual const wxFont& GetFont() const;
#ifdef __WXGTK__
	virtual GdkWindow* GetGDKWindow() const;
#endif
	virtual wxLayoutDirection GetLayoutDirection() const;
	virtual int GetLogicalFunction() const;
	virtual void GetLogicalScale(double *x, double *y);
	virtual int GetMapMode() const;
	virtual void GetMultiLineTextExtent(const wxString& string, wxCoord *width, wxCoord *height, wxCoord *heightLine = NULL, wxFont *font = NULL) const;
	virtual wxSize GetPPI() const;
	virtual const wxPen& GetPen() const;
	virtual wxBitmap GetSelectedBitmap() const;
	virtual const wxColour& GetTextBackground() const;
	virtual const wxColour& GetTextForeground() const;
	virtual void GetUserScale(double *x, double *y) const;
	virtual bool IsOk() const;
	virtual bool Ok() const;
	virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
	virtual void SetBackground(const wxBrush& brush);
	virtual void SetBackgroundMode(int mode);
	virtual void SetBrush(const wxBrush& brush);
	virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
	virtual void SetFont(const wxFont& font);
	virtual void SetLayoutDirection(wxLayoutDirection dir);
	virtual void SetLogicalFunction(int function);
	virtual void SetLogicalOrigin( wxCoord x, wxCoord y );
	virtual void SetLogicalScale(double x, double y);
	virtual void SetMapMode( int mode );
	virtual void SetPalette(const wxPalette& palette);
	virtual void SetPen(const wxPen& pen);
	virtual void SetTextBackground(const wxColour& colour);
	virtual void SetTextForeground(const wxColour& colour);
	virtual void SetUserScale( double x, double y );
	virtual bool StartDoc( const wxString& message );
	virtual void StartPage();

protected:
		// protected data members
	/*! \brief Global graphics scale. */
	double m_nScale;
	/*! \brief wxGraphicsContext usage flag. */
	static bool m_fEnableGC;
	/*! \brief Pointer to wrapped device context. */
	wxWindowDC *m_pTargetDC;

#if wxUSE_GRAPHICS_CONTEXT
	/*! \brief Pointer to wxGraphicsContext instance. */
    wxGraphicsContext *m_pGC;
#endif

    // protected function
	/**
	 * \brief Scale given value.
	 * \param val Value to scale
	 * \return Scaled value 
	 */
    wxCoord Scale(wxCoord val){return (wxCoord)ceil((double)val*m_nScale);}

	/**
	 * \brief Initialize wxGraphicsContext.
	 */
    void InitGC();
	/**
	 * \brief Uninitialize wxGraphicsContext.
	 */
    void UninitGC();

	virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop = wxCOPY, bool useMask = false, wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord);
	virtual void DoCrossHair(wxCoord x, wxCoord y);
	virtual void DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc);
	virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask = false);
	virtual void DoDrawCheckMark(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea);
	virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
	virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
	virtual void DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset);
	virtual void DoDrawPoint(wxCoord x, wxCoord y);
	virtual void DoDrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle);
	virtual void DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle = wxODDEVEN_RULE);
	virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle);
	virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius);
	virtual void DoDrawSpline(wxList *points);
	virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
	virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style = wxFLOOD_SURFACE);
	virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const;
	virtual void DoGetClippingBox(wxCoord *x, wxCoord *y, wxCoord *w, wxCoord *h) const;
	virtual void DoGetClippingRegion(wxCoord *x, wxCoord *y, wxCoord *w, wxCoord *h);
	virtual void DoGetDeviceOrigin(wxCoord *x, wxCoord *y) const;
	virtual void DoGetLogicalOrigin(wxCoord *x, wxCoord *y) const;
	virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const;
	virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;
	virtual void DoGetSize(int *width, int *height) const;
	virtual void DoGetSizeMM(int* width, int* height) const;
	virtual void DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *theFont = NULL) const;
	virtual void DoGradientFillConcentric(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, const wxPoint& circleCenter);
	virtual void DoGradientFillLinear(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, wxDirection nDirection = wxEAST);
	virtual void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	virtual void DoSetClippingRegionAsRegion(const wxRegion& region);

};

#else // ! wxVERSION_NUMBER < 2900

#include <wx/dcclient.h>
class WXDLLIMPEXP_SF wxSFScaledDC;

class wxSFDCImplWrapper : public wxDCImpl
{
public:
	friend class wxSFScaledDC;
	
	wxSFDCImplWrapper( wxDCImpl *orig, double scale ) : wxDCImpl( orig->GetOwner() )
	{
		m_pOrig = orig;
		m_nScale = scale;
	}
    virtual ~wxSFDCImplWrapper()
	{
	}

    wxDC *GetOwner() const { return m_pOrig->GetOwner(); }

    wxWindow* GetWindow() const { return m_pOrig->GetWindow(); }

    virtual bool IsOk() const { return m_pOrig->IsOk(); }

    // query capabilities

    virtual bool CanDrawBitmap() const { return m_pOrig->CanDrawBitmap(); }
    virtual bool CanGetTextExtent() const { return m_pOrig->CanGetTextExtent(); }

    // get Cairo context
    virtual void* GetCairoContext() const
    {
        return m_pOrig->GetCairoContext();
    }

    // query dimension, colour deps, resolution

    virtual void DoGetSize(int *width, int *height) const { m_pOrig->DoGetSize( width, height); }
    void GetSize(int *width, int *height) const
    {
        DoGetSize(width, height);
        return ;
    }

    wxSize GetSize() const
    {
        int w, h;
        DoGetSize(&w, &h);
        return wxSize(w, h);
    }

    virtual void DoGetSizeMM(int* width, int* height) const { m_pOrig->DoGetSizeMM( width, height); }

    virtual int GetDepth() const { return m_pOrig->GetDepth(); }
    virtual wxSize GetPPI() const { return m_pOrig->GetPPI(); }

    // Right-To-Left (RTL) modes

    virtual void SetLayoutDirection(wxLayoutDirection dir) { m_pOrig->SetLayoutDirection( dir ); }
    virtual wxLayoutDirection GetLayoutDirection() const { return m_pOrig->GetLayoutDirection(); }

    // page and document

    virtual bool StartDoc(const wxString& message) { return m_pOrig->StartDoc( message ); }
    virtual void EndDoc() { return m_pOrig->EndDoc(); }

    virtual void StartPage() { m_pOrig->StartPage(); }
    virtual void EndPage() { m_pOrig->EndPage(); }

    // flushing the content of this dc immediately eg onto screen
    virtual void Flush() { m_pOrig->Flush(); }

    // bounding box

    virtual void CalcBoundingBox(wxCoord x, wxCoord y) { m_pOrig->CalcBoundingBox( x, y); }

    wxCoord MinX() const { return m_pOrig->MinX(); }
    wxCoord MaxX() const { return m_pOrig->MaxX(); }
    wxCoord MinY() const { return m_pOrig->MinY(); }
    wxCoord MaxY() const { return m_pOrig->MaxY(); }

    // setters and getters

    virtual void SetFont(const wxFont& font) { m_pOrig->SetFont( font ); }
    virtual const wxFont& GetFont() const { return m_pOrig->GetFont(); }

    virtual void SetPen(const wxPen& pen) { m_pOrig->SetPen( pen ); }
    virtual const wxPen& GetPen() const { return m_pOrig->GetPen(); }

    virtual void SetBrush(const wxBrush& brush) { m_pOrig->SetBrush( brush ); }
    virtual const wxBrush& GetBrush() const { return m_pOrig->GetBrush(); }

    virtual void SetBackground(const wxBrush& brush) { m_pOrig->SetBackground( brush ); }
    virtual const wxBrush& GetBackground() const { return m_pOrig->GetBackground(); }

    virtual void SetBackgroundMode(int mode) { m_pOrig->SetBackgroundMode( mode ); }
    virtual int GetBackgroundMode() const { return m_pOrig->GetBackgroundMode(); }

    virtual void SetTextForeground(const wxColour& colour) { m_pOrig->SetTextForeground( colour ); }
    virtual const wxColour& GetTextForeground() const { return m_pOrig->GetTextForeground(); }

    virtual void SetTextBackground(const wxColour& colour) { m_pOrig->SetTextBackground( colour ); }
    virtual const wxColour& GetTextBackground() const { return m_pOrig->GetTextBackground(); }

#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) { m_pOrig->SetPalette( palette ); }
#endif // wxUSE_PALETTE

    // inherit the DC attributes (font and colours) from the given window
    //
    // this is called automatically when a window, client or paint DC is
    // created
    virtual void InheritAttributes(wxWindow *win) { m_pOrig->InheritAttributes( win ); }


    // logical functions

    virtual void SetLogicalFunction(wxRasterOperationMode function) { m_pOrig->SetLogicalFunction( function ); }
    virtual wxRasterOperationMode GetLogicalFunction() const { return m_pOrig->GetLogicalFunction(); }

    // text measurement

    virtual wxCoord GetCharHeight() const { return m_pOrig->GetCharHeight(); }
    virtual wxCoord GetCharWidth() const { return m_pOrig->GetCharWidth(); }
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 const wxFont *theFont = NULL) const { m_pOrig->DoGetTextExtent( string, x, y, descent, externalLeading, theFont ); }
    virtual void GetMultiLineTextExtent(const wxString& string,
                                        wxCoord *width,
                                        wxCoord *height,
                                        wxCoord *heightLine = NULL,
                                        const wxFont *font = NULL) const { m_pOrig->GetMultiLineTextExtent( string, width, height, heightLine, font ); }
    virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const { return m_pOrig->DoGetPartialTextExtents( text, widths); }

    // clearing

    virtual void Clear() { m_pOrig->Clear(); }

    // clipping

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) { m_pOrig->DoSetClippingRegion( x, y, width, height ); }

    // NB: this function works with device coordinates, not the logical ones!
    virtual void DoSetDeviceClippingRegion(const wxRegion& region) { m_pOrig->DoSetDeviceClippingRegion( region ); }

    virtual void DoGetClippingBox(wxCoord *x, wxCoord *y,
                                  wxCoord *w, wxCoord *h) const { m_pOrig->DoGetClippingBox( x, y, w, h ); }

    virtual void DestroyClippingRegion() { m_pOrig->DestroyClippingRegion(); }


    // coordinates conversions and transforms

    virtual wxCoord DeviceToLogicalX(wxCoord x) const { return m_pOrig->DeviceToLogicalX(x); }
    virtual wxCoord DeviceToLogicalY(wxCoord y) const { return m_pOrig->DeviceToLogicalY(y); }
    virtual wxCoord DeviceToLogicalXRel(wxCoord x) const { return m_pOrig->DeviceToLogicalXRel(x); }
    virtual wxCoord DeviceToLogicalYRel(wxCoord y) const { return m_pOrig->DeviceToLogicalYRel(y); }
    virtual wxCoord LogicalToDeviceX(wxCoord x) const { return m_pOrig->LogicalToDeviceX(x); }
    virtual wxCoord LogicalToDeviceY(wxCoord y) const { return m_pOrig->LogicalToDeviceY(y); }
    virtual wxCoord LogicalToDeviceXRel(wxCoord x) const { return m_pOrig->LogicalToDeviceXRel(x); }
    virtual wxCoord LogicalToDeviceYRel(wxCoord y) const { return m_pOrig->LogicalToDeviceYRel(y); }

    virtual void SetMapMode(wxMappingMode mode) { m_pOrig->SetMapMode(mode); }
    virtual wxMappingMode GetMapMode() const { return m_pOrig->GetMapMode(); }

    virtual void SetUserScale(double x, double y) { m_pOrig->SetUserScale( x, y ); }
    virtual void GetUserScale(double *x, double *y) const { m_pOrig->GetUserScale( x, y ); }

    virtual void SetLogicalScale(double x, double y) { m_pOrig->SetLogicalScale( x, y ); }
    virtual void GetLogicalScale(double *x, double *y) { m_pOrig->GetLogicalScale( x, y ); }
	
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y) { m_pOrig->SetLogicalOrigin( x, y ); }
    virtual void DoGetLogicalOrigin(wxCoord *x, wxCoord *y) const { m_pOrig->DoGetLogicalOrigin( x, y); }

    virtual void SetDeviceOrigin(wxCoord x, wxCoord y) { m_pOrig->SetDeviceOrigin( x, y ); }
    virtual void DoGetDeviceOrigin(wxCoord *x, wxCoord *y) const { m_pOrig->DoGetDeviceOrigin( x, y ); }

    virtual void SetDeviceLocalOrigin( wxCoord x, wxCoord y ) { m_pOrig->SetDeviceLocalOrigin( x, y ); }

    virtual void ComputeScaleAndOrigin() { m_pOrig->ComputeScaleAndOrigin(); }

    // this needs to overidden if the axis is inverted
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp) { m_pOrig->SetAxisOrientation( xLeftRight, yBottomUp); }

    // ---------------------------------------------------------
    // the actual drawing API

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) 
	{
		return m_pOrig->DoFloodFill( ScaleCoord(x), ScaleCoord(y), col, style );
	}

    virtual void DoGradientFillLinear(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      wxDirection nDirection = wxEAST)
	{
		m_pOrig->DoGradientFillLinear( wxRect( ScaleInt(rect.x), ScaleInt(rect.y), ScaleInt(rect.width), ScaleInt(rect.height)),
										initialColour, destColour, nDirection );
	}

    virtual void DoGradientFillConcentric(const wxRect& rect,
                                        const wxColour& initialColour,
                                        const wxColour& destColour,
                                        const wxPoint& circleCenter)
	{
		m_pOrig->DoGradientFillConcentric( wxRect( ScaleInt(rect.x), ScaleInt(rect.y), ScaleInt(rect.width), ScaleInt(rect.height)),
											initialColour, destColour,
											wxPoint( ScaleInt(circleCenter.x), ScaleInt(circleCenter.y)) );
	}

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
	{
		return m_pOrig->DoGetPixel( (wxCoord)ceil((double)x*m_nScale), (wxCoord)ceil((double)y*m_nScale), col );
	}

    virtual void DoDrawPoint(wxCoord x, wxCoord y)
	{
		m_pOrig->DoDrawPoint( ScaleCoord(x), ScaleCoord(y) );
	}
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
	{
		m_pOrig->DoDrawLine( ScaleCoord(x1), ScaleCoord(y1), ScaleCoord(x2), ScaleCoord(y2) );
	}

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc)
	{
		m_pOrig->DoDrawArc( ScaleCoord(x1), ScaleCoord(y1),
							ScaleCoord(x2), ScaleCoord(y2),
							ScaleCoord(xc), ScaleCoord(yc) );
	}
						   
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
	{
		m_pOrig->DoDrawCheckMark( ScaleCoord(x), ScaleCoord(y), ScaleCoord(width), ScaleCoord(height) );
	}
								 
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea)
	{
		m_pOrig->DoDrawEllipticArc( ScaleCoord(x), ScaleCoord(y), ScaleCoord(w), ScaleCoord(h), sa, ea );
	}

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
	{
		m_pOrig->DoDrawRectangle( ScaleCoord(x), ScaleCoord(y), ScaleCoord(width), ScaleCoord(height) );
	}
	
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius)
	{
		m_pOrig->DoDrawRoundedRectangle( ScaleCoord(x), ScaleCoord(y), ScaleCoord(width), ScaleCoord(height), radius*m_nScale );
	}
	
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height)
	{
		m_pOrig->DoDrawEllipse( ScaleCoord(x), ScaleCoord(y), ScaleCoord(width), ScaleCoord(height) );
	}

    virtual void DoCrossHair(wxCoord x, wxCoord y)
	{
		m_pOrig->DoCrossHair( ScaleCoord(x), ScaleCoord(y) );
	}

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
	{
		m_pOrig->DoDrawIcon( icon, ScaleCoord(x), ScaleCoord(y) );
	}
	
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false)
	{
		m_pOrig->DoDrawBitmap( bmp, ScaleCoord(x), ScaleCoord(y), useMask );
	}

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y)
	{
        wxFont font = GetFont();
        wxFont prevfont = font;

        if(font != wxNullFont)
        {
            font.SetPointSize(int(font.GetPointSize()*m_nScale));
            SetFont(font);
        }
		
        m_pOrig->DoDrawText( text, ScaleCoord(x), ScaleCoord(y) );

        SetFont(prevfont);
	}
	
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle)
	{
        wxFont font = GetFont();
        wxFont prevfont = font;

        if(font != wxNullFont)
        {
            font.SetPointSize(int(font.GetPointSize()*m_nScale));
            SetFont(font);
        }
		
		m_pOrig->DoDrawRotatedText( text, ScaleCoord(x), ScaleCoord(y), angle );

        SetFont(prevfont);
	}

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY,
                        bool useMask = false,
                        wxCoord xsrcMask = wxDefaultCoord,
                        wxCoord ysrcMask = wxDefaultCoord)
	{
		return m_pOrig->DoBlit( ScaleCoord(xdest), ScaleCoord(ydest),
								width, height, source, xsrc, ysrc, rop, useMask, xsrcMask, ysrcMask );
	}

    virtual bool DoStretchBlit(wxCoord xdest, wxCoord ydest,
                               wxCoord dstWidth, wxCoord dstHeight,
                               wxDC *source,
                               wxCoord xsrc, wxCoord ysrc,
                               wxCoord srcWidth, wxCoord srcHeight,
                               wxRasterOperationMode rop = wxCOPY,
                               bool useMask = false,
                               wxCoord xsrcMask = wxDefaultCoord,
                               wxCoord ysrcMask = wxDefaultCoord)
	{
		return m_pOrig->DoStretchBlit( ScaleCoord(xdest), ScaleCoord(ydest), ScaleCoord(dstWidth), ScaleCoord(dstHeight),
								source, xsrc, ysrc, srcWidth, srcHeight, rop, useMask, xsrcMask, ysrcMask );
	}

    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const
	{
		return m_pOrig->DoGetAsBitmap( subrect );
	}

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset )
	{
        wxPoint *updPoints = new wxPoint[n];

        for(int i = 0; i < n; i++)
        {
            (updPoints + i)->x = ScaleInt((points + i)->x);
            (updPoints + i)->y = ScaleInt((points + i)->y);
        }

        m_pOrig->DoDrawLines( n, updPoints, ScaleCoord(xoffset), ScaleCoord(yoffset) );

        delete [] updPoints;
	}
	
    virtual void DrawLines(const wxPointList *list,
                           wxCoord xoffset, wxCoord yoffset )
	{
		int i = 0;
		wxPoint *pts = new wxPoint[list->GetCount()];
		
		wxPointList::compatibility_iterator node = list->GetFirst();
		while( node )
		{
			*(pts + i) = *node->GetData();
			i++;
			node = node->GetNext();
		}
		
		wxSFDCImplWrapper::DoDrawLines( i, pts, xoffset, yoffset );
		
		delete [] pts;
	}

    virtual void DoDrawPolygon(int n, wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE)
	{
        wxPoint *updPoints = new wxPoint[n];

        for(int i = 0; i < n; i++)
        {
            (updPoints + i)->x = ScaleInt((points + i)->x);
            (updPoints + i)->y = ScaleInt((points + i)->y);
        }

        m_pOrig->DoDrawPolygon(n, updPoints, ScaleCoord(xoffset), ScaleCoord(yoffset), fillStyle);

        delete [] updPoints;
	}
	
    virtual void DoDrawPolyPolygon(int n, int count[], wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               wxPolygonFillMode fillStyle)
	{
        int nTotalPoints = 0;

        for(int i = 0; i < n; i++)nTotalPoints += count[i];

        wxPoint *updPoints = new wxPoint[nTotalPoints];

        for(int i = 0; i < nTotalPoints; i++)
        {
            (updPoints + i)->x = ScaleInt((points + i)->x);
            (updPoints + i)->y = ScaleInt((points + i)->y);
        }

        m_pOrig->DoDrawPolyPolygon(n, count, updPoints, ScaleCoord(xoffset), ScaleCoord(yoffset), fillStyle);

        delete [] updPoints;
	}
							
    void DrawPolygon(const wxPointList *list,
                     wxCoord xoffset, wxCoord yoffset,
                     wxPolygonFillMode fillStyle )
	{
		int i = 0;
		wxPoint *pts = new wxPoint[list->GetCount()];
		
		wxPointList::compatibility_iterator node = list->GetFirst();
		while( node )
		{
			*(pts + i) = *node->GetData();
			i++;
			node = node->GetNext();
		}
		
		wxSFDCImplWrapper::DoDrawPolygon( i, pts, xoffset, yoffset, fillStyle );
		
		delete [] pts;
	}


#if wxUSE_SPLINES
    void DrawSpline(wxCoord x1, wxCoord y1,
                            wxCoord x2, wxCoord y2,
                            wxCoord x3, wxCoord y3);
    void DrawSpline(int n, wxPoint points[]);
    void DrawSpline(const wxPointList *points) { DoDrawSpline(points); }

    virtual void DoDrawSpline(const wxPointList *points)
	{
		wxPoint *pPt;
		wxPointList updPoints;
		
		wxPointList::compatibility_iterator node = points->GetFirst();
		while( node )
		{
			pPt = node->GetData();
			updPoints.Append( new wxPoint( ScaleInt(pPt->x), ScaleInt(pPt->y)) );
			node = node->GetNext();
		}
		
		m_pOrig->DoDrawSpline( &updPoints );
		
		updPoints.DeleteContents( true );
		updPoints.Clear();
	}
#endif

    // ---------------------------------------------------------
    // wxMemoryDC Impl API

    virtual void DoSelect(const wxBitmap& bmp) { m_pOrig->DoSelect( bmp ); }

    virtual const wxBitmap& GetSelectedBitmap() const { return m_pOrig->GetSelectedBitmap(); }
    virtual wxBitmap& GetSelectedBitmap() { return m_pOrig->GetSelectedBitmap(); }
	
    // ---------------------------------------------------------
    // wxPrinterDC Impl API

    virtual wxRect GetPaperRect() const { return m_pOrig->GetPaperRect(); }

    virtual int GetResolution() const { return m_pOrig->GetResolution(); }

protected:
	/*! \brief Pointer to original DC implementation. */
	wxDCImpl	*m_pOrig;
	/*! \brief Global graphics scale. */
	double		m_nScale;

	/**
	 * \brief Scale given value.
	 * \param val Value to scale
	 * \return Scaled value 
	 */
    wxCoord ScaleCoord(wxCoord val){return (wxCoord)ceil((double)val*m_nScale);}
	
	/**
	 * \brief Scale given value.
	 * \param val Value to scale
	 * \return Scaled value 
	 */
    wxCoord ScaleInt(int val){return (int)ceil((double)val*m_nScale);}
};

/*! \brief Class acts as a wrapper for given DC class and provides modified
 * drawing functions cooperating with the shape canvas able to draw scaled graphics.
 * All drawing operations performed by the shapes should be done via this class otherwise
 * the global scalling capabilities provided by the shape canvas wont be available.
 * \sa wxSFShapeCanvas
 */
class WXDLLIMPEXP_SF wxSFScaledDC : public wxDC {

public:
	wxSFScaledDC( wxWindowDC* target, double scale );
	virtual ~wxSFScaledDC();
};
#endif // wxVERSION_NUMBER < 2900

#endif // _WXSFSCALEDDC_H
