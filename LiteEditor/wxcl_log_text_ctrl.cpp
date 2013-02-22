#include "wxcl_log_text_ctrl.h"
#include <wx/settings.h>
#include "drawingutils.h"

wxclTextCtrl::wxclTextCtrl(wxTextCtrl* text)
    : m_pTextCtrl(text)
{
    DoInit();
}

wxclTextCtrl::~wxclTextCtrl()
{
}

void wxclTextCtrl::Reset()
{
    DoInit();
}

void wxclTextCtrl::DoInit()
{
    m_pTextCtrl->Clear();
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    f.SetFamily(wxFONTFAMILY_TELETYPE);
    m_pTextCtrl->SetFont(f);
    m_pTextCtrl->SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    wxTextAttr defaultStyle;
    defaultStyle.SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    defaultStyle.SetTextColour(DrawingUtils::GetOutputPaneFgColour());
    defaultStyle.SetFont(f);
    m_pTextCtrl->SetDefaultStyle(defaultStyle);
}
