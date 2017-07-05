#include "PHPRefactoringPreviewDlg.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include "clPatch.h"
#include <wx/msgdlg.h>

PHPRefactoringPreviewDlg::PHPRefactoringPreviewDlg(wxWindow* parent, const wxString& patchFile,
                                                   const wxString& patchContent)
    : PHPRefactoringPreviewBaseDlg(parent)
    , m_patchFile(patchFile)
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
    // Apply the patch
    try {
        clPatch patcher;
        // We pass "--verbose" otherwise it crashes oftenly on Windows... go figure...
        patcher.Patch(m_patchFile, "", "--ignore-whitespace --verbose -p1 < ");
    } catch(clException& e) {
        wxMessageBox(e.What(), "CodeLite", wxICON_ERROR | wxOK | wxCENTER, this);
    }
    EndModal(wxID_OK);
}
