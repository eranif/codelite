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
#include <wx/dcclient.h>
#include <math.h>

#include <wx/wxsf/Defs.h>

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
#endif // _WXSFSCALEDDC_H
