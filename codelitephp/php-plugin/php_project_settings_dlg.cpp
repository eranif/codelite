#include "php_project_settings_dlg.h"
#include <globals.h>
#include <windowattrmanager.h>
#include "php_workspace.h"
#include <event_notifier.h>
#include "FileMappingDlg.h"

PHPProjectSettingsDlg::PHPProjectSettingsDlg(wxWindow* parent, const wxString& projectName)
    : PHPProjectSettingsBase(parent)
    , m_dirty(false)
    , m_projectName(projectName)
    , m_resyncNeeded(false)
{
    MSWSetNativeTheme(m_treebook41->GetTreeCtrl());
    const PHPProjectSettingsData& data = PHPWorkspace::Get()->GetProject(m_projectName)->GetSettings();

    // General settings
    m_choicebook1->SetSelection(data.GetRunAs() == PHPProjectSettingsData::kRunAsCLI ? 0 : 1);
    m_filePickerPHPExe->SetPath(data.GetPhpExe());
    m_filePickerIndex->SetPath(data.GetIndexFile());
    m_textCtrlProgramArgs->ChangeValue(data.GetArgs());
    m_dirPickerWorkingDirectory->SetPath(data.GetWorkingDirectory());
    m_textCtrlPHPIncludePath->ChangeValue(data.GetIncludePath());
    m_checkBoxPauseWhenExecutionEnds->SetValue(data.IsPauseWhenExeTerminates());
    m_textCtrlWebSiteURL->ChangeValue(data.GetProjectURL());
    m_checkBoxSystemBrowser->SetValue(data.IsUseSystemBrowser());

    wxString fileExts = PHPWorkspace::Get()->GetProject(m_projectName)->GetImportFileSpec();
    fileExts.Replace(";", ",");
    m_pgPropFileTypes->SetValueFromString(fileExts);

    wxString excludeFolders = PHPWorkspace::Get()->GetProject(m_projectName)->GetExcludeFolders();
    excludeFolders.Replace(";", ",");
    m_pgPropExcludeFolders->SetValueFromString(excludeFolders);

    if(!data.GetPhpIniFile().IsEmpty()) {
        m_filePickerPhpIni->SetPath(data.GetPhpIniFile());
    }

    const wxStringMap_t& mapping = data.GetFileMapping();
    wxStringMap_t::const_iterator iter = mapping.begin();
    for(; iter != mapping.end(); ++iter) {
        wxVector<wxVariant> cols;
        cols.push_back(iter->first);
        cols.push_back(iter->second);
        m_dvListCtrlFileMapping->AppendItem(cols);
    }

    // Code Completion settings
    m_textCtrlCCIncludePath->ChangeValue(data.GetCcIncludePath());

    SetDirty(false);
    GetSizer()->Fit(this);
    SetName("PHPProjectSettingsDlg");
    WindowAttrManager::Load(this);

    m_dvListCtrlFileMapping->Bind(
        wxEVT_COMMAND_MENU_SELECTED, &PHPProjectSettingsDlg::OnNewFileMapping, this, wxID_NEW);
    m_dvListCtrlFileMapping->Bind(
        wxEVT_COMMAND_MENU_SELECTED, &PHPProjectSettingsDlg::OnDeleteFileMapping, this, wxID_DELETE);
    m_dvListCtrlFileMapping->Bind(
        wxEVT_COMMAND_MENU_SELECTED, &PHPProjectSettingsDlg::OnEditFileMapping, this, wxID_EDIT);
}

PHPProjectSettingsDlg::~PHPProjectSettingsDlg()
{

    m_dvListCtrlFileMapping->Unbind(
        wxEVT_COMMAND_MENU_SELECTED, &PHPProjectSettingsDlg::OnNewFileMapping, this, wxID_NEW);
    m_dvListCtrlFileMapping->Unbind(
        wxEVT_COMMAND_MENU_SELECTED, &PHPProjectSettingsDlg::OnDeleteFileMapping, this, wxID_DELETE);
    m_dvListCtrlFileMapping->Unbind(
        wxEVT_COMMAND_MENU_SELECTED, &PHPProjectSettingsDlg::OnEditFileMapping, this, wxID_EDIT);
}

void PHPProjectSettingsDlg::OnOK(wxCommandEvent& event)
{
    if(IsDirty()) {
        OnApply(event);
    }
    EndModal(wxID_OK);
}

void PHPProjectSettingsDlg::OnApply(wxCommandEvent& event) { Save(); }

void PHPProjectSettingsDlg::OnApplyUI(wxUpdateUIEvent& event) { event.Enable(this->m_dirty); }

void PHPProjectSettingsDlg::OnAddCCPath(wxCommandEvent& event)
{
    wxString path = wxDirSelector();
    if(path.IsEmpty() == false) {
        wxString currentValue = m_textCtrlCCIncludePath->GetValue();
        currentValue.Trim().Trim(false);

        if(currentValue.IsEmpty() == false) {
            currentValue << wxT("\n") << path;
        } else {
            currentValue << path;
        }
        m_textCtrlCCIncludePath->ChangeValue(currentValue);
    }
}

void PHPProjectSettingsDlg::OnAddIncludePath(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("Select folder"));
    if(!path.IsEmpty()) {
        wxArrayString curIncPaths = wxStringTokenize(m_textCtrlPHPIncludePath->GetValue(), "\n", wxTOKEN_STRTOK);
        if(curIncPaths.Index(path) == wxNOT_FOUND) {
            curIncPaths.Add(path);
        }

        // User SetValue to trigger an event
        m_textCtrlPHPIncludePath->SetValue(wxJoin(curIncPaths, '\n'));
    }
}

