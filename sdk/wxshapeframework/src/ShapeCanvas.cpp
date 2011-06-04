/***************************************************************
 * Name:      ShapeCanvas.cpp
 * Purpose:   Implements shape canvas class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/
 
#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <wx/dcbuffer.h>
#include <wx/sstream.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>

#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/DiagramManager.h"
#include "wx/wxsf/ScaledDC.h"
#include "wx/wxsf/ShapeDataObject.h"
#include "wx/wxsf/EditTextShape.h"
#include "wx/wxsf/BitmapShape.h"
#include "wx/wxsf/SFEvents.h"
#include "wx/wxsf/CommonFcn.h"
#include "wx/wxsf/ControlShape.h"
#include "wx/wxsf/CommonFcn.h"

#ifndef __WXMSW__

/* Copyright (c) Julian Smart */
static const char * page_xpm[] = {
/* columns rows colors chars-per-pixel */
"32 32 37 1",
"5 c #7198D9",
", c #769CDA",
"2 c #DCE6F6",
"i c #FFFFFF",
"e c #779DDB",
": c #9AB6E4",
"9 c #EAF0FA",
"- c #B1C7EB",
"$ c #6992D7",
"y c #F7F9FD",
"= c #BED0EE",
"q c #F0F5FC",
"; c #A8C0E8",
"@ c #366BC2",
"  c None",
"u c #FDFEFF",
"8 c #5987D3",
"* c #C4D5F0",
"7 c #7CA0DC",
"O c #487BCE",
"< c #6B94D7",
"& c #CCDAF2",
"> c #89A9DF",
"3 c #5584D1",
"w c #82A5DE",
"1 c #3F74CB",
"+ c #3A70CA",
". c #3569BF",
"% c #D2DFF4",
"# c #3366BB",
"r c #F5F8FD",
"0 c #FAFCFE",
"4 c #DFE8F7",
"X c #5E8AD4",
"o c #5282D0",
"t c #B8CCEC",
"6 c #E5EDF9",
/* pixels */
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"       .XXXooOO++@#             ",
"       $%&*=-;::>,<1            ",
"       $2%&*=-;::><:3           ",
"       $42%&*=-;::<&:3          ",
"       56477<<<<8<<9&:X         ",
"       59642%&*=-;<09&:5        ",
"       5q9642%&*=-<<<<<#        ",
"       5qqw777<<<<<88:>+        ",
"       erqq9642%&*=t;::+        ",
"       eyrqq9642%&*=t;:O        ",
"       eyywwww777<<<<t;O        ",
"       e0yyrqq9642%&*=to        ",
"       e00yyrqq9642%&*=o        ",
"       eu0wwwwwww777<&*X        ",
"       euu00yyrqq9642%&X        ",
"       eiuu00yyrqq9642%X        ",
"       eiiwwwwwwwwww742$        ",
"       eiiiuu00yyrqq964$        ",
"       eiiiiuu00yyrqq96$        ",
"       eiiiiiuu00yyrqq95        ",
"       eiiiiiiuu00yyrqq5        ",
"       eeeeeeeeeeeeee55e        ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                "
};
#endif

const char* wxSFShadowBrush_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 2 1",
"  c None",
". c #C0C0C0",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . .",
". . . . . . . . ",
" . . . . . . . ."};

// global page setup data
wxPageSetupDialogData* g_pageSetupData = (wxPageSetupDialogData*) NULL;
// global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL ;

// static data
int wxSFShapeCanvas::m_nRefCounter = 0;
wxBitmap wxSFShapeCanvas::m_OutBMP;
// this flag has sense only if wxUSE_GRAPHICS_CONTEXT is present...
bool wxSFShapeCanvas::m_fEnableGC = false;

using namespace wxSFCommonFcn;

IMPLEMENT_DYNAMIC_CLASS(wxSFCanvasSettings, xsSerializable);

wxSFCanvasSettings::wxSFCanvasSettings() : xsSerializable()
{
    m_nScale = 1;
	m_nMinScale = sfdvSHAPECANVAS_SCALE_MIN;
	m_nMaxScale = sfdvSHAPECANVAS_SCALE_MAX;	
    m_nBackgroundColor = sfdvSHAPECANVAS_BACKGROUNDCOLOR;
    m_nCommonHoverColor = sfdvSHAPECANVAS_HOVERCOLOR;
    m_nGridSize = sfdvSHAPECANVAS_GRIDSIZE;
	m_nGridLineMult = sfdvSHAPECANVAS_GRIDLINEMULT;
    m_nGridColor = sfdvSHAPECANVAS_GRIDCOLOR;
	m_nGridStyle = sfdvSHAPECANVAS_GRIDSTYLE;
	m_nGradientFrom = sfdvSHAPECANVAS_GRADIENT_FROM;
	m_nGradientTo = sfdvSHAPECANVAS_GRADIENT_TO;
	m_nStyle = sfdvSHAPECANVAS_STYLE;
	m_nShadowOffset = sfdvSHAPECANVAS_SHADOWOFFSET;
	m_ShadowFill = sfdvSHAPECANVAS_SHADOWBRUSH;
	m_nPrintHAlign = sfdvSHAPECANVAS_PRINT_HALIGN;
	m_nPrintVAlign = sfdvSHAPECANVAS_PRINT_VALIGN;
	m_nPrintMode = sfdvSHAPECANVAS_PRINT_MODE;

    XS_SERIALIZE(m_nScale, wxT("scale"));
	XS_SERIALIZE_EX(m_nMinScale, wxT("min_scale"), sfdvSHAPECANVAS_SCALE_MIN);
    XS_SERIALIZE_EX(m_nMaxScale, wxT("max_scale"), sfdvSHAPECANVAS_SCALE_MAX);
	XS_SERIALIZE_EX(m_nStyle, wxT("style"), sfdvSHAPECANVAS_STYLE);
    XS_SERIALIZE_EX(m_nBackgroundColor, wxT("background_color"), sfdvSHAPECANVAS_BACKGROUNDCOLOR);
	XS_SERIALIZE_EX(m_nGradientFrom, wxT("gradient_from"), sfdvSHAPECANVAS_GRADIENT_FROM);
	XS_SERIALIZE_EX(m_nGradientTo, wxT("gradient_to"), sfdvSHAPECANVAS_GRADIENT_TO);
    XS_SERIALIZE_EX(m_nCommonHoverColor, wxT("hover_color"), sfdvSHAPECANVAS_HOVERCOLOR);
    XS_SERIALIZE_EX(m_nGridSize, wxT("grid_size"), sfdvSHAPECANVAS_GRIDSIZE);
    XS_SERIALIZE_EX(m_nGridLineMult, wxT("grid_line_mult"), sfdvSHAPECANVAS_GRIDLINEMULT);
    XS_SERIALIZE_EX(m_nGridColor, wxT("grid_color"), sfdvSHAPECANVAS_GRIDCOLOR);
    XS_SERIALIZE_EX(m_nGridStyle, wxT("grid_style"), sfdvSHAPECANVAS_GRIDSTYLE);
    XS_SERIALIZE_EX(m_nShadowOffset, wxT("shadow_offset"), sfdvSHAPECANVAS_SHADOWOFFSET);
    XS_SERIALIZE_EX(m_ShadowFill, wxT("shadow_fill"), sfdvSHAPECANVAS_SHADOWBRUSH);
    XS_SERIALIZE_EX(m_nPrintHAlign, wxT("print_halign"), sfdvSHAPECANVAS_PRINT_HALIGN);
    XS_SERIALIZE_EX(m_nPrintVAlign, wxT("print_valign"), sfdvSHAPECANVAS_PRINT_VALIGN);
    XS_SERIALIZE_EX(m_nPrintMode, wxT("print_mode"), sfdvSHAPECANVAS_PRINT_MODE);
    XS_SERIALIZE(m_arrAcceptedShapes, wxT("accepted_shapes"));
}

static const wxChar* dataFormatID = wxT("ShapeFrameWorkDataFormat1_0");

BEGIN_EVENT_TABLE(wxSFShapeCanvas, wxScrolledWindow)
	EVT_PAINT(wxSFShapeCanvas::_OnPaint)
	EVT_ERASE_BACKGROUND(wxSFShapeCanvas::_OnEraseBackground)
	EVT_LEFT_DOWN(wxSFShapeCanvas::_OnLeftDown)
	EVT_LEFT_UP(wxSFShapeCanvas::_OnLeftUp)
	EVT_RIGHT_DOWN(wxSFShapeCanvas::_OnRightDown)
	EVT_RIGHT_UP(wxSFShapeCanvas::_OnRightUp)
	EVT_LEFT_DCLICK(wxSFShapeCanvas::_OnLeftDoubleClick)
	EVT_RIGHT_DCLICK(wxSFShapeCanvas::_OnRightDoubleClick)
	EVT_MOTION(wxSFShapeCanvas::_OnMouseMove)
	EVT_MOUSEWHEEL(wxSFShapeCanvas::_OnMouseWheel)
	EVT_KEY_DOWN(wxSFShapeCanvas::_OnKeyDown)
	EVT_ENTER_WINDOW(wxSFShapeCanvas::_OnEnterWindow)
	EVT_LEAVE_WINDOW(wxSFShapeCanvas::_OnLeaveWindow)
	EVT_SIZE(wxSFShapeCanvas::_OnResize)
END_EVENT_TABLE()

wxSFShapeCanvas::wxSFShapeCanvas()
{
    // NOTE: user must call Create() and wxSFShapeCanvas::SetDiagramManager() to complete
    // canvas initialization!
}

wxSFShapeCanvas::wxSFShapeCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    // NOTE: user must call wxSFShapeCanvas::SetDiagramManager() to complete
    // canvas initialization!

    // initialize shape manager
    wxASSERT_MSG( manager, wxT("Shape manager has not been properly set in shape canvas's constructor."));
    if(!manager)return;
    wxASSERT_MSG( manager, wxT("Shape manager must be initialized before a creation of assigned canvas."));
    if(!manager->GetRootItem())return;

    m_pManager = manager;
    m_pManager->SetShapeCanvas(this);

    Create(parent, id, pos, size, style);

    m_shpMultiEdit.SetParentManager(m_pManager);

    SaveCanvasState();
}

wxSFShapeCanvas::~wxSFShapeCanvas(void)
{
	m_shpMultiEdit.SetParentManager(NULL);
	
    if( --m_nRefCounter == 0) DeinitializePrinting();
	
	//DeleteAllTextCtrls();
	//Clear();
}
bool wxSFShapeCanvas::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    // NOTE: user must call wxSFShapeCanvas::SetDiagramManager() to complete
    // canvas initialization!

    // perform basic window initialization
    wxScrolledWindow::Create(parent, id, pos, size, style, name);

	// set drop target
	m_formatShapes.SetId(dataFormatID);
	SetDropTarget(new wxSFCanvasDropTarget(new wxSFShapeDataObject(m_formatShapes), this));
	m_fDnDStartedHere = false;

	// initialize data members
	m_fCanSaveStateOnMouseUp = false;

	m_nWorkingMode = modeREADY;
	m_pSelectedHandle = NULL;
	m_pNewLineShape = NULL;
	m_pUnselectedShapeUnderCursor = NULL;
	m_pSelectedShapeUnderCursor = NULL;
	m_pTopmostShapeUnderCursor = NULL;

	// initialize multiedit rectangle
	m_shpMultiEdit.SetId(0);
	m_shpMultiEdit.CreateHandles();
	m_shpMultiEdit.Select(true);
	m_shpMultiEdit.Show(false);
	m_shpMultiEdit.ShowHandles(true);

	m_CanvasHistory.SetParentCanvas(this);

	if( ++m_nRefCounter == 1 )
	{
	    // initialize printing
	    InitializePrinting();

        // initialize output bitmap
        int nWidth, nHeight;
        wxDisplaySize(&nWidth, &nHeight);

        if( !m_OutBMP.Create(nWidth, nHeight) ) wxLogError(wxT("Couldn't create output bitmap."));
	}

    SetScrollbars(5, 5, 100, 100);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    return true;
}

