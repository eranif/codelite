#ifndef NEWTHEMEDIALOG_H
#define NEWTHEMEDIALOG_H
#include "syntaxhighlightbasedlg.h"
#include "lexer_configuration.h"

class NewThemeDialog : public NewThemeDialogBase
{
private:
    void DoInitialize(LexerConf::Ptr_t lexer);
    
public:
    NewThemeDialog(wxWindow* parent, LexerConf::Ptr_t lexer);
    virtual ~NewThemeDialog();

    wxString GetLexerName() const {
        return m_choiceLanguage->GetStringSelection();
    }

    wxString GetThemeName() const {
        return m_textCtrlName->GetValue();
    }

    wxString GetBaseTheme() const {
        return m_choiceBaseTheme->GetStringSelection();
    }
    
protected:
    virtual void OnLexerSelected(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // NEWTHEMEDIALOG_H
