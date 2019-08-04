#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "NewProjectDialogBase.h"
#include <list>
#include <vector>
#include "project.h"
#include <codelite_exports.h>

struct __project_data {
    ProjectPtr project;
};

class WXDLLIMPEXP_SDK NewProjectDialog : public NewProjectDialogBase
{
protected:
    std::list<ProjectPtr> m_list;
    ProjectData m_projectData;
    std::vector<__project_data> m_projects;

public:
    NewProjectDialog(wxWindow* parent);
    virtual ~NewProjectDialog();
    ProjectData GetProjectData() const;
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWPROJECTDIALOG_H