void wxSFShapeCanvas::SetDiagramManager(wxSFDiagramManager *manager)
{
    m_pManager = manager;
    if( m_pManager )
    {
        m_pManager->SetShapeCanvas(this);
        m_shpMultiEdit.SetParentManager(m_pManager);
    }
}

//----------------------------------------------------------------------------------//
// Painting functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::EnableGC(bool enab)
{
    if( enab )
    {
#if wxUSE_GRAPHICS_CONTEXT
        m_fEnableGC = true;
#else
		m_fEnableGC = false;
        wxASSERT_MSG( m_fEnableGC, wxT("Couldn't enable Graphics context due to missing wxUSE_GRAPHICS_CONTEXT") );
#endif
    }
    else
        m_fEnableGC = false;
}

void wxSFShapeCanvas::_OnPaint(wxPaintEvent& event)
{
	wxUnusedVar( event );
	
	// use double-buffered painting
	wxBufferedPaintDC paintDC( this );

	wxSFScaledDC dc( (wxWindowDC*)&paintDC, m_Settings.m_nScale );

	PrepareDC( dc );
	dc.PrepareGC();

	DrawContent(dc, sfFROM_PAINT);


	/*int sx, sy, x, y;

	wxPaintDC paintDC(this);

	GetClientSize(&sx, &sy);

	wxSFScaledPaintDC dc(m_OutBMP, m_Settings.m_nScale);

	if(dc.IsOk())
	{
        // prepare window dc
        PrepareDC(dc);
#if wxUSE_GRAPHICS_CONTEXT
        dc.PrepareGC();
#endif

        DrawContent(dc, sfFROM_PAINT);
        dc.GetDeviceOrigin(&x, &y);

        paintDC.Blit(0, 0, sx, sy, &dc, -x, -y);
	}*/

}

void wxSFShapeCanvas::DrawContent(wxDC& dc, bool fromPaint)
{
    wxASSERT( m_pManager );
    if(!m_pManager)return;
    wxASSERT(m_pManager->GetRootItem());
    if(!m_pManager->GetRootItem())return;

    wxSFShapeBase *pShape = NULL, *pParentShape = NULL;
	wxSFLineShape *pLine = NULL;

    #if wxUSE_GRAPHICS_CONTEXT
    wxSFScaledDC::EnableGC( false );
    #endif

	// erase background
	if( m_Settings.m_nStyle & sfsGRADIENT_BACKGROUND )
	{
	    wxSize nBcgSize = GetVirtualSize() + wxSize(m_Settings.m_nGridSize.x, m_Settings.m_nGridSize.y);
	    if( m_Settings.m_nScale != 1.f )
            dc.GradientFillLinear(wxRect(wxPoint(0, 0), wxSize( nBcgSize.x/m_Settings.m_nScale, nBcgSize.y/m_Settings.m_nScale )), m_Settings.m_nGradientFrom, m_Settings.m_nGradientTo, wxSOUTH);
        else
            dc.GradientFillLinear(wxRect(wxPoint(0, 0),  GetVirtualSize() + wxSize(m_Settings.m_nGridSize.x, m_Settings.m_nGridSize.y)), m_Settings.m_nGradientFrom, m_Settings.m_nGradientTo, wxSOUTH);
	}
	else
	{
		dc.SetBackground(wxBrush(m_Settings.m_nBackgroundColor));
		dc.Clear();
	}

	// show grid
	if( ContainsStyle(sfsGRID_SHOW) )
	{
		int linedist = m_Settings.m_nGridSize.x * m_Settings.m_nGridLineMult;
		
		if( linedist * m_Settings.m_nScale > 3 )
		{
			wxRect gridRct(wxPoint(0, 0), GetVirtualSize()+m_Settings.m_nGridSize);
			int maxx = int(gridRct.GetRight()/m_Settings.m_nScale);
			int maxy = int(gridRct.GetBottom()/m_Settings.m_nScale);

			dc.SetPen( wxPen(m_Settings.m_nGridColor, 1, m_Settings.m_nGridStyle) );
			for(int x = gridRct.GetLeft(); x <= maxx; x += linedist)
			{
				dc.DrawLine(x, 0, x, maxy);
			}
			for(int y = gridRct.GetTop(); y <= maxy; y += linedist)
			{
				dc.DrawLine(0, y, maxx, y);
			}
		}
	}

    #if wxUSE_GRAPHICS_CONTEXT
    wxSFScaledDC::EnableGC( m_fEnableGC );
    #endif

	if(fromPaint)
	{
		wxRect updRct;
		wxRect bbRct;

		ShapeList m_lstToDraw;
		ShapeList m_lstLinesToDraw;

		// get all existing shapes
		m_pManager->GetShapes( CLASSINFO(wxSFShapeBase), m_lstToDraw, xsSerializable::searchDFS );

		// get the update rect list
		wxRegionIterator upd(GetUpdateRegion());
		// combine updated rectangles
		bool fFirstRun = true;
		while(upd)
		{
			if( fFirstRun )
			{
				updRct = DP2LP(upd.GetRect().Inflate(5, 5));
				fFirstRun = false;
			}
			else
				updRct.Union(DP2LP(upd.GetRect().Inflate(5, 5)));

			upd++;
		}

		if(m_nWorkingMode == modeSHAPEMOVE)
		{
			ShapeList m_lstSelected;

			// draw unselected non line-based shapes first...
			ShapeList::compatibility_iterator node = m_lstToDraw.GetFirst();
			while(node)
			{
				pShape = (wxSFShapeBase*)node->GetData();
				pParentShape = pShape->GetParentShape();
				
				pLine = wxDynamicCast( pShape, wxSFLineShape );

				if ( !pLine || pLine->IsStandAlone() )
				{
					if( pShape->Intersects(updRct) )
					{
						if( pParentShape )
						{
							pLine = wxDynamicCast( pParentShape, wxSFLineShape );
							if( !pLine || pLine->IsStandAlone() ) pShape->Draw(dc, sfWITHOUTCHILDREN);
						}
						else
							pShape->Draw(dc, sfWITHOUTCHILDREN);
					}
				}
				else
					m_lstLinesToDraw.Append( pShape );

				node = node->GetNext();
			}

			// ... and draw connections
			node = m_lstLinesToDraw.GetFirst();
			while(node)
			{
				pLine = (wxSFLineShape*)node->GetData();

                pLine->GetCompleteBoundingBox(bbRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN | wxSFShapeBase::bbSHADOW);
                if( bbRct.Intersects(updRct) )
                {
					pLine->Draw(dc, pLine->GetLineMode() == wxSFLineShape::modeREADY);
				}

				node = node->GetNext();
			}
		}
		else
		{
			// draw parent shapes (children are processed by parent objects)
			ShapeList::compatibility_iterator node = m_lstToDraw.GetFirst();
			while(node)
			{
				pShape = node->GetData();
				pParentShape = pShape->GetParentShape();
				
				pLine = wxDynamicCast( pShape, wxSFLineShape );

				if( !pLine || pLine->IsStandAlone() )
				{
					if( pShape->Intersects(updRct) )
					{
						if( pParentShape )
						{
							pLine = wxDynamicCast( pParentShape, wxSFLineShape );
							if( !pLine || pLine->IsStandAlone() ) pShape->Draw(dc, sfWITHOUTCHILDREN);
						}
						else
							pShape->Draw(dc, sfWITHOUTCHILDREN);
					}
				}
				else
					m_lstLinesToDraw.Append( pShape );

				node = node->GetNext();
			}

			// draw connections
			node = m_lstLinesToDraw.GetFirst();
			while(node)
			{
				pLine = (wxSFLineShape*)node->GetData();

                pLine->GetCompleteBoundingBox(bbRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);
                if( bbRct.Intersects(updRct) )
                {
					pLine->Draw(dc, pLine->GetLineMode() == wxSFLineShape::modeREADY);
				}

				node = node->GetNext();
			}
		}

        #if wxUSE_GRAPHICS_CONTEXT
        wxSFScaledDC::EnableGC( false );
        #endif

		// draw multiselection if neccessary
		if(m_shpMultiEdit.IsVisible())
		{
			m_shpMultiEdit.Draw(dc);
		}
	}
	else
	{
		// draw parent shapes (children are processed by parent objects)
		SerializableList::compatibility_iterator node = m_pManager->GetRootItem()->GetFirstChildNode();
		while(node)
		{
			pShape = (wxSFShapeBase*)node->GetData();
			
			pLine = wxDynamicCast( pShape, wxSFLineShape );

			if( !pLine || pLine->IsStandAlone() )
			{
				pShape->Draw(dc);
			}

			node = node->GetNext();
		}

		// draw connections
		node = m_pManager->GetRootItem()->GetFirstChildNode();
		while(node)
		{
			pLine = wxDynamicCast( node->GetData(), wxSFLineShape );
		
			if( pLine && !pLine->IsStandAlone() )
			{
				pLine->Draw(dc);
			}
			node = node->GetNext();
		}
	}

    #if wxUSE_GRAPHICS_CONTEXT
    wxSFScaledDC::EnableGC( false );
    #endif
}

void wxSFShapeCanvas::_OnEraseBackground(wxEraseEvent& event)
{
	// do nothing to suppress window flickering
	
	wxUnusedVar( event );
}

void wxSFShapeCanvas::RefreshCanvas(bool erase, wxRect rct)
{
    /*wxRect updRct = LP2DP(rct);

    updRct.Inflate(int(10/m_Settings.m_nScale), int(10/m_Settings.m_nScale));

    RefreshRect(updRct, erase);*/

	wxPoint lpos = DP2LP(wxPoint(0, 0));

	rct.Inflate(int(20/m_Settings.m_nScale), int(20/m_Settings.m_nScale));
	rct.Offset(-lpos.x, -lpos.y);

	RefreshRect(wxRect(int(rct.x*m_Settings.m_nScale), int(rct.y*m_Settings.m_nScale), int(rct.width*m_Settings.m_nScale), int(rct.height*m_Settings.m_nScale)), erase);
}

