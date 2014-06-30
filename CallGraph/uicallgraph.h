//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : uicallgraph.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  7 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __UICALLGRAPH_H__
#define __UICALLGRAPH_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/scrolwin.h>
#include <wx/grid.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class uicallgraph
///////////////////////////////////////////////////////////////////////////////
class uicallgraph : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panel3;
		wxStaticText* m_staticText1;
		wxSpinCtrl* m_spinNT;
		wxStaticText* m_staticText2;
		wxSpinCtrl* m_spinET;
		wxCheckBox* m_checkBoxHP;
		wxCheckBox* m_checkBoxHN;
		wxButton* m_buttonRefresh;
		wxStaticLine* m_staticline1;
		wxSplitterWindow* m_splitter;
		wxPanel* m_panel1;
		wxScrolledWindow* m_scrolledWindow;
		wxMenu* m_menu1;
		wxPanel* m_panel2;
		wxGrid* m_grid;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRefreshClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMove( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseWheel( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnSaveCallGraph( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClosePanel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnZoomIn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnZoomOut( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnZoom100( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnZoomOriginal( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		uicallgraph( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,300 ), long style = wxTAB_TRAVERSAL ); 
		~uicallgraph();
		
		void m_splitterOnIdle( wxIdleEvent& )
		{
			m_splitter->SetSashPosition( 400 );
			m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( uicallgraph::m_splitterOnIdle ), NULL, this );
		}
		
		void m_scrolledWindowOnContextMenu( wxMouseEvent &event )
		{
			m_scrolledWindow->PopupMenu( m_menu1, event.GetPosition() );
		}
	
};

#endif //__UICALLGRAPH_H__
