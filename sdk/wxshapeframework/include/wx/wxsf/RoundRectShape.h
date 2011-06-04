/***************************************************************
 * Name:      RoundRectShape.h
 * Purpose:   Defines rounded rectangular shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFROUNDRECTSHAPE_H
#define _WXSFROUNDRECTSHAPE_H

#include <wx/wxsf/RectShape.h>

// default values

/*! \brief Default value of wxSFRoundRectShape::m_nRadius data member. */
#define sfdvROUNDRECTSHAPE_RADIUS 20

/*!
 * \brief Class ecapsulating rounded rectangle. It extends the basic rectangular shape.
 */
class WXDLLIMPEXP_SF wxSFRoundRectShape :	public wxSFRectShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFRoundRectShape);

    /*! \brief Default constructor. */
	wxSFRoundRectShape(void);
	/*!
     * \brief User constructor.
	 * \param pos Initial position
	 * \param size Initial size
	 * \param radius Corner radius
	 * \param manager Pointer of parent diagram manager
	 */
	wxSFRoundRectShape(const wxRealPoint& pos, const wxRealPoint &size, double radius, wxSFDiagramManager* manager);
	/*!
     * \brief Copy constructor.
	 * \param obj Refernce to the source object.
	 */
	wxSFRoundRectShape(const wxSFRoundRectShape& obj);
	/*! \brief Destructor. */
	virtual ~wxSFRoundRectShape(void);

	// public virtual functions
    /*!
	 * \brief Test whether the given point is inside the shape. The function
     * can be overrided if neccessary.
     * \param pos Examined point
     * \return TRUE if the point is inside the shape area, otherwise FALSE
     */
	virtual bool Contains(const wxPoint &pos);

	// public member data accessors
	/*!
	 * \brief Set corner radius.
	 * \param radius New corner radius
	 */
	void SetRadius(double radius){m_nRadius = radius;}
	/*!
	 * \brief Get current corner radius.
	 * \return Current corner radius
	 */
	double GetRadius(){return m_nRadius;}

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

	// protected functions
	/*!
     * \brief Auxiliary function. Checks whether the point is inside a circle with given center. The circle's radius
	 * is the roundrect corner radius.
	 * \param pos Examined point
	 * \param center Circle center
	 */
	bool IsInCircle(const wxPoint& pos, const wxPoint& center);

	// protected data members
	/*! \brief Corner radius. */
	double m_nRadius;

private:

	// private functions

	 /*! \brief Initialize serializable properties. */
	void MarkSerializableDataMembers();
};

#endif //_WXSFROUNDRECTSHAPE_H