//----------------------------------------------------------------------------------//
// Virtual functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::OnLeftDown(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

	wxASSERT(m_pManager);
	if(!m_pManager)return;

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	m_fCanSaveStateOnMouseUp = false;

	switch(m_nWorkingMode)
	{
	case modeREADY:
		{
			m_pSelectedHandle = GetTopmostHandleAtPosition(lpos);

			if(!m_pSelectedHandle)
			{
				wxSFShapeBase* pSelectedShape = GetShapeAtPosition(lpos);
				//wxSFShapeBase* pSelectedShape = GetShapeUnderCursor();

				if(pSelectedShape)
				{
					// perform selection
					ShapeList m_lstSelection;
					GetSelectedShapes(m_lstSelection);

					// cancel previous selections if neccessary...
					if( m_lstSelection.IndexOf(pSelectedShape->GetParentShape()) != wxNOT_FOUND )
					{
						if( !pSelectedShape->ContainsStyle( wxSFShapeBase::sfsPROPAGATE_SELECTION ) ) DeselectAll();
					}
					else if(!event.ControlDown())
					{
						if(m_lstSelection.IndexOf(pSelectedShape) == wxNOT_FOUND)
						{
							DeselectAll();
						}
					}
					
					if( pSelectedShape->ContainsStyle( wxSFShapeBase::sfsPROPAGATE_SELECTION ) && pSelectedShape->GetParentShape() )
					{
						pSelectedShape->GetParentShape()->Select(true);
					}
					else
						pSelectedShape->Select(true);

					GetSelectedShapes(m_lstSelection);
					// remove child shapes from the selection
					ValidateSelection(m_lstSelection);

					if( m_lstSelection.GetCount() > 1 )
					{
						HideAllHandles();
					}
					m_shpMultiEdit.Show(false);
					//m_shpMultiEdit.ShowHandles(false);
					
					// call user defined actions
					pSelectedShape->OnLeftClick(FitPositionToGrid(lpos));

					// inform selected shapes about begin of dragging...
					wxSFShapeBase *pShape;
					ShapeList lstConnections;

					ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
					while(node)
					{
						pShape = node->GetData();
						pShape->_OnBeginDrag(FitPositionToGrid(lpos));

						// inform also connections assigned to the shape and its children
						lstConnections.Clear();
						AppendAssignedConnections( pShape, lstConnections, true );

						ShapeList::compatibility_iterator lnode = lstConnections.GetFirst();
						while( lnode )
						{
							lnode->GetData()->_OnBeginDrag(FitPositionToGrid(lpos));
							lnode = lnode->GetNext();
						}

						node = node->GetNext();
					}

					m_nWorkingMode = modeSHAPEMOVE;
				}
				else
				{
					DeselectAll();
					if( ContainsStyle(sfsMULTI_SELECTION) )
					{
                        m_shpMultiEdit.Show(true);
                        m_shpMultiEdit.ShowHandles(false);
                        m_shpMultiEdit.SetRelativePosition(wxRealPoint(lpos.x, lpos.y));
                        m_shpMultiEdit.SetRectSize(wxRealPoint(0, 0));
                        m_nWorkingMode = modeMULTISELECTION;
					}
					else
					{
					    m_nWorkingMode = modeREADY;
					}
				}

				// update canvas
				Refresh(false);
			}
			else
			{
				if(m_pSelectedHandle->GetParentShape() == &m_shpMultiEdit)
				{
					if( ContainsStyle(sfsMULTI_SIZE_CHANGE) )
					{
					    m_nWorkingMode = modeMULTIHANDLEMOVE;
					}
					else
                        m_nWorkingMode = modeREADY;
				}
				else
				{
					m_nWorkingMode = modeHANDLEMOVE;
					switch(m_pSelectedHandle->GetType())
					{
                    case wxSFShapeHandle::hndLINESTART:
                        {
                            wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                            pLine->SetLineMode(wxSFLineShape::modeSRCCHANGE);
                            pLine->SetUnfinishedPoint(lpos);
                        }
                        break;

                    case wxSFShapeHandle::hndLINEEND:
                        {
                            wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                            pLine->SetLineMode(wxSFLineShape::modeTRGCHANGE);
                            pLine->SetUnfinishedPoint(lpos);
                        }
                        break;

                    default:
                        break;
					}
				}
				m_pSelectedHandle->_OnBeginDrag(FitPositionToGrid(lpos));
			}
		}
		break;

	case modeCREATECONNECTION:
		{
		    // update the line shape being created
		    if(m_pNewLineShape)
		    {
		        //wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
				wxSFShapeBase* pShapeUnder = GetShapeUnderCursor();
                //if(pShapeUnder && !pShapeUnder->IsKindOf(CLASSINFO(wxSFLineShape)))
				if(pShapeUnder)
                {
					/*if((m_pNewLineShape->GetSrcShapeId() == -1) &&
                        (pShapeUnder->IsConnectionAccepted(m_pNewLineShape->GetClassInfo()->GetClassName())))
                        //(!pShapeUnder->GetAcceptedTrgNeighbours().IsEmpty()))
                    {
                        m_pNewLineShape->SetSrcShapeId(pShapeUnder->GetId());

						// swith on the "under-construcion" mode
						m_pNewLineShape->SetUnfinishedPoint(lpos);
                    }
                    else */
					if((m_pNewLineShape->GetTrgShapeId() == -1) &&
                            (pShapeUnder != m_pNewLineShape) &&
                            (pShapeUnder->GetId() != -1) &&
                            (pShapeUnder->IsConnectionAccepted(m_pNewLineShape->GetClassInfo()->GetClassName())))
                    {
                        // find out whether the target shape can be connected to the source shape
                        wxSFShapeBase* pSourceShape = m_pManager->FindShape(m_pNewLineShape->GetSrcShapeId());

                        if( pSourceShape &&
                            pShapeUnder->IsSrcNeighbourAccepted(pSourceShape->GetClassInfo()->GetClassName()) &&
                            pSourceShape->IsTrgNeighbourAccepted(pShapeUnder->GetClassInfo()->GetClassName()) )
                        {
                            m_pNewLineShape->SetTrgShapeId(pShapeUnder->GetId());
                            m_pNewLineShape->CreateHandles();
							
                            m_pNewLineShape->SetEndingConnectionPoint( pShapeUnder->GetNearestConnectionPoint( Conv2RealPoint(lpos) ) );

                            // swith off the "under-construcion" mode
                            m_pNewLineShape->SetLineMode(wxSFLineShape::modeREADY);

                            // inform user that the line is completed
                            if( !OnPreConnectionFinished(m_pNewLineShape) )
							{
								m_pManager->RemoveShape( m_pNewLineShape );
								
								m_nWorkingMode = modeREADY;
								m_pNewLineShape = NULL;
								return;
							}
							
							OnConnectionFinished(m_pNewLineShape);

							m_pNewLineShape->Update();
                            m_pNewLineShape->Refresh();

                            m_nWorkingMode = modeREADY;
                            m_pNewLineShape = NULL;

                            SaveCanvasState();
                        }
                    }
                }
                else
                {
                    if(m_pNewLineShape->GetSrcShapeId() != -1)
                    {
                        wxPoint flpos = FitPositionToGrid(lpos);
                        m_pNewLineShape->GetControlPoints().Append(new wxRealPoint(flpos.x, flpos.y));
                    }
                }
		    }
		}
		break;

	default:
		m_nWorkingMode = modeREADY;
	}
}

void wxSFShapeCanvas::OnLeftUp(wxMouseEvent &event)
{
    // HINT: override it for custom actions...

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeMULTIHANDLEMOVE:
	case modeHANDLEMOVE:
		{
			// resize parent shape to fit all its children if neccessary
			if( m_pSelectedHandle->GetParentShape()->GetParentShape() )
			{
				//((wxSFShapeBase*)m_pSelectedHandle->GetParentShape()->GetParentShape())->FitToChildren();
				((wxSFShapeBase*)m_pSelectedHandle->GetParentShape()->GetParentShape())->Update();
			}

			// if the handle is line handle then return the line to normal state
			// and re-assign line's source/target shape
			switch(m_pSelectedHandle->GetType())
			{
            case wxSFShapeHandle::hndLINESTART:
            case wxSFShapeHandle::hndLINEEND:
                {
                    wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                    pLine->SetLineMode(wxSFLineShape::modeREADY);

					//wxSFShapeBase* pParentShape = GetShapeAtPosition(lpos, 1, searchBOTH);
					wxSFShapeBase* pParentShape = GetShapeUnderCursor();

					//if(pParentShape && !pParentShape->IsKindOf(CLASSINFO(wxSFLineShape))  && (pParentShape->IsConnectionAccepted(pLine->GetClassInfo()->GetClassName())))
                    if(pParentShape && (pParentShape != pLine) && (pParentShape->IsConnectionAccepted(pLine->GetClassInfo()->GetClassName())))
                    {
                        if(m_pSelectedHandle->GetType() == wxSFShapeHandle::hndLINESTART)
                        {
                            wxSFShapeBase* pTrgShape = m_pManager->FindShape( pLine->GetTrgShapeId() );
                            if( pTrgShape && pParentShape->IsTrgNeighbourAccepted(pTrgShape->GetClassInfo()->GetClassName()))
                            {
                                pLine->SetSrcShapeId(pParentShape->GetId());
                            }
                        }
                        else
                        {
                            wxSFShapeBase* pSrcShape = m_pManager->FindShape( pLine->GetSrcShapeId() );
                            if( pSrcShape && pParentShape->IsSrcNeighbourAccepted(pSrcShape->GetClassInfo()->GetClassName()))
                            {
                                pLine->SetTrgShapeId(pParentShape->GetId());
                            }
                        }
                    }
                }
                break;

            default:
                break;
			}

			m_pSelectedHandle->_OnEndDrag(lpos);

            m_pSelectedHandle = NULL;
			if(m_fCanSaveStateOnMouseUp)SaveCanvasState();
		}
		break;

	case modeSHAPEMOVE:
		{
		    //wxSFShapeBase* pParentShape = NULL;

			ShapeList m_lstSelection;
			GetSelectedShapes(m_lstSelection);

			ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
			while(node)
			{
				wxSFShapeBase* pShape = node->GetData();
				pShape->_OnEndDrag(lpos);

                ReparentShape( pShape, lpos );

				node = node->GetNext();
			}

			if(m_lstSelection.GetCount()>1)
			{
				m_shpMultiEdit.Show(true);
				m_shpMultiEdit.ShowHandles(true);
			}
			else
				m_shpMultiEdit.Show(false);

			MoveShapesFromNegatives();

			if(m_fCanSaveStateOnMouseUp)SaveCanvasState();
		}
		break;

	case modeMULTISELECTION:
		{
			ShapeList m_lstSelection;
/*            ShapeList shapes;
            m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);*/

			wxRect selRect(m_shpMultiEdit.GetBoundingBox().GetLeftTop(), m_shpMultiEdit.GetBoundingBox().GetRightBottom());
			ShapeList::compatibility_iterator node = m_lstCurrentShapes.GetFirst();
			while(node)
			{
				wxSFShapeBase* pShape = node->GetData();
				if(pShape->IsActive() && selRect.Contains(pShape->GetBoundingBox()))
				{
					pShape->Select(true);
					//pShape->ShowHandles(true);
					m_lstSelection.Append(pShape);
				}
				node = node->GetNext();
			}

			ValidateSelection(m_lstSelection);

			if( !m_lstSelection.IsEmpty() )
			{
				HideAllHandles();
				m_shpMultiEdit.Show(true);
				m_shpMultiEdit.ShowHandles(true);
			}
			else
				m_shpMultiEdit.Show(false);

		}
		break;

    default:
        break;

	}

    if(m_nWorkingMode != modeCREATECONNECTION)
    {
        // update canvas
        m_nWorkingMode = modeREADY;
        UpdateMultieditSize();
        UpdateVirtualSize();
        Refresh();
    }
}

void wxSFShapeCanvas::OnLeftDoubleClick(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
    case modeREADY:
        {
            //wxSFShapeBase* pShape = GetShapeAtPosition(lpos);
			wxSFShapeBase* pShape = GetShapeUnderCursor();
            if(pShape)
            {
                pShape->OnLeftDoubleClick(lpos);
                pShape->Refresh();

                // double click onto a line shape always change its set of
                // control points so the canvas state should be saved now...
                if(pShape->IsKindOf(CLASSINFO(wxSFLineShape)))SaveCanvasState();
            }
        }
        break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnRightDown(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
    case modeREADY:
        {
            DeselectAll();

            //wxSFShapeBase* pShape = GetShapeAtPosition(lpos);
			wxSFShapeBase* pShape = GetShapeUnderCursor();
            if(pShape)
            {
                pShape->Select(true);
                pShape->OnRightClick(lpos);
                //pShape->Refresh();
            }
        }
        break;

    default:
        break;
	}

	Refresh();
}

