/***************************************************************
 * Name:      CanvasHistory.h
 * Purpose:   Defines manager for stored canvas states
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFCANVASHISTORY_H
#define _WXSFCANVASHISTORY_H

#include <wx/wxsf/CanvasState.h>

#define sfDEFAULT_MAX_CANVAS_STATES 25

class WXDLLIMPEXP_SF wxSFShapeCanvas;

/*! \brief
 * Container class that manages stored canvas states (canvas snapshots) and implements
 * basic Undo/Redo functionality.
 *
 * Two different working modes are available: 'histUSE_SERIALIZATION' mode uses basic
 * serialization functionality encapsulated by a diagram manager for storing
 * of current canvas content, but in the 'histUSE_CLONING' mode full copy of
 * diagram manager content is done via its copy constructor. The first mode is
 * slower than the second one, but do not require implementation of xsSerializable::Clone()
 * virtual function in all classes derived from xsSerializable like the second
 * posible working mode.
 * \sa wxSFCanvasState, wxSFCanvasHistory::MODE, xsSerializable::Clone, wxXmlSerializer::CopyItems
 */
class WXDLLIMPEXP_SF wxSFCanvasHistory : public wxObject
{
public:

	enum MODE
	{
		/*! \brief Use serialization for storing of a canvas content */
		histUSE_SERIALIZATION,
		/*! \brief Use diagram manager's copy constructor for storing of a canvas content */
		histUSE_CLONING
	};

    /*!
     * \brief Default constructor.
     * \param hmode Working mode (see MODE enumeration for more details)
     */
	wxSFCanvasHistory(MODE hmode = histUSE_SERIALIZATION);
    /*!
     * \brief User constructor.
     * \param canvas Pointer to managed canvas
     * \param hmode Working mode (see MODE enumeration for more details)
     * \sa MODE
     */
	wxSFCanvasHistory(wxSFShapeCanvas *canvas, MODE hmode = histUSE_SERIALIZATION);
	/*! \brief Destructor. */
	~wxSFCanvasHistory(void);

	// public member data accessors

    /*!
     * \brief Set history working mode.
	 *
	 * For more details about available working modes see the wxSFCanvasHistory class
	 * description. Note that all stored canvas history will be cleared after
	 * usage of this function.
     * \param hmode Working mode
     * \sa MODE
     */
	void SetMode(MODE hmode);
	/*! \brief
     * Set total number of stored canvas states.
	 * \param depth Number of stored canvas states
	 * \sa GetHistoryDepth
	 */
	void SetHistoryDepth(size_t depth){m_nHistoryDepth = depth;}
	/*! \brief
     * Set pointer to the parent shapes canvas. All Undo/Redo operation defined by this class
	 * will be performed on this shape canvas instance.
	 * \param canvas Pointer to parent shape canvas
	 */
	void SetParentCanvas(wxSFShapeCanvas* canvas){m_pParentCanvas = canvas;}

	/*! \brief Get currently used working mode */
	MODE GetMode(){return m_nWorkingMode;}
	/*! \brief
     * Get total number of canvas states which can be stored at the same time.
	 * \return Number of allowed concuretly stored canvas states
	 * \sa SetHistoryDepth
	 */
	size_t GetHistoryDepth(){return m_nHistoryDepth;}

	// public functions
	/*! \brief Save current canvas state. */
	void SaveCanvasState();
	/*! \brief Perform the 'Undo' operation. */
	void RestoreOlderState();
	/*! \brief Perform the 'Redo' operation. */
	void RestoreNewerState();
	/*! \brief Clear all canvas history. */
	void Clear();

    /*! \brief
     * The function gives information whether the 'Undo' operation is available
     * (exists any stored canvas state older than the current one.
     * \return TRUE if the 'Undo' operation can be performed, otherwise FALSE
     */
	bool CanUndo();
	/*! \brief
     * The function gives information whether the 'Redo' operation is available
	 * (exists any stored canvas state newer than the current one.
	 * \return TRUE if the 'Undo' operation can be performed, otherwise FALSE
	 */
	bool CanRedo();

protected:

	// protected data members
	/*! \brief Pointer to the parent canvas. */
	wxSFShapeCanvas * m_pParentCanvas;
	/*! \brief
     * List of stored canvas state instances.
	 * \sa wxSFCanvasState
	 */
	StateList m_lstCanvasStates;
	/*! \brief Auxilary pointer to current canvas state. */
	wxSFCanvasState *m_pCurrentCanvasState;
	/*! \brief Canvas history mode */
	MODE m_nWorkingMode;

    /*! \brief Total allowed amount of stored canvas states. */
	size_t m_nHistoryDepth;
};

#endif //_WXSFCANVASHISTORY_H
