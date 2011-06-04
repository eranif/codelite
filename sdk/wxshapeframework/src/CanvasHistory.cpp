/***************************************************************
 * Name:      CanvasHistory.cpp
 * Purpose:   Implements manager for stored canvas states
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

#include <wx/mstream.h>

#include "wx/wxsf/CanvasHistory.h"
#include "wx/wxsf/ShapeCanvas.h"

wxSFCanvasHistory::wxSFCanvasHistory(MODE hmode)
{
	m_nWorkingMode = hmode;

	m_pParentCanvas = NULL;
    m_pCurrentCanvasState = NULL;
	m_nHistoryDepth = sfDEFAULT_MAX_CANVAS_STATES;

	m_lstCanvasStates.DeleteContents(true);
}

wxSFCanvasHistory::wxSFCanvasHistory(wxSFShapeCanvas *canvas, MODE hmode)
{
	wxASSERT(canvas);

	m_nWorkingMode = hmode;

	m_pParentCanvas = canvas;
	m_pCurrentCanvasState = NULL;
	m_nHistoryDepth = sfDEFAULT_MAX_CANVAS_STATES;

	m_lstCanvasStates.DeleteContents(true);
}

wxSFCanvasHistory::~wxSFCanvasHistory(void)
{
	Clear();
}

//----------------------------------------------------------------------------------//
// Public functions
//----------------------------------------------------------------------------------//

void wxSFCanvasHistory::SetMode(MODE hmode)
{
	Clear();

	m_nWorkingMode = hmode;
}

void wxSFCanvasHistory::Clear()
{
	m_lstCanvasStates.Clear();
	m_pCurrentCanvasState = NULL;
}

void wxSFCanvasHistory::SaveCanvasState()
{
    wxASSERT(m_pParentCanvas);
    wxASSERT(m_pParentCanvas->GetDiagramManager());

	if( m_nWorkingMode == histUSE_CLONING )
	{
		if(m_pParentCanvas && m_pParentCanvas->GetDiagramManager())
		{
			// create temporal copy of current diagram manager
			wxSFDiagramManager *pDataManager = (wxSFDiagramManager*)m_pParentCanvas->GetDiagramManager()->Clone();
			if( !pDataManager ) return;

			// delete all states newer than the current state
			if( m_pCurrentCanvasState )
			{
				StateList::compatibility_iterator delnode = m_lstCanvasStates.GetLast();
				while(delnode->GetData() != m_pCurrentCanvasState)
				{
					m_lstCanvasStates.DeleteNode(delnode);
					delnode = m_lstCanvasStates.GetLast();
				}
			}

			// create and append new canvas state
			m_pCurrentCanvasState = new wxSFCanvasState(pDataManager);
			m_lstCanvasStates.Append(m_pCurrentCanvasState);

			// check the history bounds
			if(m_lstCanvasStates.GetCount() > m_nHistoryDepth)
			{
				m_lstCanvasStates.DeleteNode(m_lstCanvasStates.GetFirst());
			}
		}
	}
	else if( m_nWorkingMode == histUSE_SERIALIZATION )
	{
		wxMemoryOutputStream outstream;

		if(outstream.IsOk() && m_pParentCanvas && m_pParentCanvas->GetDiagramManager())
		{
			// serialize canvas to memory stream
			m_pParentCanvas->GetDiagramManager()->SerializeToXml(outstream);

			// delete all states newer than the current state
			if( m_pCurrentCanvasState )
			{
				StateList::compatibility_iterator delnode = m_lstCanvasStates.GetLast();
				while(delnode->GetData() != m_pCurrentCanvasState)
				{
					m_lstCanvasStates.DeleteNode(delnode);
					delnode = m_lstCanvasStates.GetLast();
				}
			}

			// create and append new canvas state
			m_pCurrentCanvasState = new wxSFCanvasState(outstream.GetOutputStreamBuffer());
			m_lstCanvasStates.Append(m_pCurrentCanvasState);

			// check the history bounds
			if(m_lstCanvasStates.GetCount() > m_nHistoryDepth)
			{
				m_lstCanvasStates.DeleteNode(m_lstCanvasStates.GetFirst());
			}
		}
	}
}

void wxSFCanvasHistory::RestoreOlderState()
{
	if( !m_pCurrentCanvasState ) return;

	// move to previous canvas state and restore it if exists
	StateList::compatibility_iterator node = m_lstCanvasStates.Find(m_pCurrentCanvasState)->GetPrevious();
	if( node ) m_pCurrentCanvasState = node->GetData();
	else
        m_pCurrentCanvasState = NULL;

	if( m_pCurrentCanvasState )
	{
		m_pCurrentCanvasState->Restore(m_pParentCanvas);
	}
}

void wxSFCanvasHistory::RestoreNewerState()
{
    if( !m_pCurrentCanvasState ) return;

	// move to next canvas state and restore it if exists
	StateList::compatibility_iterator node = m_lstCanvasStates.Find(m_pCurrentCanvasState)->GetNext();
	if( node ) m_pCurrentCanvasState = node->GetData();
	else
        m_pCurrentCanvasState = NULL;

	if( m_pCurrentCanvasState )
	{
		m_pCurrentCanvasState->Restore(m_pParentCanvas);
	}
}

bool wxSFCanvasHistory::CanUndo()
{
	return ((m_pCurrentCanvasState != NULL) && (m_pCurrentCanvasState != m_lstCanvasStates.GetFirst()->GetData()));
}

bool wxSFCanvasHistory::CanRedo()
{
	return ((m_pCurrentCanvasState != NULL) && (m_pCurrentCanvasState != m_lstCanvasStates.GetLast()->GetData()));
}
