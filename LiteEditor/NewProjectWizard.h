#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H

#include "wxcrafter.h"
#include <list>
#include <map>
#include "project.h"

class NewProjectWizard : public NewProjectWizardBase
{
    std::list<ProjectPtr>   m_list;
    std::map<wxString, int> m_mapImages;
    ProjectData             m_projectData;

public:
    NewProjectWizard(wxWindow* parent);
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
