/***************************************************************
 * Name:      CircleArrow.h
 * Purpose:   Defines circle arrow for line shapes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2009-04-19
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFCIRCLEARROW_H
#define _WXSFCIRCLEARROW_H

#include <wx/wxsf/SolidArrow.h>

// default values
#define sfdvARROW_RADIUS 4

/*!
 * \brief Class extends the wxSFSolidBase class and encapsulates
 * arrow shape consisting of a filled circle located at the end of the
 * parent line shape.
 */
class WXDLLIMPEXP_SF wxSFCircleArrow :	public wxSFSolidArrow
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFCircleArrow);

    /*! \brief Default constructor. */
	wxSFCircleArrow(void);
	/*!
     * \brief User constructor.
	 * \param parent Pointer to the parent shape
	 */
	wxSFCircleArrow(wxSFShapeBase* parent);
    /*!
     * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFCircleArrow(const wxSFCircleArrow& obj);
	/*! \brief Destructor. */
	virtual ~wxSFCircleArrow(void);
	
	// public member data accessors
	/**
	 * \brief Set radius of circle arrow
	 * \param radius Radius of circle arrow
	 */
	void SetRadius(int radius) {m_nRadius = radius;}
	
	/**
	 * \brief Get radius of circle arrow
	 * \return Radius if circle arrow
	 */
	int GetRadius() const {return m_nRadius;}
	
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
	/*! \brief Circle radius */
	int m_nRadius;
};

#endif //_WXSFCIRCLEARROW_H
