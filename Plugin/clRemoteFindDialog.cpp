#include "clRemoteFindDialog.h"

#include "JSON.h"
#include "clThemedTextCtrl.hpp"
#include "cl_config.h"
#include "globals.h"
#include "sessionmanager.h"
#include "ssh_account_info.h"
#include "wx/arrstr.h"
#include "wx/tokenzr.h"

clRemoteFindDialog::clRemoteFindDialog(wxWindow* parent, const wxString& account_name, const wxString& rootpath)
    : clRemoteFindDialogBase(parent)
    , m_root_path(rootpath)
{
    auto accounts = SSHAccountInfo::Load();
    if(!accounts.empty()) {
        wxArrayString accounts_arr;
        accounts_arr.reserve(accounts.size());
        for(const auto& acc : accounts) {
            accounts_arr.Add(acc.GetAccountName());
        }
        m_choiceAccounts->Append(accounts_arr);
        m_choiceAccounts->SetStringSelection(account_name);
        m_choiceAccounts->Enable(false);
    }

    // read the find what list
    SessionManager::Get().LoadFindInFilesSession(&m_data);

    UpdateCombo(m_comboBoxFindWhat, m_data.find_what_array, m_data.find_what);
    UpdateCombo(m_comboBoxWhere, m_data.where_array, m_data.where);
    UpdateCombo(m_comboBoxTypes, m_data.files_array, m_data.files);
    m_checkBoxCase->SetValue(m_data.flags & wxFRD_MATCHCASE);
    m_checkBoxWholeWord->SetValue(m_data.flags & wxFRD_MATCHWHOLEWORD);
    m_comboBoxFindWhat->GetTextCtrl()->SelectAll();
    CenterOnParent();
}

clRemoteFindDialog::~clRemoteFindDialog()
{
    m_data.find_what_array = m_comboBoxFindWhat->GetStrings();
    m_data.find_what = m_comboBoxFindWhat->GetStringSelection();
    m_data.where_array = m_comboBoxWhere->GetStrings();
    m_data.where = m_comboBoxWhere->GetStringSelection();
    m_data.files_array = m_comboBoxTypes->GetStrings();
    m_data.files = m_comboBoxTypes->GetStringSelection();
    m_data.flags = 0;
    if(m_checkBoxCase->IsChecked()) {
        m_data.flags |= wxFRD_MATCHCASE;
    }
    if(m_checkBoxWholeWord->IsChecked()) {
        m_data.flags |= wxFRD_MATCHWHOLEWORD;
    }

    // truncate the number of items we keep in the history
    static constexpr int max_size = 20;
    if(m_data.where_array.size() > max_size) {
        m_data.where_array.resize(max_size);
    }

    if(m_data.find_what_array.size() > max_size) {
        m_data.find_what_array.resize(max_size);
    }

    if(m_data.files_array.size() > max_size) {
        m_data.files_array.resize(max_size);
    }

    // store the find in files session
    SessionManager::Get().SaveFindInFilesSession(m_data);
}

void clRemoteFindDialog::UpdateCombo(clThemedComboBox* cb, const wxArrayString& options, const wxString& lastSelection)
{
    cb->Clear();
    cb->Append(options);
    int where = cb->FindString(lastSelection);
    if(where != wxNOT_FOUND) {
        cb->SetSelection(where);
    } else {
        cb->SetValue(lastSelection);
    }
}

void clRemoteFindDialog::OnOK_UI(wxUpdateUIEvent& event) { event.Enable(CanOk()); }

void clRemoteFindDialog::SetFileTypes(const wxString& filetypes) { m_comboBoxTypes->SetValue(filetypes); }

void clRemoteFindDialog::SetFindWhat(const wxString& findWhat) { m_comboBoxFindWhat->SetValue(findWhat); }

wxArrayString clRemoteFindDialog::GetGrepCommand() const
{
    wxArrayString command;
    command.Add("find");
    command.Add(GetWhere());
    command.Add("-type");
    command.Add("f");
    command.Add("\\(");

    wxArrayString types = ::wxStringTokenize(GetFileExtensions(), ";, ", wxTOKEN_STRTOK);
    for(size_t i = 0; i < types.size(); ++i) {
        command.Add("-iname");
        command.Add("\\" + types[i]);
        command.Add("-o"); // logical OR
    }
    command.RemoveAt(command.size() - 1); // remove the last "-o"
    command.Add("\\)");
    command.Add("|");
    command.Add("xargs");
    command.Add("grep");
    if(!m_checkBoxCase->IsChecked()) {
        command.Add("-i");
    }
    if(m_checkBoxWholeWord->IsChecked()) {
        command.Add("-w");
    }
    command.Add("--line-number");
    command.Add("--with-filename");
    command.Add(GetFindWhat());
    return command;
}

wxString clRemoteFindDialog::GetWhere() const { return m_comboBoxWhere->GetStringSelection(); }

wxString clRemoteFindDialog::GetFindWhat() const { return m_comboBoxFindWhat->GetStringSelection(); }

wxString clRemoteFindDialog::GetFileExtensions() const { return m_comboBoxTypes->GetStringSelection(); }

bool clRemoteFindDialog::IsIcase() const { return !m_checkBoxCase->IsChecked(); }

bool clRemoteFindDialog::IsWholeWord() const { return m_checkBoxWholeWord->IsChecked(); }

void clRemoteFindDialog::OnSearch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(CanOk()) {
        EndModal(wxID_OK);
    }
}

bool clRemoteFindDialog::CanOk() const
{
    return !m_comboBoxFindWhat->GetStringSelection().empty() && !m_comboBoxTypes->GetStringSelection().empty() &&
           !m_comboBoxWhere->GetStrings().empty() && !m_choiceAccounts->GetStringSelection().empty();
}