void wxSFShapeCanvas::OnRightUp(wxMouseEvent &event)
{
	// HINT: override it for custom actions...

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeREADY:
		break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnRightDoubleClick(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
    case modeREADY:
        {
            //wxSFShapeBase* pShape = GetShapeAtPosition(lpos);
			wxSFShapeBase* pShape = GetShapeUnderCursor();
            if(pShape)
            {
                pShape->OnRightDoubleClick(lpos);
                pShape->Refresh();
            }
        }
        break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnMouseMove(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

	wxASSERT(m_pManager);
	if(!m_pManager)return;

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeREADY:
	case modeCREATECONNECTION:
		{
			if(!event.Dragging())
			{
				// send event to multiedit shape
				if( m_shpMultiEdit.IsVisible() )m_shpMultiEdit._OnMouseMove(lpos);

				// send event to all user shapes
				ShapeList::compatibility_iterator node = m_lstCurrentShapes.GetFirst();
				while(node)
				{
					node->GetData()->_OnMouseMove(lpos);
					node = node->GetNext();
				}

				// update unfinished line if any
				if(m_pNewLineShape)
				{
                    wxRect lineRct, updLineRct;
                    m_pNewLineShape->GetCompleteBoundingBox(lineRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);

				    m_pNewLineShape->SetUnfinishedPoint(FitPositionToGrid(lpos));
				    m_pNewLineShape->GetCompleteBoundingBox(updLineRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);

					lineRct.Union(updLineRct);
					
					m_pNewLineShape->Update();

					RefreshCanvas(false, lineRct);
				}
			}
		}
		break;

	case modeHANDLEMOVE:
	case modeMULTIHANDLEMOVE:
		{
			if(event.Dragging())
			{
				if( m_pSelectedHandle ) m_pSelectedHandle->_OnDragging( FitPositionToGrid(lpos) );
				if( m_nWorkingMode == modeMULTIHANDLEMOVE ) UpdateMultieditSize();

				m_fCanSaveStateOnMouseUp = true;
			}
			else
			{
			    if( m_pSelectedHandle ) m_pSelectedHandle->_OnEndDrag(lpos);

				m_pSelectedHandle = NULL;
				m_nWorkingMode = modeREADY;
			}
		}
		// do not always 'break'; allow shape's highlighting in modeHANDLEMOVE as well
		if(m_nWorkingMode == modeMULTIHANDLEMOVE)break;

	case modeSHAPEMOVE:
		{
			if( event.Dragging() )
			{
				if( ContainsStyle( sfsGRID_USE ) )
				{
					if( ( abs( event.GetPosition().x - m_nPrevMousePos.x ) < m_Settings.m_nGridSize.x ) &&
						( abs( event.GetPosition().y - m_nPrevMousePos.y ) < m_Settings.m_nGridSize.y ) )
						return;
				}
				m_nPrevMousePos = event.GetPosition();

				if( event.ControlDown() || event.ShiftDown() )
				{
					ShapeList lstSelection;
					GetSelectedShapes(lstSelection);

                    DeselectAll();

					DoDragDrop(lstSelection, lpos);
				}
				else
				{
					//ShapeList shapes;
					wxSFShapeBase* pShape;
					ShapeList lstConnections;

					//m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);
					ShapeList::compatibility_iterator node = m_lstCurrentShapes.GetFirst();
					while(node)
					{
						pShape = node->GetData();

						if(pShape->IsSelected() && (m_nWorkingMode == modeSHAPEMOVE))
						{
							pShape->_OnDragging(FitPositionToGrid(lpos));

							// move also connections assigned to this shape and its children
							lstConnections.Clear();

							AppendAssignedConnections( pShape, lstConnections, true );

							ShapeList::compatibility_iterator lnode = lstConnections.GetFirst();
							while( lnode )
							{
								lnode->GetData()->_OnDragging(FitPositionToGrid(lpos));
								lnode = lnode->GetNext();
							}
							
							// update connections assigned to this shape
							lstConnections.Clear();
							
							m_pManager->GetAssignedConnections( pShape, CLASSINFO(wxSFLineShape), wxSFShapeBase::lineBOTH, lstConnections );
							lnode = lstConnections.GetFirst();
							while( lnode )
							{
								lnode->GetData()->Update();
								lnode = lnode->GetNext();
							}
							
						}
						else
							pShape->_OnMouseMove(lpos);

						node = node->GetNext();
					}

					m_fCanSaveStateOnMouseUp = true;
				}
			}
			else
				m_nWorkingMode = modeREADY;
		}
		break;

	case modeMULTISELECTION:
		{
			wxRect shpRct = m_shpMultiEdit.GetBoundingBox();
			m_shpMultiEdit.SetRectSize(wxRealPoint(lpos.x - shpRct.GetLeft(), lpos.y - shpRct.GetTop()));
			Refresh(false);
		}
		break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnMouseWheel(wxMouseEvent& event)
{
    // HINT: override it for custom actions...
	
	if( event.ControlDown() )
	{
		double nScale = GetScale();
		nScale += (double)event.GetWheelRotation()/(event.GetWheelDelta()*10);
		
		if( nScale < m_Settings.m_nMinScale ) nScale = m_Settings.m_nMinScale;
		if( nScale > m_Settings.m_nMaxScale ) nScale = m_Settings.m_nMaxScale;
		
		SetScale( nScale );
		Refresh(false);
	}

	event.Skip();
}

void wxSFShapeCanvas::OnKeyDown(wxKeyEvent &event)
{
	// HINT: override it for custom actions...

	wxASSERT(m_pManager);
	if(!m_pManager)return;

	wxSFShapeBase *pShape;
	ShapeList m_lstSelection;
	GetSelectedShapes(m_lstSelection);

	switch(event.GetKeyCode())
	{
	case WXK_DELETE:
		{
		    // send event to selected shapes
			ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
			while(node)
			{
				pShape = node->GetData();
				if( pShape->ContainsStyle(wxSFShapeBase::sfsPROCESS_DEL) )
				{
					pShape->_OnKey(event.GetKeyCode());
					node = node->GetNext();
					m_lstSelection.DeleteObject(pShape);
				}
				else
					node = node->GetNext();
			}
			
			ClearTemporaries();

            // delete selected shapes
			m_pManager->RemoveShapes(m_lstSelection);
			m_shpMultiEdit.Show(false);
			SaveCanvasState();
			Refresh(false);
		}
		break;

	case WXK_ESCAPE:
		{
			switch(m_nWorkingMode)
			{
            case modeCREATECONNECTION:
                {
					AbortInteractiveConnection();
                }
                break;

            case modeHANDLEMOVE:
                {
                    if(m_pSelectedHandle && m_pSelectedHandle->GetParentShape()->IsKindOf(CLASSINFO(wxSFLineShape)))
                    {
                        m_pSelectedHandle->_OnEndDrag(wxPoint(0, 0));

                        wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                        pLine->SetLineMode(wxSFLineShape::modeREADY);

                        m_pSelectedHandle = NULL;
                    }
                }
                break;

            default:
                // send event to selected shapes
                ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
                while(node)
                {
                    node->GetData()->_OnKey(event.GetKeyCode());
                    node = node->GetNext();
                }
                break;
			}
			m_nWorkingMode = modeREADY;
			Refresh(false);
		}
		break;

	case WXK_LEFT:
	case WXK_RIGHT:
	case WXK_UP:
	case WXK_DOWN:
		{
			wxSFShapeBase *pShape, *pLine;
			ShapeList lstConnections;

			ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
			while(node)
			{
				pShape = node->GetData();
				pShape->_OnKey(event.GetKeyCode());

				// inform also connections assigned to this shape
				lstConnections.Clear();
				AppendAssignedConnections( pShape, lstConnections, true );

				ShapeList::compatibility_iterator lnode = lstConnections.GetFirst();
				while( lnode )
				{
					pLine = lnode->GetData();
					if( !pLine->IsSelected() ) pLine->_OnKey(event.GetKeyCode());
					lnode = lnode->GetNext();
				}

				node = node->GetNext();
			}

			// send the event to multiedit ctrl if displayed
			if( m_shpMultiEdit.IsVisible() )
			{
				m_shpMultiEdit._OnKey(event.GetKeyCode());
			}
		}
		break;

	default:
		{
			ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
			while(node)
			{
				node->GetData()->_OnKey(event.GetKeyCode());
				node = node->GetNext();
			}

			if(m_shpMultiEdit.IsVisible()) UpdateMultieditSize();
		}
	}
}

void wxSFShapeCanvas::OnConnectionFinished(wxSFLineShape* connection)
{
	// HINT: override to perform user-defined actions...

	// ... standard implementation generates the wxEVT_SF_LINE_DONE event.
	long id = -1;
	if( connection ) id = connection->GetId();

    wxSFShapeEvent event( wxEVT_SF_LINE_DONE, id);
    event.SetShape( connection );
    ProcessEvent( event );
}

bool wxSFShapeCanvas::OnPreConnectionFinished(wxSFLineShape* connection)
{
	// HINT: override to perform user-defined actions...

	// ... standard implementation generates the wxEVT_SF_LINE_DONE event.
	long id = -1;
	if( connection ) id = connection->GetId();

    wxSFShapeEvent event( wxEVT_SF_LINE_BEFORE_DONE, id);
    event.SetShape( connection );
    ProcessEvent( event );
	
	if( event.IsVetoed() ) return false;
		
	return true;
}

void wxSFShapeCanvas::OnTextChange(wxSFEditTextShape* shape)
{
    // HINT: override it for custom actions...

	// ... standard implementation generates the wxEVT_SF_TEXT_CHANGE event.
	long id = -1;
	if( shape ) id = shape->GetId();

    wxSFShapeTextEvent event( wxEVT_SF_TEXT_CHANGE, id);
    event.SetShape( shape );
	event.SetText( shape->GetText() );
    ProcessEvent( event );
}

void wxSFShapeCanvas::OnUpdateVirtualSize(wxRect& virtrct)
{
	// HINT: override it for custom actions...
}

//----------------------------------------------------------------------------------//
// Private event handlers functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::_OnLeftDown(wxMouseEvent& event)
{
    this->OnLeftDown(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnLeftDoubleClick(wxMouseEvent& event)
{
    this->OnLeftDoubleClick(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnLeftUp(wxMouseEvent& event)
{
    this->OnLeftUp(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnRightDown(wxMouseEvent& event)
{
    this->OnRightDown(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnRightDoubleClick(wxMouseEvent& event)
{
    this->OnRightDoubleClick(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnRightUp(wxMouseEvent& event)
{
    this->OnRightUp(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnMouseMove(wxMouseEvent& event)
{
	// search for any shape located under the mouse cursor (used by wxSFShapeCanvas::GetShapeUnderCursor())

	wxSFShapeBase *pShape;

	wxPoint lpos = DP2LP(event.GetPosition());

	wxSFShapeBase *selShape = NULL, *unselShape = NULL, *topShape = NULL;
	wxSFLineShape *selLine = NULL, *unselLine = NULL, *topLine = NULL;

	m_pTopmostShapeUnderCursor = NULL;
	m_lstCurrentShapes.Clear();

	m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), m_lstCurrentShapes);
	ShapeList::compatibility_iterator node = m_lstCurrentShapes.GetLast();
	while( node )
	{
		pShape = node->GetData();
		if( pShape->IsVisible() && pShape->IsActive() && pShape->Contains(lpos) )
		{
			if( pShape->IsKindOf(CLASSINFO(wxSFLineShape)) )
			{
				if( !topLine ) topLine = (wxSFLineShape*)pShape;
				if( pShape->IsSelected() )
				{
					if( !selLine ) selLine = (wxSFLineShape*)pShape;
				}
				else if (!unselLine ) unselLine = (wxSFLineShape*)pShape;
			}
			else
			{
				if( !topShape ) topShape = pShape;
				if( pShape->IsSelected() )
				{
					if( !selShape ) selShape = pShape;
				}
				else if (!unselShape ) unselShape = pShape;
			}
		}

		node = node->GetPrevious();
	}

	// set pointer to logically topmost selected and unselected shape under the mouse cursor
	if( topLine )
	{
		m_pTopmostShapeUnderCursor = topLine;
	}
	else
		m_pTopmostShapeUnderCursor = topShape;

	if( selLine ) m_pSelectedShapeUnderCursor = selLine;
	else
		m_pSelectedShapeUnderCursor = selShape;

	if( unselLine ) m_pUnselectedShapeUnderCursor = unselLine;
	else
		m_pUnselectedShapeUnderCursor = unselShape;

	// call user event handler
    this->OnMouseMove(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnMouseWheel(wxMouseEvent& event)
{
	if( ContainsStyle( sfsPROCESS_MOUSEWHEEL ) ) this->OnMouseWheel(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnKeyDown(wxKeyEvent& event)
{
    this->OnKeyDown(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnEnterWindow(wxMouseEvent& event)
{
	m_nPrevMousePos = event.GetPosition();

    wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeMULTISELECTION:
        if( !event.LeftIsDown() )
        {
            UpdateMultieditSize();
            m_shpMultiEdit.Show(false);
            m_nWorkingMode = modeREADY;
            Refresh(false);
        }
		break;

    case modeHANDLEMOVE:
        if( !event.LeftIsDown() )
        {
            if(m_pSelectedHandle)
            {
				if(m_pSelectedHandle->GetParentShape()->IsKindOf(CLASSINFO(wxSFLineShape)))
				{
					wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
					pLine->SetLineMode(wxSFLineShape::modeREADY);
				}
				else if(m_pSelectedHandle->GetParentShape()->IsKindOf(CLASSINFO(wxSFBitmapShape)))
				{
					wxSFBitmapShape* pBitmap = (wxSFBitmapShape*)m_pSelectedHandle->GetParentShape();
					pBitmap->OnEndHandle(*m_pSelectedHandle);
				}

                m_pSelectedHandle->_OnEndDrag(lpos);

                SaveCanvasState();
				m_nWorkingMode = modeREADY;
				m_pSelectedHandle = NULL;

                Refresh(false);
            }
        }
        break;

	case modeMULTIHANDLEMOVE:
        if( !event.LeftIsDown() )
        {
            if(m_pSelectedHandle)
            {
                m_pSelectedHandle->_OnEndDrag(lpos);

                SaveCanvasState();
                m_nWorkingMode = modeREADY;
                Refresh(false);
            }
        }
		break;

    case modeSHAPEMOVE:
        if( !event.LeftIsDown() )
        {
			ShapeList m_lstSelection;
			GetSelectedShapes(m_lstSelection);

            MoveShapesFromNegatives();
            UpdateVirtualSize();

            if(m_lstSelection.GetCount() > 1)
            {
                UpdateMultieditSize();
                m_shpMultiEdit.Show(true);
                m_shpMultiEdit.ShowHandles(true);
            }

            ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
            while( node )
            {
                node->GetData()->_OnEndDrag(lpos);
                node = node->GetNext();
            }

            m_nWorkingMode = modeREADY;
            Refresh(false);
        }
        break;

    default:
        break;
	}

	event.Skip();
}

void wxSFShapeCanvas::_OnLeaveWindow(wxMouseEvent& event)
{
	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeMULTISELECTION:
		break;

	case modeSHAPEMOVE:
		{
			/*ShapeList m_lstSelection;
			GetSelectedShapes(m_lstSelection);

			if( ContainsStyle(sfsDND) )
			{
				DoDragDrop(m_lstSelection, lpos);
			}*/
			/*else
			{
				MoveShapesFromNegatives();
				UpdateVirtualSize();

				if(m_lstSelection.GetCount() > 1)
				{
					UpdateMultieditSize();
					m_shpMultiEdit.Show(true);
					m_shpMultiEdit.ShowHandles(true);
				}

				m_nWorkingMode = modeREADY;
				Refresh(false);
			}*/
		}
		break;

    case modeCREATECONNECTION:
        break;

    case modeHANDLEMOVE:
        break;

	case modeMULTIHANDLEMOVE:
		break;

	default:
        m_nWorkingMode = modeREADY;
        Refresh(false);
		break;
	}

	event.Skip();
}

void wxSFShapeCanvas::_OnResize(wxSizeEvent &event)
{
	if( m_Settings.m_nStyle & sfsGRADIENT_BACKGROUND )
	{
		Refresh(false);
	}

	event.Skip();
}

//----------------------------------------------------------------------------------//
// Canvas positions recalculations
//----------------------------------------------------------------------------------//

wxPoint wxSFShapeCanvas::DP2LP(const wxPoint& pos) const
{
	int x,y;
	CalcUnscrolledPosition(pos.x, pos.y, &x, &y);

	return wxPoint(int(x/m_Settings.m_nScale), int(y/m_Settings.m_nScale));
}

wxRect wxSFShapeCanvas::DP2LP(const wxRect& rct) const
{
	int x,y;
	CalcUnscrolledPosition(rct.x, rct.y, &x, &y);

	return wxRect(int(x/m_Settings.m_nScale), int(y/m_Settings.m_nScale), int(rct.width/m_Settings.m_nScale), int(rct.height/m_Settings.m_nScale));
}

wxPoint wxSFShapeCanvas::LP2DP(const wxPoint& pos) const
{
	int x,y;
	CalcScrolledPosition(pos.x, pos.y, &x, &y);

	return wxPoint(int(x*m_Settings.m_nScale), int(y*m_Settings.m_nScale));
}

wxRect wxSFShapeCanvas::LP2DP(const wxRect& rct) const
{
	int x,y;
	CalcScrolledPosition(rct.x, rct.y, &x, &y);

	return wxRect(int(x*m_Settings.m_nScale), int(y*m_Settings.m_nScale), int(rct.width*m_Settings.m_nScale), int(rct.height*m_Settings.m_nScale));
}

void wxSFShapeCanvas::SetScale(double scale)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;
	
	if( scale != 1 )
	{
		ShapeList lstShapes;
		m_pManager->GetShapes(CLASSINFO(wxSFControlShape), lstShapes);
		if( !lstShapes.IsEmpty() )
		{
			wxMessageBox( wxT("Couldn't change scale of shape canvas containing control (GUI) shapes."), wxT("wxShapeFramework"), wxICON_WARNING | wxOK );
			scale = 1;
		}
	}

	if(scale != 0)m_Settings.m_nScale = scale;
	else
		m_Settings.m_nScale = 1;

	// rescale all bitmap shapes if neccessary
	if( !m_fEnableGC )
	{
		ShapeList lstShapes;
        m_pManager->GetShapes(CLASSINFO(wxSFBitmapShape), lstShapes);

        ShapeList::compatibility_iterator node = lstShapes.GetFirst();
        while(node)
        {
            node->GetData()->Scale(1, 1);
            node = node->GetNext();
        }
	}

    UpdateVirtualSize();
}

void wxSFShapeCanvas::SetScaleToViewAll()
{
	wxSize physRct = GetClientSize();
	wxRect virtRct = GetTotalBoundingBox();

	double hz = (double)physRct.GetWidth() / virtRct.GetRight();
	double vz = (double)physRct.GetHeight() / virtRct.GetBottom();

	if( hz < vz )
	{
		if( hz < 1 ) SetScale(hz);
		else
			SetScale(1);
	}
	else
	{
		if( vz < 1 )SetScale(vz);
		else
			SetScale(1);
	}
}

void wxSFShapeCanvas::ScrollToShape(wxSFShapeBase* shape)
{
	wxASSERT(shape);
	if(shape)
	{
		int ux, uy;
		GetScrollPixelsPerUnit(&ux, &uy);
		wxSize szCanvas = GetClientSize();
		wxRealPoint ptPos = shape->GetCenter();
		
		Scroll(((ptPos.x * m_Settings.m_nScale) - szCanvas.x/2)/ux, ((ptPos.y * m_Settings.m_nScale) - szCanvas.y/2)/uy);
	}
}

wxPoint wxSFShapeCanvas::FitPositionToGrid(const wxPoint& pos)
{
	if( ContainsStyle(sfsGRID_USE) )
	{
		return wxPoint(pos.x / m_Settings.m_nGridSize.x * m_Settings.m_nGridSize.x,
			pos.y / m_Settings.m_nGridSize.y * m_Settings.m_nGridSize.y);
	}
	else
		return pos;
}

//----------------------------------------------------------------------------------//
// Shapes handling functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::LoadCanvas(const wxString& file)
{
 	wxASSERT(m_pManager);
	if(!m_pManager)return;

    bool fChartLoaded = false;

    wxXmlDocument xmlDoc;
    xmlDoc.Load(file);

    wxXmlNode* root = xmlDoc.GetRoot();
    if(root)
    {
        m_pManager->Clear();
        ClearCanvasHistory();

        if(root->GetName() == wxT("chart"))
        {
            // the file contains only chart without any additional info
            m_pManager->DeserializeObjects(NULL, root);
        }
        else if(root->GetName() == wxT("canvas"))
        {
            // the file contains chart and additional canvas info
            wxXmlNode* child = root->GetChildren();
            while(child)
            {
                if(child->GetName() == wxT("settings"))
                {
                    m_Settings.DeserializeObject(child->GetChildren());

                    m_pManager->GetAcceptedShapes().Clear();
                    WX_APPEND_ARRAY(m_pManager->GetAcceptedShapes(), m_Settings.m_arrAcceptedShapes);
                }
                else if(child->GetName() == wxT("chart"))
                {
                    // only one chart can be loaded at once
                    if(!fChartLoaded)
                    {
                        m_pManager->DeserializeObjects(NULL, child);

                        fChartLoaded = true;
                    }
                }
                child = child->GetNext();
            }
        }
        else
            wxMessageBox(wxT("Unknown file format."), wxT("wxShapeFramework"), wxOK | wxICON_WARNING);

        SetScale(m_Settings.m_nScale);
        SaveCanvasState();
        UpdateVirtualSize();
        Refresh(false);
    }
}

void wxSFShapeCanvas::SaveCanvas(const wxString& file)
{
 	wxASSERT(m_pManager);
	if(!m_pManager)return;

	// create root node and save canvas properties
	wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("canvas"));

    // initialize settings
    m_Settings.m_arrAcceptedShapes.Clear();
    WX_APPEND_ARRAY(m_Settings.m_arrAcceptedShapes, m_pManager->GetAcceptedShapes());

    // serialize settings
    wxXmlNode* settings = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("settings"));
    settings->AddChild(m_Settings.SerializeObject(settings));
    root->AddChild(settings);

    // create chart node
    wxXmlNode* chart = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("chart"));
    m_pManager->SerializeObjects(m_pManager->GetRootItem(), chart, false);
    root->AddChild(chart);

    // create and save XML document
    wxXmlDocument xmlDoc;
    xmlDoc.SetRoot(root);
    xmlDoc.Save(file, 2);
}

void wxSFShapeCanvas::StartInteractiveConnection(wxClassInfo* shapeInfo, const wxPoint& pos,  wxSF::ERRCODE *err)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;
	
	if(!m_pManager)
	{
		if( err ) *err = wxSF::errINVALID_INPUT;
		return;
	}
	else if( err ) *err = wxSF::errOK;

	wxPoint lpos = DP2LP(pos);

    if((m_nWorkingMode == modeREADY) && shapeInfo->IsKindOf(CLASSINFO(wxSFLineShape)))
    {
        wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
		if( pShapeUnder && (pShapeUnder->GetId() != -1) && pShapeUnder->IsConnectionAccepted(shapeInfo->GetClassName()) )
		{
			m_pNewLineShape = (wxSFLineShape*)m_pManager->AddShape(shapeInfo, sfDONT_SAVE_STATE);
			if(m_pNewLineShape)
			{
				m_nWorkingMode = modeCREATECONNECTION;
				m_pNewLineShape->SetLineMode(wxSFLineShape::modeUNDERCONSTRUCTION);

                 m_pNewLineShape->SetSrcShapeId(pShapeUnder->GetId());

                 // switch on the "under-construcion" mode
                 m_pNewLineShape->SetUnfinishedPoint(lpos);
				 // assign starting point of new line shapes to the nearest connection point of 
				 // connected shape if exists
				 m_pNewLineShape->SetStartingConnectionPoint( pShapeUnder->GetNearestConnectionPoint( Conv2RealPoint(lpos) ) );
			}
            else if( err ) *err = wxSF::errNOT_CREATED;
        }
		else if( err ) *err = wxSF::errNOT_ACCEPTED;
    }
	else if( err ) *err = wxSF::errINVALID_INPUT;
}

void wxSFShapeCanvas::StartInteractiveConnection(wxSFLineShape* shape, const wxPoint& pos, wxSF::ERRCODE *err)
{
	wxASSERT(m_pManager);
	wxASSERT(shape);
	
	if(!m_pManager || !shape)
	{
		if( err ) *err = wxSF::errINVALID_INPUT;
		return;
	}
	else if( err ) *err = wxSF::errOK;

	wxPoint lpos = DP2LP(pos);

    if((m_nWorkingMode == modeREADY) && shape->IsKindOf(CLASSINFO(wxSFLineShape)))
    {
        wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
		if( pShapeUnder && (pShapeUnder->GetId() != -1) && pShapeUnder->IsConnectionAccepted(shape->GetClassInfo()->GetClassName()) )
		{
			if( m_pManager->Contains(shape) ) m_pNewLineShape = shape;
			else m_pNewLineShape = (wxSFLineShape*)m_pManager->AddShape( shape, NULL, wxDefaultPosition, sfINITIALIZE, sfDONT_SAVE_STATE );
			
			if(m_pNewLineShape)
			{
				m_nWorkingMode = modeCREATECONNECTION;
				m_pNewLineShape->SetLineMode(wxSFLineShape::modeUNDERCONSTRUCTION);

				m_pNewLineShape->SetSrcShapeId(pShapeUnder->GetId());

                // swith on the "under-construcion" mode
                m_pNewLineShape->SetUnfinishedPoint(lpos);
				 // assign starting point of new line shapes to the nearest connection point of 
				 // connected shape if exists
				 m_pNewLineShape->SetStartingConnectionPoint( pShapeUnder->GetNearestConnectionPoint( Conv2RealPoint(lpos) ) );

            }
			else if( err ) *err = wxSF::errNOT_CREATED;
        }
		else if( err ) *err = wxSF::errNOT_ACCEPTED;
    }
	else if( err ) *err = wxSF::errINVALID_INPUT;
}

void wxSFShapeCanvas::AbortInteractiveConnection()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    if(m_pNewLineShape)
    {
        m_pManager->RemoveShape(m_pNewLineShape);
        m_pNewLineShape = NULL;
        OnConnectionFinished(NULL);
    }
	m_nWorkingMode = modeREADY;
	Refresh(false);
}

void wxSFShapeCanvas::SaveCanvasToBMP(const wxString& file)
{
    // create memory DC a draw the canvas content into

    wxRect bmpBB = GetTotalBoundingBox();
    bmpBB.Inflate(m_Settings.m_nGridSize);

    wxBitmap outbmp(bmpBB.GetRight(), bmpBB.GetBottom());
	wxMemoryDC dc( outbmp );

    //wxSFScaledPaintDC outdc(outbmp, 1);
	wxSFScaledDC outdc((wxWindowDC*)&dc, 1);

    if(outdc.IsOk())
    {
        DrawContent(outdc, sfNOT_FROM_PAINT);
        outbmp.SaveFile(file, wxBITMAP_TYPE_BMP);
        wxMessageBox(wxString::Format(wxT("The chart has been saved to '%s'."), file.GetData()), wxT("ShapeFramework"));
    }
    else
        wxMessageBox(wxT("Could not create output bitmap."), wxT("wxShapeFramework"), wxOK | wxICON_WARNING);
}

void wxSFShapeCanvas::GetSelectedShapes(ShapeList& selection)
{
 	wxASSERT(m_pManager);
	if(!m_pManager)return;

	selection.Clear();

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
		wxSFShapeBase* pShape = node->GetData();
		if(pShape->IsSelected())selection.Append(pShape);
		node = node->GetNext();
	}
}

wxSFShapeBase* wxSFShapeCanvas::GetShapeUnderCursor(SEARCHMODE mode)
{
	switch( mode )
	{
	case searchBOTH:
		return m_pTopmostShapeUnderCursor;

	case searchSELECTED:
		return m_pSelectedShapeUnderCursor;

	case searchUNSELECTED:
		return m_pUnselectedShapeUnderCursor;

	default:
		return NULL;
	}
}

wxSFShapeHandle* wxSFShapeCanvas::GetTopmostHandleAtPosition(const wxPoint& pos)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return NULL;

    wxSFShapeHandle* pHandle;

	// first test multiedit handles...
	if(m_shpMultiEdit.IsVisible())
	{
		HandleList::compatibility_iterator hnode = m_shpMultiEdit.GetHandles().GetFirst();
		while(hnode)
		{
			pHandle = hnode->GetData();
			if(pHandle->IsVisible() && pHandle->Contains(pos))return pHandle;
			hnode = hnode->GetNext();
		}
	}

	// ... then test normal handles
    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	HandleList::compatibility_iterator hnode;
	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
		// iterate through all shape's handles
		if(node->GetData()->ContainsStyle(wxSFShapeBase::sfsSIZE_CHANGE))
		{
            hnode = node->GetData()->GetHandles().GetFirst();
            while(hnode)
            {
                pHandle = hnode->GetData();
                if(pHandle->IsVisible() && pHandle->Contains(pos))return pHandle;
                hnode = hnode->GetNext();
            }
		}
		node = node->GetNext();
	}

	return NULL;
}

wxSFShapeBase* wxSFShapeCanvas::GetShapeAtPosition(const wxPoint& pos, int zorder, SEARCHMODE mode)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return NULL;

    return m_pManager->GetShapeAtPosition(pos, zorder, (wxSFDiagramManager::SEARCHMODE)mode);
}

void wxSFShapeCanvas::GetShapesAtPosition(const wxPoint& pos, ShapeList& shapes)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	m_pManager->GetShapesAtPosition( pos, shapes );
}

void wxSFShapeCanvas::GetShapesInside(const wxRect& rct, ShapeList& shapes)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	m_pManager->GetShapesInside( rct, shapes );
}

void wxSFShapeCanvas::DeselectAll()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
		node->GetData()->Select(false);
		node = node->GetNext();
	}

    m_shpMultiEdit.Show(false);
}

void wxSFShapeCanvas::SelectAll()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	if( !shapes.IsEmpty() )
	{
		ShapeList::compatibility_iterator node = shapes.GetFirst();
		while(node)
		{
			node->GetData()->Select(true);
			node = node->GetNext();
		}

		ShapeList lstSelection;
		GetSelectedShapes(lstSelection);
		ValidateSelection(lstSelection);

		HideAllHandles();
		UpdateMultieditSize();
		m_shpMultiEdit.Show(true);
		m_shpMultiEdit.ShowHandles(true);

		Refresh(false);
	}
}

void wxSFShapeCanvas::HideAllHandles()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
		node->GetData()->ShowHandles(false);
		node = node->GetNext();
	}
}

