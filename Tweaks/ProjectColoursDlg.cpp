#include "ProjectColoursDlg.h"
#include "tweaks_settings.h"
#include "workspace.h"

ProjectColoursDlg::ProjectColoursDlg(wxWindow* parent, const wxString& project)
    : ProjectColoursDlgBase(parent)
{
    wxArrayString projects;
    WorkspaceST::Get()->GetProjectList( projects );
    m_choiceProject->Append(projects);
    
    if ( ! project.IsEmpty() ) {
        // Edit mode
        m_choiceProject->SetStringSelection( project );
        m_choiceProject->Enable(false);
    }

    TweaksSettings s;
    ProjectTweaks tweaks = s.Load().GetProjectTweaks(project);
    m_colourPickerTabBG->SetColour( tweaks.GetTabBgColour() );
    m_colourPickerTabFG->SetColour( tweaks.GetTabFgColour() );
}

ProjectColoursDlg::~ProjectColoursDlg()
{
}

void ProjectColoursDlg::OnProjectChanged(wxCommandEvent& event)
{
}

ProjectTweaks ProjectColoursDlg::GetSelection() const
{
    ProjectTweaks projectTweaks;
    projectTweaks.SetProjectName( m_choiceProject->GetStringSelection() );
    projectTweaks.SetTabBgColour( m_colourPickerTabBG->GetColour() );
    projectTweaks.SetTabFgColour( m_colourPickerTabFG->GetColour() );
    return projectTweaks;
}

void ProjectColoursDlg::OnOK(wxCommandEvent& event)
{
    TweaksSettings settings;
    settings.Load();
    settings.UpdateProject( GetSelection() );
    settings.Save();
    EndModal(wxID_OK);
}

void ProjectColoursDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_choiceProject->GetStringSelection().IsEmpty() );
}
