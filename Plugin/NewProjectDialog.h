#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "NewProjectDialogBase.h"
#include <list>
#include <vector>
#include "project.h"
#include <codelite_exports.h>
#include <unordered_map>

class WXDLLIMPEXP_SDK NewProjectDialog : public NewProjectDialogBase
{
protected:
    std::list<ProjectPtr> m_list;
    ProjectData m_projectData;
    std::unordered_map<int, ProjectPtr> m_projectsMap;

public:
    NewProjectDialog(wxWindow* parent);
    virtual ~NewProjectDialog();
    ProjectData GetProjectData() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWPROJECTDIALOG_H