void wxSFShapeCanvas::ShowShadows(bool show, SHADOWMODE style)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    wxSFShapeBase *pShape;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
	    pShape = node->GetData();

        if( show )pShape->RemoveStyle(wxSFShapeBase::sfsSHOW_SHADOW);

	    switch(style)
	    {
	        case shadowTOPMOST:
                if( !pShape->GetParentShape() )
                {
                    if( show )pShape->AddStyle(wxSFShapeBase::sfsSHOW_SHADOW);
                    else
                        pShape->RemoveStyle(wxSFShapeBase::sfsSHOW_SHADOW);
                }
                break;

            case shadowALL:
                if( show )pShape->AddStyle(wxSFShapeBase::sfsSHOW_SHADOW);
                else
                    pShape->RemoveStyle(wxSFShapeBase::sfsSHOW_SHADOW);
                break;
	    }

		node = node->GetNext();
	}
}

void wxSFShapeCanvas::ValidateSelection(ShapeList& selection)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	ShapeList lstShapesToRemove;

	wxSFShapeBase *pShape;

	// find child shapes that have parents in the list
	ShapeList::compatibility_iterator node = selection.GetFirst();
	while(node)
	{
		pShape = node->GetData();
		if( selection.IndexOf(pShape->GetParentShape()) != wxNOT_FOUND )
		{
			lstShapesToRemove.Append(pShape);
		}

		node = node->GetNext();
	}

	// remove child shapes with parents from the list
	node = lstShapesToRemove.GetFirst();
	while(node)
	{
		pShape = node->GetData();

		pShape->Select(false);
		//pShape->ShowHandles(false);
		selection.DeleteObject(pShape);

		node = node->GetNext();
	}

	node = selection.GetFirst();
	while(node)
	{
		pShape = node->GetData();

		// move selected shapes to the back of the global list
        ((xsSerializable*)pShape->GetParent())->GetChildrenList().DeleteObject(pShape);
        ((xsSerializable*)pShape->GetParent())->GetChildrenList().Append(pShape);

		node = node->GetNext();
	}
}

