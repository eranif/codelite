/***************************************************************
 * Name:      ArrowBase.h
 * Purpose:   Defines line arrow base class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFARROWBASE_H
#define _WXSFARROWBASE_H

#include <wx/wxsf/ShapeBase.h>

// default values
#define sfdvARROW_FILL wxBrush(*wxWHITE)
#define sfdvARROW_BORDER wxPen(*wxBLACK)

class WXDLLIMPEXP_SF wxSFLineShape;

/*! \brief Base class for a line arrows */
class WXDLLIMPEXP_SF wxSFArrowBase : public xsSerializable
{
public:
	friend class wxSFLineShape;

	XS_DECLARE_CLONABLE_CLASS(wxSFArrowBase);
    /*! \brief Default constructor */
	wxSFArrowBase(void);
	/*!
	 * \brief Constructor.
	 * \param parent Pointer to a parent shape
	 */
	wxSFArrowBase(wxSFShapeBase* parent);
	/*!
	 * \brief Copy constructor.
	 * \param obj Source base arrow shape
	 */
	wxSFArrowBase(const wxSFArrowBase& obj);
	/*! \brief Destructor. */
	~wxSFArrowBase(void);

	// public member data accessors
	/*!
	 * \brief Set a parent of the arrow shape.
	 * \param parent Pointer to the parent shape
	 */
	void SetParentShape(wxSFShapeBase* parent){m_pParentShape = parent;}
	/*!
	 * \brief Get pointer to a parent shape.
	 * \return Pointer to a parent shape if exists, otherwise NULL
	 */
	wxSFShapeBase* GetParentShape(){return m_pParentShape;}

	// public functions

	// public virtual functions
	/*!
	 * \brief Draw arrow shape at the end of a virtual line.
	 * \param from Start of the virtual line
	 * \param to End of the virtual line
	 * \param dc Device context for drawing
	 */
	virtual void Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc);

protected:

	// protected member data
	/*! \brief Pointer to a parent shape. */
	wxSFShapeBase* m_pParentShape;
	
	// protected functions
	/*!
	 * \brief Rotate and move arrow's vertices in accordance of virtual line at which end the arrow will be placed.
	 * \param trg Pointer to array where translated vertices will be stored
	 * \param src Pointer to array of source vertices
	 * \param n Number of vertices
	 * \param from Start of the virtual line
	 * \param to End of the virtual line
	 */
	void TranslateArrow(wxPoint *trg, const wxRealPoint *src, int n, const wxRealPoint &from, const wxRealPoint& to);
};

#endif //_WXSFARROWBASE_H