void PHPProjectSettingsDlg::OnArgumentsEntered(wxCommandEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnIndexFileSelected(wxFileDirPickerEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnPHPExecChanged(wxFileDirPickerEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnPageChanged(wxChoicebookEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnPauseWhenExeTerminates(wxCommandEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnProjectURLChanged(wxCommandEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnWorkingDirectoryChanged(wxFileDirPickerEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::Save()
{
    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetProject(m_projectName);
    CHECK_PTR_RET(pProject);

    PHPProjectSettingsData& data = pProject->GetSettings();

    // General settings
    data.SetRunAs(m_choicebook1->GetSelection() == 0 ? PHPProjectSettingsData::kRunAsCLI :
                                                       PHPProjectSettingsData::kRunAsWebsite);
    data.SetPhpExe(m_filePickerPHPExe->GetPath());
    data.SetIndexFile(m_filePickerIndex->GetPath());
    data.SetArgs(m_textCtrlProgramArgs->GetValue());
    data.SetWorkingDirectory(m_dirPickerWorkingDirectory->GetPath());
    data.SetIncludePath(m_textCtrlPHPIncludePath->GetValue());
    data.SetPauseWhenExeTerminates(m_checkBoxPauseWhenExecutionEnds->IsChecked());
    data.SetPhpIniFile(m_filePickerPhpIni->GetPath());
    data.SetProjectURL(m_textCtrlWebSiteURL->GetValue());
    data.SetUseSystemBrowser(m_checkBoxSystemBrowser->IsChecked());

    // Code Completion settings
    data.SetCcIncludePath(m_textCtrlCCIncludePath->GetValue());

    // Save the file mapping
    wxStringMap_t mapping;
    int itemCount = m_dvListCtrlFileMapping->GetItemCount();
    for(int i = 0; i < itemCount; ++i) {
        wxVariant source, target;
        m_dvListCtrlFileMapping->GetValue(source, i, 0);
        m_dvListCtrlFileMapping->GetValue(target, i, 1);
        mapping.insert(std::make_pair(source.GetString(), target.GetString()));
    }
    data.SetFileMapping(mapping);

    wxString fileExts = m_pgPropFileTypes->GetValue().GetString();
    fileExts.Replace(",", ";");
    pProject->SetImportFileSpec(fileExts);

    wxString excludeDirs = m_pgPropExcludeFolders->GetValue().GetString();
    excludeDirs.Replace(",", ";");
    pProject->SetExcludeFolders(excludeDirs);

    // Save the project content
    pProject->Save();
    SetDirty(false);
}

void PHPProjectSettingsDlg::OnUpdateApplyUI(wxCommandEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnPHPIniSelected(wxFileDirPickerEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnUseSystemBrowser(wxCommandEvent& event) { SetDirty(true); }

void PHPProjectSettingsDlg::OnFileMappingItemActivated(wxDataViewEvent& event) { EditItem(event.GetItem()); }

void PHPProjectSettingsDlg::OnFileMappingMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_NEW);
    menu.Append(wxID_EDIT);
    menu.Append(wxID_DELETE);

    menu.Enable(wxID_EDIT, m_dvListCtrlFileMapping->GetSelectedItemsCount() == 1);
    menu.Enable(wxID_DELETE, event.GetItem().IsOk());
    m_dvListCtrlFileMapping->PopupMenu(&menu);
}

void PHPProjectSettingsDlg::OnDeleteFileMapping(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_dvListCtrlFileMapping->GetSelections(items);
    for(size_t i = 0; i < items.GetCount(); ++i) {
        m_dvListCtrlFileMapping->DeleteItem(m_dvListCtrlFileMapping->ItemToRow(items.Item(i)));
        SetDirty(true);
    }
}

void PHPProjectSettingsDlg::OnEditFileMapping(wxCommandEvent& e)
{
    wxDataViewItemArray items;
    m_dvListCtrlFileMapping->GetSelections(items);
    if(items.GetCount() != 1) return;

    wxDataViewItem item = items.Item(0);
    EditItem(item);
}

void PHPProjectSettingsDlg::OnNewFileMapping(wxCommandEvent& e)
{
    FileMappingDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetSourceFolder());
        cols.push_back(dlg.GetTargetFolder());
        m_dvListCtrlFileMapping->AppendItem(cols);
        SetDirty(true);
    }
}

void PHPProjectSettingsDlg::EditItem(const wxDataViewItem& item)
{
    if(!item.IsOk()) {
        return;
    }

    wxVariant source, target;
    unsigned int row = m_dvListCtrlFileMapping->ItemToRow(item);
    m_dvListCtrlFileMapping->GetValue(source, row, 0);
    m_dvListCtrlFileMapping->GetValue(target, row, 1);

    FileMappingDlg dlg(this);
    dlg.SetSourceFolder(source.GetString());
    dlg.SetTargetFolder(target.GetString());
    if(dlg.ShowModal() == wxID_OK) {
        m_dvListCtrlFileMapping->SetValue(dlg.GetSourceFolder(), row, 0);
        m_dvListCtrlFileMapping->SetValue(dlg.GetTargetFolder(), row, 1);
        SetDirty(true);
    }
}
void PHPProjectSettingsDlg::OnTextctrlviewfilterTextUpdated(wxCommandEvent& event) { SetDirty(true); }
void PHPProjectSettingsDlg::OnPgmgrviewPgChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    SetDirty(true);
    m_resyncNeeded = true;
}