void wxSFShapeCanvas::ValidateSelectionForClipboard(ShapeList& selection)
{
    // remove topmost shapes without sfsPARENT_CHANGE style from the selection

    wxSFShapeBase* pShape;

    ShapeList::compatibility_iterator node = selection.GetFirst();
    while(node)
    {
        pShape = node->GetData();

        if(pShape->GetParentShape()
            && !pShape->ContainsStyle(wxSFShapeBase::sfsPARENT_CHANGE)
            && (selection.IndexOf(pShape->GetParentShape()) == wxNOT_FOUND))
        {
            selection.DeleteObject(pShape);
            node = selection.GetFirst();
        }
        else
		{
			AppendAssignedConnections( pShape, selection, false );
            node = node->GetNext();
		}
    }
}

void wxSFShapeCanvas::AppendAssignedConnections(wxSFShapeBase *shape, ShapeList& selection, bool childrenonly)
{
	// add connections assigned to copied topmost shapes and their children to the copy list

	ShapeList lstConnections;
	ShapeList lstChildren;

	ShapeList::compatibility_iterator lnode, cnode;

	shape->GetChildShapes(sfANY, lstChildren, sfRECURSIVE);

	// get connections assigned to the parent shape
	if( !childrenonly )m_pManager->GetAssignedConnections(shape, CLASSINFO(wxSFLineShape), wxSFShapeBase::lineBOTH, lstConnections);
	// get connections assigned to its child shape
	cnode = lstChildren.GetFirst();
	while(cnode)
	{
		// get connections assigned to the child shape
		m_pManager->GetAssignedConnections(cnode->GetData(), CLASSINFO(wxSFLineShape), wxSFShapeBase::lineBOTH, lstConnections);
		cnode = cnode->GetNext();
	}

	// insert connections to the copy list
	lnode = lstConnections.GetFirst();
	while(lnode)
	{
		if( selection.IndexOf( lnode->GetData() ) == wxNOT_FOUND ) selection.Append( lnode->GetData() );

		lnode = lnode->GetNext();
	}
}

