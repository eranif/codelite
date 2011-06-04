/***************************************************************
 * Name:      Thumbnail.h
 * Purpose:   Defines canvas thumbnail class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2009-06-09
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFTHUMBNAIL_H
#define _WXSFTHUMBNAIL_H

#include <wx/wxsf/ShapeCanvas.h>

/**
 * \brief Class encalpsulating a shape canvas' thumbnail. This GUI control derived from wxPanel can be associated with
 * one shape canvas and can be used for previewing and manipulating of it.
 */ 
class WXDLLIMPEXP_SF wxSFThumbnail : public wxPanel
{
public:	
	/** \brief Internally used IDs */
	enum IDS
	{
		ID_UPDATETIMER = wxID_HIGHEST + 1,
		IDM_SHOWELEMENTS,
		IDM_SHOWCONNECTIONS
	};
	/** \brief Thumbnail style */
	enum THUMBSTYLE
	{
		/** \brief Show diagram elements (excluding connections) in the thumbnail. */
		tsSHOW_ELEMENTS = 1,		
		/** \brief Show diagram connections in the thumbnail. */
		tsSHOW_CONNECTIONS = 2		
	};
	
	/**
	 * \brief Constructor.
	 * \param parent Pointer to parent window
	 */ 
	wxSFThumbnail(wxWindow *parent);
	/** \brief Destructor. */
	virtual ~wxSFThumbnail();
	
	// public member data accessors
	/**
	 * \brief Set the thumbnail style.
	 * \param style Style value composed of predefined flags
	 * \sa THUMBSTYLE
	 */
	void SetThumbStyle(int style) { m_nThumbStyle = style; }
	/**
	 * \brief Get current thumbnail style.
	 * \return Style value composed of predefined flags
	 * \sa THUMBSTYLE
	 */
	int GetThumbStyle() { return m_nThumbStyle; }
	
	// public functions
	/**
	 * \brief Set canvas managed by the thumbnail.
	 * \param canvas Pointer to shape canvas
	 */
	void SetCanvas(wxSFShapeCanvas *canvas);
	
	// public virtual functions
	/**
	 * \brief Implementation of drawing of the thumbnail's content. This virtual function can be overrided
	 * by the user for customization of the thumbnail appearance.
	 * \param dc Reference to output device context
	 */
	virtual void DrawContent(wxDC &dc);
	
protected:
	// protected data members
	wxSFShapeCanvas *m_pCanvas;		/** \brief Pointer to managed shape canvas. */
	wxTimer m_UpdateTimer;			/** \brief Timer user for the thumbnail's update */
	wxPoint m_nPrevMousePos;		/** \brief Auxiliary varialble */
	double m_nScale;				/** \brief Current thumbnail's scale */
	int m_nThumbStyle;				/** \brief Current thumbnail's style */
	
	// protected event handlers
	/** \brief Internally used event handler. */
	void _OnPaint( wxPaintEvent &event );
	
	// protected functions
	/**
	 * \brief Get offset (view start) of managed shape canvas defined in pixels.
	 * \return Canvas offset in pixels
	 */
	wxSize GetCanvasOffset();
	
private:
	// private event handlers
	/** \brief Internally used event handler. */
	void _OnEraseBackground( wxEraseEvent& event );
	/** \brief Internally used event handler. */
	void _OnMouseMove( wxMouseEvent &event );
	/** \brief Internally used event handler. */
	void _OnLeftDown( wxMouseEvent &event );
	/** \brief Internally used event handler. */
	void _OnRightDown( wxMouseEvent &event );
	/** \brief Internally used event handler. */
	void _OnTimer( wxTimerEvent &event );
	
	/** \brief Internally used event handler. */
	void _OnShowElements( wxCommandEvent &event );
	/** \brief Internally used event handler. */
	void _OnShowConnections( wxCommandEvent &event );
	/** \brief Internally used event handler. */
	void _OnUpdateShowElements( wxUpdateUIEvent &event );
	/** \brief Internally used event handler. */
	void _OnUpdateShowConnections( wxUpdateUIEvent &event );
	
	DECLARE_EVENT_TABLE();
};

#endif //_WXSFTHUMBNAIL_H
