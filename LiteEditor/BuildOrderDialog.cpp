#include "BuildOrderDialog.h"
#include "globals.h"
#include "imanager.h"
#include "project.h"
#include "workspace.h"
#include <wx/msgdlg.h>

BuildOrderDialog::BuildOrderDialog(wxWindow* parent, const wxString& projectName)
    : BuildOrderDialogBase(parent)
    , m_projectName(projectName)
{
    SetLabel(_("Edit build order for project '") + projectName + "'");

    m_dvListCtrlProjects->SetSortFunction(
        [](clRowEntry* a, clRowEntry* b) { return a->GetLabel(0).CmpNoCase(b->GetLabel(0)) < 0; });

    // determine the current project configuration
    BuildConfigPtr selBuildConf = clCxxWorkspaceST::Get()->GetProjBuildConf(m_projectName, wxEmptyString);
    wxString config_name;
    if(selBuildConf) {
        config_name = selBuildConf->GetName();
    }

    ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(m_projectName);
    if(proj) {
        // populate the choice control with the list of available configurations for this project
        ProjectSettingsPtr settings = proj->GetSettings();
        if(settings) {
            ProjectSettingsCookie cookie;
            BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
            while(bldConf) {
                wxString curr_config_name = bldConf->GetName();
                m_choiceProjectConfig->Append(curr_config_name);
                bldConf = settings->GetNextBuildConfiguration(cookie);
            }
        }
    }

    int index = m_choiceProjectConfig->FindString(config_name);
    if(index != wxNOT_FOUND) {
        m_choiceProjectConfig->SetSelection(index);
        m_current_configuration = config_name;
        Initialise(config_name);
    }
    ::clSetDialogBestSizeAndPosition(this);
}

BuildOrderDialog::~BuildOrderDialog() {}

void BuildOrderDialog::OnApplyButton(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Save(m_current_configuration);
}

void BuildOrderDialog::OnApplyButtonUI(wxUpdateUIEvent& event) { event.Enable(m_dirty); }
void BuildOrderDialog::OnConfigChanged(wxCommandEvent& event) { Initialise(event.GetString()); }

void BuildOrderDialog::OnMoveDown(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int index = m_dvListCtrlBuildOrder->GetSelectedRow();
    if(index == (int)(m_dvListCtrlBuildOrder->GetItemCount() - 1)) {
        return;
    }
    index++;
    // we can basically move the next item up
    DoMoveUp(index, m_dvListCtrlBuildOrder->GetItemText(m_dvListCtrlBuildOrder->RowToItem(index - 1)));
}

void BuildOrderDialog::OnMoveUp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int index = m_dvListCtrlBuildOrder->GetSelectedRow();
    if(index == wxNOT_FOUND || index == 0) {
        return;
    }

    DoMoveUp(index, m_dvListCtrlBuildOrder->GetItemText(m_dvListCtrlBuildOrder->RowToItem(index)));
}

void BuildOrderDialog::DoMoveUp(int index, const wxString& projectName)
{
    wxString text = m_dvListCtrlBuildOrder->GetItemText(m_dvListCtrlBuildOrder->RowToItem(index));
    if(index == 0) {
        return;
    }

    wxArrayString projects = GetBuildOrderProjects();

    int prev_row = index - 1;
    projects.Insert(text, prev_row);
    // we now have 2 instances of "text", remove the unwanted one
    projects.RemoveAt(index + 1);

    int new_sel = projects.Index(projectName);

    m_dvListCtrlBuildOrder->DeleteAllItems();
    m_dvListCtrlBuildOrder->Begin();
    for(const wxString& project : projects) {
        m_dvListCtrlBuildOrder->AppendItem(project);
    }
    m_dvListCtrlBuildOrder->Commit();
    if(new_sel != wxNOT_FOUND) {
        m_dvListCtrlBuildOrder->SelectRow(new_sel);
        m_dvListCtrlBuildOrder->EnsureVisible(m_dvListCtrlBuildOrder->RowToItem(new_sel));
    }
    m_dirty = true;
}

