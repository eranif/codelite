#include "LanguageServerPage.h"
#include <algorithm>
#include <macros.h>
#include "LanguageServerProtocol.h"

LanguageServerPage::LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data)
    : LanguageServerPageBase(parent)
{
    m_textCtrlName->SetValue(data.GetName());
    m_dirPickerWorkingDir->SetPath(data.GetWorkingDirectory());
    m_filePickerExe->SetPath(data.GetExepath());
    m_textCtrlArgs->SetValue(data.GetArgs());

    const wxArrayString& langs = data.GetLanguages();
    wxStringSet_t checkedLanguages;
    for(const wxString& lang : langs) {
        checkedLanguages.insert(lang);
    }

    const std::set<wxString>& langsSet = LanguageServerProtocol::GetSupportedLanguages();
    std::for_each(langsSet.begin(), langsSet.end(), [&](const wxString& lang) {
        int where = m_checkListBoxLanguages->Append(lang);
        m_checkListBoxLanguages->Check(where, (checkedLanguages.count(lang) != 0));
    });
    SetSize(400, 300);
    GetSizer()->Fit(this);
}

LanguageServerPage::~LanguageServerPage() {}
