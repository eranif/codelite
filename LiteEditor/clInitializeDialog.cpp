#include "clInitializeDialog.h"
#include "ColoursAndFontsManager.h"
#include "event_notifier.h"
#include <wx/msgdlg.h>

clInitializeDialog::clInitializeDialog(wxWindow* parent)
    : clInitializeDialogBase(parent)
{
    CallAfter(&clInitializeDialog::StartUpgrade);
    EventNotifier::Get()->Connect(
        wxEVT_UPGRADE_LEXERS_START, clCommandEventHandler(clInitializeDialog::OnLexersUpgradeStart), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_UPGRADE_LEXERS_END, clCommandEventHandler(clInitializeDialog::OnLexersUpgradeEnd), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_UPGRADE_LEXERS_PROGRESS, clCommandEventHandler(clInitializeDialog::OnLexersUpgradeProgress), NULL, this);
    CenterOnScreen();
}

clInitializeDialog::~clInitializeDialog()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_UPGRADE_LEXERS_START, clCommandEventHandler(clInitializeDialog::OnLexersUpgradeStart), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_UPGRADE_LEXERS_END, clCommandEventHandler(clInitializeDialog::OnLexersUpgradeEnd), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_UPGRADE_LEXERS_PROGRESS, clCommandEventHandler(clInitializeDialog::OnLexersUpgradeProgress), NULL, this);
}

void clInitializeDialog::OnLexersUpgradeEnd(clCommandEvent& e)
{
    EndModal(wxID_OK);
}

void clInitializeDialog::OnLexersUpgradeProgress(clCommandEvent& e)
{
    e.Skip();
    m_gauge->SetValue(e.GetInt());
    m_banner->SetText(_("CodeLite upgrade"), e.GetString());
    wxSafeYield(this);
}

void clInitializeDialog::StartUpgrade()
{
    ColoursAndFontsManager::Get().Load(); // it will perform upgrade
}

void clInitializeDialog::OnLexersUpgradeStart(clCommandEvent& e)
{
    e.Skip();
    static bool rangeSet = false;
    if(!rangeSet) {
        m_gauge->SetRange(e.GetInt());
        rangeSet = true;
    }
}
