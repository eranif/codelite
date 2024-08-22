//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : FrameCanvas.h
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

#ifndef FRAMECANVAS_H
#define FRAMECANVAS_H

#include "ErdPanel.h"

#include <wx/clipbrd.h>
#include <wx/hashmap.h>
#include <wx/list.h>
#include <wx/wx.h>
#include <wx/wxsf/DiagramManager.h>
#include <wx/wxsf/ShapeCanvas.h> // Base class: wxSFShapeCanvas

#ifdef DBL_MYSQL
#include "../Main/MySqlDbAdapter.h"
#endif

#include "DnDTableShape.h"
#include "IDbAdapter.h"
#include "TableSettings.h"
#include "ViewSettings.h"

class ErdPanel;
/*! \brief Frame canvas. */
class FrameCanvas : public wxSFShapeCanvas {

	public:
		FrameCanvas() {
			;
		}
		FrameCanvas(wxSFDiagramManager* manager,IDbAdapter* dbAdapter, wxPanel* parentPanel, wxWindowID id);
		virtual ~FrameCanvas();
		virtual void OnConnectionFinished(wxSFLineShape* connection);
		virtual void OnKeyDown(wxKeyEvent& event);
		virtual void OnRightDown(wxMouseEvent& event);
		virtual void OnLeftDown(wxMouseEvent& event);
		virtual void OnLeftDoubleClick(wxMouseEvent& event);
		virtual PRECONNECTIONFINISHEDSTATE OnPreConnectionFinished(wxSFLineShape* connection);
		virtual void OnDrop(wxCoord x, wxCoord y, wxDragResult def, const ShapeList& dropped);

		virtual void OnPaste(const ShapeList& pasted);

		void OnPopupClick(wxCommandEvent &evt);

		wxString GetSqlScript();
		void UpdateERD();

	protected:
		ErdPanel* m_pParentPanel;
		IDbAdapter* m_pDbAdapter;
		wxSFShapeBase *m_pSelectedShape;

		wxString m_srcCol;
		wxString m_dstCol;
		wxPoint m_mousePos;

};

#endif // FRAMECANVAS_H
