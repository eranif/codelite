//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdPanel.h
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

#ifndef ERDPANEL_H
#define ERDPANEL_H

#include "ErdCommitWizard.h"
#include "ErdTable.h"
#include "ErdView.h"
#include "FrameCanvas.h"
#include "GUI.h" // Base class: _ErdPanel
#include "IDbAdapter.h"
#include "dbeclasstype.h"

#include <wx/textfile.h>
#include <wx/wx.h>
#include <wx/wxsf/ShapeBase.h>
#include <wx/wxsf/wxShapeFramework.h>

class FrameCanvas;

class ErdInfo : public xsSerializable
{
public:
    XS_DECLARE_CLONABLE_CLASS(ErdInfo);

    ErdInfo();
    ErdInfo(const ErdInfo& obj);
    ~ErdInfo() { ; }

    void SetAdapterType(const IDbAdapter::TYPE& type) { this->m_adapterType = type; }
    const IDbAdapter::TYPE& GetAdapterType() const { return m_adapterType; }

protected:
    IDbAdapter::TYPE m_adapterType;
};

/*! \brief Editor for ERD creating. */
class ErdPanel : public _ErdPanel
{
public:
    DECLARE_DYNAMIC_CLASS(ErdPanel)
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    // public data accessors
    MODE GetToolMode() { return m_nToolMode; }

    void SetToolMode(MODE m) { m_nToolMode = m; }

    ErdPanel();
    ErdPanel(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pConnections);
    ErdPanel(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pConnections, Table* pTable);
    ErdPanel(wxWindow* parent, IDbAdapter* dbAdapter, xsSerializable* pConnections, xsSerializable* pItems);
    virtual ~ErdPanel();
    void Init(wxWindow* parent, IDbAdapter* dbAdapter);
    bool LoadERD(const wxString& path);
    bool SaveERD(const wxString& path);

    wxSFShapeCanvas* GetCanvas() { return (wxSFShapeCanvas*)m_pFrameCanvas; }

protected:
    wxSFDiagramManager m_diagramManager;
    FrameCanvas* m_pFrameCanvas;
    ErdTable* m_pErdTable;

    IDbAdapter* m_pDbAdapter;
    xsSerializable* m_pConnections;

    MODE m_nToolMode;

    // event handlers
    void OnTool(wxCommandEvent& event);
    void OnToolUpdate(wxUpdateUIEvent& event);
    void OnLoad(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));
    void OnCommit(wxCommandEvent& WXUNUSED(event));
    void OnSaveSql(wxCommandEvent& WXUNUSED(event));
    void OnSaveImg(wxCommandEvent& WXUNUSED(event));
    void OnZoom100(wxCommandEvent& WXUNUSED(event));
    void OnZoomAll(wxCommandEvent& WXUNUSED(event));
    void OnPrint(wxCommandEvent& WXUNUSED(event));
    void OnPreview(wxCommandEvent& WXUNUSED(event));
    void OnCopy(wxCommandEvent& WXUNUSED(event));
    void OnCut(wxCommandEvent& WXUNUSED(event));
    void OnPaste(wxCommandEvent& WXUNUSED(event));
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUndo(wxCommandEvent& WXUNUSED(event));
    void OnRedo(wxCommandEvent& WXUNUSED(event));
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnAlignCircle(wxCommandEvent& WXUNUSED(event));
    void OnAlignMesh(wxCommandEvent& WXUNUSED(event));
    void OnAlignVTree(wxCommandEvent& WXUNUSED(event));
    void OnAlignHTree(wxCommandEvent& WXUNUSED(event));
    void OnPageClosing(wxBookCtrlEvent& event);
    void OnSelectAll(wxCommandEvent& evt);
    void OnCommitUpdate(wxUpdateUIEvent& event);

    DECLARE_EVENT_TABLE();
};

#endif // ERDPANEL_H
