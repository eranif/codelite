//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newprojectdlg.h
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

#ifndef __newprojectdlg__
#define __newprojectdlg__

#include "wxcrafter.h"
#include "project.h"
#include "list"
#include "map"

class NewProjectDlgData : public SerializedObject
{
    size_t m_flags;
    int    m_sashPosition;
    int    m_category;
public:
    enum {
        NpSeparateDirectory = 0x00000001
    };

public:
    NewProjectDlgData();
    virtual ~NewProjectDlgData();

    void Serialize(Archive &arch);
    void DeSerialize(Archive &arch);

    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    void SetSashPosition(int sashPosition) {
        this->m_sashPosition = sashPosition;
    }
    int GetSashPosition() const {
        return m_sashPosition;
    }
    void SetCategory(int category) {
        this->m_category = category;
    }
    int GetCategory() const {
        return m_category;
    }
};

/** Implementing NewProjectBaseDlg */
class NewProjectDlg : public NewProjectDlgBaseClass
{
    std::list<ProjectPtr> m_list;
    std::map<wxString,int> m_mapImages;
    ProjectData m_projectData;

protected:
    // Handlers for NewProjectBaseDlg events.
    void OnProjectNameChanged( wxCommandEvent& event );
    void OnCreate(wxCommandEvent &event);
    void OnTemplateSelected( wxListEvent& event );
    void OnCategorySelected( wxCommandEvent& event );

    ProjectPtr FindProject(const wxString &name);
    void FillProjectTemplateListCtrl(const wxString& category);

    void UpdateFullFileName();
    void UpdateProjectPage();

    /**
     * @brief
     * @param event
     */
    virtual void OnBrowseProjectPath( wxCommandEvent& event );

    /**
     * @brief
     * @param event
     */
    virtual void OnProjectPathUpdated( wxCommandEvent& event );

    virtual void OnOKUI(wxUpdateUIEvent& event);

public:

    /** Constructor */
    NewProjectDlg( wxWindow* parent );
    virtual ~NewProjectDlg();

    const ProjectData& GetProjectData() const {
        return m_projectData;
    }
};

#endif // __newprojectdlg__
