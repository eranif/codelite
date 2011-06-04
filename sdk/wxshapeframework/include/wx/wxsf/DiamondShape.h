/***************************************************************
 * Name:      DiamondShape.h
 * Purpose:   Defines diamond shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFDIAMONDSHAPE_H
#define _WXSFDIAMONDSHAPE_H

#include <wx/wxsf/PolygonShape.h>

/*!
 * \brief Class encapsulating the diamond shape. It extends the basic polygon shape.
 * \sa wxSFPolygonShape
 */
class WXDLLIMPEXP_SF wxSFDiamondShape : public wxSFPolygonShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxSFDiamondShape);

    /*! \brief Default constructor. */
    wxSFDiamondShape();
    /*!
	 * \brief User constructor.
     * \param pos Initial position
     * \param manager Pointer to parent diagram manager
     */
	wxSFDiamondShape(const wxRealPoint& pos, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFDiamondShape(const wxSFDiamondShape& obj);
	/*! \brief Destructor. */
    virtual ~wxSFDiamondShape();

    // public virtual functions
    /*!
	 * \brief Test whether the given point is inside the shape. The function
     * can be overrided if neccessary.
     * \param pos Examined point
     * \return TRUE if the point is inside the shape area, otherwise FALSE
     */
    virtual bool Contains(const wxPoint& pos);
};

#endif //_WXSFDIAMONDSHAPE_H
