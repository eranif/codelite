/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>

#include "file_logger.h"
#include "memcheckdefs.h"
#include "memchecksettingsdialog.h"
#include "memchecksettings.h"
#include "windowattrmanager.h"

MemCheckSettingsDialog::MemCheckSettingsDialog(wxWindow* parent, MemCheckSettings* settings)
    : MemCheckSettingsDialogBase(parent)
    , m_settings(settings)
{
    m_settings->LoadFromConfig();

    m_choiceEngine->Set(m_settings->GetAvailableEngines());
    m_choiceEngine->SetSelection(m_settings->GetAvailableEngines().Index(m_settings->GetEngine()));
    m_sliderPageCapacity->SetValue(m_settings->GetResultPageSize());
    m_sliderPageCapacity->SetMax(m_settings->GetResultPageSizeMax());
    m_checkBoxOmitNonWorkspace->SetValue(m_settings->GetOmitNonWorkspace());
    m_checkBoxOmitDuplications->SetValue(m_settings->GetOmitDuplications());
    m_checkBoxOmitSuppressed->SetValue(m_settings->GetOmitSuppressed());

    m_filePickerValgrindBinary->SetPath(m_settings->GetValgrindSettings().GetBinary());
    m_checkBoxOutputInPrivateFolder->SetValue(m_settings->GetValgrindSettings().GetOutputInPrivateFolder());
    m_filePickerValgrindOutputFile->SetPath(m_settings->GetValgrindSettings().GetOutputFile());
    m_textCtrlValgrindMandatoryOptions->ChangeValue(
        wxString::Format("%s %s=<file> %s=<file> ...",
                         m_settings->GetValgrindSettings().GetMandatoryOptions(),
                         m_settings->GetValgrindSettings().GetOutputFileOption(),
                         m_settings->GetValgrindSettings().GetSuppressionFileOption()));
    m_textCtrlValgrindOptions->ChangeValue(m_settings->GetValgrindSettings().GetOptions());
    m_checkBoxSuppFileInPrivateFolder->SetValue(m_settings->GetValgrindSettings().GetSuppFileInPrivateFolder());
    m_listBoxSuppFiles->Set(m_settings->GetValgrindSettings().GetSuppFiles());
    SetName("MemCheckSettingsDialog");
    WindowAttrManager::Load(this);
}

MemCheckSettingsDialog::~MemCheckSettingsDialog() {}

void MemCheckSettingsDialog::OnOK(wxCommandEvent& event)
{
    wxString outputFileMsg;
    if(!m_checkBoxOutputInPrivateFolder->IsChecked() && m_filePickerValgrindOutputFile->GetPath().IsEmpty())
        outputFileMsg = wxT("If you don't want to use output file in private folder, you have to set a file manulay.");

    wxString suppFileMsg;
    if(!m_checkBoxSuppFileInPrivateFolder->IsChecked() && m_listBoxSuppFiles->IsEmpty())
        suppFileMsg = wxT("If you don't want to use default supp in private folder, you have to set at least one "
                          "suppression file manulay.");

    if(!outputFileMsg.IsEmpty() || !suppFileMsg.IsEmpty()) {
        wxMessageBox(wxString::Format("Wrong Valgrind option\n\n\n* %s\n\n* %s", outputFileMsg, suppFileMsg),
                     wxT("Invalid Valgrind settings"),
                     wxICON_ERROR);
        return;
    }

    m_settings->SetEngine(m_settings->GetAvailableEngines().Item(m_choiceEngine->GetSelection()));
    m_settings->SetResultPageSize(m_sliderPageCapacity->GetValue());
    m_settings->SetOmitNonWorkspace(m_checkBoxOmitNonWorkspace->GetValue());
    m_settings->SetOmitDuplications(m_checkBoxOmitDuplications->GetValue());
    m_settings->SetOmitSuppressed(m_checkBoxOmitSuppressed->GetValue());

    m_settings->GetValgrindSettings().SetBinary(m_filePickerValgrindBinary->GetPath());
    m_settings->GetValgrindSettings().SetOutputInPrivateFolder(m_checkBoxOutputInPrivateFolder->GetValue());
    m_settings->GetValgrindSettings().SetOutputFile(m_filePickerValgrindOutputFile->GetPath());
    m_settings->GetValgrindSettings().SetOptions(m_textCtrlValgrindOptions->GetValue());
    m_settings->GetValgrindSettings().SetSuppFileInPrivateFolder(m_checkBoxSuppFileInPrivateFolder->GetValue());
    m_settings->GetValgrindSettings().SetSuppFiles(m_listBoxSuppFiles->GetStrings());

    m_settings->SavaToConfig();

    EndModal(wxID_OK);
}

void MemCheckSettingsDialog::ValgrindResetOptions(wxCommandEvent& event)
{
    ValgrindSettings defaultValgrindSettings;
    m_textCtrlValgrindOptions->ChangeValue(defaultValgrindSettings.GetOptions());
}

void MemCheckSettingsDialog::OnValgrindOutputFileChanged(wxFileDirPickerEvent& event)
{
    // TODO ? update m_textCtrlValgrindMandatoryOptions ?
}

void MemCheckSettingsDialog::OnAddSupp(wxCommandEvent& event)
{
    // ATTN  m_mgr->GetTheApp()
    wxFileDialog openFileDialog(wxTheApp->GetTopWindow(),
                                _("Add suppression file(s)"),
                                "",
                                "",
                                "suppression files (*.supp)|*.supp|all files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if(openFileDialog.ShowModal() == wxID_CANCEL) return;

    wxArrayString paths;
    openFileDialog.GetPaths(paths);
    m_listBoxSuppFiles->Append(paths);
}

void MemCheckSettingsDialog::OnDelSupp(wxCommandEvent& event)
{
    wxArrayInt selections;
    m_listBoxSuppFiles->GetSelections(selections);
    for(int i = selections.GetCount() - 1; i >= 0; --i) {
        m_listBoxSuppFiles->Delete(selections.Item(i));
    }
}

void MemCheckSettingsDialog::OnSuppListRightDown(wxMouseEvent& event)
{
    wxMenuItem* menuItem(NULL);
    wxMenu menu;

    int index = m_listBoxSuppFiles->HitTest(event.GetPosition());
    wxArrayInt selections;
    m_listBoxSuppFiles->GetSelections(selections);
    if(selections.Index(index) == wxNOT_FOUND) {
        m_listBoxSuppFiles->SetSelection(wxNOT_FOUND);
        m_listBoxSuppFiles->SetSelection(index);
    }

    menuItem = menu.Append(XRCID("memcheck_add_supp"), _("Add suppression file(s)..."));
    menuItem = menu.Append(XRCID("memcheck_del_supp"), _("Remove suppression file(s)"));
    menuItem->Enable(m_listBoxSuppFiles->HitTest(event.GetPosition()) != wxNOT_FOUND);

    menu.Connect(XRCID("memcheck_add_supp"),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(MemCheckSettingsDialog::OnAddSupp),
                 NULL,
                 (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_del_supp"),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(MemCheckSettingsDialog::OnDelSupp),
                 NULL,
                 (wxEvtHandler*)this);
    PopupMenu(&menu);
}

void MemCheckSettingsDialog::OnFilePickerValgrindOutputFileUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_checkBoxOutputInPrivateFolder->IsChecked());
}
