#include "LanguageServerPage.h"
#include <algorithm>
#include <macros.h>
#include "LanguageServerProtocol.h"
#include "globals.h"

LanguageServerPage::LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data)
    : LanguageServerPageBase(parent)
{
    m_textCtrlName->SetValue(data.GetName());
    m_dirPickerWorkingDir->SetPath(data.GetWorkingDirectory());
    m_filePickerExe->SetPath(data.GetExepath());
    m_textCtrlArgs->SetValue(data.GetArgs());
    m_checkBoxEnabled->SetValue(data.IsEnabled());
    
    const wxArrayString& langs = data.GetLanguages();
    wxStringSet_t checkedLanguages;
    for(const wxString& lang : langs) {
        checkedLanguages.insert(lang);
    }

    const std::set<wxString>& langsSet = LanguageServerProtocol::GetSupportedLanguages();
    wxVector<wxVariant> cols;
    std::for_each(langsSet.begin(), langsSet.end(), [&](const wxString& lang) {
        cols.clear();
        cols.push_back(::MakeCheckboxVariant(lang, (checkedLanguages.count(lang) != 0), wxNOT_FOUND));
        m_dvListCtrl->AppendItem(cols);
    });
}

LanguageServerPage::LanguageServerPage(wxWindow* parent)
    : LanguageServerPageBase(parent)
{
    const std::set<wxString>& langsSet = LanguageServerProtocol::GetSupportedLanguages();
    wxVector<wxVariant> cols;
    std::for_each(langsSet.begin(), langsSet.end(), [&](const wxString& lang) {
        cols.clear();
        cols.push_back(::MakeCheckboxVariant(lang, false, wxNOT_FOUND));
        m_dvListCtrl->AppendItem(cols);
    });
}

LanguageServerPage::~LanguageServerPage() {}

LanguageServerEntry LanguageServerPage::GetData() const
{
    LanguageServerEntry d;
    d.SetName(m_textCtrlName->GetValue());
    d.SetArgs(m_textCtrlArgs->GetValue());
    d.SetExepath(m_filePickerExe->GetPath());
    d.SetWorkingDirectory(m_dirPickerWorkingDir->GetPath());
    d.SetLanguages(GetLanguages());
    d.SetEnabled(m_checkBoxEnabled->IsChecked());
    return d;
}

wxArrayString LanguageServerPage::GetLanguages() const
{
    wxArrayString selectedLanguages;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(m_dvListCtrl->IsItemChecked(item, 0)) { selectedLanguages.Add(m_dvListCtrl->GetItemText(item, 0)); }
    }
    return selectedLanguages;
}
