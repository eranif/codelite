/***************************************************************
 * Name:      OrthoShape.h
 * Purpose:   Defines orthogonal line shape class
 * Author:    Michal Bližňák (michal.bliznak@gmail.com)
 * Created:   2009-04-26
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFORTHOSHAPE_H
#define _WXSFORTHOSHAPE_H

#include <wx/wxsf/LineShape.h>

/*!
 * \brief Orthogonal line shape. The class extends wxSFLineShape class and allows
 * user to create connection line orthgonal to base axis.
 */
class WXDLLIMPEXP_SF wxSFOrthoLineShape : public wxSFLineShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxSFOrthoLineShape);
	
	/*! \brief Connection points used by the line segment */
	typedef struct _SEGMENTCPS {
		const wxSFConnectionPoint *cpSrc;
		const wxSFConnectionPoint *cpTrg;
	} SEGMENTCPS;

    /*! \brief Default constructor. */
    wxSFOrthoLineShape();
    /*!
     * \brief User constructor.
     * \param src ID of the source shape
     * \param trg ID of the target shape
     * \param path Array of line's control points
     * \param manager Parent parent diagram manager
     */
	wxSFOrthoLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager);
	/*!
     * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFOrthoLineShape(const wxSFOrthoLineShape& obj);
	/*! \brief Destructor. */
    virtual ~wxSFOrthoLineShape();

protected:

    // protected virtual functions
    /*!
     * \brief Internal function used for drawing of completed line shape.
     * \param dc Refernce of the device context where the shape will be darwn to
     */
	virtual void DrawCompleteLine(wxDC& dc);
	/*!
     * \brief Get index of the line segment intersecting the given point.
	 * \param pos Examined point
	 * \return Zero-based index of line segment located under the given point
	 */
	virtual int GetHitLinesegment(const wxPoint& pos);
	/**
	 * \brief Draw one orthogonal line segment.
	 * \param dc Device context
	 * \param src Starting point of the ortho line segment
	 * \param trg Ending point of the ortho line segment
	 * \param cps Connection points used by the line segment
	 */
	virtual void DrawLineSegment(wxDC& dc, const wxRealPoint& src, const wxRealPoint& trg, const SEGMENTCPS& cps );

	// protected functions
	/**
	 * \brief Get first part of orthogonal line segment.
	 * \param src Staring point of the ortho line segment
	 * \param trg Ending point of the ortho line segment
	 * \param subsrc Starting point of the first part of ortho line segment
	 * \param subtrg Ending point of the first part of ortho line segment
	 * \param cps Connection points used by the line segment
	 */
	void GetFirstSubsegment( const wxRealPoint& src, const wxRealPoint& trg, wxRealPoint& subsrc, wxRealPoint& subtrg, const SEGMENTCPS& cps );
	/**
	 * \brief Get middle part of orthogonal line segment.
	 * \param src Staring point of the ortho line segment
	 * \param trg Ending point of the ortho line segment
	 * \param subsrc Starting point of the second part of ortho line segment
	 * \param subtrg Ending point of the second part of ortho line segment
	 * \param cps Connection points used by the line segment
	 */
	void GetMiddleSubsegment( const wxRealPoint& src, const wxRealPoint& trg, wxRealPoint& subsrc, wxRealPoint& subtrg, const SEGMENTCPS& cps );
	/**
	 * \brief Get last part of orthogonal line segment.
	 * \param src Staring point of the ortho line segment
	 * \param trg Ending point of the ortho line segment
	 * \param subsrc Starting point of the third part of ortho line segment
	 * \param subtrg Ending point of the third part of ortho line segment
	 * \param cps Connection points used by the line segment
	 */
	void GetLastSubsegment( const wxRealPoint& src, const wxRealPoint& trg, wxRealPoint& subsrc, wxRealPoint& subtrg, const SEGMENTCPS& cps );
	
	/*!
	 * \brief Get direction of the line segment.
	 * \param src Staring point of the ortho line segment
	 * \param trg Ending point of the ortho line segment
	 * \param cps Connection points used by the line segment
	 * \return Direction number
	 */
	double GetSegmentDirection(const wxRealPoint& src, const wxRealPoint& trg, const SEGMENTCPS& cps );
	
	/*!
	 * \brief Determine which available connection points are used by given line segment.
	 * \param src Potential source connection point (can be NULL)
	 * \param trg Potential target connection point (can be NULL)
	 * \param i Index of the line segment
	 * \return Structure containing used connection points
	 */
	SEGMENTCPS GetUsedConnectionPoints( const wxSFConnectionPoint *src, const wxSFConnectionPoint *trg, size_t i) const;
	
	/*!
	 * \brief Determine whether a line using give connection point should be drawn as two-segmented.
	 * \param cps Used connection points
	 * \return TRUE if the line should be just two-segmented
	 */
	bool IsTwoSegment( const SEGMENTCPS& cps);
};

#endif //_WXSFORTHOSHAPE_H
