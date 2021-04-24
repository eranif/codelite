#include "JSON.h"
#include "clRemoteFindDialog.h"
#include "cl_config.h"
#include "globals.h"
#include "ssh_account_info.h"
#include "wx/arrstr.h"
#include "wx/tokenzr.h"

#define INVALID_POS ((size_t)-1)

static const wxString defaultFileTypes = "*.cpp;*.c;*.hpp;*.h;*.txt;*.py;*.php;*.yml;Makefile";
struct clRemoteFindDialogData {
    wxArrayString find_what_arr;
    wxString last_find_what;
    wxArrayString where_arr;
    wxString last_where;
    wxArrayString file_types;
    wxString last_file_types;
};

clRemoteFindDialog::clRemoteFindDialog(wxWindow* parent, const wxString& account_name)
    : clRemoteFindDialogBase(parent)
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
    }

    // read the find what list
    clRemoteFindDialogData data;
    clConfig::Get().Read("remote_find_in_files", [&data](const JSONItem& item) -> void {
        data.find_what_arr = item["find_what_arr"].toArrayString();
        data.last_find_what = item["last_find_what"].toString();
        data.where_arr = item["where_arr"].toArrayString();
        data.last_where = item["last_where"].toString();
        data.file_types = item["file_types"].toArrayString();
        data.last_file_types = item["last_file_types"].toString();
    });

    if(data.file_types.empty() && data.file_types.empty()) {
        data.file_types.push_back(defaultFileTypes);
        data.last_file_types = defaultFileTypes;
    }

    UpdateCombo(m_comboBoxFindWhat, data.find_what_arr, data.last_find_what);
    UpdateCombo(m_comboBoxWhere, data.where_arr, data.last_where);
    UpdateCombo(m_comboBoxTypes, data.file_types, data.last_file_types);
}

clRemoteFindDialog::~clRemoteFindDialog()
{
    clRemoteFindDialogData data;
    data.find_what_arr = m_comboBoxFindWhat->GetStrings();
    data.last_find_what = m_comboBoxFindWhat->GetStringSelection();
    data.where_arr = m_comboBoxWhere->GetStrings();
    data.last_where = m_comboBoxWhere->GetStringSelection();
    data.file_types = m_comboBoxTypes->GetStrings();
    data.last_file_types = m_comboBoxTypes->GetStringSelection();

    clConfig::Get().Write("remote_find_in_files", [&data]() -> JSONItem {
        JSONItem item = JSONItem::createObject();
        item.addProperty("find_what_arr", data.find_what_arr);
        item.addProperty("last_find_what", data.last_find_what);
        item.addProperty("where_arr", data.where_arr);
        item.addProperty("last_where", data.last_where);
        item.addProperty("file_types", data.file_types);
        item.addProperty("last_file_types", data.last_file_types);
        return item;
    });
}

void clRemoteFindDialog::UpdateCombo(clThemedComboBox* cb, const wxArrayString& options, const wxString& lastSelection)
{
    cb->Clear();
    cb->Append(options);
    cb->SetStringSelection(lastSelection);
}

void clRemoteFindDialog::OnOK_UI(wxUpdateUIEvent& event)
{
    event.Enable(!m_comboBoxFindWhat->GetStringSelection().empty() && !m_comboBoxTypes->GetStringSelection().empty() &&
                 !m_comboBoxWhere->GetStrings().empty() && !m_choiceAccounts->GetStringSelection().empty());
}

void clRemoteFindDialog::SetWhere(const wxString& where)
{
    m_comboBoxWhere->SetValue(where);
}

void clRemoteFindDialog::SetFileTypes(const wxString& filetypes)
{
    m_comboBoxTypes->SetValue(filetypes);
}

void clRemoteFindDialog::SetFindWhat(const wxString& findWhat)
{
    m_comboBoxFindWhat->SetValue(findWhat);
}

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
    command.Add("--line-number");
    command.Add("--with-filename");
    command.Add(GetFindWhat());
    return command;
}

wxString clRemoteFindDialog::GetWhere() const
{
    return m_comboBoxWhere->GetStringSelection();
}

wxString clRemoteFindDialog::GetFindWhat() const
{
    return m_comboBoxFindWhat->GetStringSelection();
}

wxString clRemoteFindDialog::GetFileExtensions() const
{
    return m_comboBoxTypes->GetStringSelection();
}
