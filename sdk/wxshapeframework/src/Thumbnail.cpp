/***************************************************************
 * Name:      Thumbnail.h
 * Purpose:   Implements canvas thumbnail class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2009-06-09
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/
 
#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <wx/dcbuffer.h>

#include "wx/wxsf/Thumbnail.h"
#include "wx/wxsf/BitmapShape.h"

BEGIN_EVENT_TABLE(wxSFThumbnail, wxPanel)
	EVT_PAINT( wxSFThumbnail::_OnPaint )
	EVT_ERASE_BACKGROUND( wxSFThumbnail::_OnEraseBackground )
	EVT_MOTION( wxSFThumbnail::_OnMouseMove )
	EVT_LEFT_DOWN( wxSFThumbnail::_OnLeftDown )
	EVT_RIGHT_DOWN( wxSFThumbnail::_OnRightDown )
	EVT_TIMER( wxSFThumbnail::ID_UPDATETIMER, wxSFThumbnail::_OnTimer )
	EVT_UPDATE_UI( wxSFThumbnail::IDM_SHOWELEMENTS, wxSFThumbnail::_OnUpdateShowElements )
	EVT_UPDATE_UI( wxSFThumbnail::IDM_SHOWCONNECTIONS, wxSFThumbnail::_OnUpdateShowConnections )
	EVT_MENU( wxSFThumbnail::IDM_SHOWELEMENTS, wxSFThumbnail::_OnShowElements )
	EVT_MENU( wxSFThumbnail::IDM_SHOWCONNECTIONS, wxSFThumbnail::_OnShowConnections )
END_EVENT_TABLE()

//----------------------------------------------------------------------------------//
// constructor and destructor
//----------------------------------------------------------------------------------//

wxSFThumbnail::wxSFThumbnail(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 150), wxTAB_TRAVERSAL | wxFULL_REPAINT_ON_RESIZE )
{
	SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	SetSizeHints( wxSize(10, 10), wxDefaultSize );
	
	m_pCanvas = NULL;
	m_nScale = 1;
	m_nThumbStyle = tsSHOW_ELEMENTS | tsSHOW_CONNECTIONS;
	
	m_UpdateTimer.SetOwner(this, ID_UPDATETIMER);
}

wxSFThumbnail::~wxSFThumbnail()
{
	m_pCanvas = NULL;
	
	m_UpdateTimer.Stop();
}

//----------------------------------------------------------------------------------//
// public functions           
//----------------------------------------------------------------------------------//

void wxSFThumbnail::SetCanvas(wxSFShapeCanvas* canvas)
{
	m_pCanvas = canvas;
	
	if( m_pCanvas )	m_UpdateTimer.Start(100);
	else
	{
		m_UpdateTimer.Stop();
		Refresh(false);
	}		
}

void wxSFThumbnail::DrawContent(wxDC& dc)
{
	// HINT: overload it for custom actions...
	
	wxSFShapeBase *pShape;
	
	SerializableList::compatibility_iterator node = m_pCanvas->GetDiagramManager()->GetRootItem()->GetFirstChildNode();
	while( node )
	{
		pShape = wxDynamicCast( node->GetData(), wxSFShapeBase );
		
		if( pShape )
		{
			if( (m_nThumbStyle & tsSHOW_CONNECTIONS) && pShape->IsKindOf(CLASSINFO(wxSFLineShape)) ) pShape->Draw( dc, sfWITHOUTCHILDREN );
			else if( m_nThumbStyle & tsSHOW_ELEMENTS )
			{
				if( pShape->IsKindOf(CLASSINFO(wxSFBitmapShape)) ) 
				{
					dc.SetPen( wxPen( *wxBLACK, 1, wxPENSTYLE_DOT) );
					dc.SetBrush( *wxWHITE_BRUSH );
					
					dc.DrawRectangle( pShape->GetBoundingBox() );
					
					dc.SetBrush( wxNullBrush );
					dc.SetPen( wxNullPen );
				}
				else if( !pShape->IsKindOf(CLASSINFO(wxSFLineShape)) ) pShape->Draw( dc, sfWITHOUTCHILDREN );
			}
		}
						
		node = node->GetNext();
	}
}

//----------------------------------------------------------------------------------//
// protected event handlers
//----------------------------------------------------------------------------------//

void wxSFThumbnail::_OnEraseBackground(wxEraseEvent& event)
{
	wxUnusedVar( event );
	// do nothing to eliminate flickering on Windows
}

void wxSFThumbnail::_OnLeftDown(wxMouseEvent& event)
{
	m_nPrevMousePos = event.GetPosition();
}

void wxSFThumbnail::_OnRightDown(wxMouseEvent& event)
{
	wxMenu menuPopup;
	
	menuPopup.AppendCheckItem( IDM_SHOWELEMENTS, wxT("Show elements") );
	menuPopup.AppendCheckItem( IDM_SHOWCONNECTIONS, wxT("Show connections") );
	
	PopupMenu( &menuPopup, event.GetPosition() );
}

void wxSFThumbnail::_OnMouseMove(wxMouseEvent& event)
{
	if( m_pCanvas && IsShown() && event.Dragging() )
	{
		int ux, uy;
		m_pCanvas->GetScrollPixelsPerUnit( &ux, &uy );
		
		wxPoint szDelta = event.GetPosition() - m_nPrevMousePos;
		wxSize szCanvasOffset = GetCanvasOffset();
		
		m_pCanvas->Scroll( (double(szDelta.x)/m_nScale + szCanvasOffset.x)/ux, (double(szDelta.y)/m_nScale + szCanvasOffset.y)/uy );
		
		m_nPrevMousePos = event.GetPosition();
		
		Refresh(false);
	}
}

void wxSFThumbnail::_OnPaint(wxPaintEvent& event)
{
	wxUnusedVar( event );
	
	#if wxVERSION_NUMBER < 2900 && wxUSE_GRAPHICS_CONTEXT
	bool fGCEnabled = wxSFShapeCanvas::IsGCEnabled();
    wxSFScaledDC::EnableGC( false );
    #endif
	
	wxBufferedPaintDC dc(this);
	
	// clear background
	dc.SetBackground( wxBrush(wxColour(150, 150, 150)) );
	dc.Clear();
	
	if( m_pCanvas )
	{
		wxSize szCanvas = m_pCanvas->GetClientSize();
		wxSize szVirtCanvas = m_pCanvas->GetVirtualSize();
		wxSize szCanvasOffset = GetCanvasOffset();
		wxSize szThumb = GetClientSize();
		
		// scale and copy bitmap to DC
		double cx = szVirtCanvas.x, cy = szVirtCanvas.y, tx = szThumb.x, ty = szThumb.y;
		
		if( (tx/ty) > (cx/cy) )	m_nScale = ty/cy;
		else
			m_nScale = tx/cx;

		// draw virtual canvas area
		dc.SetPen(*wxWHITE_PEN);
		dc.SetBrush( wxBrush(wxColour(240, 240, 240)) );
		dc.DrawRectangle(0, 0, double(szVirtCanvas.x)*m_nScale, double(szVirtCanvas.y)*m_nScale);
		
		// draw top level shapes
		wxSFScaledDC sdc( (wxWindowDC*)&dc, m_nScale * m_pCanvas->GetScale() );
		this->DrawContent( sdc );
		
		// draw canvas client area
		dc.SetPen(*wxRED_PEN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(double(szCanvasOffset.x)*m_nScale, double(szCanvasOffset.y)*m_nScale, double(szCanvas.x)*m_nScale, double(szCanvas.y)*m_nScale);
		
		dc.SetBrush(wxNullBrush);
		dc.SetPen(wxNullPen);
	} 
	
	dc.SetBackground( wxNullBrush );
	
    #if wxVERSION_NUMBER < 2900 && wxUSE_GRAPHICS_CONTEXT
    wxSFScaledDC::EnableGC( fGCEnabled );
    #endif
}

void wxSFThumbnail::_OnTimer(wxTimerEvent& event)
{
	wxUnusedVar( event );

	if( m_pCanvas && IsShown() ) Refresh(false);
}

void wxSFThumbnail::_OnShowConnections(wxCommandEvent& event)
{
	wxUnusedVar( event );

	if( m_nThumbStyle & tsSHOW_CONNECTIONS ) m_nThumbStyle &= ~tsSHOW_CONNECTIONS;
	else
		m_nThumbStyle |= tsSHOW_CONNECTIONS;
}

void wxSFThumbnail::_OnShowElements(wxCommandEvent& event)
{
	wxUnusedVar( event );

	if( m_nThumbStyle & tsSHOW_ELEMENTS ) m_nThumbStyle &= ~tsSHOW_ELEMENTS;
	else
		m_nThumbStyle |= tsSHOW_ELEMENTS;
}

void wxSFThumbnail::_OnUpdateShowConnections(wxUpdateUIEvent& event)
{
	event.Check( m_nThumbStyle & tsSHOW_CONNECTIONS );
}

void wxSFThumbnail::_OnUpdateShowElements(wxUpdateUIEvent& event)
{
	event.Check( m_nThumbStyle & tsSHOW_ELEMENTS );
}

//----------------------------------------------------------------------------------//
// protected functions     
//----------------------------------------------------------------------------------//

wxSize wxSFThumbnail::GetCanvasOffset()
{
	if( m_pCanvas )
	{
		int ux, uy, offsetx, offsety;
		
		m_pCanvas->GetScrollPixelsPerUnit( &ux, &uy );
		m_pCanvas->GetViewStart( &offsetx, &offsety );
		
		return wxSize( offsetx*ux, offsety*uy );
	}
	return wxSize();
}
