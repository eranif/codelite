#include "clRemoteFindDialog.h"

#include "ColoursAndFontsManager.h"
#include "JSON.h"
#include "StringUtils.h"
#include "clThemedTextCtrl.hpp"
#include "clWorkspaceManager.h"
#include "cl_config.h"
#include "globals.h"
#include "sessionmanager.h"
#include "ssh/ssh_account_info.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

namespace
{

void UpdateComboBox(clComboBox* cb, const wxArrayString& arr, const wxString& str)
{
    cb->Clear();
    if(arr.empty()) {
        return;
    }
    cb->Append(arr);
    size_t where = cb->FindString(str);
    if(where == wxString::npos) {
        where = cb->Append(str);
    }

    if(where != wxString::npos) { // this shouldn't happen... but better be safe than sorry
        cb->SetSelection(where);
    }
}
} // namespace

clRemoteFindDialog::clRemoteFindDialog(wxWindow* parent, const wxString& account_name, const wxString& rootpath)
    : clRemoteFindDialogBase(parent)
    , m_root_path(rootpath)
{
    wxString last_tool = clConfig::Get().Read("remote_find_replace_tool/last_tool", wxString("grep"));
    m_choiceTool->SetSelection(last_tool == "grep" ? 0 : 1);

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
    if(m_data.where.empty() && clWorkspaceManager::Get().GetWorkspace()) {
        m_data.where = clWorkspaceManager::Get().GetWorkspace()->GetFileName().BeforeLast('/');
    }

    UpdateComboBox(m_comboBoxFindWhat, m_data.find_what_array, m_data.find_what);
    UpdateComboBox(m_comboBoxWhere, m_data.where_array, m_data.where);
    UpdateComboBox(m_comboBoxTypes, m_data.files_array, m_data.files);
    UpdateComboBox(m_comboBoxReplaceWith, m_data.replace_with_array, m_data.replace_with);

    auto lex = ColoursAndFontsManager::Get().GetLexer("default");
    auto font = lex->GetFontForStyle(0, this);
    m_comboBoxFindWhat->SetFont(font);
    m_comboBoxWhere->SetFont(font);
    m_comboBoxTypes->SetFont(font);
    m_comboBoxReplaceWith->SetFont(font);

    m_checkBoxCase->SetValue(m_data.flags & wxFRD_MATCHCASE);
    m_checkBoxWholeWord->SetValue(m_data.flags & wxFRD_MATCHWHOLEWORD);
    m_comboBoxFindWhat->SelectAll();

    CallAfter(&clRemoteFindDialog::DoShowControls);
    GetSizer()->Fit(this);
    CenterOnParent();
}

clRemoteFindDialog::~clRemoteFindDialog()
{
    m_data.find_what_array =
        StringUtils::AppendAndMakeUnique(m_comboBoxFindWhat->GetStrings(), m_comboBoxFindWhat->GetValue());
    m_data.find_what = m_comboBoxFindWhat->GetValue();
    m_data.replace_with_array =
        StringUtils::AppendAndMakeUnique(m_comboBoxReplaceWith->GetStrings(), m_comboBoxReplaceWith->GetValue());
    m_data.replace_with = m_comboBoxReplaceWith->GetValue();
    m_data.where_array = StringUtils::AppendAndMakeUnique(m_comboBoxWhere->GetStrings(), m_comboBoxWhere->GetValue());
    m_data.where = m_comboBoxWhere->GetValue();
    m_data.files_array = StringUtils::AppendAndMakeUnique(m_comboBoxTypes->GetStrings(), m_comboBoxTypes->GetValue());
    m_data.files = m_comboBoxTypes->GetValue();
    m_data.flags = 0;
    if(m_checkBoxCase->IsChecked()) {
        m_data.flags |= wxFRD_MATCHCASE;
    }
    if(m_checkBoxWholeWord->IsChecked()) {
        m_data.flags |= wxFRD_MATCHWHOLEWORD;
    }

    // store the find in files session
    clConfig::Get().Write("remote_find_replace_tool/last_tool", m_choiceTool->GetStringSelection());
    SessionManager::Get().SaveFindInFilesSession(m_data);
}

void clRemoteFindDialog::SetFileTypes(const wxString& filetypes) { m_comboBoxTypes->SetValue(filetypes); }

void clRemoteFindDialog::SetFindWhat(const wxString& findWhat) { m_comboBoxFindWhat->SetValue(findWhat); }

wxString clRemoteFindDialog::GetWhere() const { return m_comboBoxWhere->GetValue(); }

wxString clRemoteFindDialog::GetFindWhat() const { return m_comboBoxFindWhat->GetValue(); }
wxString clRemoteFindDialog::GetReplaceWith() const { return m_comboBoxReplaceWith->GetValue(); }

wxString clRemoteFindDialog::GetFileExtensions() const { return m_comboBoxTypes->GetValue(); }

bool clRemoteFindDialog::IsIcase() const { return !m_checkBoxCase->IsChecked(); }

bool clRemoteFindDialog::IsWholeWord() const { return m_checkBoxWholeWord->IsChecked(); }

void clRemoteFindDialog::OnFind(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}

void clRemoteFindDialog::OnReplace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_isReplace = true;
    EndModal(wxID_OK);
}
void clRemoteFindDialog::OnFindUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_comboBoxFindWhat->GetValue().empty() && !m_comboBoxTypes->GetValue().empty());
}

void clRemoteFindDialog::OnReplaceUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_comboBoxFindWhat->GetValue().empty() && !m_comboBoxTypes->GetValue().empty());
}
void clRemoteFindDialog::DoShowControls()
{
    switch(m_choiceTool->GetSelection()) {
    case 1: // sed
        DoShowReplaceControls(true);
        DoShowSearchControls(false);
        break;
    default:
    case 0: // grep
        DoShowReplaceControls(false);
        DoShowSearchControls(true);
        break;
    }
    GetSizer()->Layout();
    SendSizeEvent();
}

void clRemoteFindDialog::OnTool(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoShowControls();
}

void clRemoteFindDialog::DoShowSearchControls(bool show)
{
    m_buttonFind->Show(show);
    if(show) {
        m_buttonFind->SetDefault();
    }
}

void clRemoteFindDialog::DoShowReplaceControls(bool show)
{
    m_buttonReplace->Show(show);
    m_staticTextReplace->Show(show);
    m_comboBoxReplaceWith->Show(show);
    if(show) {
        m_buttonReplace->SetDefault();
    }
}
