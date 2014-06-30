//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : NewProjectWizard.h
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

#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H

#include "wxcrafter.h"
#include <list>
#include <map>
#include "project.h"
#include "cl_command_event.h"

class NewProjectWizard : public NewProjectWizardBase
{
    std::list<ProjectPtr>   m_list;
    std::map<wxString, int> m_mapImages;
    ProjectData             m_projectData;
    clNewProjectEvent::Template::Vec_t m_additionalTemplates;
    
public:
    NewProjectWizard(wxWindow* parent, const clNewProjectEvent::Template::Vec_t& additionalTemplates);
    virtual ~NewProjectWizard();
    
    const ProjectData& GetProjectData() const {
        return m_projectData;
    }
    
private:
    void UpdateFullFileName();
    void UpdateProjectPage();
    
    bool CheckProjectName();
    bool CheckProjectPath();
    bool CheckProjectTemplate();
    
protected:
    virtual void OnFinish(wxWizardEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
    virtual void OnBrowseProjectPath(wxCommandEvent& event);
    virtual void OnItemSelected(wxDataViewEvent& event);
    virtual void OnProjectNameChanged(wxCommandEvent& event);
    virtual void OnProjectPathUpdated(wxCommandEvent& event);
};
#endif // NEWPROJECTWIZARD_H
