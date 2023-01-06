#include "AboutDlg.h"

#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "wxcsvninfo.h"

extern const char* WXC_LICENSE_GPL2;

wxcAboutDlg::wxcAboutDlg(wxWindow* parent)
    : wxcAboutDlgBaseClass(parent)
{
    wxString buildNumber;
    wxString vers;
    buildNumber << GIT_REVISION;
    vers << "wxCrafter-" << GIT_REVISION;
    SetLabel(vers);

    m_staticTextBuildNumber->SetLabel(buildNumber);
    m_staticTextVersion->SetLabel(vers);

    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->ApplySystemColours(m_stcLicense);
    }

    m_stcLicense->SetText(WXC_LICENSE_GPL2);
    m_stcLicense->SetReadOnly(true);

    ::clSetDialogBestSizeAndPosition(this);
}

wxcAboutDlg::~wxcAboutDlg() {}

void wxcAboutDlg::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_staticBitmap178->Refresh();
}
