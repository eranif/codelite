#include "NewThemeDialog.h"
#include "windowattrmanager.h"
#include "ColoursAndFontsManager.h"

NewThemeDialog::NewThemeDialog(wxWindow* parent, LexerConf::Ptr_t lexer)
    : NewThemeDialogBase(parent)
{
    Center();
    SetName("NewThemeDialog");
    WindowAttrManager::Load(this);
    DoInitialize(lexer);
}

void NewThemeDialog::OnOkUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlName->IsEmpty()); }

void NewThemeDialog::OnLexerSelected(wxCommandEvent& event)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer(m_choiceLanguage->GetStringSelection());
    if(lexer) {
        DoInitialize(lexer);
    }
}

void NewThemeDialog::DoInitialize(LexerConf::Ptr_t lexer)
{
    m_choiceBaseTheme->Clear();
    m_choiceLanguage->Clear();

    // Populate the fields
    wxArrayString allLexers = ColoursAndFontsManager::Get().GetAllLexersNames();
//    int sel = allLexers.Index(lexer->GetName());
    m_choiceLanguage->Append(allLexers);
    m_choiceLanguage->SetStringSelection(lexer->GetName());

    m_choiceBaseTheme->Append(ColoursAndFontsManager::Get().GetAvailableThemesForLexer(lexer->GetName()));
    if(!m_choiceBaseTheme->IsEmpty()) {
        m_choiceBaseTheme->SetSelection(0);
    }
}
