#include "LanguageServerPage.h"

#include "ColoursAndFontsManager.h"
#include "JSON.h"
#include "LSP/LanguageServerProtocol.h"
#include "StringUtils.h"
#include "globals.h"

#include <algorithm>
#include <macros.h>
#include <wx/choicdlg.h>
#include <wx/dirdlg.h>
#include <wx/tokenzr.h>

LanguageServerPage::LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data)
    : LanguageServerPageBase(parent)
{
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) {
        lex->ApplySystemColours(m_stcCommand);
        lex->ApplySystemColours(m_stcInitOptions);
    }

    m_textCtrlName->SetValue(data.GetName());
    m_textCtrlWD->SetValue(data.GetWorkingDirectory());

    m_stcCommand->SetText(data.GetCommand(true));
    m_stcInitOptions->SetText(data.GetInitOptions());

    m_checkBoxEnabled->SetValue(data.IsEnabled());
    const wxArrayString& langs = data.GetLanguages();
    wxString languages = wxJoin(langs, ';');
    this->m_textCtrlLanguages->SetValue(languages);
    this->m_comboBoxConnection->SetValue(data.GetConnectionString());
    m_checkBoxDiagnostics->SetValue(data.IsDisaplayDiagnostics());
}

LanguageServerPage::LanguageServerPage(wxWindow* parent)
    : LanguageServerPageBase(parent)
{
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) {
        lex->ApplySystemColours(m_stcCommand);
        lex->ApplySystemColours(m_stcInitOptions);
    }
}

LanguageServerPage::~LanguageServerPage() {}

LanguageServerEntry LanguageServerPage::GetData() const
{
    // build the command
    LanguageServerEntry d;
    d.SetName(m_textCtrlName->GetValue());
    d.SetCommand(m_stcCommand->GetText());
    d.SetWorkingDirectory(m_textCtrlWD->GetValue());
    d.SetLanguages(GetLanguages());
    d.SetEnabled(m_checkBoxEnabled->IsChecked());
    d.SetConnectionString(m_comboBoxConnection->GetValue());
    d.SetDisaplayDiagnostics(m_checkBoxDiagnostics->IsChecked());
    d.SetInitOptions(m_stcInitOptions->GetText());
    return d;
}

wxArrayString LanguageServerPage::GetLanguages() const
{
    wxArrayString selectedLanguages;
    wxString langStr = m_textCtrlLanguages->GetValue();
    selectedLanguages = ::wxStringTokenize(langStr, ";,", wxTOKEN_STRTOK);
    return selectedLanguages;
}

void LanguageServerPage::OnSuggestLanguages(wxCommandEvent& event)
{
    const std::set<wxString>& langSet = LanguageServerProtocol::GetSupportedLanguages();
    wxArrayString arrLang;
    for(const wxString& lang : langSet) {
        arrLang.Add(lang);
    }

    wxArrayInt selections;
    int count = ::wxGetSelectedChoices(selections, _("Select the supported languages by this server:"), _("CodeLite"),
                                       arrLang, GetParent());
    if(count == wxNOT_FOUND) {
        return;
    }

    wxString newText;
    for(int sel : selections) {
        newText << arrLang.Item(sel) << ";";
    }
    m_textCtrlLanguages->ChangeValue(newText);
}
void LanguageServerPage::OnCommandUI(wxUpdateUIEvent& event) { event.Enable(true); }
void LanguageServerPage::OnBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path(m_textCtrlWD->GetValue());
    wxString new_path =
        wxDirSelector(_("Select a working directory:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        m_textCtrlWD->SetValue(new_path);
    }
}

bool LanguageServerPage::ValidateData(wxString* message) const
{
    // check that the "initializationOptions" are in the correct form
    wxString init_options = m_stcInitOptions->GetText();
    init_options.Trim().Trim(false);

    if(init_options.empty()) {
        return true;
    }

    JSON root{ init_options };
    if(!root.isOk()) {
        (*message) << m_textCtrlName->GetValue() << ": invalid JSON input in `initializationOptions`";
        return false;
    }

    if(!root.toElement().isObject()) {
        (*message) << m_textCtrlName->GetValue() << ": `initializationOptions` must be a JSON object";
        return false;
    }
    return true;
}