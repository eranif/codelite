/***************************************************************
 * Name:      SFEvents.h
 * Purpose:   Defines shape events classes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-09-11
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSFEVENTS_H
#define _WXSFEVENTS_H

#include <wx/event.h>
#include <wx/dnd.h>

#include <wx/wxsf/Defs.h>
#include <wx/wxsf/ShapeBase.h>

class WXDLLIMPEXP_SF wxSFShapeEvent;
class WXDLLIMPEXP_SF wxSFShapeTextEvent;
class WXDLLIMPEXP_SF wxSFShapeDropEvent;
class WXDLLIMPEXP_SF wxSFShapePasteEvent;
class WXDLLIMPEXP_SF wxSFShapeHandleEvent;
class WXDLLIMPEXP_SF wxSFShapeKeyEvent;
class WXDLLIMPEXP_SF wxSFShapeMouseEvent;
class WXDLLIMPEXP_SF wxSFShapeChildDropEvent;

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_LINE_DONE, 7770)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_TEXT_CHANGE, 7771)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_ON_DROP, 7772)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_ON_PASTE, 7773)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_LEFT_DOWN, 7774)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_LEFT_DCLICK, 7775)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_RIGHT_DOWN, 7776)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_RIGHT_DCLICK, 7777)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_DRAG_BEGIN, 7778)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_DRAG, 7779)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_DRAG_END, 7780)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_HANDLE_BEGIN, 7781)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_HANDLE, 7782)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_HANDLE_END, 7783)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_KEYDOWN, 7784)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_MOUSE_ENTER, 7785)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_MOUSE_OVER, 7786)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_MOUSE_LEAVE, 7787)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_SHAPE_CHILD_DROP, 7788)
	DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SF, wxEVT_SF_LINE_BEFORE_DONE, 7789)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxSFShapeEventFunction)(wxSFShapeEvent&);
typedef void (wxEvtHandler::*wxSFShapeTextEventFunction)(wxSFShapeTextEvent&);
typedef void (wxEvtHandler::*wxSFShapeDropEventFunction)(wxSFShapeDropEvent&);
typedef void (wxEvtHandler::*wxSFShapePasteEventFunction)(wxSFShapePasteEvent&);
typedef void (wxEvtHandler::*wxSFShapeHandleEventFunction)(wxSFShapeHandleEvent&);
typedef void (wxEvtHandler::*wxSFShapeKeyEventFunction)(wxSFShapeKeyEvent&);
typedef void (wxEvtHandler::*wxSFShapeMouseEventFunction)(wxSFShapeMouseEvent&);
typedef void (wxEvtHandler::*wxSFShapeChildDropEventFunction)(wxSFShapeChildDropEvent&);

#define wxSFShapeEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeEventFunction, &func)

#define wxSFShapeTextEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeTextEventFunction, &func)

#define wxSFShapeDropEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeDropEventFunction, &func)

#define wxSFShapePasteEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapePasteEventFunction, &func)
	
#define wxSFShapeHandleEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeHandleEventFunction, &func)
	
#define wxSFShapeKeyEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeKeyEventFunction, &func)
	
#define wxSFShapeMouseEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeMouseEventFunction, &func)
	
#define wxSFShapeChildDropEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSFShapeChildDropEventFunction, &func)


/*! \brief Event table macro mapping event wxEVT_SF_LINE_DONE. This event occures
 * when the interactive connection creation process is finished. The generated event
 * object holds a pointer to the new line shape. */
#define EVT_SF_LINE_DONE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_LINE_DONE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

/*! \brief Event table macro mapping event wxEVT_SF_TEXT_CHANGE. This event occures
 * when the editable text shape's content is changed. */
#define EVT_SF_TEXT_CHANGE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_TEXT_CHANGE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeTextEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

/*! \brief Event table macro mapping event wxEVT_SF_ON_DROP. This event occures
 * when dragged shapes (via D&D operation) are dropped to a canvas. */
#define EVT_SF_ON_DROP(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_ON_DROP, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeDropEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

/*! \brief Event table macro mapping event wxEVT_SF_ON_PASTE. This event occures
 * when shapes stored in the clipboard are pasted to a canvas. */
