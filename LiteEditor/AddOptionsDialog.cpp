#include "AddOptionsDialog.h"
#include <wx/tokenzr.h>
#include "ColoursAndFontsManager.h"
#include <wx/sstream.h>
#include <wx/txtstrm.h>

AddOptionsDialog::AddOptionsDialog(wxWindow* parent, const wxString& value)
    : AddOptionsDialogBase(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_stc);

    wxArrayString lines = ::wxStringTokenize(value, ";");
    for(const wxString& line : lines) {
        m_stc->AppendText(line + "\n");
    }
}

AddOptionsDialog::~AddOptionsDialog() {}

wxString AddOptionsDialog::GetValue() const
{
    wxStringInputStream input(m_stc->GetText());
    wxTextInputStream text(input);

    wxString value;
    while(!input.Eof()) {
        // Read the next line
        value += text.ReadLine();
        value += ";";
    }
    return value.BeforeLast(';');
}
