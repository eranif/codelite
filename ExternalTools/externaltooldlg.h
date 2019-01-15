//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : externaltooldlg.h
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

#ifndef __externaltooldlg__
#define __externaltooldlg__

#include "external_tools.h"
#include "externaltoolsdata.h"
#include <vector>

class ExternalToolData : public wxClientData
{
public:
    wxString m_id;
    wxString m_name;
    wxString m_path;
    wxString m_workingDirectory;
    wxString m_icon16;
    wxString m_icon24;
    bool m_captureOutput;
    bool m_saveAllFiles;
    bool m_callOnFileSave;

public:
    ExternalToolData(const ToolInfo& ti)
        : m_id(ti.GetId())
        , m_name(ti.GetName())
        , m_path(ti.GetPath())
        , m_workingDirectory(ti.GetWd())
        , m_icon16(ti.GetIcon16())
        , m_icon24(ti.GetIcon24())
        , m_captureOutput(ti.GetCaptureOutput())
        , m_saveAllFiles(ti.GetSaveAllFiles())
        , m_callOnFileSave(ti.IsCallOnFileSave())
    {
    }

    ExternalToolData(const wxString& id, const wxString& name, const wxString& path, const wxString& workingDirectory,
                     const wxString& icon16, const wxString& icon24, bool captureOutput, bool saveAllFiles,
                     bool callOnFileSave)
        : m_id(id)
        , m_name(name)
        , m_path(path)
        , m_workingDirectory(workingDirectory)
        , m_icon16(icon16)
        , m_icon24(icon24)
        , m_captureOutput(captureOutput)
        , m_saveAllFiles(saveAllFiles)
        , m_callOnFileSave(callOnFileSave)
    {
    }

    virtual ~ExternalToolData() {}
};

class IManager;

/** Implementing ExternalToolBaseDlg */
class ExternalToolDlg : public ExternalToolBaseDlg
{
    IManager* m_mgr;

private:
    void DoUpdateEntry(const wxDataViewItem& item, const wxString& id, const wxString& name, const wxString& path,
                       const wxString& workingDirectory, const wxString& icon16, const wxString& icon24,
                       bool captureOutput, bool saveAllFiles, bool callOnFileSave);
    void DoEditEntry(const wxDataViewItem& item);
    ExternalToolData* GetToolData(const wxDataViewItem& item);
    void DoClear();
    void DoDeleteItem(const wxDataViewItem& item);

protected:
    // Handlers for ExternalToolBaseDlg events.
    void OnItemActivated(wxDataViewEvent& event);
    void OnButtonNew(wxCommandEvent& event);
    void OnButtonNewUI(wxUpdateUIEvent& event);
    void OnButtonEdit(wxCommandEvent& event);
    void OnButtonEditUI(wxUpdateUIEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteUI(wxUpdateUIEvent& event);

public:
    /** Constructor */
    ExternalToolDlg(wxWindow* parent, IManager* mgr);
    virtual ~ExternalToolDlg();
    std::vector<ToolInfo> GetTools();
    void SetTools(const std::vector<ToolInfo>& tools);
};

#endif // __externaltooldlg__
