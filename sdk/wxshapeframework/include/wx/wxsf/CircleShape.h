/***************************************************************
 * Name:      CircleShape.h
 * Purpose:   Defines circle shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFCIRCLESHAPE_H
#define _WXSFCIRCLESHAPE_H

#include <wx/wxsf/FixedRectShape.h>

/*! \brief Class encapsulating the circle shape. */
class WXDLLIMPEXP_SF wxSFCircleShape : public wxSFSquareShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxSFCircleShape);

    /*! \brief Default constructor. */
	wxSFCircleShape(void);
	/*! \brief
     * User constructor.
	 * \param pos Initial shape position
	 * \param radius Circle radius
	 * \param manager Pointer to parent manager
	 */
	wxSFCircleShape(const wxRealPoint& pos, double radius, wxSFDiagramManager* manager);
	/*! \brief
     * Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFCircleShape(const wxSFCircleShape& obj);
	/*! \brief Destructor. */
    virtual ~wxSFCircleShape();

    // public virtual functions
    /*!
	 * \brief Test whether the given point is inside the shape. The function
     * can be overrided if neccessary.
     * \param pos Examined point
     * \return TRUE if the point is inside the shape area, otherwise FALSE
     */
	virtual bool Contains(const wxPoint& pos);
	/*!
	 * \brief Get intersection point of the shape border and a line leading from
	 * 'start' point to 'end' point. The function can be overrided if neccessary.
	 * \param start Starting point of the virtual intersection line
     * \param end Ending point of the virtual intersection line
	 * \return Intersection point
	 */
	virtual wxRealPoint GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end);

protected:

    // protected virtual functions
	/*!
	 * \brief Draw the shape in the normal way. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawNormal(wxDC& dc);
	/*!
	 * \brief Draw the shape in the hower mode (the mouse cursor is above the shape).
	 * The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHover(wxDC& dc);
	/*!
	 * \brief Draw the shape in the highlighted mode (another shape is dragged over this
	 * shape and this shape will accept the dragged one if it will be dropped on it).
	 * The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shape will be drawn to
	 */
	virtual void DrawHighlighted(wxDC& dc);
	/*!
	 * \brief Draw shadow under the shape. The function can be overrided if neccessary.
	 * \param dc Reference to device context where the shadow will be drawn to
	 */
	virtual void DrawShadow(wxDC& dc);
};

#endif //_WXSFCIRCLESHAPE_H
