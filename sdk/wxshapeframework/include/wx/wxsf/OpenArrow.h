/***************************************************************
 * Name:      OpenArrow.h
 * Purpose:   Defines open arrow for line shapes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFOPENARROW_H
#define _WXSFOPENARROW_H

#include <wx/wxsf/ArrowBase.h>

/*!
 * \brief Class extends the wxSFArrowBase class and encapsulates
 * arrow shape consisting of single two lines leading from the end of the
 * parent line shape.
 */
class WXDLLIMPEXP_SF wxSFOpenArrow : public wxSFArrowBase
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFOpenArrow);

    /*! \brief Default constructor. */
	wxSFOpenArrow(void);
	/*!
     * \brief User constructor.
	 * \param parent"> Pointer to the parent shape
	 */
	wxSFOpenArrow(wxSFShapeBase* parent);
	/*!
     * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFOpenArrow(const wxSFOpenArrow& obj);
	/*! \brief Destructor. */
	virtual ~wxSFOpenArrow(void);
	
	// public member data accessors
	/**
	 * \brief Set a pen used for drawing of the arrow.
	 * \param pen Reference to the pen
	 */
	void SetArrowPen(const wxPen& pen) {m_Pen = pen;}
	/**
	 * \brief Get current pen used for drawing of the arrow.
	 * \return Constant reference to current pen
	 */
	const wxPen& GetArrowPen() const {return m_Pen;}

	// public virtual functions
	/*!
     * \brief Draw arrow shape at the end of a virtual line.
	 * \param from Start of the virtual line
	 * \param to End of the virtual line
	 * \param dc Device context for drawing
	 */
	virtual void Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc);
	
protected: 
	// protected data members
	/*! \brief Arrow pen */
	wxPen m_Pen;
};

#endif //_WXSFOPENARROW_H