#define EVT_SF_ON_PASTE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_ON_PASTE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapePasteEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_LEFT_DOWN. This event occures
 * when the shape is clicked by a left mouse button (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_LEFT_DOWN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_LEFT_DOWN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_LEFT_DCLICK. This event occures
 * when the shape is double-clicked by a left mouse button (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_LEFT_DCLICK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_LEFT_DCLICK, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_RIGHT_DOWN. This event occures
 * when the shape is clicked by a right mouse button (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_RIGHT_DOWN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_RIGHT_DOWN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_RIGHT_DCLICK. This event occures
 * when the shape is double-clicked by a right mouse button (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_RIGHT_DCLICK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_RIGHT_DCLICK, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_DRAG_BEGIN. This event occures
 * when the shape has started to be dragged. (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_DRAG_BEGIN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_DRAG_BEGIN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_DRAG. This event occures
 * when the shape is dragging (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_DRAG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_DRAG, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_DRAG_END. This event occures
 * when the shape's dragging was finished (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_DRAG_END(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_DRAG_END, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_HANDLE_BEGIN. This event occures
 * when the shape's handle has started to be dragged. (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_HANDLE_BEGIN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_HANDLE_BEGIN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeHandleEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_HANDLE. This event occures
 * when the shape's handle is dragging (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_HANDLE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_HANDLE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeHandleEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_HANDLE_END. This event occures
 * when the shape's dragging was finished (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_HANDLE_END(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_HANDLE_END, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeHandleEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_KEYDOWN. This event occures
 * when the any key is pressed on selected shape (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_KEYDOWN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_KEYDOWN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeKeyEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_MOUSE_ENTER. This event occures
 * when the mouse cursor enters the shape's area (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_MOUSE_ENTER(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_MOUSE_ENTER, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_MOUSE_OVER. This event occures
 * when the mouse cursor is moving over the shape's area (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_MOUSE_OVER(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_MOUSE_OVER, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_MOUSE_LEAVE. This event occures
 * when the mouse cursor leaves the shape's area (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_MOUSE_LEAVE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_MOUSE_LEAVE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeMouseEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_SHAPE_CHILD_DROP. This event occures
 * when another shape is dropped onto the new parent shape (sfsEMIT_EVENTS shape style must be in use). */
#define EVT_SF_SHAPE_CHILD_DROP(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_SHAPE_CHILD_DROP, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeChildDropEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*! \brief Event table macro mapping event wxEVT_SF_LINE_BEFORE_DONE. This event occures
 * when the interactive connection creation process is finished but this has been added in 
 * order to allow the developper to cancel the creation if required. The generated event
 * object holds a pointer to the new line shape. */
#define EVT_SF_LINE_BEFORE_DONE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SF_LINE_BEFORE_DONE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxSFShapeEventFunction, &fn ), \
        (wxObject *) NULL \
    ),
	
/*!
 * \brief Class encapsulates generic wxSF shape's event.
 */
class WXDLLIMPEXP_SF wxSFShapeEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeEvent(wxEventType cmdType = wxEVT_NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeEvent(const wxSFShapeEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeEvent();

    // public member data accessors
    /*!
     * \brief Insert a shape object to the event object.
     * \param shape Pointer to the shape object
     */
    void SetShape(wxSFShapeBase* shape){m_Shape = shape;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    wxSFShapeBase* GetShape(){return m_Shape;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeEvent(*this); }
	
	/*!
     * \brief Check if the event has been vetoed or not.
     * \return TRUE if the event has been vetoed.
     */
	bool IsVetoed() {return m_Vetoed;}
	
	/*!
	 * \brief Set the veto flag to true.
	 */
	void Veto() {m_Vetoed = true;};


private:
    // private data members
    /*! \brief Pointer to stored shape object. */
    wxSFShapeBase* m_Shape;
	bool m_Vetoed;
};

/*!
 * \brief Class encapsulates wxEVT_SF_SHAPE_KEYDOWN event.
 */
class WXDLLIMPEXP_SF wxSFShapeKeyEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeKeyEvent(wxEventType cmdType = wxEVT_NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeKeyEvent(const wxSFShapeKeyEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeKeyEvent();

    // public member data accessors
    /*!
     * \brief Insert a shape object to the event object.
     * \param shape Pointer to the shape object
     */
    void SetShape(wxSFShapeBase* shape){m_Shape = shape;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    wxSFShapeBase* GetShape(){return m_Shape;}
	/**
	 * \brief Set key code.
	 * \param KeyCode Code of pressed key
	 */
	void SetKeyCode(int KeyCode) {this->m_KeyCode = KeyCode;}
	/**
	 * \brief Get key code.
	 * \return Code of pressed key
	 */
	int GetKeyCode() const {return m_KeyCode;}


    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeKeyEvent(*this); }


private:
    // private data members
    /*! \brief Pointer to stored shape object. */
    wxSFShapeBase* m_Shape;
	/*! \brief Code of pressed key. */
	int m_KeyCode;
};

/*!
 * \brief Class encapsulates mouse events generated by a shape.
 */
class WXDLLIMPEXP_SF wxSFShapeMouseEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeMouseEvent(wxEventType cmdType = wxEVT_NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeMouseEvent(const wxSFShapeMouseEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeMouseEvent();

    // public member data accessors
    /*!
     * \brief Insert a shape object to the event object.
     * \param shape Pointer to the shape object
     */
    void SetShape(wxSFShapeBase* shape){m_Shape = shape;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    wxSFShapeBase* GetShape(){return m_Shape;}
	/**
	 * \brief Set absolute position of mouse cursor.
	 * \param MousePosition Mouse cursor's absolute position
	 */
	void SetMousePosition(const wxPoint& MousePosition) {this->m_MousePosition = MousePosition;}
	/**
	 * \brief Get absolute position of mouse cursor
	 * \return Mouse cursor's absolute position
	 */
	const wxPoint& GetMousePosition() const {return m_MousePosition;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeMouseEvent(*this); }


private:
    // private data members
    /*! \brief Pointer to stored shape object. */
    wxSFShapeBase* m_Shape;
	/*! \brief Code of pressed key. */
	wxPoint m_MousePosition;
};


/*!
 * \brief Class encapsulates wxEVT_SF_TEXT_CHANGE event.
 */
class WXDLLIMPEXP_SF wxSFShapeTextEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeTextEvent(wxEventType cmdType = wxEVT_NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeTextEvent(const wxSFShapeTextEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeTextEvent();

    // public member data accessors
    /*!
     * \brief Insert a shape object to the event object.
     * \param shape Pointer to the shape object
     */
    void SetShape(wxSFShapeBase* shape){m_Shape = shape;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    wxSFShapeBase* GetShape(){return m_Shape;}
    /*!
     * \brief Set new text shape.
     * \param txt New text content.
     */
	void SetText(const wxString& txt){m_Text = txt;}
    /*!
     * \brief Get a shape text.
     * \return Shape text content.
     */
	wxString GetText(){return m_Text;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeTextEvent(*this); }


private:
    // private data members
    /*! \brief Pointer to stored shape object. */
    wxSFShapeBase* m_Shape;
	/*! \brief New shape text. */
	wxString m_Text;
};

/*!
 * \brief Class encapsulates handle-related events.
 */
class WXDLLIMPEXP_SF wxSFShapeHandleEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeHandleEvent(wxEventType cmdType = wxEVT_NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeHandleEvent(const wxSFShapeHandleEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeHandleEvent();

    // public member data accessors
    /*!
     * \brief Insert a shape object to the event object.
     * \param shape Pointer to the shape object
     */
    void SetShape(wxSFShapeBase* shape){m_Shape = shape;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    wxSFShapeBase* GetShape(){return m_Shape;}
	/**
	 * \brief Set pointer to dragged shape handle.
	 * \param Handle Pointer to shape handle
	 * \sa wxSFShapeHandle
	 */
	void SetHandle(wxSFShapeHandle& Handle) {this->m_Handle = &Handle;}
	/**
	 * \brief Get pointer to dragged shape handle.
	 * \return Pointer to shape handle
	 * \sa wxSFShapeHandle
	 */
	wxSFShapeHandle& GetHandle() const {return *m_Handle;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeHandleEvent(*this); }


private:
    // private data members
    /*! \brief Pointer to stored shape object. */
    wxSFShapeBase* m_Shape;
	/*! \brief Dragged handle. */
	wxSFShapeHandle* m_Handle;
};

/*!
 * \brief Class encapsulates wxEVT_SF_ON_DROP event.
 */
class WXDLLIMPEXP_SF wxSFShapeDropEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeDropEvent(wxEventType cmdType = wxEVT_NULL, wxCoord x = 0, wxCoord y = 0, wxSFShapeCanvas* target = NULL, wxDragResult def = wxDragNone, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeDropEvent(const wxSFShapeDropEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeDropEvent();

    // public member data accessors
    /*!
     * \brief Copy given shapes to the internal list of dropped shapes.
     * \param list Reference to a list of copied shapes
     */
    void SetDroppedShapes(const ShapeList& list);
	 /*!
     * \brief Set a position where the shapes were dropped.
     * \param pos Position
     */
	void SetDropPosition(const wxPoint& pos){m_nDropPosition = pos;}
	 /*!
     * \brief Set drag result.
     * \param def Drag result
     */
	void SetDragResult(wxDragResult def){m_nDragResult = def;}
	/*!
	 * \brief Set drop target (shape canvas where shapes have been dropped to).
	 * \param target Pointer to drop target (shape canvas)
	 */
	void SetDropTarget(wxSFShapeCanvas *target){m_pDropTarget = target;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    ShapeList& GetDroppedShapes(){return m_lstDroppedShapes;}
    /*!
     * \brief Get drop position.
     * \return Position.
     */
	wxPoint GetDropPosition(){return m_nDropPosition;}
    /*!
     * \brief Get drag result.
     * \return Drag result.
     */
	wxDragResult GetDragResult(){return m_nDragResult;}
	/*!
	 * \brief Get drop target (shape canvas where shapes have been dropped to).
	 * \return Pointer to drop target (shape canvas)
	 */
	wxSFShapeCanvas* GetDropTarget(){return m_pDropTarget;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeDropEvent(*this); }


private:
    // private data members
    /*! \brief List of dropped shapes. */
    ShapeList m_lstDroppedShapes;
	/*! \brief Drop target. */
	wxSFShapeCanvas *m_pDropTarget;
	/*! \brief Drop position. */
	wxPoint m_nDropPosition;
	/*! \brief Drag result. */
	wxDragResult m_nDragResult;
};

/*!
 * \brief Class encapsulates wxEVT_SF_ON_PASTE event.
 */
class WXDLLIMPEXP_SF wxSFShapePasteEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapePasteEvent(wxEventType cmdType = wxEVT_NULL, wxSFShapeCanvas *target = NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapePasteEvent(const wxSFShapePasteEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapePasteEvent();

    // public member data accessors
    /*!
     * \brief Copy given shapes to the internal list of pasted shapes.
     * \param list Reference to a list of copied shapes
     */
    void SetPastedShapes(const ShapeList& list);
	/*!
	 * \brief Set drop target (shape canvas where shapes have been pasted to).
	 * \param target Pointer to drop target (shape canvas)
	 */
	void SetDropTarget(wxSFShapeCanvas *target){m_pDropTarget = target;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    ShapeList& GetPastedShapes(){return m_lstPastedShapes;}
	/*!
	 * \brief Get drop target (shape canvas where shapes have been pasted to).
	 * \return Pointer to drop target (shape canvas)
	 */
	wxSFShapeCanvas* GetDropTarget(){return m_pDropTarget;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapePasteEvent(*this); }


private:
    // private data members
    /*! \brief List of pasted shapes. */
    ShapeList m_lstPastedShapes;
	/*! \brief Drop target. */
	wxSFShapeCanvas *m_pDropTarget;
};

/*!
 * \brief Class encapsulates wxEVT_SF_SHAPE_CHILD_DROP event.
 */
class WXDLLIMPEXP_SF wxSFShapeChildDropEvent : public wxEvent
{
public:
    /*! \brief Constructor */
    wxSFShapeChildDropEvent(wxEventType cmdType = wxEVT_NULL, int id = 0);
    /*! \brief Copy constructor */
    wxSFShapeChildDropEvent(const wxSFShapeChildDropEvent& obj);
    /*! \brief Destructor */
    virtual ~wxSFShapeChildDropEvent();

    // public member data accessors
    /*!
     * \brief Insert a shape object to the event object.
     * \param shape Pointer to the shape object
     */
    void SetShape(wxSFShapeBase* shape){m_Shape = shape;}
    /*!
     * \brief Get a shape object from the event object.
     * \return Pointer to the shape object.
     */
    wxSFShapeBase* GetShape(){return m_Shape;}
	/**
	 * \brief Set a pointer to dropped child shape.
	 * \param ChildShape Pointer to dropped child shape
	 */
	void SetChildShape(wxSFShapeBase* ChildShape) {this->m_ChildShape = ChildShape;}
	/**
	 * \brief Get a pointer to dropped child shape.
	 * \return Pointer to dropped child shape
	 */
	wxSFShapeBase* GetChildShape() {return m_ChildShape;}

    /*! \brief Clone this event object and return pointer to the new instance. */
    wxEvent* Clone() const { return new wxSFShapeChildDropEvent(*this); }


private:
    // private data members
    /*! \brief Pointer to stored shape object. */
    wxSFShapeBase* m_Shape;
	/*! \brief Pointer to dropped child object. */
	wxSFShapeBase* m_ChildShape;
};

#endif // _WXSFEVENTS_H
