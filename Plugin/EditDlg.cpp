#include "EditDlg.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "windowattrmanager.h"

EditDlg::EditDlg(wxWindow* parent, const wxString &text)
    : EditDlgBase(parent)
{
    LexerConfPtr lex = EditorConfigST::Get()->GetLexer("text");
    lex->Apply( m_stc10 );
    m_stc10->SetText( text );
    WindowAttrManager::Load(this, "EditDlg");
}

EditDlg::~EditDlg()
{
    WindowAttrManager::Save(this, "EditDlg");
}


wxString clGetTextFromUser(const wxString &initialValue, wxWindow* parent)
{
    EditDlg dlg(parent, initialValue);
    if ( dlg.ShowModal() == wxID_OK ) {
        return dlg.GetText();
    }
    return wxEmptyString;
}