void BuildOrderDialog::OnMoveLeft(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int selected_row = m_dvListCtrlBuildOrder->GetSelectedRow();
    if(selected_row == wxNOT_FOUND) {
        return;
    }

    wxString project_name = m_dvListCtrlBuildOrder->GetItemText(m_dvListCtrlBuildOrder->RowToItem(selected_row));
    m_dvListCtrlBuildOrder->DeleteItem(selected_row);

    // m_dvListCtrlProjects is ordered list, it will be placed correctly
    m_dvListCtrlProjects->AppendItem(project_name);
    m_dvListCtrlProjects->Refresh();
    m_dirty = true;
}

void BuildOrderDialog::OnMoveLeftUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlBuildOrder->GetSelectedRow() != wxNOT_FOUND);
}

void BuildOrderDialog::OnMoveRight(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int selected_row = m_dvListCtrlProjects->GetSelectedRow();
    if(selected_row == wxNOT_FOUND) {
        return;
    }

    // remove the item from the project list
    wxString project_name = m_dvListCtrlProjects->GetItemText(m_dvListCtrlProjects->RowToItem(selected_row));
    m_dvListCtrlProjects->DeleteItem(selected_row);

    // append it in the build order
    m_dvListCtrlBuildOrder->AppendItem(project_name);
    m_dvListCtrlBuildOrder->Refresh();
    int last_row = m_dvListCtrlBuildOrder->GetItemCount() - 1;
    m_dvListCtrlBuildOrder->EnsureVisible(m_dvListCtrlBuildOrder->RowToItem(last_row));
    m_dirty = true;
}

void BuildOrderDialog::OnMoveRightUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlProjects->GetSelectedRow() != wxNOT_FOUND);
}

void BuildOrderDialog::Initialise(const wxString& config_name)
{
    if(config_name.empty())
        return;

    if(m_dirty && !m_current_configuration.empty()) {
        if(::wxMessageBox(_("You have un-saved changes, would you like to save them before changing configuration?"),
                          "CodeLite", wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES) {
            Save(m_current_configuration);
        }
    }

    // the choice control is already up-to-date
    DoPopulateControl(config_name);
    m_current_configuration = config_name;
}

void BuildOrderDialog::DoPopulateControl(const wxString& configuration)
{
    wxString errMsg;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(m_projectName, errMsg);
    if(!proj) {
        return;
    }

    m_dvListCtrlBuildOrder->DeleteAllItems();
    m_dvListCtrlProjects->DeleteAllItems();

    // initialize the build order listbox
    wxStringSet_t deps_set;
    wxArrayString depArr = proj->GetDependencies(configuration);
    size_t i = 0;
    for(i = 0; i < depArr.GetCount(); i++) {
        deps_set.insert(depArr[i]);
        m_dvListCtrlBuildOrder->AppendItem(depArr[i]);
    }

    // initialize the project dependencies view
    wxArrayString projArr;
    clGetManager()->GetWorkspace()->GetProjectList(projArr);

    // add only projects that do not exist in the dependencies view
    for(i = 0; i < projArr.GetCount(); i++) {
        if((deps_set.count(projArr[i]) == 0) && (projArr.Item(i) != m_projectName)) {
            m_dvListCtrlProjects->AppendItem(projArr.Item(i));
        }
    }
}

void BuildOrderDialog::Save(const wxString& config_name)
{
    // Save only if its dirty...
    if(m_dirty) {
        ProjectPtr proj = clCxxWorkspaceST::Get()->GetProject(m_projectName);
        wxArrayString depsArr = GetBuildOrderProjects();
        proj->SetDependencies(depsArr, config_name);
    }
    m_dirty = false;
}

wxArrayString BuildOrderDialog::GetBuildOrderProjects() const
{
    wxArrayString projects;
    projects.reserve(m_dvListCtrlBuildOrder->GetItemCount());

    for(size_t i = 0; i < m_dvListCtrlBuildOrder->GetItemCount(); ++i) {
        projects.Add(m_dvListCtrlBuildOrder->GetItemText(m_dvListCtrlBuildOrder->RowToItem(i)));
    }
    return projects;
}

void BuildOrderDialog::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Save(m_current_configuration);
    EndModal(wxID_OK);
}
