#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "NewProjectDialogBase.hpp"
#include "codelite_exports.h"
#include "project.h"

#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK NewProjectDialog : public NewProjectDialogBase
{
protected:
    void OnCompilerChanged(wxCommandEvent& event) override;
    void OnOK(wxCommandEvent& event) override;
    void OnNameTyped(wxCommandEvent& event) override;
    void OnPathSelected(wxFileDirPickerEvent& event) override;
    std::list<ProjectPtr> m_list;
    ProjectData m_projectData;
    std::unordered_map<wxString, ProjectPtr> m_projectsMap;
    std::map<wxString, wxArrayString> m_categories;
    bool m_userTypeName = false;

protected:
    wxArrayString GetProjectsTypesForCategory(const wxString& category);
    void OnCategoryChanged(wxCommandEvent& event) override;

public:
    NewProjectDialog(wxWindow* parent);
    ~NewProjectDialog() override;
    ProjectData GetProjectData() const;

protected:
    void OnOKUI(wxUpdateUIEvent& event) override;
};
#endif // NEWPROJECTDIALOG_H
