/***************************************************************
 * Name:      FixedRectShape.h
 * Purpose:   Defines square shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSQUARESHAPE_H
#define _WXSFSQUARESHAPE_H

#include <wx/wxsf/RectShape.h>

/*!
 * \brief Class encapsulating the square shape. It extends the basic rectangular shape.
 * \sa wxSFRectShape
 */
class WXDLLIMPEXP_SF wxSFSquareShape : public wxSFRectShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFSquareShape);

    /*! \brief  Default constructor. */
	wxSFSquareShape(void);
	/*!
	 * \brief User constructor.
	 * \param pos Initial position
	 * \param size Initial size
	 * \param manager Pointer to parent diagram manager
	 */
	wxSFSquareShape(const wxRealPoint& pos, double size, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFSquareShape(const wxSFSquareShape& obj);
	/*! \brief Destructor. */
    virtual ~wxSFSquareShape();

    // public virtual functions
    /*!
	 * \brief Scale the shape size by in both directions. The function can be overrided if necessary
     * (new implementation should call default one ore scale shape's children manualy if neccesary).
     * \param x Horizontal scale factor
     * \param y Vertical scale factor
     * \param children TRUE if the shape's children shoould be scaled as well, otherwise the shape will be updated after scaling via Update() function.
     */
    virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);
	/*!
	 * \brief Event handler called during dragging of the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * Default implementation does nothing.
	 * \param handle Reference to dragged handle
	 */
    virtual void OnHandle(wxSFShapeHandle& handle);
};

#endif //_WXSFSQUARESHAPE_H
