/***************************************************************
 * Name:      RectShape.h
 * Purpose:   Defines rectangular shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFRECTSHAPE_H
#define _WXSFRECTSHAPE_H

#include <wx/wxsf/ShapeBase.h>

// default values
/*! \brief Default value of wxSFRectShape::m_nRectSize data member. */
#define sfdvRECTSHAPE_SIZE wxRealPoint(100, 50)
/*! \brief Default value of wxSFRectShape::m_Fill data member. */
#define sfdvRECTSHAPE_FILL wxBrush(*wxWHITE)
/*! \brief Default value of wxSFRectShape::m_Border data member. */
#define sfdvRECTSHAPE_BORDER wxPen(*wxBLACK)
/*!
 * \brief Class encapsulates basic rectangle shape which is used as a base class
 * for many other shapes that can be bounded by a simple rectangle. The class
 * provides all functionality needed for manipulating the rectangle's (bounding box)
 * size and position.
 */
class WXDLLIMPEXP_SF wxSFRectShape : public wxSFShapeBase
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFRectShape);

    /*! \brief Default constructor. */
	wxSFRectShape(void);
	/*!
     * \brief User constructor.
	 * \param pos Initial position
	 * \param size Initial size
	 * \param manager Pointer to parent diagram manager
	 */
	wxSFRectShape(const wxRealPoint& pos, const wxRealPoint& size, wxSFDiagramManager* manager);
	/*!
     * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFRectShape(const wxSFRectShape& obj);
	/*! \brief Destructor. */
	virtual ~wxSFRectShape(void);

	// public virtual functions
    /*!
	 * \brief Get shapes's bounding box. The function can be overrided if neccessary.
     * \return Bounding rectangle
     */
	virtual wxRect GetBoundingBox();
	/*!
	 * \brief Get intersection point of the shape border and a line leading from
	 * 'start' point to 'end' point. The function can be overrided if neccessary.
	 * \param start Starting point of the virtual intersection line
     * \param end Ending point of the virtual intersection line
	 * \return Intersection point
	 */
	virtual wxRealPoint GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end);

	/*!
	 * \brief Function called by the framework responsible for creation of shape handles
     * at the creation time. The function can be overrided if neccesary.
     */
	virtual void CreateHandles();
	/*!
	 * \brief Event handler called during dragging of the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * \param handle Reference to dragged handle
	 */
	virtual void OnHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called when the user started to drag the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * \param handle Reference to dragged handle
	 */
	virtual void OnBeginHandle(wxSFShapeHandle& handle);

    /*! \brief Resize the shape to bound all child shapes. The function can be overrided if neccessary. */
	virtual void FitToChildren();
    /*!
	 * \brief Scale the shape size by in both directions. The function can be overrided if necessary
     * (new implementation should call default one ore scale shape's children manualy if neccesary).
     * \param x Horizontal scale factor
     * \param y Vertical scale factor
     * \param children TRUE if the shape's children shoould be scaled as well, otherwise the shape will be updated after scaling via Update() function.
     */
	virtual void Scale(double x, double y, bool children = sfWITHCHILDREN);

	// public data accessors
	/*!
	 * \brief Set rectangle's fill style.
	 * \param brush Refernce to a brush object
	 */
	void SetFill(const wxBrush& brush){m_Fill = brush;}
	/*!
	 * \brief Get current fill style.
	 * \return Current brush
	 */
	wxBrush GetFill() const {return m_Fill;}
	/*!
	 * \brief Set rectangle's border style.
	 * \param pen Reference to a pen object
	 */
	void SetBorder(const wxPen& pen){m_Border = pen;}
	/*!
	 * \brief Get current border style.
	 * \return Current pen
	 */
	wxPen GetBorder() const {return m_Border;}
	/*!
	 * \brief Set the rectangle size.
	 * \param size New size
	 */
	void SetRectSize(const wxRealPoint& size){m_nRectSize = size;}
	/*!
	 * \brief Set the rectangle size.
	 * \param x Horizontal size
	 * \param y Verical size
	 */
	void SetRectSize(double x, double y){m_nRectSize.x = x; m_nRectSize.y = y;}
	/*!
	 * \brief Get the rectangle size.
	 * \return Current size
	 */
	wxRealPoint GetRectSize() const {return m_nRectSize;}


protected:

	// protected data members
	/*! \brief Pen object used for drawing of the rectangle border. */
	wxPen m_Border;
	/*! \brief Brush object used for drawing of the rectangle body. */
	wxBrush m_Fill;
	/*! \brief The rectangle size. */
	wxRealPoint m_nRectSize;

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

	/*!
     * \brief Event handler called during dragging of the right shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnRightHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called during dragging of the left shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnLeftHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called during dragging of the top shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnTopHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called during dragging of the bottom shape handle.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnBottomHandle(wxSFShapeHandle& handle);

private:

	// private functions

	 /*! \brief Initialize serializable properties. */
	void MarkSerializableDataMembers();

    /*! \brief Auxiliary data member. */
    wxRealPoint m_nPrevSize;
    /*! \brief Auxiliary data member. */
    wxRealPoint m_nPrevPosition;
};

#endif //_WXSFRECTSHAPE_H
