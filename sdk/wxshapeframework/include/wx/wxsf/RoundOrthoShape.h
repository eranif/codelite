/***************************************************************
 * Name:      RoundOrthoShape.h
 * Purpose:   Defines rounded orthogonal line shape class
 * Author:    Michal Bližňák (michal.bliznak@gmail.com)
 * Created:   2011-01-27
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFROUNDORTHOSHAPE_H
#define _WXSFROUNDORTHOSHAPE_H

#include <wx/wxsf/OrthoShape.h>

// default values
/*! \brief Default value of wxSFRoundOrthoLineShape::m_nMaxRadius data member */
#define sfdvROUNDORTHOSHAPE_MAXRADIUS 7

/*!
 * \brief Rounded orthogonal line shape. The class extends wxSFOrthoLineShape class and allows
 * user to create connection line orthogonal to base axis with rounded corners.
 */
class WXDLLIMPEXP_SF wxSFRoundOrthoLineShape : public wxSFOrthoLineShape
{
public:
	XS_DECLARE_CLONABLE_CLASS(wxSFRoundOrthoLineShape);

	/*! \brief Default constructor. */
	wxSFRoundOrthoLineShape();
	/*!
	 * \brief User constructor.
	 * \param src ID of the source shape
	 * \param trg ID of the target shape
	 * \param path Array of line's control points
	 * \param manager Parent parent diagram manager
	 */
	wxSFRoundOrthoLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	wxSFRoundOrthoLineShape(const wxSFRoundOrthoLineShape& obj);
	/*! \brief Destructor. */
	virtual ~wxSFRoundOrthoLineShape();

	// public members accessors
	/*!
	 * \brief Set maximal value of corner radius.
	 * \param maxr Maximal value of corner radius
	 */
	void SetMaxRadius(int maxr) { this->m_nMaxRadius = maxr;}
	/*!
	 * \brief Get current maximal value of corner radius.
	 * \return Current maximal corner radius
	 */
	int GetMaxRadius() const { return m_nMaxRadius; }

protected:
	// protected data members
	int m_nMaxRadius;

	// protected virtual functions
	/**
	 * \brief Draw one orthogonal line segment.
	 * \param dc Device context
	 * \param src Starting point of the ortho line segment.
	 * \param trg Ending point of the ortho line segment.
	 */
	virtual void DrawLineSegment(wxDC& dc, const wxRealPoint& src, const wxRealPoint& trg);
};

#endif //_WXSFROUNDORTHOSHAPE_H
