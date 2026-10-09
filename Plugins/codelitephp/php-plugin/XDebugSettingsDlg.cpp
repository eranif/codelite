#include "XDebugSettingsDlg.h"

#include "FileMappingDlg.h"
#include "globals.h"

#include <wx/dirdlg.h>
#include <wx/menu.h>
#include <wx/tokenzr.h>

XDebugSettingsDlg::XDebugSettingsDlg(wxWindow* parent, XDebugSettings& settings)
    : XDebugSettingsDlgBase(parent)
    , m_settings(settings)
{
    MSWSetNativeTheme(m_treebook412->GetTreeCtrl());
    const PHPProjectSettingsData& data = m_settings.GetData();

    m_choicebook16->ChangeSelection(data.GetRunAs() == PHPProjectSettingsData::kRunAsCLI ? 0 : 1);
    m_filePickerPHPExe11->SetPath(data.GetPhpExe());
    m_filePickerPhpIni13->SetPath(data.GetPhpIniFile());
    m_filePickerIndex15->SetPath(data.GetIndexFile());
    m_dirPickerWorkingDirectory17->SetPath(data.GetWorkingDirectory());
    m_textCtrlProgramArgs19->ChangeValue(data.GetArgs());
    m_checkBoxPauseWhenExecutionEnds21->SetValue(data.IsPauseWhenExeTerminates());
    m_textCtrlWebSiteURL25->ChangeValue(data.GetProjectURL());
    m_checkBoxSystemBrowser27->SetValue(data.IsUseSystemBrowser());
    m_textCtrlPHPIncludePath34->ChangeValue(data.GetIncludePath());

    for (const auto& [source, target] : data.GetFileMapping()) {
        wxVector<wxVariant> cols;
        cols.push_back(source);
        cols.push_back(target);
        m_dvListCtrlFileMapping43->AppendItem(cols);
    }

    // The base class has no event handlers, so connect them here. The control events reach the dialog because
    // command events propagate to the parent
    Bind(wxEVT_TEXT, &XDebugSettingsDlg::OnChanged, this);
    Bind(wxEVT_CHECKBOX, &XDebugSettingsDlg::OnChanged, this);
    Bind(wxEVT_FILEPICKER_CHANGED, &XDebugSettingsDlg::OnChanged, this);
    Bind(wxEVT_DIRPICKER_CHANGED, &XDebugSettingsDlg::OnChanged, this);
    Bind(wxEVT_CHOICEBOOK_PAGE_CHANGED, &XDebugSettingsDlg::OnChanged, this);
    m_button1254->Bind(wxEVT_BUTTON, &XDebugSettingsDlg::OnOK, this);
    m_button1456->Bind(wxEVT_BUTTON, &XDebugSettingsDlg::OnApply, this);
    m_button1456->Bind(wxEVT_UPDATE_UI, &XDebugSettingsDlg::OnApplyUI, this);
    m_button1733->Bind(wxEVT_BUTTON, &XDebugSettingsDlg::OnAddIncludePath, this);
    m_dvListCtrlFileMapping43->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &XDebugSettingsDlg::OnFileMappingMenu, this);
    m_dvListCtrlFileMapping43->Bind(
        wxEVT_DATAVIEW_ITEM_ACTIVATED, &XDebugSettingsDlg::OnFileMappingItemActivated, this);
    m_dvListCtrlFileMapping43->Bind(wxEVT_MENU, &XDebugSettingsDlg::OnNewFileMapping, this, wxID_NEW);
    m_dvListCtrlFileMapping43->Bind(wxEVT_MENU, &XDebugSettingsDlg::OnEditFileMapping, this, wxID_EDIT);
    m_dvListCtrlFileMapping43->Bind(wxEVT_MENU, &XDebugSettingsDlg::OnDeleteFileMapping, this, wxID_DELETE);

    GetSizer()->Fit(this);
    ::clSetDialogBestSizeAndPosition(*this);
}

