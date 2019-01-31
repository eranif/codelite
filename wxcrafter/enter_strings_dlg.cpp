#include "enter_strings_dlg.h"
#include <ColoursAndFontsManager.h>
#include <lexer_configuration.h>
#include <windowattrmanager.h>

EnterStringsDlg::EnterStringsDlg(wxWindow* parent, const wxString& value)
    : EnterStringsDlgBase(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text", "Default");
    if(lexer) { lexer->Apply(m_stc); }
    m_stc->SetText(value);
    SetName("EnterStringsDlg");
    WindowAttrManager::Load(this);
}

EnterStringsDlg::~EnterStringsDlg() {}

wxString EnterStringsDlg::GetValue() const { return m_stc->GetText(); }

void EnterStringsDlg::SetMessage(const wxString& msg)
{
    if(msg.IsEmpty() == false) {
        m_staticTextMessage->SetLabel(msg);
        m_staticTextMessage->Show();
    } else {
        m_staticTextMessage->Hide();
    }
}
