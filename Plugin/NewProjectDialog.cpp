#include "NewProjectDialog.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "clWorkspaceManager.h"
#include "cl_config.h"
#include "debuggermanager.h"
#include "wxStringHash.h"
#include <globals.h>
#include <unordered_set>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>

#define CONFIG_LAST_SELECTED_CATEGORY "NewProject/LastCategory"
#define CONFIG_LAST_SELECTED_TYPE "NewProject/LastType"
#define CONFIG_USE_SEPARATE_FOLDER "NewProjectDialog/UseSeparateFolder"
#define CONFIG_LAST_COMPILER "NewProjectDialog/LastCompiler"
#define CONFIG_LAST_DEBUGGER "NewProjectDialog/LastDebugger"
#define CONFIG_LAST_BUILD_SYSTEM "NewProjectDialog/LastBuildSystem"

static bool SetChoiceOptions(wxChoice* choice, const wxArrayString& values, const wxString& defaultValue)
{
    int match = wxNOT_FOUND;
    choice->Clear();
    for(const wxString& v : values) {
        int where = choice->Append(v);
        if(v == defaultValue) { match = where; }
    }

    if(match != wxNOT_FOUND) { choice->SetSelection(match); }
    return (match != wxNOT_FOUND);
}

NewProjectDialog::NewProjectDialog(wxWindow* parent)
    : NewProjectDialogBase(parent)
{
    ::GetProjectTemplateList(m_list);

    // If we have a workspace, set the project path in the workspace path
    if(clWorkspaceManager::Get().IsWorkspaceOpened()) {
        const wxFileName& fn = clWorkspaceManager::Get().GetWorkspace()->GetFileName();
        m_dirPicker->SetPath(fn.GetPath());
        m_textCtrlName->ChangeValue(fn.GetDirs().Last());
    }

    wxString lastBuildSystem = "CodeLite Make Generator";
    wxString lastCategory;
    wxString lastType;
    wxString lastCompiler;
    wxString lastDebugger;

    lastBuildSystem = clConfig::Get().Read(CONFIG_LAST_BUILD_SYSTEM, lastBuildSystem);
    lastCategory = clConfig::Get().Read(CONFIG_LAST_SELECTED_CATEGORY, lastCategory);
    lastType = clConfig::Get().Read(CONFIG_LAST_SELECTED_TYPE, lastType);
    lastCompiler = clConfig::Get().Read(CONFIG_LAST_COMPILER, lastCompiler);
    lastDebugger = clConfig::Get().Read(CONFIG_LAST_DEBUGGER, lastDebugger);
    bool checked = clConfig::Get().Read(CONFIG_USE_SEPARATE_FOLDER, true);

    // Populate the project type choices
    std::unordered_set<wxString> S;
    wxArrayString categories;
    for(const auto& proj : m_list) {
        if(S.count(proj->GetName()) == 0) {
            S.insert(proj->GetName());
        } else {
            continue;
        }

        m_projectsMap.insert({ proj->GetName(), proj });
        wxString internalType = proj->GetProjectInternalType();
        if(internalType.IsEmpty()) { internalType = "General"; }

        if(m_categories.count(internalType) == 0) {
            m_categories.insert({ internalType, wxArrayString() });
            categories.Add(internalType);
        }
        wxArrayString& arr = m_categories[internalType];
        arr.Add(proj->GetName());
    }

    if(SetChoiceOptions(m_choiceCategory, categories, lastCategory)) {
        // Load all the projects belong to this category
        wxArrayString types = GetProjectsTypesForCategory(lastCategory);
        SetChoiceOptions(m_choiceType, types, lastType);
    }

    // list of compilers
    wxArrayString compilerChoices;

    // Get list of compilers from configuration file
    BuildSettingsConfigCookie cookie;
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while(cmp) {
        compilerChoices.Add(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    SetChoiceOptions(m_choiceCompiler, compilerChoices, lastCompiler);

    // Get list of debuggers
    wxArrayString debuggerChoices;
    wxArrayString knownDebuggers = DebuggerMgr::Get().GetAvailableDebuggers();
    debuggerChoices.insert(debuggerChoices.end(), knownDebuggers.begin(), knownDebuggers.end());
    SetChoiceOptions(m_choiceDebugger, debuggerChoices, lastDebugger);

    // build system
    std::list<wxString> builders;
    wxArrayString knownBuilders;
    BuildManagerST::Get()->GetBuilders(builders);
    for(const wxString& builderName : builders) {
        knownBuilders.Add(builderName);
    }
    SetChoiceOptions(m_choiceBuild, knownBuilders, lastBuildSystem);

    m_checkBoxSepFolder->SetValue(checked);
    CenterOnParent();
    ::clSetSmallDialogBestSizeAndPosition(this);
}

NewProjectDialog::~NewProjectDialog()
{
    clConfig::Get().Write(CONFIG_LAST_SELECTED_CATEGORY, m_choiceCategory->GetStringSelection());
    clConfig::Get().Write(CONFIG_LAST_SELECTED_TYPE, m_choiceType->GetStringSelection());
    clConfig::Get().Write(CONFIG_LAST_COMPILER, m_choiceCompiler->GetStringSelection());
    clConfig::Get().Write(CONFIG_LAST_BUILD_SYSTEM, m_choiceBuild->GetStringSelection());
    clConfig::Get().Write(CONFIG_LAST_DEBUGGER, m_choiceDebugger->GetStringSelection());
    clConfig::Get().Write(CONFIG_USE_SEPARATE_FOLDER, m_checkBoxSepFolder->IsChecked());
}

wxArrayString NewProjectDialog::GetProjectsTypesForCategory(const wxString& category)
{
    if(m_categories.count(category) == 0) { return wxArrayString(); }
    const wxArrayString& projects = m_categories[category];
    return projects;
}

void NewProjectDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->IsEmpty() && !m_dirPicker->GetPath().IsEmpty() &&
                 m_choiceCategory->GetSelection() != wxNOT_FOUND && m_choiceType->GetSelection() != wxNOT_FOUND);
}

