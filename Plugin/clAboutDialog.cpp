#include "ColoursAndFontsManager.h"
#include "bitmap_loader.h"
#include "clAboutDialog.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include <wx/filename.h>

clAboutDialog::clAboutDialog(wxWindow* parent, const wxString& version)
    : clAboutDialogBase(parent)
{
    m_staticTextVersion->SetLabel(version);
    // Load the license file
    wxFileName license(clStandardPaths::Get().GetDataDir(), "LICENSE");
    wxString fileContent;
    FileUtils::ReadFileContent(license, fileContent);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stcLicense);
        lexer->Apply(m_stcCredits);
    }

    // Set the license file
    m_stcLicense->SetText(fileContent);
    m_stcLicense->SetReadOnly(true);

    // Set the credits
    wxString credits;

    credits << "\n==============================\n\n";
    credits << _("Eran Ifrah (author & project admin)") << "\n";
    credits << _("David G. Hart") << "\n\n";
    credits << "==============================\n\n\n";
    credits << _("Contributors:\n");
    credits << _("Frank Lichtner") << "\n";
    credits << _("Jacek Kucharski") << "\n";
    credits << _("Marrianne Gagnon") << "\n";
    credits << _("Scott Dolim") << "\n";
    m_stcCredits->SetText(credits);
    m_stcCredits->SetReadOnly(true);
    ::clSetSmallDialogBestSizeAndPosition(this);
}

clAboutDialog::~clAboutDialog() {}
