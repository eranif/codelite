/***************************************************************
 * Name:      CurveShape.h
 * Purpose:   Defines curve shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFCURVESHAPE_H
#define _WXSFCURVESHAPE_H

#include <wx/wxsf/LineShape.h>

/*!
 * \brief Interpolation line shape. The class extends wxSFLineShape class and allows
 * user to create curved connection line.
 */
class WXDLLIMPEXP_SF wxSFCurveShape : public wxSFLineShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxSFCurveShape);

    /*! \brief Default constructor. */
    wxSFCurveShape();
    /*!
     * \brief User constructor.
     * \param src ID of the source shape
     * \param trg ID of the target shape
     * \param path Array of line's control points
     * \param manager Parent parent diagram manager
     */
	wxSFCurveShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager);
	/*!
     * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFCurveShape(const wxSFCurveShape& obj);
	/*! \brief Destructor. */
    virtual ~wxSFCurveShape();

    // public virtual function
    /*!
     * \brief Get line's bounding box. The function can be overrided if neccessary.
     * \return Bounding rectangle
     */
    virtual wxRect GetBoundingBox();

    // public functions
    /*!
     * \brief Get a line point laying on the given line segment and shifted
     * from the beggining of the segment by given offset.
     * \param segment Zero-based index of the line segment
     * \param offset Real value in the range from 0 to 1 which determines
     * the linepoint offset inside the line segment
     * \return Line point
     */
    wxRealPoint GetPoint(size_t segment, double offset);

protected:

    // protected virtual functions
    /*!
     * \brief Internal function used for drawing of completed line shape.
     * \param dc Refernce of the device context where the shape will be darwn to
     */
	virtual void DrawCompleteLine(wxDC& dc);

private:

	// private functions
	/*! \brief Auxiliary drawing function. */
	void GetSegmentQuaternion(size_t segment, wxRealPoint& A, wxRealPoint& B, wxRealPoint& C, wxRealPoint& D);
	/*! \brief Auxiliary drawing function. */
	void Catmul_Rom_Kubika(const wxRealPoint& A, const wxRealPoint& B, const wxRealPoint& C, const wxRealPoint& D, wxDC& dc);
	/*! \brief Auxiliary drawing function. */
	wxRealPoint Coord_Catmul_Rom_Kubika(const wxRealPoint& p1, const wxRealPoint& p2, const wxRealPoint& p3, const wxRealPoint& p4, double t);

};

#endif //_WXSFCURVESHAPE_H
