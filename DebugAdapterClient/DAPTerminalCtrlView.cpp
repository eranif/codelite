#include "DAPTerminalCtrlView.h"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

DAPTerminalCtrlView::DAPTerminalCtrlView(wxWindow* parent, clModuleLogger& log)
    : wxPanel(parent)
    , LOG(log)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    ApplyTheme();

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &DAPTerminalCtrlView::OnThemeChanged, this);
}

DAPTerminalCtrlView::~DAPTerminalCtrlView()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &DAPTerminalCtrlView::OnThemeChanged, this);
}

void DAPTerminalCtrlView::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void DAPTerminalCtrlView::ApplyTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->ApplySystemColours(m_ctrl);
    MSWSetWindowDarkTheme(m_ctrl);
}

void DAPTerminalCtrlView::AppendLine(const wxString& line) { m_ctrl->AppendText(line + "\n"); }

void DAPTerminalCtrlView::ScrollToEnd()
{
    int lastPos = m_ctrl->GetLastPosition();
    m_ctrl->SetCurrentPos(lastPos);
    m_ctrl->SetSelectionStart(lastPos);
    m_ctrl->SetSelectionEnd(lastPos);
    m_ctrl->ScrollToEnd();
}
