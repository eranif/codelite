#include "PHPRefactoringPreviewDlg.h"
#include "ColoursAndFontsManager.h"
#include "clPatch.h"
#include "lexer_configuration.h"
#include <wx/msgdlg.h>

PHPRefactoringPreviewDlg::PHPRefactoringPreviewDlg(wxWindow* parent, const wxString& patchContent)
    : PHPRefactoringPreviewBaseDlg(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("diff");
    if(lexer) {
        lexer->Apply(m_stcPreview, true);
    }
    m_stcPreview->SetText(patchContent);
    m_stcPreview->SetReadOnly(true);
    m_stcPreview->SetFirstVisibleLine(0);
    CentreOnParent();
}

PHPRefactoringPreviewDlg::~PHPRefactoringPreviewDlg() {}

void PHPRefactoringPreviewDlg::OnApplyChanges(wxCommandEvent& event)
{
    event.Skip();
    EndModal(wxID_OK);
}
