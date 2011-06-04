/***************************************************************
 * Name:      DiamondArrow.h
 * Purpose:   Defines diamond arrow for line shapes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2009-04-18
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFDIAMONDARROW_H
#define _WXSFDIAMONDARROW_H

#include <wx/wxsf/SolidArrow.h>

/*!
 * \brief Class extends the wxSFSolidArrow class and encapsulates
 * arrow shape consisting of filled diamond located the end of the
 * parent line shape.
 */
class WXDLLIMPEXP_SF wxSFDiamondArrow : public wxSFSolidArrow
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFDiamondArrow);

    /*! \brief Default constructor. */
	wxSFDiamondArrow(void);
	/*!
     * \brief User constructor.
	 * \param parent"> Pointer to the parent shape
	 */
	wxSFDiamondArrow(wxSFShapeBase* parent);
	/*!
     * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFDiamondArrow(const wxSFDiamondArrow& obj);
	/*! \brief Destructor. */
	virtual ~wxSFDiamondArrow(void);

	// public virtual functions
	/*!
     * \brief Draw arrow shape at the end of a virtual line.
	 * \param from Start of the virtual line
	 * \param to End of the virtual line
	 * \param dc Device context for drawing
	 */
	virtual void Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc);
};

#endif //_WXSFDIAMONDARROW_H