ProjectData NewProjectDialog::GetProjectData() const
{
    wxString sel = m_choiceType->GetStringSelection();
    if(sel.IsEmpty()) { return ProjectData(); }
    if(m_projectsMap.count(sel) == 0) { return ProjectData(); }

    ProjectData data;
    data.m_builderName = m_choiceBuild->GetStringSelection();
    data.m_cmpType = m_choiceCompiler->GetStringSelection();
    data.m_name = m_textCtrlName->GetValue();
    data.m_debuggerType = m_choiceDebugger->GetStringSelection();

    wxFileName path(m_dirPicker->GetPath(), "");
    if(m_checkBoxSepFolder->IsChecked()) { path.AppendDir(data.m_name); }
    data.m_path = path.GetPath();
    data.m_sourceTemplate = "C++ Project";
    data.m_srcProject = m_projectsMap.find(sel)->second;
    return data;
}

void NewProjectDialog::OnPathSelected(wxFileDirPickerEvent& event)
{
    wxUnusedVar(event);
    if(!m_userTypeName) {
        wxFileName path(m_dirPicker->GetPath(), "");
        if(path.GetDirCount()) { m_textCtrlName->ChangeValue(path.GetDirs().Last()); }
    }
}

void NewProjectDialog::OnNameTyped(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_userTypeName = true;
}

void NewProjectDialog::OnCategoryChanged(wxCommandEvent& event)
{
    wxString sel = m_choiceCategory->GetStringSelection();
    if(sel.IsEmpty()) { return; }
    wxArrayString a = GetProjectsTypesForCategory(sel);
    SetChoiceOptions(m_choiceType, a, wxEmptyString);
}
void NewProjectDialog::OnOK(wxCommandEvent& event)
{
    if(m_textCtrlName->GetValue().Contains(" ")) {
        ::wxMessageBox(_("Project name must not contain spaces"), "CodeLite", wxICON_WARNING);
        return;
    }
    event.Skip();
}
