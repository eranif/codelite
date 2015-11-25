/***************************************************************
 * Name:      MultiSelRect.h
 * Purpose:   Defines aux. multiselection shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFMULTIRECT_H
#define _WXSFMULTIRECT_H

#include <wx/wxsf/RectShape.h>

/*!
 * \brief Auxiliary class encapsulating multiselection rectangle used
 * in the shape canvas. The class shouldn't be used directly.
 * \sa wxSFShapeCanvas
 */
class WXDLLIMPEXP_SF wxSFMultiSelRect : public wxSFRectShape
{
public:
    /*! \brief Default constructor. */
	wxSFMultiSelRect(void);
	/*! \brief Destructor. */
	virtual ~wxSFMultiSelRect(void);

	// public virtual functions
	/*!
     * \brief Event handler called at the begining of the shape handle dragging process.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	/*!
	 * \brief Event handler called during dragging of the shape handle.
	 * The function can be overrided if necessary.
	 *
	 * The function is called by the framework (by the shape canvas).
	 * \param handle Reference to dragged handle
	 */
	virtual void OnHandle(wxSFShapeHandle& handle);
	/*!
     * \brief Event handler called at the end of the shape handle dragging process.
	 * The function can be overrided if neccessary.
	 * \param handle Reference to dragged shape handle
	 */
	virtual void OnEndHandle(wxSFShapeHandle& handle);

protected:
	// protected virtual functions
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
	/*! \brief Auxiliary function. */
	bool AnyWidthExceeded(const wxPoint& delta);
	/*! \brief Auxiliary function. */
	bool AnyHeightExceeded(const wxPoint& delta);
};

#endif //_WXSFMULTIRECT_H
