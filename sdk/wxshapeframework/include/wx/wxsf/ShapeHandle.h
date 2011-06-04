/***************************************************************
 * Name:      ShapeHandle.h
 * Purpose:   Defines shape handle class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFSHAPEHANDLE_H
#define _WXSFSHAPEHANDLE_H

#include <wx/list.h>

#include <wx/wxsf/ScaledDC.h>

class WXDLLIMPEXP_SF wxSFShapeBase;

/*! \brief Class encapsulates shape's handle. The class shouldn't be used separately; see
 * wxSFShapeBase class for more detailed information about functions used for managing of shape
 * handles and handling their events */
class wxSFShapeHandle : public wxObject
{
public:

    friend class wxSFShapeBase;
    friend class wxSFShapeCanvas;

	DECLARE_DYNAMIC_CLASS(wxSFShapeHandle);

    /*! \brief Handle type */
	enum HANDLETYPE
	{
		hndLEFTTOP,
		hndTOP,
		hndRIGHTTOP,
		hndRIGHT,
		hndRIGHTBOTTOM,
		hndBOTTOM,
		hndLEFTBOTTOM,
		hndLEFT,
		hndLINECTRL,
		hndLINESTART,
		hndLINEEND,
		hndUNDEF
	};

    /*! \brief Default constructor */
	wxSFShapeHandle(void);
	/*!
	 * \brief User constructor
	 * \param parent Parent shape
	 * \param type Handle type
	 * \param id Handle ID (usefull only for line controls handles)
	 */
	wxSFShapeHandle(wxSFShapeBase* parent, HANDLETYPE type, long id = -1);
	/*!
	 * \brief Copy constructor
	 * \param obj Source object
	 */
	wxSFShapeHandle(const wxSFShapeHandle& obj);
	/*! \brief Destructor */
	virtual ~wxSFShapeHandle(void);

	// public members accessors
	/*!
	 * \brief Get current handle position.
	 * \return Handle position
	 */
	wxPoint GetPosition() const {return m_nCurrPos;}
	/*!
	 * \brief Get current handle delta (difference between current and previous position).
	 * \return Handle delta
	 */
	wxPoint GetDelta() const {return m_nCurrPos - m_nPrevPos;}
	/*!
	 * \brief Get current total handle delta (difference between current and starting position
	 * stored at the begining of the dragging process).
	 * \return Total handle delta
	 */
	wxPoint GetTotalDelta() const {return m_nCurrPos - m_nStartPos;}
	/*!
	 * \brief Set handle type.
	 * \param type Handle type
	 * \sa HANDLETYPE
	 */
	void SetType(HANDLETYPE type){m_nType = type;}
	/*!
	 * \brief Get handle type.
	 * \return Handle type
	 * \sa HANDLETYPE
	 */
	HANDLETYPE GetType(){return m_nType;}
	/*!
	 * \brief Show/hide handle
	 * \param show TRUE if the handle should be visible (active), otherwise FALSE
	 */
	void Show(bool show){m_fVisible = show;}
	/*! \brief Function returns TRUE if the handle is visible, otherwise FALSE */
	bool IsVisible(){return m_fVisible;}
	/*!
	 * \brief Get parent shape.
	 * \return Pointer to parent shape
	 */
	wxSFShapeBase* GetParentShape(){return m_pParentShape;}
	/*!
	 * \brief Set handle's ID.
	 * \param id Handle's ID
	 */
	void SetId(long id){m_nId = id;}
	/*!
	 * \brief Get handle's ID.
	 * \return id Handle's ID
	 */
    long GetId(){return m_nId;}

	// public functions
	/*! \brief Refresh (repaint) the handle */
	void Refresh();
	/*!
	 * \brief Find out whether given point is inside the handle.
	 * \param pos Examined point
	 * \return TRUE if the point is inside the handle, otherwise FALSE
	 */
	bool Contains(const wxPoint& pos);

protected:

	// protected data members
	HANDLETYPE m_nType;
	wxSFShapeBase *m_pParentShape;

	bool m_fVisible;
	bool m_fMouseOver;

	// protected functions
	/*!
	 * \brief Draw handle.
	 * \param dc Device context where the handle will be drawn
	 */
	void Draw(wxDC& dc);
	/*!
	 * \brief Draw handle in the normal way.
	 * \param dc Device context where the handle will be drawn
	 */
	void DrawNormal(wxDC& dc);
	/*!
	 * \brief Draw handle in the "hover" way (the mouse pointer is above the handle area).
	 * \param dc Device context where the handle will be drawn
	 */
	void DrawHover(wxDC& dc);

	/*!
	 * \brief Set parent shape.
	 * \param parent Pointer to parent shape
	 */
	 void SetParentShape(wxSFShapeBase *parent){m_pParentShape = parent;}

    /*!
     * \brief Get handle rectangle.
     * \return Handle rectangle
     */
	wxRect GetHandleRect() const;

private:

	// private data memders
	wxPoint m_nStartPos;
	wxPoint m_nPrevPos;
	wxPoint m_nCurrPos;

	long m_nId;

    /*!
     * \brief Event handler called when the mouse pointer is moving above shape canvas.
     * \param pos Current mouse position
     */
	void _OnMouseMove(const wxPoint& pos);
    /*!
     * \brief Event handler called when the handle is started to be dragged.
     * \param pos Current mouse position
     */
	void _OnBeginDrag(const wxPoint& pos);
    /*!
     * \brief Event handler called when the handle is dragged.
     * \param pos Current mouse position
     */
	void _OnDragging(const wxPoint& pos);
    /*!
     * \brief Event handler called when the handle is released.
     * \param pos Current mouse position
     */
	void _OnEndDrag(const wxPoint& pos);
};

WX_DECLARE_LIST(wxSFShapeHandle, HandleList);

#endif //_WXSFSHAPEHANDLE_H
