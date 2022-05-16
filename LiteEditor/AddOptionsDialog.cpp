#include "AddOptionsDialog.h"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "file_logger.h"

#include <globals.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>

AddOptionsDialog::AddOptionsDialog(wxWindow* parent, const wxString& value)
    : AddOptionsDialogBase(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_stc);

    wxArrayString lines = StringUtils::BuildArgv(value);
    wxString content;
    for(const wxString& line : lines) {
        content << line << "\n";
    }
    if(!content.IsEmpty()) {
        content.RemoveLast();
    }
    m_stc->SetText(content);
    clSetSmallDialogBestSizeAndPosition(this);
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
