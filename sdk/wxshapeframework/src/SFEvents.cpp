/***************************************************************
 * Name:      SFEvents.cpp
 * Purpose:   Implements shape events classes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-09-11
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/SFEvents.h"

DEFINE_EVENT_TYPE( wxEVT_SF_LINE_DONE );
DEFINE_EVENT_TYPE( wxEVT_SF_TEXT_CHANGE );
DEFINE_EVENT_TYPE( wxEVT_SF_ON_DROP );
DEFINE_EVENT_TYPE( wxEVT_SF_ON_PASTE );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_LEFT_DOWN );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_LEFT_DCLICK );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_RIGHT_DOWN );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_RIGHT_DCLICK);
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_DRAG_BEGIN);
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_DRAG );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_DRAG_END );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_HANDLE_BEGIN );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_HANDLE );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_HANDLE_END );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_KEYDOWN );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_MOUSE_ENTER );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_MOUSE_OVER );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_MOUSE_LEAVE );
DEFINE_EVENT_TYPE( wxEVT_SF_SHAPE_CHILD_DROP );
DEFINE_EVENT_TYPE( wxEVT_SF_LINE_BEFORE_DONE );

//----------------------------------------------------------------------------------//
// wxSFShapeEvent class
//----------------------------------------------------------------------------------//

wxSFShapeEvent::wxSFShapeEvent(wxEventType cmdType, int id)
: wxEvent(id, cmdType), m_Vetoed(false)
{
    m_Shape = NULL;
}

wxSFShapeEvent::wxSFShapeEvent(const wxSFShapeEvent& obj)
: wxEvent(obj), m_Vetoed(obj.m_Vetoed)
{
    m_Shape = obj.m_Shape;
}

wxSFShapeEvent::~wxSFShapeEvent()
{
}

//----------------------------------------------------------------------------------//
// wxSFShapeEvent class
//----------------------------------------------------------------------------------//

wxSFShapeTextEvent::wxSFShapeTextEvent(wxEventType cmdType, int id)
: wxEvent(id, cmdType)
{
    m_Shape = NULL;
	m_Text = wxT("");
}

wxSFShapeTextEvent::wxSFShapeTextEvent(const wxSFShapeTextEvent& obj)
: wxEvent(obj)
{
    m_Shape = obj.m_Shape;
	m_Text = obj.m_Text;
}

wxSFShapeTextEvent::~wxSFShapeTextEvent()
{
}

//----------------------------------------------------------------------------------//
// wxSFShapeDropEvent class
//----------------------------------------------------------------------------------//

wxSFShapeDropEvent::wxSFShapeDropEvent(wxEventType cmdType, wxCoord x, wxCoord y, wxSFShapeCanvas* target, wxDragResult def, int id)
: wxEvent(id, cmdType)
{
	m_nDropPosition = wxPoint(x, y);
	m_nDragResult = def;
	m_pDropTarget = target;
}

wxSFShapeDropEvent::wxSFShapeDropEvent(const wxSFShapeDropEvent& obj)
: wxEvent(obj)
{
	SetDroppedShapes(obj.m_lstDroppedShapes);
	
	m_nDropPosition = obj.m_nDropPosition;
	m_nDragResult = obj.m_nDragResult;
	m_pDropTarget = obj.m_pDropTarget;
}

wxSFShapeDropEvent::~wxSFShapeDropEvent()
{
	m_lstDroppedShapes.Clear();
}

void wxSFShapeDropEvent::SetDroppedShapes(const ShapeList &list)
{
	ShapeList::compatibility_iterator node = list.GetFirst();
	while(node)
	{
		m_lstDroppedShapes.Append(node->GetData());
		node = node->GetNext();
	}
}

//----------------------------------------------------------------------------------//
// wxSFShapeDropEvent class
//----------------------------------------------------------------------------------//

wxSFShapePasteEvent::wxSFShapePasteEvent(wxEventType cmdType, wxSFShapeCanvas *target, int id)
: wxEvent(id, cmdType)
{
	m_pDropTarget = target;
}

wxSFShapePasteEvent::wxSFShapePasteEvent(const wxSFShapePasteEvent& obj)
: wxEvent(obj)
{
	SetPastedShapes(obj.m_lstPastedShapes);
	
	m_pDropTarget = obj.m_pDropTarget;
}

wxSFShapePasteEvent::~wxSFShapePasteEvent()
{
	m_lstPastedShapes.Clear();
}

void wxSFShapePasteEvent::SetPastedShapes(const ShapeList &list)
{
	ShapeList::compatibility_iterator node = list.GetFirst();
	while(node)
	{
		m_lstPastedShapes.Append(node->GetData());
		node = node->GetNext();
	}
}

//----------------------------------------------------------------------------------//
// wxSFShapeKeyEvent class
//----------------------------------------------------------------------------------//

wxSFShapeKeyEvent::wxSFShapeKeyEvent(const wxSFShapeKeyEvent& obj) : wxEvent(obj)
{
	m_Shape = obj.m_Shape;
	m_KeyCode = obj.m_KeyCode;
}

wxSFShapeKeyEvent::wxSFShapeKeyEvent(wxEventType cmdType, int id) : wxEvent(id, cmdType)
{
	m_Shape = NULL;
	m_KeyCode = 0;
}

wxSFShapeKeyEvent::~wxSFShapeKeyEvent()
{
}

//----------------------------------------------------------------------------------//
// wxSFShapeHandleEvent class
//----------------------------------------------------------------------------------//

wxSFShapeHandleEvent::wxSFShapeHandleEvent(const wxSFShapeHandleEvent& obj) : wxEvent(obj)
{
	m_Shape = obj.m_Shape;
	m_Handle = obj.m_Handle;
}

wxSFShapeHandleEvent::wxSFShapeHandleEvent(wxEventType cmdType, int id) : wxEvent(id, cmdType)
{
	m_Shape = NULL;
	m_Handle = NULL;
}

wxSFShapeHandleEvent::~wxSFShapeHandleEvent()
{
}

//----------------------------------------------------------------------------------//
// wxSFShapeChildDropEvent class
//----------------------------------------------------------------------------------//

wxSFShapeChildDropEvent::wxSFShapeChildDropEvent(const wxSFShapeChildDropEvent& obj) : wxEvent(obj)
{
	m_Shape = obj.m_Shape;
	m_ChildShape = obj.m_ChildShape;
}

wxSFShapeChildDropEvent::wxSFShapeChildDropEvent(wxEventType cmdType, int id) : wxEvent(id, cmdType)
{
	m_Shape = NULL;
	m_ChildShape = NULL;
}

wxSFShapeChildDropEvent::~wxSFShapeChildDropEvent()
{
}

//----------------------------------------------------------------------------------//
// wxSFShapeMouseEvent class
//----------------------------------------------------------------------------------//

wxSFShapeMouseEvent::wxSFShapeMouseEvent(const wxSFShapeMouseEvent& obj) : wxEvent(obj)
{
	m_Shape = obj.m_Shape;
	m_MousePosition = obj.m_MousePosition;
}

wxSFShapeMouseEvent::wxSFShapeMouseEvent(wxEventType cmdType, int id) : wxEvent(id, cmdType)
{
	m_Shape = NULL;
	m_MousePosition = wxDefaultPosition;
}

wxSFShapeMouseEvent::~wxSFShapeMouseEvent()
{
}