void XDebugSettingsDlg::Save()
{
    PHPProjectSettingsData& data = m_settings.GetData();
    data.SetRunAs(m_choicebook16->GetSelection() == 0 ? PHPProjectSettingsData::kRunAsCLI
                                                      : PHPProjectSettingsData::kRunAsWebsite);
    data.SetPhpExe(m_filePickerPHPExe11->GetPath());
    data.SetPhpIniFile(m_filePickerPhpIni13->GetPath());
    data.SetIndexFile(m_filePickerIndex15->GetPath());
    data.SetWorkingDirectory(m_dirPickerWorkingDirectory17->GetPath());
    data.SetArgs(m_textCtrlProgramArgs19->GetValue());
    data.SetPauseWhenExeTerminates(m_checkBoxPauseWhenExecutionEnds21->IsChecked());
    data.SetProjectURL(m_textCtrlWebSiteURL25->GetValue());
    data.SetUseSystemBrowser(m_checkBoxSystemBrowser27->IsChecked());
    data.SetIncludePath(m_textCtrlPHPIncludePath34->GetValue());

    wxStringMap_t mapping;
    int itemCount = m_dvListCtrlFileMapping43->GetItemCount();
    for (int i = 0; i < itemCount; ++i) {
        wxVariant source, target;
        m_dvListCtrlFileMapping43->GetValue(source, i, 0);
        m_dvListCtrlFileMapping43->GetValue(target, i, 1);
        mapping.insert({source.GetString(), target.GetString()});
    }
    data.SetFileMapping(mapping);

    m_settings.Save();
    m_dirty = false;
}

void XDebugSettingsDlg::OnChanged(wxCommandEvent& event)
{
    event.Skip();
    m_dirty = true;
}

void XDebugSettingsDlg::OnOK(wxCommandEvent& event)
{
    if (m_dirty) {
        Save();
    }
    EndModal(wxID_OK);
}

void XDebugSettingsDlg::OnApply(wxCommandEvent& event) { Save(); }

void XDebugSettingsDlg::OnApplyUI(wxUpdateUIEvent& event) { event.Enable(m_dirty); }

void XDebugSettingsDlg::OnAddIncludePath(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("Select folder"));
    if (!path.IsEmpty()) {
        wxArrayString curIncPaths = wxStringTokenize(m_textCtrlPHPIncludePath34->GetValue(), "\n", wxTOKEN_STRTOK);
        if (curIncPaths.Index(path) == wxNOT_FOUND) {
            curIncPaths.Add(path);
        }

        // Use SetValue to mark the dialog as changed
        m_textCtrlPHPIncludePath34->SetValue(wxJoin(curIncPaths, '\n'));
    }
}

void XDebugSettingsDlg::OnFileMappingMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_NEW);
    menu.Append(wxID_EDIT);
    menu.Append(wxID_DELETE);

    menu.Enable(wxID_EDIT, m_dvListCtrlFileMapping43->GetSelectedItemsCount() == 1);
    menu.Enable(wxID_DELETE, event.GetItem().IsOk());
    m_dvListCtrlFileMapping43->PopupMenu(&menu);
}

void XDebugSettingsDlg::OnFileMappingItemActivated(wxDataViewEvent& event) { EditFileMapping(event.GetItem()); }

void XDebugSettingsDlg::OnNewFileMapping(wxCommandEvent& event)
{
    FileMappingDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK) {
        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetSourceFolder());
        cols.push_back(dlg.GetTargetFolder());
        m_dvListCtrlFileMapping43->AppendItem(cols);
        m_dirty = true;
    }
}

void XDebugSettingsDlg::OnEditFileMapping(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlFileMapping43->GetSelections(items);
    if (items.GetCount() == 1) {
        EditFileMapping(items.Item(0));
    }
}

void XDebugSettingsDlg::OnDeleteFileMapping(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlFileMapping43->GetSelections(items);
    for (const auto& item : items) {
        m_dvListCtrlFileMapping43->DeleteItem(m_dvListCtrlFileMapping43->ItemToRow(item));
        m_dirty = true;
    }
}

void XDebugSettingsDlg::EditFileMapping(const wxDataViewItem& item)
{
    if (!item.IsOk()) {
        return;
    }

    wxVariant source, target;
    unsigned int row = m_dvListCtrlFileMapping43->ItemToRow(item);
    m_dvListCtrlFileMapping43->GetValue(source, row, 0);
    m_dvListCtrlFileMapping43->GetValue(target, row, 1);

    FileMappingDlg dlg(this);
    dlg.SetSourceFolder(source.GetString());
    dlg.SetTargetFolder(target.GetString());
    if (dlg.ShowModal() == wxID_OK) {
        m_dvListCtrlFileMapping43->SetValue(dlg.GetSourceFolder(), row, 0);
        m_dvListCtrlFileMapping43->SetValue(dlg.GetTargetFolder(), row, 1);
        m_dirty = true;
    }
}