void wxSFShapeCanvas::ReparentShape(wxSFShapeBase *shape, const wxPoint& parentpos)
{
    // is shape dropped into accepting shape?
    wxSFShapeBase *pParentShape = GetShapeAtPosition( parentpos, 1, searchUNSELECTED );
    //if( !pParentShape || !pShape->IsInside( pParentShape->GetBoundingBox()) )pParentShape = GetShapeAtPosition(Conv2Point(pShape->GetAbsolutePosition()), 1, searchUNSELECTED);

    if(pParentShape && !pParentShape->IsChildAccepted( shape->GetClassInfo()->GetClassName() ))pParentShape = NULL;

    // set new parent
    if((shape->ContainsStyle(wxSFShapeBase::sfsPARENT_CHANGE)) && !shape->IsKindOf(CLASSINFO(wxSFLineShape)))
    {
        wxSFShapeBase *pPrevParent = shape->GetParentShape();

        if(pParentShape)
        {
            if( pParentShape->GetParentShape() != shape )
            {
                wxRealPoint apos = shape->GetAbsolutePosition() - pParentShape->GetAbsolutePosition();
                shape->SetRelativePosition(apos);

                shape->Reparent(pParentShape);

                // notify the parent shape about dropped child
                pParentShape->OnChildDropped(apos, shape);
            }
        }
        else
        {
            if(shape->GetParentShape())
            {
                shape->MoveBy(((wxSFShapeBase*)shape->GetParentShape())->GetAbsolutePosition());
            }
            shape->Reparent(m_pManager->GetRootItem());
        }

        if( pPrevParent ) pPrevParent->Update();
        if( pParentShape ) pParentShape->Update();
		if( shape->IsKindOf( CLASSINFO(wxSFControlShape)) ) shape->Update();
    }
}

void wxSFShapeCanvas::RemoveFromTemporaries(wxSFShapeBase* shape)
{
	if( shape )
	{
		m_lstCurrentShapes.DeleteObject( shape );

		if( m_pNewLineShape == shape ) m_pNewLineShape = NULL;

		if( m_pUnselectedShapeUnderCursor == shape ) m_pUnselectedShapeUnderCursor = NULL;

		if( m_pSelectedShapeUnderCursor == shape ) m_pSelectedShapeUnderCursor = NULL;

		if( m_pTopmostShapeUnderCursor == shape ) m_pTopmostShapeUnderCursor = NULL;
	}
}

void wxSFShapeCanvas::ClearTemporaries()
{
	m_lstCurrentShapes.Clear();

	m_pNewLineShape = NULL;
	m_pUnselectedShapeUnderCursor = NULL;
	m_pSelectedShapeUnderCursor = NULL;
	m_pTopmostShapeUnderCursor = NULL;
}

void wxSFShapeCanvas::UpdateMultieditSize()
{
	// calculate bounding box
	wxRect unionRct;
	//bool firstRun = true;
	ShapeList m_lstSelection;
	GetSelectedShapes(m_lstSelection);
	ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
	while(node)
	{
		if(node == m_lstSelection.GetFirst())
		{
			unionRct = node->GetData()->GetBoundingBox();
			//firstRun = false;
		}
		else
			unionRct.Union(node->GetData()->GetBoundingBox());

        node = node->GetNext();
	}
	unionRct.Inflate(sfDEFAULT_ME_OFFSET, sfDEFAULT_ME_OFFSET);

	// draw rectangle
	m_shpMultiEdit.SetRelativePosition(wxRealPoint(unionRct.GetPosition().x, unionRct.GetPosition().y));
	m_shpMultiEdit.SetRectSize(wxRealPoint(unionRct.GetSize().x, unionRct.GetSize().y));
}

void wxSFShapeCanvas::SetHoverColour(const wxColour& col)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	m_Settings.m_nCommonHoverColor = col;

	// update Hover color in all existing shapes
    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
		node->GetData()->SetHoverColour(col);
		node = node->GetNext();
	}
}

wxRect wxSFShapeCanvas::GetTotalBoundingBox() const
{
    wxRect virtRct;

	wxASSERT(m_pManager);

	if(m_pManager)
	{
        // calculate total bounding box (includes all shapes)
        ShapeList shapes;
        m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

        ShapeList::compatibility_iterator node = shapes.GetFirst();
        while(node)
        {
            if(node == shapes.GetFirst())
            {
                virtRct = node->GetData()->GetBoundingBox();
            }
            else
                virtRct.Union(node->GetData()->GetBoundingBox());

            node = node->GetNext();
        }
	}

    return virtRct;
}

wxRect wxSFShapeCanvas::GetSelectionBB()
{
    wxRect bbRct;

    // get selected shapes
    ShapeList lstSelection;
    GetSelectedShapes(lstSelection);

    ShapeList::compatibility_iterator node = lstSelection.GetFirst();
    while(node)
    {
        node->GetData()->GetCompleteBoundingBox(bbRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN | wxSFShapeBase::bbCONNECTIONS | wxSFShapeBase::bbSHADOW);
        node = node->GetNext();
    }

    return bbRct;
}

void wxSFShapeCanvas::UpdateVirtualSize()
{
    wxRect virtRct = GetTotalBoundingBox();
	
	// allow user to modify calculated virtual canvas size
	this->OnUpdateVirtualSize( virtRct );

    // update virtual area of the scrolled window if neccessary
    if(!virtRct.IsEmpty())
    {
		SetVirtualSize(int(virtRct.GetRight()*m_Settings.m_nScale), int(virtRct.GetBottom()*m_Settings.m_nScale));
    }
    else
        SetVirtualSize(500, 500);
}

void wxSFShapeCanvas::DeleteAllTextCtrls()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	wxSFContentCtrl* pTextCtrl = NULL;
	ShapeList lstShapes;
	m_pManager->GetShapes(CLASSINFO(wxSFEditTextShape), lstShapes);

	ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	while(node)
	{
		pTextCtrl = ((wxSFEditTextShape*)node->GetData())->GetTextCtrl();
		if(pTextCtrl)pTextCtrl->Quit( sfAPPLY_TEXT_CHANGES );
		node = node->GetNext();
	}
}

void wxSFShapeCanvas::MoveShapesFromNegatives()
{
	wxASSERT(m_pManager);
	if( m_pManager ) m_pManager->MoveShapesFromNegatives();
}

void wxSFShapeCanvas::CenterShapes()
{
	wxRect rctBB = GetTotalBoundingBox();
	wxRect rctPrevBB = rctBB;
	
	rctBB = rctBB.CenterIn( wxRect( wxPoint(0, 0), GetSize() ) );
	
	double nDx = rctBB.GetLeft() - rctPrevBB.GetLeft();
	double nDy = rctBB.GetTop() - rctPrevBB.GetTop();
	
	for( ShapeList::iterator it = m_lstCurrentShapes.begin(); it != m_lstCurrentShapes.end(); ++it )
	{
		wxSFShapeBase *pShape = *it;
		if( ! pShape->GetParentShape() ) pShape->MoveBy( nDx, nDy );
	}
	
	MoveShapesFromNegatives();
}

void wxSFShapeCanvas::AlignSelected(HALIGN halign, VALIGN valign)
{
    int nCnt = 0;

    wxRealPoint min_pos, max_pos, pos;
    wxRect shapeBB, updRct;
    wxSFShapeBase *pShape, *pParent;

    ShapeList lstSelection;
    GetSelectedShapes(lstSelection);

    updRct = GetSelectionBB();
    updRct.Inflate(sfDEFAULT_ME_OFFSET, sfDEFAULT_ME_OFFSET);

    // find most distant position
    ShapeList::compatibility_iterator node = lstSelection.GetFirst();
    while(node)
    {
        pShape = node->GetData();

        if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
        {
            pos = pShape->GetAbsolutePosition();
            shapeBB = pShape->GetBoundingBox();

            if( nCnt == 0 )
            {
                min_pos = pos;
                max_pos.x = pos.x + shapeBB.GetWidth();
                max_pos.y = pos.y + shapeBB.GetHeight();
            }
            else
            {
                if( pos.x < min_pos.x )min_pos.x = pos.x;
                if( pos.y < min_pos.y )min_pos.y = pos.y;
                if( (pos.x + shapeBB.GetWidth()) > max_pos.x )max_pos.x = pos.x + shapeBB.GetWidth();
                if( (pos.y + shapeBB.GetHeight()) > max_pos.y )max_pos.y = pos.y + shapeBB.GetHeight();
            }

            nCnt++;
        }

        node = node->GetNext();
    }

    // if only one non-line shape is in the selection then alignment has no sense so exit...
    if(nCnt < 2) return;

    // set new positions
    node = lstSelection.GetFirst();
    while(node)
    {
        pShape = (wxSFShapeBase*)node->GetData();

        if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
        {
            pos = pShape->GetAbsolutePosition();
            shapeBB = pShape->GetBoundingBox();

            switch(halign)
            {
                case halignLEFT:
                    pShape->MoveTo(min_pos.x, pos.y);
                    break;

                case halignRIGHT:
                    pShape->MoveTo(max_pos.x - shapeBB.GetWidth(), pos.y);
                    break;

                case halignCENTER:
                    pShape->MoveTo((max_pos.x + min_pos.x)/2 - shapeBB.GetWidth()/2, pos.y);
                    break;

                default:
                    break;
            }

            switch(valign)
            {
                case valignTOP:
                    pShape->MoveTo(pos.x, min_pos.y);
                    break;

                case valignBOTTOM:
                    pShape->MoveTo(pos.x, max_pos.y - shapeBB.GetHeight());
                    break;

                case valignMIDDLE:
                    pShape->MoveTo(pos.x, (max_pos.y + min_pos.y)/2 - shapeBB.GetHeight()/2);
                    break;

                default:
                    break;
            }

            // update the shape and its parent
            pShape->Update();
            pParent = (wxSFShapeBase*)pShape->GetParentShape();
            if(pParent)
            {
                pParent->Update();
            }
        }

        node = node->GetNext();
    }

    if(!updRct.IsEmpty())
    {
        UpdateMultieditSize();
        SaveCanvasState();
        RefreshCanvas(false, updRct);
    }
}

//----------------------------------------------------------------------------------//
// Clipboard and D&D functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::Copy()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return;

	wxASSERT(m_pManager);
	if(!m_pManager)return;

	// copy selected shapes to the clipboard

	//if( wxTheClipboard->Open() )
	if( wxTheClipboard->IsOpened() || ( !wxTheClipboard->IsOpened() && wxTheClipboard->Open()) )
	{
		ShapeList lstSelection;
		GetSelectedShapes(lstSelection);

        ValidateSelectionForClipboard(lstSelection);

        if( !lstSelection.IsEmpty() )
        {
            wxSFShapeDataObject* dataObj = new wxSFShapeDataObject(m_formatShapes, lstSelection, m_pManager);
            wxTheClipboard->SetData(dataObj);
        }

		if( wxTheClipboard->IsOpened() )wxTheClipboard->Close();
	}
}

void wxSFShapeCanvas::Cut()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return;

	wxASSERT(m_pManager);
	if(!m_pManager)return;

	Copy();
	
	ClearTemporaries();

	// remove selected shapes
	ShapeList lstSelection;
	GetSelectedShapes(lstSelection);

    ValidateSelectionForClipboard(lstSelection);

    if( !lstSelection.IsEmpty() )
    {
        m_pManager->RemoveShapes(lstSelection);
        m_shpMultiEdit.Show(false);
        SaveCanvasState();
        Refresh(false);
    }
}

void wxSFShapeCanvas::Paste()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return;

	wxASSERT(m_pManager);
	if(!m_pManager)return;

    if( wxTheClipboard->IsOpened() || ( !wxTheClipboard->IsOpened() && wxTheClipboard->Open()) )
	{
		// store previous canvas content
		ShapeList lstOldContent;
		m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), lstOldContent);

		// read data object from the clipboars
		wxSFShapeDataObject dataObj(m_formatShapes);
		if(wxTheClipboard->GetData(dataObj))
		{
/*			wxMemoryBuffer dataBuffer = CreateMembufferFromString(dataObj.m_Data.GetText());
			wxMemoryInputStream instream(dataBuffer.GetData(), dataBuffer.GetDataLen()-1);*/

			wxStringInputStream instream( dataObj.m_Data.GetText() );

			if(instream.IsOk())
			{
				// deserialize XML data
				m_pManager->DeserializeFromXml(instream);

				// find dropped shapes
				ShapeList lstNewContent;
				ShapeList lstCurrContent;

				m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), lstCurrContent);
				ShapeList::compatibility_iterator node = lstCurrContent.GetFirst();
				while( node )
				{
					wxSFShapeBase *pShape = node->GetData();
					if( lstOldContent.IndexOf(pShape) == wxNOT_FOUND  ) lstNewContent.Append(pShape);

					node = node->GetNext();
				}

				// call user-defined handler
				this->OnPaste(lstNewContent);

				SaveCanvasState();
				Refresh();
			}
		}
		if( wxTheClipboard->IsOpened() )wxTheClipboard->Close();
	}
}

