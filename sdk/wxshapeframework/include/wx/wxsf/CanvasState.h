/***************************************************************
 * Name:      CanvasState.h
 * Purpose:   Defines container for stored canvas state
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFCANVASSTATE_H
#define _WXSFCANVASSTATE_H

#include <wx/wxprec.h>

#include <wx/wxsf/Defs.h>
#include <wx/wxsf/DiagramManager.h>

class wxSFShapeCanvas;
/*! \brief
 * Class which stores one saved state of the shape canvas. The instaces of this
 * class are managed by the wxSFCanvasHistory class which performs all related Undo/Redo
 * operations.
 * \sa wxSFShapeCanvas
 */
class wxSFCanvasState : public wxObject
{
friend class wxSFCanvasHistory;

public:
    /*! \brief
     * Constructor for 'histUSE_SERIALIZATION' mode.
     * \param data Pointer to the stream buffer containig serialized content of the shape canvas
     */
	wxSFCanvasState(wxStreamBuffer* data);
	/*! \brief
     * Constructor for 'histUSE_CLONING' mode.
     * \param data Pointer to temporal data manager
     */
	wxSFCanvasState(wxSFDiagramManager *data);
	/*! \brief Destructor. */
	~wxSFCanvasState(void);

protected:

	// protected functions
	/*! \brief
     * Restore stored canvas state.
	 * \param canvas Pointer to the shape canvas which content will be replaced by the stored one
	 */
	void Restore(wxSFShapeCanvas* canvas);

	// protected data members
	/*! \brief Memory buffer used during the serialization/deserialization operations. */
	wxMemoryBuffer m_dataBuffer;
	/*! \brief Data manager used for storing of temporal canvas state. */
	wxSFDiagramManager *m_pDataManager;
};

WX_DECLARE_LIST(wxSFCanvasState, StateList);

#endif //_WXSFCANVASSTATE_H
