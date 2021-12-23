#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "NewProjectDialogBase.h"
#include "project.h"

#include <codelite_exports.h>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK NewProjectDialog : public NewProjectDialogBase
{
protected:
    virtual void OnCompilerChanged(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnNameTyped(wxCommandEvent& event);
    virtual void OnPathSelected(wxFileDirPickerEvent& event);
    std::list<ProjectPtr> m_list;
    ProjectData m_projectData;
    std::unordered_map<wxString, ProjectPtr> m_projectsMap;
    std::map<wxString, wxArrayString> m_categories;
    bool m_userTypeName = false;

protected:
    wxArrayString GetProjectsTypesForCategory(const wxString& category);
    void OnCategoryChanged(wxCommandEvent& event);

public:
    NewProjectDialog(wxWindow* parent);
    virtual ~NewProjectDialog();
    ProjectData GetProjectData() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // NEWPROJECTDIALOG_H
