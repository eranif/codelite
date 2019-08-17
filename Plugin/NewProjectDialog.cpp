#include "NewProjectDialog.h"
#include <globals.h>
#include "clWorkspaceManager.h"
#include <wx/arrstr.h>
#include <unordered_set>
#include "build_settings_config.h"
#include "debuggermanager.h"
#include "buildmanager.h"
#include "wxStringHash.h"

NewProjectDialog::NewProjectDialog(wxWindow* parent)
    : NewProjectDialogBase(parent)
{
    ::GetProjectTemplateList(m_list);

    // If we have a workspace, set the project path in the workspace path
    if(clWorkspaceManager::Get().IsWorkspaceOpened()) {
        m_dirPicker->SetPath(clWorkspaceManager::Get().GetWorkspace()->GetFileName().GetPath());
    }

    // Populate the project type choices
    std::unordered_set<wxString> S;
    for(auto& p : m_list) {
        if(S.count(p->GetName()) == 0) {
            S.insert(p->GetName());
        } else {
            continue;
        }
        int where = m_choiceType->Append(p->GetName());
        m_projectsMap.insert({ where, p });
    }

    if(!m_choiceType->IsEmpty()) { m_choiceType->SetSelection(0); }

    // list of compilers
    wxArrayString compilerChoices;

    // Get list of compilers from configuration file
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while(cmp) {
        compilerChoices.Add(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }
    m_choiceCompiler->Append(compilerChoices);
    if(!compilerChoices.IsEmpty()) { m_choiceCompiler->SetSelection(0); }

    // Get list of debuggers
    wxArrayString debuggerChoices;
    wxArrayString knownDebuggers = DebuggerMgr::Get().GetAvailableDebuggers();
    debuggerChoices.insert(debuggerChoices.end(), knownDebuggers.begin(), knownDebuggers.end());
    m_choiceDebugger->Append(debuggerChoices);
    if(!m_choiceDebugger->IsEmpty()) { m_choiceDebugger->SetSelection(0); }

    // build system
    {
        std::list<wxString> builders;
        wxArrayString knownBuilders;
        BuildManagerST::Get()->GetBuilders(builders);
        for(const wxString& builderName : builders) {
            knownBuilders.Add(builderName);
        }
        m_choiceBuild->Append(knownBuilders);
        int where = m_choiceBuild->FindString("Default");
        if(where != wxNOT_FOUND) {
            m_choiceBuild->SetSelection(where);
        } else if(!m_choiceBuild->IsEmpty()) {
            m_choiceBuild->SetSelection(0);
        }
    }
    CenterOnParent();
}

NewProjectDialog::~NewProjectDialog() {}

void NewProjectDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->IsEmpty() && !m_dirPicker->GetPath().IsEmpty());
}

ProjectData NewProjectDialog::GetProjectData() const
{
    int sel = m_choiceType->GetSelection();
    if(sel == wxNOT_FOUND) { return ProjectData(); }
    if(m_projectsMap.count(sel) == 0) { return ProjectData(); }

    ProjectData data;
    data.m_builderName = m_choiceBuild->GetStringSelection();
    data.m_cmpType = m_choiceCompiler->GetStringSelection();
    data.m_name = m_textCtrlName->GetValue();
    data.m_debuggerType = m_choiceDebugger->GetStringSelection();
    data.m_path = m_dirPicker->GetPath();
    data.m_sourceTemplate = "C++ Project";
    data.m_srcProject = m_projectsMap.find(sel)->second;
    return data;
}
