#include "gitCommitEditor.h"

#include "drawingutils.h"
#include "lexer_configuration.h"
#include "editor_config.h"

GitCommitEditor::GitCommitEditor(wxWindow* parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style)
    : wxStyledTextCtrl(parent, id, position, size, style)
{
    InitStyles();
}

void GitCommitEditor::InitStyles()
{
    LexerConfPtr diffLexer = EditorConfigST::Get()->GetLexer("Diff");
    if ( diffLexer ) {
        diffLexer->Apply( this );
        this->SetLexer(wxSTC_LEX_DIFF);
    }
}
