#include "NewLanguageServerDlg.h"
#include "LanguageServerProtocol.h"
#include <macros.h>

NewLanguageServerDlg::NewLanguageServerDlg(wxWindow* parent)
    : NewLanguageServerDlgBase(parent)
{
    const std::set<wxString>& langsSet = LanguageServerProtocol::GetSupportedLanguages();
    for(const wxString& lang : langsSet) {
        m_checkListBoxLanguages->Append(lang);
    }
}

NewLanguageServerDlg::~NewLanguageServerDlg() {}

void NewLanguageServerDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxFileName exe(m_filePickerExe->GetPath());
    wxArrayInt checkedLanguages;
    m_checkListBoxLanguages->GetSelections(checkedLanguages);
    event.Enable(!checkedLanguages.IsEmpty() && exe.FileExists());
}

LanguageServerEntry NewLanguageServerDlg::GetData() const
{
    LanguageServerEntry d;
    d.SetName(m_textCtrlName->GetValue());
    d.SetArgs(m_textCtrlArgs->GetValue());
    d.SetExepath(m_filePickerExe->GetPath());
    d.SetWorkingDirectory(m_dirPickerWorkingDir->GetPath());
    d.SetLanguages(GetLanguages());
    return d;
}

wxArrayString NewLanguageServerDlg::GetLanguages() const
{
    wxArrayInt checkedLanguages;
    wxArrayString allLanguages = m_checkListBoxLanguages->GetStrings();
    m_checkListBoxLanguages->GetSelections(checkedLanguages);
    wxArrayString selectedLanguages;
    for(size_t i = 0; i < checkedLanguages.size(); ++i) {
        selectedLanguages.Add(allLanguages.Item(checkedLanguages.Item(i)));
    }
    return selectedLanguages;
}
