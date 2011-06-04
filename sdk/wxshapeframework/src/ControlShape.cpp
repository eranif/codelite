/***************************************************************
 * Name:      ControlShape.cpp
 * Purpose:   Implements GUI control shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2008-04-30
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/ControlShape.h"
#include "wx/wxsf/DiagramManager.h"
#include "wx/wxsf/ShapeCanvas.h"

XS_IMPLEMENT_CLONABLE_CLASS(wxSFControlShape, wxSFRectShape);

wxSFControlShape::wxSFControlShape() : wxSFRectShape()
{
    m_pControl = NULL;
    m_nProcessEvents = sfdvCONTROLSHAPE_PROCESSEVENTS;
    m_ModFill = sfdvCONTROLSHAPE_MODFILL;
    m_ModBorder = sfdvCONTROLSHAPE_MODBORDER;
    m_nControlOffset = sfdvCONTROLSHAPE_CONTROLOFFSET;

    m_pEventSink = new EventSink(this);

    m_Fill = *wxTRANSPARENT_BRUSH;
    m_Border = *wxTRANSPARENT_PEN;

    MarkSerializableDataMembers();
}

wxSFControlShape::wxSFControlShape(wxWindow *ctrl, const wxRealPoint& pos, const wxRealPoint& size, wxSFDiagramManager* manager)
: wxSFRectShape(pos, size, manager)
{
    SetControl(ctrl);
    m_nProcessEvents = sfdvCONTROLSHAPE_PROCESSEVENTS;
    m_ModFill = sfdvCONTROLSHAPE_MODFILL;
    m_ModBorder = sfdvCONTROLSHAPE_MODBORDER;
    m_nControlOffset = sfdvCONTROLSHAPE_CONTROLOFFSET;

    m_pEventSink = new EventSink(this);

    m_Fill = *wxTRANSPARENT_BRUSH;
    m_Border = *wxTRANSPARENT_PEN;

    MarkSerializableDataMembers();
}

wxSFControlShape::wxSFControlShape(const wxSFControlShape& obj)
: wxSFRectShape(obj)
{
    m_pControl = NULL;
    m_nProcessEvents = obj.m_nProcessEvents;
    m_ModFill = obj.m_ModFill;
    m_ModBorder = obj.m_ModBorder;
    m_nControlOffset = obj.m_nControlOffset;

    m_pEventSink = new EventSink(this);

    MarkSerializableDataMembers();
}

wxSFControlShape::~wxSFControlShape()
{
    if( m_pControl ) m_pControl->Destroy();

    if( m_pEventSink ) delete m_pEventSink;
}

void wxSFControlShape::MarkSerializableDataMembers()
{
    XS_SERIALIZE_EX(m_nProcessEvents, wxT("process_events"), sfdvCONTROLSHAPE_PROCESSEVENTS);
    XS_SERIALIZE_EX(m_nControlOffset, wxT("offset"), sfdvCONTROLSHAPE_CONTROLOFFSET);
    XS_SERIALIZE_EX(m_ModFill, wxT("modification_fill"), sfdvCONTROLSHAPE_MODFILL);
    XS_SERIALIZE_EX(m_ModBorder, wxT("modification_border"), sfdvCONTROLSHAPE_MODBORDER);
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

void wxSFControlShape::SetControl(wxWindow *ctrl, bool fit)
{
    if( m_pControl ) m_pControl->Reparent( m_pPrevParent );

    m_pControl = ctrl;

    if( m_pControl )
    {
        m_pPrevParent = ctrl->GetParent();

        if( m_pParentManager )
        {
            wxSFShapeCanvas *pCanvas = ((wxSFDiagramManager*)m_pParentManager)->GetShapeCanvas();

            // reparent GUI control if necessary
            if( pCanvas && ( (wxWindow*)pCanvas != m_pPrevParent ) ) m_pControl->Reparent( (wxWindow*)pCanvas );

            // redirect mouse events to the event sink for their delayed processing
            m_pControl->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(EventSink::_OnMouseButton), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(EventSink::_OnMouseButton), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(EventSink::_OnMouseButton), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(EventSink::_OnMouseButton), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(EventSink::_OnMouseButton), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(EventSink::_OnMouseButton), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_MOTION, wxMouseEventHandler(EventSink::_OnMouseMove), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(EventSink::_OnKeyDown), NULL, m_pEventSink);
            m_pControl->Connect(wxEVT_SIZE, wxSizeEventHandler(EventSink::_OnSize), NULL, m_pEventSink);
        }

        if( fit ) UpdateShape();

        UpdateControl();
    }
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFControlShape::FitToChildren()
{	
    wxRect ctrlRct;
    wxRect bbRct = GetBoundingBox();
	
	if( m_pControl ) ctrlRct = wxRect(m_pControl->GetPosition(), m_pControl->GetSize());
	else
		ctrlRct = bbRct;

    wxSFRectShape::FitToChildren();

    if( bbRct.Intersects(ctrlRct) && !bbRct.Contains(ctrlRct) ) UpdateShape();
}

void wxSFControlShape::Scale(double x, double y, bool children)
{
    wxSFRectShape::Scale(x, y, children);
    UpdateControl();
}

void wxSFControlShape::MoveTo(double x, double y)
{
    wxSFRectShape::MoveTo(x, y);
    UpdateControl();
}

void wxSFControlShape::MoveBy(double x, double y)
{
    wxSFRectShape::MoveBy(x, y);
    UpdateControl();
}

void wxSFControlShape::OnBeginDrag(const wxPoint& pos)
{
	wxUnusedVar( pos );
	
    m_PrevFill = m_Fill;
    m_Fill = m_ModFill;

    if( m_pParentManager )
    {
        wxSFShapeCanvas *pCanvas = ((wxSFDiagramManager*)m_pParentManager)->GetShapeCanvas();

        if( pCanvas )
        {
            m_nPrevStyle = pCanvas->GetStyle();
            pCanvas->RemoveStyle(wxSFShapeCanvas::sfsDND);
        }
    }

    if( m_pControl )
    {
        m_pControl->Hide();
        m_pControl->Disconnect(wxEVT_SIZE, wxSizeEventHandler(EventSink::_OnSize), NULL, m_pEventSink);
    }
	
	wxSFShapeBase::OnBeginDrag(pos);
}

void wxSFControlShape::OnEndDrag(const wxPoint& pos)
{
    m_Fill = m_PrevFill;

    if( m_pParentManager )
    {
        wxSFShapeCanvas *pCanvas = ((wxSFDiagramManager*)m_pParentManager)->GetShapeCanvas();

        if( pCanvas ) pCanvas->SetStyle(m_nPrevStyle);
    }

    UpdateControl();

    if( m_pControl )
    {
        m_pControl->Connect(wxEVT_SIZE, wxSizeEventHandler(EventSink::_OnSize), NULL, m_pEventSink);

        m_pControl->Show();
        m_pControl->SetFocus();
    }
	
	wxSFShapeBase::OnEndDrag(pos);
}

void wxSFControlShape::OnBeginHandle(wxSFShapeHandle& handle)
{
    m_PrevBorder = m_Border;
    m_Border = m_ModBorder;

    m_PrevFill = m_Fill;
    m_Fill = m_ModFill;

    if( m_pControl )
    {
        m_pControl->Hide();
        m_pControl->Disconnect(wxEVT_SIZE, wxSizeEventHandler(EventSink::_OnSize), NULL, m_pEventSink);
    }
	
	// call default handler
    wxSFRectShape::OnBeginHandle(handle);
}

void wxSFControlShape::OnHandle(wxSFShapeHandle& handle)
{
	// call default handler
    wxSFRectShape::OnHandle(handle);
	
	UpdateControl();
}

void wxSFControlShape::OnEndHandle(wxSFShapeHandle& handle)
{
    m_Border = m_PrevBorder;
    m_Fill = m_PrevFill;

    if( m_pControl )
    {
        m_pControl->Show();
        m_pControl->SetFocus();

        m_pControl->Connect(wxEVT_SIZE, wxSizeEventHandler(EventSink::_OnSize), NULL, m_pEventSink);
    }
	
	// call default handler
    wxSFRectShape::OnEndHandle(handle);
}

void wxSFControlShape::Update()
{
	wxSFShapeBase::Update();
	UpdateControl();
}


//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

void wxSFControlShape::UpdateControl()
{
    if( m_pControl )
    {
        int x = 0, y = 0;
		
        wxRect minBB = m_pControl->GetMinSize();
        wxRect rctBB = GetBoundingBox().Deflate(m_nControlOffset, m_nControlOffset);

        if( rctBB.GetWidth() < minBB.GetWidth() )
        {
            rctBB.SetWidth(minBB.GetWidth());
            m_nRectSize.x = minBB.GetWidth() + 2*m_nControlOffset;
        }

        if( rctBB.GetHeight() < minBB.GetHeight() )
        {
            rctBB.SetHeight(minBB.GetHeight());
            m_nRectSize.y = minBB.GetHeight() + 2*m_nControlOffset;
        }

        GetParentCanvas()->CalcUnscrolledPosition(0, 0, &x, &y);

        // set the control's dimensions and position according to the parent control shape
        m_pControl->SetSize(rctBB.GetWidth(), rctBB.GetHeight());
        m_pControl->Move(rctBB.GetLeft() - x, rctBB.GetTop() - y);
    }
}

void wxSFControlShape::UpdateShape()
{
	if( m_pControl )
	{
		wxSize nCtrlSize = m_pControl->GetSize();
		wxPoint nCtrlPos = m_pControl->GetPosition();

		m_nRectSize.x = nCtrlSize.x + 2*m_nControlOffset;
		m_nRectSize.y = nCtrlSize.y + 2*m_nControlOffset;

		GetShapeManager()->GetShapeCanvas()->Refresh(false);
	}
}

//----------------------------------------------------------------------------------//
// private functions
//----------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------//
// EventSink class
//----------------------------------------------------------------------------------//

EventSink::EventSink()
{
    m_pParentShape = NULL;
}

EventSink::EventSink(wxSFControlShape *parent)
{
    wxASSERT(parent);
    m_pParentShape = parent;
}

EventSink::~EventSink()
{
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

void EventSink::_OnMouseButton(wxMouseEvent &event)
{
    if( m_pParentShape->GetEventProcessing() & wxSFControlShape::evtMOUSE2CANVAS )
    {
        wxMouseEvent updatedEvent(event);

        UpdateMouseEvent(updatedEvent);
        SendEvent(updatedEvent);
    }

    // process the event also by an original handler if requested
    if( m_pParentShape->GetEventProcessing() & wxSFControlShape::evtMOUSE2GUI ) event.Skip();

    //m_pParentShape->GetControl()->SetFocus();
}

void EventSink::_OnMouseMove(wxMouseEvent &event)
{
    if( m_pParentShape->GetEventProcessing() & wxSFControlShape::evtMOUSE2CANVAS )
    {
        wxMouseEvent updatedEvent(event);

        UpdateMouseEvent(updatedEvent);
        SendEvent(updatedEvent);
    }

    // process the event also by an original handler if requested
    if( m_pParentShape->GetEventProcessing() & wxSFControlShape::evtMOUSE2GUI ) event.Skip();
}

void EventSink::_OnKeyDown(wxKeyEvent &event)
{
    if( m_pParentShape->GetEventProcessing() & wxSFControlShape::evtKEY2CANVAS ) SendEvent(event);

    // process the event also by an original handler if requested
    if( m_pParentShape->GetEventProcessing() & wxSFControlShape::evtKEY2GUI ) event.Skip();
}

void EventSink::_OnSize(wxSizeEvent &event)
{
    event.Skip();

    m_pParentShape->UpdateShape();
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

void EventSink::SendEvent(wxEvent &event)
{
    if( m_pParentShape && m_pParentShape->GetParentManager() )
    {
        wxSFShapeCanvas *pCanvas = ((wxSFDiagramManager*)m_pParentShape->GetParentManager())->GetShapeCanvas();

        // send copy of the event to the shape canvas
        if( pCanvas ) wxPostEvent( pCanvas, event );
    }
}

void EventSink::UpdateMouseEvent(wxMouseEvent &event)
{
    int x = 0, y = 0;
    wxRealPoint nAbsPos = m_pParentShape->GetAbsolutePosition();

    m_pParentShape->GetParentCanvas()->CalcUnscrolledPosition(0, 0, &x, &y);

    event.m_x += ( (int)nAbsPos.x + m_pParentShape->GetControlOffset() - x );
    event.m_y += ( (int)nAbsPos.y + m_pParentShape->GetControlOffset() - y );
}