void wxSFShapeCanvas::OnPaste(const ShapeList& pasted)
{
    // HINT: override it for custom actions...

	// ... standard implementation generates the wxEVT_SF_ON_PASTE event.

	if( !ContainsStyle(sfsCLIPBOARD) )return;

	// create the drop event and process it
    wxSFShapePasteEvent event( wxEVT_SF_ON_PASTE, this, wxID_ANY);
    event.SetPastedShapes( pasted );
    ProcessEvent( event );
}

void wxSFShapeCanvas::Undo()
{
	if( !ContainsStyle(sfsUNDOREDO) )return;
	
	ClearTemporaries();

	m_CanvasHistory.RestoreOlderState();
	m_shpMultiEdit.Show(false);
}

void wxSFShapeCanvas::Redo()
{
	if( !ContainsStyle(sfsUNDOREDO) )return;
	
	ClearTemporaries();

	m_CanvasHistory.RestoreNewerState();
	m_shpMultiEdit.Show(false);
}

wxDragResult wxSFShapeCanvas::DoDragDrop(ShapeList &shapes, const wxPoint& start)
{
	if( !ContainsStyle(sfsDND) )return wxDragNone;

	m_nWorkingMode = modeDND;

	wxDragResult result = wxDragNone;

	ValidateSelectionForClipboard(shapes);

	if( !shapes.IsEmpty() )
	{
		DeselectAll();

		m_fDnDStartedHere = true;
		m_nDnDStartedAt = start;

		wxSFShapeDataObject dataObj(m_formatShapes, shapes, m_pManager);

		#ifdef __WXGTK__ 
		wxDropSource dndSrc(this, wxIcon(page_xpm), wxIcon(page_xpm), wxIcon(page_xpm)); 
		dndSrc.SetData(dataObj); 
		#elif __WXMAC__ 
		wxDropSource dndSrc(dataObj, this, wxDROP_ICON(page), wxDROP_ICON(page), wxDROP_ICON(page)); 
		#else 
		wxDropSource dndSrc(dataObj); 
		#endif 

		result = dndSrc.DoDragDrop(wxDrag_AllowMove);
		switch(result)
		{
		case wxDragMove:
			m_pManager->RemoveShapes(shapes);
			break;

		default:
			break;
		}

		m_fDnDStartedHere = false;

		MoveShapesFromNegatives();
		UpdateVirtualSize();

		SaveCanvasState();
		Refresh(false);
	}

	m_nWorkingMode = modeREADY;

	return result;
}

void wxSFShapeCanvas::_OnDrop(wxCoord x, wxCoord y, wxDragResult def, wxDataObject *data)
{
	if( data )
	{
/*		wxMemoryBuffer dataBuffer = CreateMembufferFromString(((wxSFShapeDataObject*)data)->m_Data.GetText());
		wxMemoryInputStream instream(dataBuffer.GetData(), dataBuffer.GetDataLen()-1);*/

		wxStringInputStream instream( ((wxSFShapeDataObject*)data)->m_Data.GetText() );

		if(instream.IsOk())
		{
			// store previous canvas content
			wxSFShapeBase *pShape;

			ShapeList lstNewContent;
			ShapeList lstCurrContent;
			ShapeList lstOldContent;

			m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), lstOldContent);

			// deserialize XML data
			m_pManager->DeserializeFromXml(instream);

			// find dropped shapes
			m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), lstCurrContent);
			ShapeList::compatibility_iterator node = lstCurrContent.GetFirst();
			while(node)
			{
				pShape = node->GetData();
				if( lstOldContent.IndexOf(pShape) == wxNOT_FOUND )
				{
				    pShape->Select( true );
				    lstNewContent.Append(pShape);
				}

				node = node->GetNext();
			}

            wxPoint lpos = DP2LP(wxPoint(x, y));

            int dx = 0, dy = 0;
            if( m_fDnDStartedHere )
            {
				dx = lpos.x - m_nDnDStartedAt.x;
                dy = lpos.y - m_nDnDStartedAt.y;
            }

            // move dropped shapes to the updated positions and reparent them if neccessary
            node = lstNewContent.GetFirst();
            while(node)
            {
                pShape = node->GetData();
                if( !pShape->GetParentShape() )
                {
					pShape->MoveBy(dx, dy);
					ReparentShape( pShape, lpos );
				}

                node = node->GetNext();
            }

            DeselectAll();

			if( !m_fDnDStartedHere )
			{
				SaveCanvasState();
				Refresh(false);
			}

			// call user-defined drop handler
			this->OnDrop(x, y, def, lstNewContent);
		}
	}
}

void wxSFShapeCanvas::OnDrop(wxCoord x, wxCoord y, wxDragResult def, const ShapeList& dropped)
{
    // HINT: override it for custom actions...

	// ... standard implementation generates the wxEVT_SF_ON_DROP event.

	if( !ContainsStyle(sfsDND) )return;

	// create the drop event and process it
    wxSFShapeDropEvent event( wxEVT_SF_ON_DROP, x, y, this, def, wxID_ANY);
    event.SetDroppedShapes( dropped );
    ProcessEvent( event );
}

void wxSFShapeCanvas::SaveCanvasState()
{
	if( !ContainsStyle(sfsUNDOREDO) )return;

	m_CanvasHistory.SaveCanvasState();
}

bool wxSFShapeCanvas::CanCopy()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return false;

	ShapeList lstSelection;
	GetSelectedShapes(lstSelection);
	return !lstSelection.IsEmpty();
}

bool wxSFShapeCanvas::CanCut()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return false;

	return CanCopy();
}

bool wxSFShapeCanvas::CanPaste()
{
	if( !ContainsStyle(sfsCLIPBOARD) )return false;

	bool result = false;

    //if( wxTheClipboard->Open() )
	if( wxTheClipboard->IsOpened() || ( !wxTheClipboard->IsOpened() && wxTheClipboard->Open()) )
	{
		result = wxTheClipboard->IsSupported(m_formatShapes);
		if( wxTheClipboard->IsOpened() )wxTheClipboard->Close();
	}

	return result;
}

bool wxSFShapeCanvas::CanUndo()
{
	if( !ContainsStyle(sfsUNDOREDO) )return false;

	return m_CanvasHistory.CanUndo();
}

bool wxSFShapeCanvas::CanRedo()
{
	if( !ContainsStyle(sfsUNDOREDO) )return false;

	return m_CanvasHistory.CanRedo();
}

bool wxSFShapeCanvas::CanAlignSelected()
{
    return ( m_shpMultiEdit.IsVisible() & (m_nWorkingMode == modeREADY) );
}

void wxSFShapeCanvas::ClearCanvasHistory()
{
	m_CanvasHistory.Clear();
}

/*wxMemoryBuffer wxSFShapeCanvas::CreateMembufferFromString(const wxString& str)
{
	// create memory stream with copied data
	wxMemoryBuffer dataBuffer;
	wxMemoryOutputStream outstream;
	wxTextOutputStream tout(outstream);

	tout.WriteString(str);

	wxStreamBuffer* strbuff = outstream.GetOutputStreamBuffer();
	strbuff->ResetBuffer();
	dataBuffer.AppendData(strbuff->GetBufferStart(), strbuff->GetDataLeft());
	dataBuffer.AppendByte(0);

	return dataBuffer;
}*/

//----------------------------------------------------------------------------------//
// printing functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::InitializePrinting()
{
    g_printData = new wxPrintData;
    // You could set an initial paper size here
//    g_printData->SetPaperId(wxPAPER_LETTER); // for Americans
    g_printData->SetPaperId(wxPAPER_A4);    // for everyone else

    g_pageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*g_pageSetupData) = *g_printData;
    // Set some initial page margins in mm.
    g_pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
    g_pageSetupData->SetMarginBottomRight(wxPoint(15, 15));
}

void wxSFShapeCanvas::DeinitializePrinting()
{
    delete g_printData;
    delete g_pageSetupData;
}

void wxSFShapeCanvas::Print(bool prompt)
{
    Print(new wxSFPrintout(wxT("wxSF Printout"), this), prompt);
}

void wxSFShapeCanvas::Print(wxSFPrintout *printout, bool prompt)
{
    wxASSERT(printout);

    wxPrintDialogData printDialogData(* g_printData);
    wxPrinter printer(& printDialogData);

	DeselectAll();

    if (!printer.Print(this, printout, prompt))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(wxT("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("wxSF Printing"), wxOK | wxICON_ERROR);
        /*else
            wxMessageBox(wxT("You canceled printing"), _T("wxSF Printing"), wxOK | wxICON_WARNING);*/
    }
    else
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();

	delete printout;
}

void wxSFShapeCanvas::PrintPreview()
{
    PrintPreview(new wxSFPrintout(wxT("wxSF Preview"), this), new wxSFPrintout(wxT("wxSF Printout"), this));
}

void wxSFShapeCanvas::PrintPreview(wxSFPrintout *preview, wxSFPrintout *printout)
{
    wxASSERT(preview);

	DeselectAll();

    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *prnPreview = new wxPrintPreview(preview, printout, &printDialogData);
    if (!prnPreview->Ok())
    {
        delete prnPreview;
        wxMessageBox(wxT("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("wxSF Previewing"), wxOK | wxICON_ERROR);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(prnPreview, this, wxT("wxSF Print Preview"), wxPoint(100, 100), wxSize(800, 700));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}

void wxSFShapeCanvas::PageSetup()
{
    (*g_pageSetupData) = *g_printData;

    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();

    (*g_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

/*#ifdef __WXMAC__
void wxSFShapeCanvas::PageMargins()
{
    (*g_pageSetupData) = *g_printData;

    wxMacPageMarginsDialog pageMarginsDialog(this, g_pageSetupData);
    pageMarginsDialog.ShowModal();

    (*g_printData) = pageMarginsDialog.GetPageSetupDialogData().GetPrintData();
    (*g_pageSetupData) = pageMarginsDialog.GetPageSetupDialogData();
}
#endif 
*/
#ifdef __WXMAC__ 
void wxSFShapeCanvas::PageMargins() 
{ 
	(*g_pageSetupData) = *g_printData; 

	wxWindow * win = this; 
	while (!win->IsTopLevel())
	{ 
		win = win->GetParent(); 
		if (!win) break; 
	} 
 
	wxMacPageMarginsDialog pageMarginsDialog((win)?(wxFrame*)win : NULL, g_pageSetupData); 
	pageMarginsDialog.ShowModal(); 
 
	(*g_printData) = pageMarginsDialog.GetPageSetupDialogData().GetPrintData(); 
	(*g_pageSetupData) = pageMarginsDialog.GetPageSetupDialogData(); 
} 
#endif

//----------------------------------------------------------------------------------//
// wxSFCanvasDropTarget class
//----------------------------------------------------------------------------------//

wxSFCanvasDropTarget::wxSFCanvasDropTarget(wxDataObject *data, wxSFShapeCanvas *parent)
: wxDropTarget(data)
{
	m_pParentCanvas = parent;
}

wxSFCanvasDropTarget::~wxSFCanvasDropTarget()
{
}

wxDragResult wxSFCanvasDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
	if(!GetData())
	{
		return wxDragNone;
	}

	m_pParentCanvas->_OnDrop(x, y, def, GetDataObject());

	return def;
}
